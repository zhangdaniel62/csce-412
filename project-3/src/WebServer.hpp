#pragma once

#include "Request.hpp"

class WebServer
{
public:
    WebServer(int id);
    void assign(Request req);
    void processCycle();
    bool isBusy();

private:
    int id;
    Request req;
    bool busy;
};