/**
 * @file LoadBalancer.cpp
 * @brief Implementation of the LoadBalancer simulation controller.
 *
 * This file implements the LoadBalancer class, which coordinates a queue of
 * incoming Requests, assigns work to a pool of WebServer instances, processes
 * requests over discrete clock cycles, and dynamically scales the server pool
 * based on queue size thresholds.
 *
 * Logging:
 * The simulation uses a shared spdlog logger (named "lb") that is expected to
 * be initialized by the program entry point (typically main.cpp). If the logger
 * is not present, the simulation still functions, but log output is suppressed.
 */
#include "LoadBalancer.h"
#include <utility>
#include <random>
#include <string>
#include <stdexcept>

// Logging through spdlog
#include <spdlog/spdlog.h>

using std::string;
using std::to_string;

/**
 * @brief Construct a LoadBalancer and initialize the simulation state.
 *
 * Creates an initial pool of servers, fills the starting request queue
 * (typically servers * 100 requests), and validates request timing parameters.
 *
 * @param numServers Initial number of WebServer instances to create.
 * @param cooldown Number of cycles to wait between scaling checks.
 * @param blocker Firewall component used to block request source IP ranges.
 * @param newReqFreq Probability (0-100) that a new request arrives each cycle.
 * @param minTimeProcessing Minimum duration for processing requests (cycles).
 * @param maxTimeProcessing Maximum duration for processing requests (cycles).
 * @param minTimeStreaming Minimum duration for streaming requests (cycles).
 * @param maxTimeStreaming Maximum duration for streaming requests (cycles).
 *
 * @throws std::invalid_argument If any request timing range is invalid.
 */
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

/**
 * @brief Check whether the queue is below the minimum target for current capacity.
 *
 * The minimum threshold is defined as 50 requests per active server.
 *
 * @return true If the current queue size is less than 50 * servers.
 * @return false Otherwise.
 */
bool LoadBalancer::underMinThreshold()
{
    unsigned int limit = 50 * servers.size();
    return requestQueue.size() < limit;
}

/**
 * @brief Check whether the queue is above the maximum target for current capacity.
 *
 * The maximum threshold is defined as 80 requests per active server.
 *
 * @return true If the current queue size is greater than 80 * servers.
 * @return false Otherwise.
 */
bool LoadBalancer::aboveMaxThreshold()
{
    unsigned int limit = 80 * servers.size();
    return requestQueue.size() > limit;
}

/**
 * @brief Add one or more servers to the pool.
 *
 * New servers are assigned unique IDs using an internal monotonically
 * increasing counter.
 *
 * @param amt Number of servers to add. If amt <= 0, no action is taken.
 */
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

/**
 * @brief Remove up to a given number of idle servers from the pool.
 *
 * Only servers that are not currently busy are eligible for removal.
 * The load balancer will not remove the final remaining server.
 *
 * @param amt Maximum number of servers to remove.
 */
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

/**
 * @brief Evaluate whether to scale the server pool up or down.
 *
 * Scaling decisions are gated by a cooldown timer. When eligible, the load
 * balancer removes one server if the queue falls below 50 * servers, and adds
 * one server if the queue rises above 80 * servers.
 */
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

/**
 * @brief Generate a new random Request.
 *
 * Randomly chooses a request type (processing or streaming), generates random
 * source/destination IPv4 addresses, and selects a duration based on the type.
 *
 * @return Request Newly generated request instance.
 */
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

/**
 * @brief Assign queued requests to idle servers.
 *
 * Iterates over the server pool and assigns the oldest queued requests to any
 * servers that are not currently busy.
 */
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

/**
 * @brief Process one simulation tick on all servers.
 *
 * Each busy server processes one cycle of work. This method also tracks the
 * number of servers that were busy this cycle and how many requests completed.
 */
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

/**
 * @brief Simulate a single clock cycle.
 *
 * The cycle performs the following actions in order:
 * 1) Potentially generate a new request
 * 2) Assign requests to idle servers
 * 3) Process active requests for one tick
 * 4) Check whether scaling actions should occur
 */
void LoadBalancer::simulateCycle()
{
    newRequest();
    assignRequests();
    process();
    checkScale();
    clock++;
}

// ======================== PUBLIC METHODS ========================

/**
 * @brief Potentially generate and enqueue a new request for this cycle.
 *
 * Uses a probability (newReqFreq) to decide whether a new request arrives.
 * If one arrives, it is generated and passed through the firewall before being
 * added to the request queue.
 */
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

/**
 * @brief Log a summary of simulation statistics.
 *
 * Prints totals for generated/processed/blocked requests and scaling actions,
 * as well as final queue size and active server count.
 */
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

/**
 * @brief Log initial simulation parameters and starting state.
 */
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

/**
 * @brief Enqueue a request if it is not blocked by the firewall.
 *
 * Requests with a blocked source IP are discarded and counted as blocked.
 * Accepted requests are appended to the internal FIFO queue.
 *
 * @param req Request to enqueue (moved into the queue on success).
 */
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

/**
 * @brief Run the simulation for a fixed number of cycles.
 *
 * @param totalCycles Number of clock cycles to simulate.
 */
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
