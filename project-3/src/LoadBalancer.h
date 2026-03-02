/**
 * @file LoadBalancer.h
 * @brief Public interface for the LoadBalancer simulation controller.
 *
 * The LoadBalancer class coordinates a queue of incoming Requests, assigns
 * them to a pool of WebServer instances, advances the simulation in discrete
 * clock cycles, and dynamically scales the server pool based on queue size.
 *
 * Scaling policy (per assignment specification):
 * - If queue size < 50 * servers, remove 1 server (subject to cooldown).
 * - If queue size > 80 * servers, add 1 server (subject to cooldown).
 */
#pragma once

#include <queue>
#include <vector>
#include <random>

#include "Request.h"
#include "WebServer.h"
#include "IPBlocker.h"

using std::queue;
using std::vector;

/**
 * @class LoadBalancer
 * @brief Manages request distribution, server processing, scaling, and statistics.
 *
 * The load balancer maintains a FIFO queue of incoming requests, assigns them
 * to available servers, and processes requests over time. A cooldown timer
 * prevents scaling decisions from happening too frequently.
 */
class LoadBalancer
{
public:
    /**
     * @brief Construct a LoadBalancer and initialize the simulation state.
     *
     * Creates an initial pool of servers and fills the starting request queue
     * (typically servers * 100 requests).
     *
     * @param numServers Initial number of WebServer instances to create.
     * @param cooldown Number of cycles to wait between scaling checks.
     * @param blocker Firewall component used to block request source IP ranges.
     * @param newReqFreq Probability (0-100) that a new request arrives each cycle.
     * @param minTimeProcessing Minimum duration for processing requests (cycles).
     * @param maxTimeProcessing Maximum duration for processing requests (cycles).
     * @param minTimeStreaming Minimum duration for streaming requests (cycles).
     * @param maxTimeStreaming Maximum duration for streaming requests (cycles).
     *
     * @throws std::invalid_argument If any request timing range is invalid.
     */
    LoadBalancer(int numServers,
                 int cooldown,
                 IPBlocker blocker,
                 int newReqFreq,
                 unsigned int minTimeProcessing,
                 unsigned int maxTimeProcessing,
                 unsigned int minTimeStreaming,
                 unsigned int maxTimeStreaming);
    /**
     * @brief Run the simulation for a fixed number of clock cycles.
     *
     * @param totalCycles Number of cycles to simulate.
     */
    void run(int totalCycles);
    /**
     * @brief Add a request to the internal queue if not blocked.
     *
     * The request's source IP is checked by the IPBlocker. If blocked, the
     * request is discarded and counted as blocked; otherwise it is enqueued.
     *
     * @param req Request to enqueue (moved into the queue on success).
     */
    void addRequest(Request req);
    /**
     * @brief Print/log a summary of final simulation statistics.
     */
    void printFinalStats();
    /**
     * @brief Print/log the initial simulation parameters and starting state.
     */
    void printInitialStats();
    /**
     * @brief Potentially generate and enqueue a new request for the current cycle.
     *
     * Uses the configured new-request frequency (newReqFreq) to decide whether
     * a new request arrives in this cycle.
     */
    void newRequest();

private:
    /** FIFO queue of requests waiting to be assigned to servers. */
    queue<Request> requestQueue;
    /** Active pool of servers managed by the load balancer. */
    vector<WebServer> servers;
    /** Current simulation clock cycle (1-indexed). */
    int clock = 1;
    /** Number of cycles required between scaling checks. */
    int cooldown;
    /** Clock value when scaling was last evaluated. */
    int lastCheckedClock;
    /** Probability (0-100) that a new request arrives each cycle. */
    int newReqFreq;
    /** Monotonically increasing ID generator for servers. */
    int nextServerId;
    /** Minimum request duration (cycles) for processing jobs. */
    int minTimeProcessing;
    /** Maximum request duration (cycles) for processing jobs. */
    int maxTimeProcessing;
    /** Minimum request duration (cycles) for streaming jobs. */
    int minTimeStreaming;
    /** Maximum request duration (cycles) for streaming jobs. */
    int maxTimeStreaming;
    /** Firewall component used to reject requests from blocked IP ranges. */
    IPBlocker blocker;
    /** Random number generator used for request/IP/time generation. */
    std::mt19937 rng;

    /**
     * @name Statistics
     * @brief Counters used for end-of-run reporting and logging.
     * @{ */
    /** Total number of requests generated/seen by the load balancer. */
    int totalRequests = 0;
    /** Total number of requests that completed processing. */
    int totalRequestsProcessed = 0;
    /** Total number of requests rejected by the firewall. */
    int totalRequestsBlocked = 0;
    /** Total number of servers added over the run (including initial servers if counted). */
    int totalServersAdded = 0;
    /** Total number of servers removed over the run. */
    int totalServersRemoved = 0;
    /** @} */

    /** Simulate a single clock cycle of the load balancer. */
    void simulateCycle();
    /** Generate a random Request instance for simulation. */
    Request generateRequest();
    /** Assign queued requests to idle servers. */
    void assignRequests();
    /** Advance processing on all busy servers by one cycle and track completions. */
    void process();
    /** Check scaling thresholds and add/remove servers if allowed by cooldown. */
    void checkScale();
    /** @return true if queue size is below 50 * servers. */
    bool underMinThreshold();
    /** @return true if queue size is above 80 * servers. */
    bool aboveMaxThreshold();
    /** Add one or more servers to increase capacity. */
    void addServer(int amt);
    /** Remove up to amt idle servers, leaving at least one server active. */
    void removeServer(int amt);
};