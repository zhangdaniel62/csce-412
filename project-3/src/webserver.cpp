#include "WebServer.hpp"

WebServer::WebServer(int id) : id(id), busy(false), req() {}

void WebServer::assign(Request req)
{
    this->req = req;
    busy = true;
}

void WebServer::processCycle()
{
    if (busy)
    {
        req.decrementTime(1);

        if (req.isComplete())
        {
            busy = false;
        }
    }
}

bool WebServer::isBusy()
{
    return busy;
}