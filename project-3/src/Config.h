/**
 * @file Config.h
 * @brief Configuration data structure for the load balancer simulation.
 *
 * This header defines the `Config` struct, which stores all simulation
 * parameters loaded from a YAML configuration file. These values control
 * runtime behavior such as scaling thresholds, request generation frequency,
 * and firewall rules.
 */
#pragma once
#include <string>
#include <vector>

/**
 * @struct Config
 * @brief Holds configuration parameters for the load balancer simulation.
 *
 * The Config struct contains all tunable parameters used by the simulation.
 * Default values are provided so the simulation can run even if certain
 * YAML keys are missing.
 */
struct Config {
    /**
     * @name Simulation Settings
     * @brief High-level runtime controls for the simulation.
     * @{ */

    /** Total number of clock cycles the simulation will run. */
    int totalCycles = 10000;

    /** Initial number of WebServer instances created at startup. */
    int initialServers = 10;

    /**
     * Number of clock cycles that must pass before another scaling
     * decision (add/remove server) is allowed.
     */
    int cooldown = 50;

    /**
     * Probability (1–100) that a new request will be generated
     * during a given clock cycle.
     */
    int newReqFreq = 50; // 1..100

    /** @} */


    /**
     * @name Request Processing Times
     * @brief Duration bounds for simulated request workloads.
     * @{ */

    /** Minimum processing time (in cycles) for processing jobs. */
    unsigned int minTimeProcessing = 20;

    /** Maximum processing time (in cycles) for processing jobs. */
    unsigned int maxTimeProcessing = 50;

    /** Minimum processing time (in cycles) for streaming jobs. */
    unsigned int minTimeStreaming = 1;

    /** Maximum processing time (in cycles) for streaming jobs. */
    unsigned int maxTimeStreaming = 20;

    /** @} */


    /**
     * @name Firewall Settings
     * @brief IP address ranges blocked from entering the system.
     * @{ */

    /**
     * List of blocked IP ranges.
     * Each pair represents a start IP and end IP (inclusive).
     * Requests whose source IP falls within any range are rejected.
     */
    std::vector<std::pair<std::string, std::string>> blockedRanges;

    /** @} */
};