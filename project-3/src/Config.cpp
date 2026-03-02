/**
 * @file Config.cpp
 * @brief YAML configuration loader for the load balancer simulation.
 *
 * This file implements `loadConfig()`, which reads a YAML configuration file
 * (via yaml-cpp) and populates a `Config` struct used by the simulation.
 *
 * Expected YAML structure (example):
 * @code{.yaml}
 * simulation:
 *   totalCycles: 10000
 *   initialServers: 10
 *   cooldown: 50
 *   newReqFreq: 10
 * requestTimes:
 *   processing:
 *     min: 2
 *     max: 20
 *   streaming:
 *     min: 5
 *     max: 40
 * firewall:
 *   blockedRanges:
 *     - start: "192.168.0.0"
 *       end:   "192.168.255.255"
 * @endcode
 */
#include "Config.h"
#include <yaml-cpp/yaml.h>
#include <stdexcept>

/**
 * @brief Load simulation settings from a YAML configuration file.
 *
 * This function parses optional YAML sections and applies basic validation.
 * Any missing keys simply keep their default values as defined in `Config`.
 *
 * @param path Path to the YAML configuration file.
 * @return Config Populated configuration struct.
 *
 * @throws YAML::BadFile If the file cannot be opened/read.
 * @throws YAML::ParserException If the YAML is malformed.
 * @throws std::invalid_argument If basic validation fails.
 */
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