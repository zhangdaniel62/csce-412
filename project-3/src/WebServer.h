#pragma once

#include "Request.h"

class WebServer
{
public:
    WebServer(int id);
    void assign(Request req);
    bool processCycle();
    bool isBusy();

private:
    int id;
    Request req;
    bool busy;
};