#pragma once

#include <queue>
#include <vector>
#include <random>

#include "Request.hpp"
#include "WebServer.hpp"
#include "IPBlocker.hpp"

using std::queue;
using std::vector;

class LoadBalancer
{
public:
    LoadBalancer(int numServers, int cooldown);
    void run(int totalCycles);
    void addRequest(Request req);

private:
    queue<Request> requestQueue;
    vector<WebServer> servers;
    int clock;
    int cooldown;
    int lastCheckedClock;
    IPBlocker blocker;


    std::mt19937 rng();
    void simulateCycle();
    void generateRequest();
    void assignRequests();
    void process();
    void checkScale();
    bool underMinthreshold();
    bool aboveMaxthreshold();
    void addServer(int amt);
    void removeServer(int amt);
};