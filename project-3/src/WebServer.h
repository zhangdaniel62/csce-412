/**
 * @file WebServer.h
 * @brief Declaration of the WebServer class used in the load balancer simulation.
 *
 * A WebServer represents a single simulated server instance that processes
 * one Request at a time. The LoadBalancer manages a collection of
 * WebServer objects and assigns work to them as requests arrive.
 */
#pragma once

#include "Request.h"

/**
 * @class WebServer
 * @brief Simulates a single server that processes one request at a time.
 *
 * Each WebServer maintains a busy/idle state and advances its assigned
 * request in discrete clock cycles via processCycle().
 */
class WebServer
{
public:
    /**
     * @brief Construct a WebServer with a unique identifier.
     *
     * @param id Unique ID assigned by the LoadBalancer.
     */
    WebServer(int id);
    /**
     * @brief Assign a request to this server.
     *
     * The server becomes busy and begins processing the request
     * on subsequent calls to processCycle().
     *
     * @param req Request to process (copied into the server).
     */
    void assign(Request req);
    /**
     * @brief Advance processing by one clock cycle.
     *
     * If the server is busy, the request's remaining time is decremented.
     * If the request completes, the server becomes idle.
     *
     * @return true If the server was busy this cycle.
     * @return false If the server was idle.
     */
    bool processCycle();
    /**
     * @brief Check whether the server is currently processing a request.
     *
     * @return true If the server is busy.
     * @return false Otherwise.
     */
    bool isBusy();

private:
    /** Unique identifier for this server instance. */
    int id;
    /** Currently assigned request (valid only if busy is true). */
    Request req;
    /** Indicates whether the server is actively processing a request. */
    bool busy;
};