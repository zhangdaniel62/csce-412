#pragma once
#include <string>
#include <vector>

struct Config {
    // simulation
    int totalCycles = 10000;
    int initialServers = 10;
    int cooldown = 50;
    int newReqFreq = 50; // 1..100

    // request times
    unsigned int minTimeProcessing = 20;
    unsigned int maxTimeProcessing = 50;
    unsigned int minTimeStreaming = 1;
    unsigned int maxTimeStreaming = 20;

    // firewall
    std::vector<std::pair<std::string, std::string>> blockedRanges;
};