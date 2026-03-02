#include "LoadBalancer.h"
#include <utility>
#include <random>
#include <string>
#include <stdexcept>

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
    // Edge cases for minTime and maxTime for processing and streaming
    if (minTimeProcessing > maxTimeProcessing)
    {
        // TODO: add logging
        throw std::invalid_argument("minTimeProcessing must be less than maxTimeProcessing");
    }
    else if (minTimeStreaming > maxTimeStreaming)
    {
        // TODO: add logging
        throw std::invalid_argument("minTimeStreaming must be less than maxTimeStreaming");
    }
    else if (minTimeStreaming < 1 || maxTimeStreaming < 1 || minTimeProcessing < 1 || maxTimeProcessing < 1)
    {
        // TODO: add logging
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

    totalRequests = requestQueue.size();
    totalServersAdded = servers.size();
    // TODO: add logging
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
    for (int i = 0; i < amt; i++)
    {
        servers.emplace_back(nextServerId++);
        totalServersAdded++;
    }
}

void LoadBalancer::removeServer(int amt)
{
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
                // TODO: add logging
                break;
            }
        }
        else
        {
            i++;
        }
    }

    totalServersRemoved += removed;
}

void LoadBalancer::checkScale()
{
    if (clock - lastCheckedClock >= cooldown)
    {
        if (underMinThreshold())
        {
            // TODO: add logging
            removeServer(1);
        }
        else if (aboveMaxThreshold())
        {
            // TODO: add logging
            addServer(1);
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
    // TODO: Add logging
    return req;
}

void LoadBalancer::assignRequests()
{
    unsigned int counter = 0;
    for (auto &server : servers)
    {
        if (requestQueue.empty())
        {
            // TODO: Add logging
            break;
        }
        else if (!server.isBusy())
        {
            counter++;
            server.assign(requestQueue.front());
            requestQueue.pop();
        }
    }
    // Add logging for total number of requests assigned
}

void LoadBalancer::process()
{
    unsigned int busyThisCycle = 0;
    unsigned int completedThisCycle = 0;

    for (auto &server : servers)
    {
        bool wasBusy = server.isBusy();
        if (wasBusy)
        {
            busyThisCycle++;
            server.processCycle();          // returns true every tick of work, but we don't need it
            if (!server.isBusy()) completedThisCycle++;
        }
    }

    totalRequestsProcessed += completedThisCycle;
    
    // TODO: Add logging
}

void LoadBalancer::simulateCycle()
{
    std::uniform_int_distribution<int> newReqDist(1, 100);
    bool generateNewReq = (newReqDist(rng) <= newReqFreq);
    if (generateNewReq)
    {
        // TODO: Add logging
        addRequest(generateRequest());
    }

    assignRequests();
    process();
    checkScale();
    clock++;
}

void LoadBalancer::printFinalStats() {
    // TODO: add logging
}

// ======================== PUBLIC METHODS ========================
void LoadBalancer::addRequest(Request req)
{
    // Check to see if IP is in correct range
    if (blocker.isBlocked(req.getIpIn()))
    {
        totalRequestsBlocked++;
        // TODO: Add logging
        return;
    }
    requestQueue.push(std::move(req));
    totalRequests++;
}

void LoadBalancer::run(int totalCycles)
{
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

    printFinalStats();
}
