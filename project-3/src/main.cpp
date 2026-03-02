#include "LoadBalancer.h"
#include "Config.h"
#include "IPBlocker.h"

#include <iostream>

Config loadConfig(const std::string &path);
int main(int argc, char *argv[])
{
    std::string path = (argc >= 2) ? argv[1] : "config.yaml";

    Config cfg = loadConfig(path);

    IPBlocker blocker;
    blocker.addIP(cfg.blockedRanges);

    LoadBalancer lb(cfg.initialServers,
                    cfg.cooldown,
                    blocker,
                    cfg.newReqFreq,
                    cfg.minTimeProcessing,
                    cfg.maxTimeProcessing,
                    cfg.minTimeStreaming,
                    cfg.maxTimeStreaming);

    lb.run(cfg.totalCycles);
}