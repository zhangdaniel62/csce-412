/**
 * @file main.cpp
 * @brief Program entry point for the load balancer simulation.
 *
 * This file initializes logging (via spdlog), loads a YAML configuration,
 * constructs the firewall (IPBlocker) and LoadBalancer, and runs the
 * simulation for the configured number of clock cycles.
 *
 * Logging:
 * A shared spdlog logger named "lb" is configured with both a colored
 * console sink and a file sink (logs/loadbalancer.txt).
 */
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

/**
 * @brief Initialize the shared simulation logger.
 *
 * Creates a spdlog logger named "lb" with two sinks:
 *  - A colored console sink for terminal output.
 *  - A file sink that appends to logs/loadbalancer.txt.
 *
 * If the logger already exists, this function does nothing.
 */
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
        "logs/loadbalancer.txt", true); // true = append mode

    std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};

    auto logger = std::make_shared<spdlog::logger>("lb", sinks.begin(), sinks.end());

    // Format
    logger->set_pattern("[%^%l%$] %v");

    // Default level (change to debug if needed)
    logger->set_level(spdlog::level::debug);

    // Auto-flush important messages
    logger->flush_on(spdlog::level::debug);

    spdlog::register_logger(logger);
}

Config loadConfig(const std::string &path);
/**
 * @brief Program entry point.
 *
 * Usage:
 * @code{.sh}
 * ./lb config.yaml
 * @endcode
 * If no argument is provided, the program defaults to "config.yaml".
 *
 * @param argc Number of command-line arguments.
 * @param argv Argument vector. argv[1] may specify the config file path.
 * @return int Exit status code (0 on success).
 */
int main(int argc, char *argv[])
{
    init_logging();

    auto logger = spdlog::get("lb");
    logger->set_level(spdlog::level::off);

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
    
    logger->set_level(spdlog::level::debug);
    lb.printInitialStats();
    spdlog::get("lb")->info("=====================================");
    spdlog::get("lb")->info("Starting load balancer...");
    lb.run(cfg.totalCycles);
    spdlog::get("lb")->info("Load balancer finished.");
    spdlog::get("lb")->info("=====================================");
    lb.printFinalStats();

    return 0;
}