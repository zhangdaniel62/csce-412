#include "WebServer.h"

WebServer::WebServer(int id) : id(id), busy(false), req() {}

void WebServer::assign(Request req)
{
    this->req = req;
    busy = true;
}

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

bool WebServer::isBusy()
{
    return busy;
}