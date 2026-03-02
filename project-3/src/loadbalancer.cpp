#include "LoadBalancer.h"
#include <utility>
#include <random>
#include <string>
#include <stdexcept>

// Logging through spdlog
#include <spdlog/spdlog.h>

using std::string;
using std::to_string;

LoadBalancer::LoadBalancer(int numServers,
                           int cooldown,
                           IPBlocker blocker,
                           int newReqFreq,
                           unsigned int minTimeProcessing,
                           unsigned int maxTimeProcessing,
                           unsigned int minTimeStreaming,
                           unsigned int maxTimeStreaming) : nextServerId(0),
                                                            cooldown(cooldown),
                                                            clock(1),
                                                            lastCheckedClock(0),
                                                            blocker(blocker),
                                                            rng(std::random_device{}()),
                                                            newReqFreq(newReqFreq)

{
    auto logger = spdlog::get("lb");
    if (!logger)
        return;

    // Edge cases for minTime and maxTime for processing and streaming
    if (minTimeProcessing > maxTimeProcessing)
    {
        logger->error("The minimum number of clock cycles for processing must be less than the maximum number of clock cycles for processing");
        throw std::invalid_argument("minTimeProcessing must be less than maxTimeProcessing");
    }
    else if (minTimeStreaming > maxTimeStreaming)
    {
        logger->error("The minimum number of clock cycles for streaming must be less than the maximum number of clock cycles for streaming");
        throw std::invalid_argument("minTimeStreaming must be less than maxTimeStreaming");
    }
    else if (minTimeStreaming < 1 || maxTimeStreaming < 1 || minTimeProcessing < 1 || maxTimeProcessing < 1)
    {
        logger->error("The minimum number of clock cycles for processing and streaming must be greater than 0 for all parameters");
        throw std::invalid_argument("minTime and maxTime must be greater than 0");
    }

    this->minTimeProcessing = minTimeProcessing;
    this->maxTimeProcessing = maxTimeProcessing;
    this->minTimeStreaming = minTimeStreaming;
    this->maxTimeStreaming = maxTimeStreaming;

    // Create servers
    servers.reserve(numServers);
    for (int i = 0; i < numServers; i++)
    {
        servers.emplace_back(nextServerId++);
    }

    // Create requests and add to queue
    while (requestQueue.size() < 100 * numServers)
    {
        addRequest(generateRequest());
    }

    // Reset number of blocked requests
    totalRequestsBlocked = 0;
    totalRequests = requestQueue.size();
    totalServersAdded = servers.size();
    logger->info("Load balancer initialized with {} servers and {} requests", servers.size(), totalRequests);
}

// ======================== PRIVATE METHODS ========================

bool LoadBalancer::underMinThreshold()
{
    unsigned int limit = 50 * servers.size();
    return requestQueue.size() < limit;
}

bool LoadBalancer::aboveMaxThreshold()
{
    unsigned int limit = 80 * servers.size();
    return requestQueue.size() > limit;
}

void LoadBalancer::addServer(int amt)
{
    if (amt <= 0)
    {
        return;
    }

    auto logger = spdlog::get("lb");
    if (logger)
    {
        logger->info("  Attempting to add {} server(s) (active: {})", amt, servers.size());
    }

    servers.reserve(servers.size() + amt);

    int added = 0;
    for (int i = 0; i < amt; i++)
    {
        servers.emplace_back(nextServerId++);
        totalServersAdded++;
        added++;
    }

    if (logger)
    {
        logger->info("  Added {} server(s); {} now active", added, servers.size());
    }
}

void LoadBalancer::removeServer(int amt)
{
    auto logger = spdlog::get("lb");
    if (logger && amt > 0)
    {
        logger->info("  Attempting to remove up to {} server(s) (active: {})", amt, servers.size());
    }

    int removed = 0;
    for (size_t i = 0; i < servers.size() && removed < amt;)
    {
        if (!servers[i].isBusy())
        {
            if (servers.size() > 1)
            {
                std::swap(servers[i], servers.back());
                servers.pop_back();
                removed++;
            }
            else
            {
                if (logger)
                {
                    logger->warn("  Cannot remove additional servers; only one server remains active");
                }
                break;
            }
        }
        else
        {
            i++;
        }
    }

    totalServersRemoved += removed;

    if (logger && amt > 0)
    {
        if (removed > 0)
        {
            logger->info("  Removed {} idle server(s); {} remain active", removed, servers.size());
        }
        else
        {
            logger->info("  No idle servers available for removal; {} remain active", servers.size());
        }
    }
}

void LoadBalancer::checkScale()
{
    auto logger = spdlog::get("lb");
    if (!logger)
        return;

    if (clock - lastCheckedClock >= cooldown)
    {
        logger->info("Cycle {}: Checking scale of servers", clock);
        if (underMinThreshold())
        {
            logger->warn("Cycle {}: The queue size ({}) is below the minimum target ({}). Attempting to remove 1 server.", clock, requestQueue.size(), 50 * servers.size());
            removeServer(1);
        }
        else if (aboveMaxThreshold())
        {
            logger->warn("Cycle {}: The queue size ({}) is above the maximum target ({}). Attempting to add 1 server.", clock, requestQueue.size(), 80 * servers.size());
            addServer(1);
        }
        else
        {
            logger->info("No servers added or removed");
        }
        lastCheckedClock = clock;
    }
}

Request LoadBalancer::generateRequest()
{

    // Create random functions to generate random requests
    std::uniform_int_distribution<int> ipDist(0, 255);
    std::uniform_int_distribution<int> typeDist(0, 1);
    std::uniform_int_distribution<int> timeDistProcessing(minTimeProcessing, maxTimeProcessing);
    std::uniform_int_distribution<int> timeDistStreaming(minTimeStreaming, maxTimeStreaming);

    // Generate all random values
    char type = (typeDist(rng) == 0) ? 'P' : 'S';
    string typeStr = (type == 'P') ? "processing" : "streaming";

    int aIn = ipDist(rng);
    int bIn = ipDist(rng);
    int cIn = ipDist(rng);
    int dIn = ipDist(rng);

    int aOut = ipDist(rng);
    int bOut = ipDist(rng);
    int cOut = ipDist(rng);
    int dOut = ipDist(rng);

    int time = (type == 'P') ? timeDistProcessing(rng) : timeDistStreaming(rng);

    // Creating ip string for IP in and IP out
    string ipIn = to_string(aIn) + "." + to_string(bIn) + "." + to_string(cIn) + "." + to_string(dIn);
    string ipOut = to_string(aOut) + "." + to_string(bOut) + "." + to_string(cOut) + "." + to_string(dOut);

    Request req(time, type, ipIn, ipOut);

    if (auto logger = spdlog::get("lb"))
    {
        logger->debug("Cycle {}: Generated a {} request with a duration of {} clock cycles from {} to {}", clock, typeStr, time, ipIn, ipOut);
    }
    return req;
}

void LoadBalancer::assignRequests()
{
    unsigned int counter = 0;
    auto logger = spdlog::get("lb");
    for (auto &server : servers)
    {
        if (requestQueue.empty())
        {
            if (logger)
            {
                logger->debug("Cycle {}: Request queue empty; exiting assignment loop after {} dispatch(es)", clock, counter);
            }
            break;
        }
        else if (!server.isBusy())
        {
            counter++;
            server.assign(requestQueue.front());
            requestQueue.pop();
        }
    }
    if (logger && counter > 0)
    {
        logger->info("Cycle {}: Assigned {} request(s). The queue now contains {} request(s).",
                     clock, counter, requestQueue.size());
    }
}

void LoadBalancer::process()
{
    unsigned int busyThisCycle = 0;
    unsigned int completedThisCycle = 0;
    auto logger = spdlog::get("lb");

    for (auto &server : servers)
    {
        bool wasBusy = server.isBusy();
        if (wasBusy)
        {
            busyThisCycle++;
            server.processCycle(); // returns true every tick of work, but we don't need it
            if (!server.isBusy())
                completedThisCycle++;
        }
    }

    totalRequestsProcessed += completedThisCycle;

    if (!logger)
        return;

    if (completedThisCycle > 0)
    {
        logger->info("Cycle {}: {} request(s) completed. {} server(s) were busy this cycle. Total processed so far: {}.",
                     clock, completedThisCycle, busyThisCycle, totalRequestsProcessed);
    }
    else if (busyThisCycle > 0 && logger->should_log(spdlog::level::debug))
    {
        logger->debug("Cycle {}: No requests completed during this cycle. {} server(s) are still processing.", clock, busyThisCycle);
    }
}

void LoadBalancer::simulateCycle()
{
    newRequest();
    assignRequests();
    process();
    checkScale();
    clock++;
}

// ======================== PUBLIC METHODS ========================

void LoadBalancer::newRequest()
{
    std::uniform_int_distribution<int> newReqDist(1, 100);
    bool generateNewReq = (newReqDist(rng) <= newReqFreq);
    if (generateNewReq)
    {
        if (auto logger = spdlog::get("lb"))
            logger->debug("Cycle {}: A new request arrived. Queue size before arrival: {}.", clock, requestQueue.size());
        addRequest(generateRequest());
    }
}

void LoadBalancer::printFinalStats()
{
    auto logger = spdlog::get("lb");
    if (!logger)
        return;

    logger->info("Final stats after {} cycles:", clock - 1);
    logger->info("  Total requests generated: {}", totalRequests);
    logger->info("  Total requests processed: {}", totalRequestsProcessed);
    logger->info("  Total requests blocked:   {}", totalRequestsBlocked);
    logger->info("  Servers added:            {}", totalServersAdded);
    logger->info("  Servers removed:          {}", totalServersRemoved);
    logger->info("  Requests still queued:    {}", requestQueue.size());
    logger->info("  Active servers:           {}", servers.size());
}
void LoadBalancer::printInitialStats()
{
    auto logger = spdlog::get("lb");
    if (!logger)
        return;

    logger->info("Initial stats at cycle {}:", clock);
    logger->info("  Active servers: {}", servers.size());
    logger->info("  Requests queued: {}", requestQueue.size());
    logger->info("  Cooldown period: {}", cooldown);
    logger->info("  New request frequency: {}%", newReqFreq);
    logger->info("  Range for streaming requests: [{}, {}]", minTimeStreaming, maxTimeStreaming);
    logger->info("  Range for processing requests: [{}, {}]", minTimeProcessing, maxTimeProcessing);
}

void LoadBalancer::addRequest(Request req)
{
    // Check to see if IP is in correct range
    if (blocker.isBlocked(req.getIpIn()))
    {
        if (auto logger = spdlog::get("lb"))
        {
            logger->warn("Cycle {}: Blocked a request originating from {}.", clock, req.getIpIn());
        }
        totalRequestsBlocked++;
        // TODO: Add logging
        return;
    }
    requestQueue.push(std::move(req));
    totalRequests++;
    if (auto logger = spdlog::get("lb"))
    {
        logger->debug("Cycle {}: The request was added to the queue. Queue size is now {}.", clock, requestQueue.size());
    }
}

void LoadBalancer::run(int totalCycles)
{
    auto logger = spdlog::get("lb");
    if (logger)
    {
        logger->info("Starting simulation for {} cycle(s). Active servers: {}. Initial queue size: {}.", totalCycles, servers.size(), requestQueue.size());
    }
    // Reset all stats for another run if needed
    totalRequests = 0;
    totalRequestsProcessed = 0;
    totalRequestsBlocked = 0;
    totalServersAdded = 0;
    totalServersRemoved = 0;
    clock = 1;

    while (clock <= totalCycles)
    {
        simulateCycle();
    }
    if (logger)
    {
        logger->info("Simulation finished. Active servers: {}. Final queue size: {}.", servers.size(), requestQueue.size());
    }
}
