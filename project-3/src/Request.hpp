#pragma once

#include <string>

class Request
{
public:
    Request();
    Request(int time, char type, std::string ipIn, std::string ipOut);
    bool isComplete();
    void decrementTime(int amt);
    std::string getIpIn();
    std::string getIpOut();
    int getTimeRemaining();
    char getJobType();
    void setIpIn(std::string ip);
    void setIpOut(std::string ip);
    void setJobType(char type);

private:
    std::string ipIn;
    std::string ipOut;
    int timeRemaining;
    char jobType;
};