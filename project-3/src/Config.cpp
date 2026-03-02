#include "Config.h"
#include <yaml-cpp/yaml.h>
#include <stdexcept>

Config loadConfig(const std::string &path)
{
    YAML::Node root = YAML::LoadFile(path);
    Config cfg;

    if (auto sim = root["simulation"])
    {
        if (sim["totalCycles"])
            cfg.totalCycles = sim["totalCycles"].as<int>();
        if (sim["initialServers"])
            cfg.initialServers = sim["initialServers"].as<int>();
        if (sim["cooldown"])
            cfg.cooldown = sim["cooldown"].as<int>();
        if (sim["newReqFreq"])
            cfg.newReqFreq = sim["newReqFreq"].as<int>();
    }

    if (auto rt = root["requestTimes"])
    {
        if (auto p = rt["processing"])
        {
            if (p["min"])
                cfg.minTimeProcessing = p["min"].as<unsigned int>();
            if (p["max"])
                cfg.maxTimeProcessing = p["max"].as<unsigned int>();
        }
        if (auto s = rt["streaming"])
        {
            if (s["min"])
                cfg.minTimeStreaming = s["min"].as<unsigned int>();
            if (s["max"])
                cfg.maxTimeStreaming = s["max"].as<unsigned int>();
        }
    }

    // Firewall ranges (optional)
    if (root["firewall"] && root["firewall"]["blockedRanges"])
    {
        for (const auto &range : root["firewall"]["blockedRanges"])
        {
            std::string start = range["start"].as<std::string>();
            std::string end = range["end"].as<std::string>();

            cfg.blockedRanges.emplace_back(end, start);
        }
    }

    // basic validation
    if (cfg.initialServers < 1)
        throw std::invalid_argument("initialServers must be >= 1");
    if (cfg.newReqFreq < 0 || cfg.newReqFreq > 100)
        throw std::invalid_argument("newReqFreq must be 0..100");
    if (cfg.minTimeProcessing < 1 || cfg.maxTimeProcessing < 1 ||
        cfg.minTimeStreaming < 1 || cfg.maxTimeStreaming < 1)
        throw std::invalid_argument("min/max times must be >= 1");
    if (cfg.minTimeProcessing > cfg.maxTimeProcessing)
        throw std::invalid_argument("processing min > max");
    if (cfg.minTimeStreaming > cfg.maxTimeStreaming)
        throw std::invalid_argument("streaming min > max");

    return cfg;
}