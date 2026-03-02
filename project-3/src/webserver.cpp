/**
 * @file WebServer.cpp
 * @brief Implementation of the WebServer component.
 *
 * The WebServer class represents a single simulated server instance
 * managed by the LoadBalancer. Each server processes one Request at a time
 * and advances its work in discrete clock cycles.
 */
#include "WebServer.h"

/**
 * @brief Construct a WebServer with a unique identifier.
 *
 * Initializes the server as idle with no active request.
 *
 * @param id Unique identifier assigned by the LoadBalancer.
 */
WebServer::WebServer(int id) : id(id), busy(false), req() {}

/**
 * @brief Assign a new request to this server.
 *
 * The server becomes busy and begins processing the provided request
 * on subsequent calls to processCycle().
 *
 * @param req Request to process (copied into the server).
 */
void WebServer::assign(Request req)
{
    this->req = req;
    busy = true;
}

/**
 * @brief Process one clock cycle of work.
 *
 * If the server is busy, the remaining time of the active request
 * is decremented by one. If the request completes, the server
 * transitions back to the idle state.
 *
 * @return true If the server was processing work this cycle.
 * @return false If the server was idle.
 */
bool WebServer::processCycle()
{
    if (busy)
    {
        req.decrementTime(1);

        if (req.isComplete())
        {
            busy = false;
            // TODO: add logging
        }
        return true;
    }
    return false;
}

/**
 * @brief Check whether the server is currently processing a request.
 *
 * @return true If the server is busy.
 * @return false If the server is idle.
 */
bool WebServer::isBusy()
{
    return busy;
}