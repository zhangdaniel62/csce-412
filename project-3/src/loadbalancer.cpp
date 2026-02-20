#include "LoadBalancer.hpp"
#include <utility>
#include <random>
#include <string>

using std::string;
using std::to_string;

LoadBalancer::LoadBalancer(int numServers, int cooldown) : numServers(numServers),
                                                           cooldown(cooldown),
                                                           clock(0),
                                                           lastCheckedClock(0),
                                                           blocker()
{
}

std::mt19937 LoadBalancer::rng() {
    return std::mt19937(std::random_device{}());
}

bool LoadBalancer::underMinthreshold()
{
    unsigned int limit = 50 * numServers;
    return requestQueue.size() < limit;
}

bool LoadBalancer::aboveMaxthreshold() {
    unsigned int limit = 80 * numServers;
    return requestQueue.size() > limit;
}

void LoadBalancer::addServer(int amt) {
    for (int i = 0; i < amt; i++) {
        servers.emplace_back(WebServer(servers.size()));
    }
}

void LoadBalancer::removeServer(int amt) {
    int removed = 0;
    for (size_t i = 0; i < servers.size() && removed < amt;) {
        if (!servers[i].isBusy()) {
            servers[i] = std::move(servers.back());
            servers.pop_back();
            removed++;
        }
        else{
            i++;
        }
    }
}

void LoadBalancer::checkScale() {
    if (clock - lastCheckedClock >= cooldown) {
        if (underMinThreshold()) {
            addServer(1);
        }
        else if (aboveMaxThreshold()) {
            removeServer(1);
        }
        lastCheckedClock = clock;
    }
}

void LoadBalancer::generateRequest() {

    // Create random functions to generate random requests
    std::uniform_int_distribution<int> ipDist(0,255);
    std::uniform_int_distribution<int> typeDist(0,1);
    std::uniform_int_distribution<int> timeDist(1, 20);

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

    int time = timeDist(rng);

    // Creating ip string for IP in and IP out
    string ipIn = to_string(aIn) + "." + to_string(bIn) + "." + to_string(cIn) + "." + to_string(dIn);
    string ipOut = to_string(aOut) + "." + to_string(bOut) + "." + to_string(cOut) + "." + to_string(dOut);

    Request req(time, type, ipIn, ipOut)

    requestQueue.push(req);
}

void LoadBalancer::assignRequests() {
    for (auto& server : servers) {
        if (requestQueue.empty()) {
            break;
        }
        else if (!server.isBusy()) {
            server.assign(requestQueue.front());
            requestQueue.pop();
        }
    }
}

void LoadBalancer::process() {
    for (auto& server : servers) {
        server.processCycle();
    }
}

void LoadBalancer::simulateCycle() {
    clock++;

    std::uniform_int_distribution<int> newReqDist(1,100);
    bool generateNewReq = (newReqDist(rng) <= 25);
    if (generateNewReq) {
        generateRequest();
    }
    
    assignRequests();

    checkScale();

}
