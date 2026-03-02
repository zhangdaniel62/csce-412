#include "Request.h"
#include <string>

using std::string;

bool Request::isComplete()
{
    return timeRemaining == 0 && ipIn != "" && ipOut != "" && jobType != '\0';
}

void Request::decrementTime(int amt)
{
    timeRemaining -= amt;
}

int Request::getTimeRemaining()
{
    return timeRemaining;
}

char Request::getJobType()
{
    return jobType;
}

void Request::setJobType(char type)
{
    jobType = type;
}

void Request::setIpIn(std::string ip)
{
    ipIn = ip;
}

void Request::setIpOut(std::string ip)
{
    ipOut = ip;
}

std::string Request::getIpIn()
{
    return ipIn;
}

std::string Request::getIpOut()
{
    return ipOut;
}

Request::Request()
{
    timeRemaining = 0;
    jobType = '\0';
    ipIn = "";
    ipOut = "";
}

Request::Request(int time, char type, string ipIn, string ipOut) : timeRemaining(time),
                                                                   jobType(type),
                                                                   ipIn(ipIn),
                                                                   ipOut(ipOut)
{
}