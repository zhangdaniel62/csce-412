#pragma once

#include <queue>
#include <vector>

#include "Request.hpp"
#include "WebServer.hpp"
#include "IPBlocker.hpp"

using std::queue;
using std::vector;

class LoadBalancer {
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

        void simulateCycle();
        void generateRequest();
        void assignRequests();
        void checkScale();
        bool underMinthreshold();
        bool aboveMaxthreshold();
        void addServer();
        void removeServer();
};