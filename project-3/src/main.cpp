#include "LoadBalancer.h"
#include "Config.h"
#include "IPBlocker.h"

#include <iostream>

// Logging through spdlog
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <filesystem>
#include <memory>
#include <vector>

void init_logging()
{
    // Prevent double initialization
    if (spdlog::get("lb")) return;

    // Ensure logs/ directory exists
    std::error_code ec;
    std::filesystem::create_directories("logs", ec);

    // Create sinks
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
        "logs/loadbalancer.log", true); // true = append mode

    std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};

    auto logger = std::make_shared<spdlog::logger>("lb", sinks.begin(), sinks.end());

    // Format
    logger->set_pattern("[%^%l%$] %v");

    // Default level (change to debug if needed)
    logger->set_level(spdlog::level::info);

    // Auto-flush important messages
    logger->flush_on(spdlog::level::info);

    spdlog::register_logger(logger);
}

Config loadConfig(const std::string &path);
int main(int argc, char *argv[])
{
    init_logging();

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
    
    lb.printInitialStats();
    spdlog::get("lb")->info("=====================================");
    spdlog::get("lb")->info("Starting load balancer...");
    lb.run(cfg.totalCycles);
    spdlog::get("lb")->info("Load balancer finished.");
    spdlog::get("lb")->info("=====================================");
    lb.printFinalStats();

    return 0;
}