#pragma once

#include <queue>
#include <vector>
#include <random>

#include "Request.h"
#include "WebServer.h"
#include "IPBlocker.h"

using std::queue;
using std::vector;

class LoadBalancer
{
public:
    LoadBalancer(int numServers,
                 int cooldown,
                 IPBlocker blocker,
                 int newReqFreq,
                 unsigned int minTimeProcessing,
                 unsigned int maxTimeProcessing,
                 unsigned int minTimeStreaming,
                 unsigned int maxTimeStreaming);
    void run(int totalCycles);
    void addRequest(Request req);

private:
    queue<Request> requestQueue;
    vector<WebServer> servers;
    int clock = 1;
    int cooldown;
    int lastCheckedClock;
    int newReqFreq;
    int nextServerId;
    int minTimeProcessing;
    int maxTimeProcessing;
    int minTimeStreaming;
    int maxTimeStreaming;
    IPBlocker blocker;
    std::mt19937 rng;

    // Stats for output
    int totalRequests = 0;
    int totalRequestsProcessed = 0;
    int totalRequestsBlocked = 0;
    int totalServersAdded = 0;
    int totalServersRemoved = 0;

    void simulateCycle();
    Request generateRequest();
    void assignRequests();
    void process();
    void checkScale();
    bool underMinThreshold();
    bool aboveMaxThreshold();
    void addServer(int amt);
    void removeServer(int amt);
    void printFinalStats();
};