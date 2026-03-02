/**
 * @file Request.cpp
 * @brief Implementation of the Request data structure.
 *
 * The Request class represents a single simulated web request handled by
 * the load balancer. Each request tracks its remaining processing time,
 * job type (processing or streaming), and source/destination IP addresses.
 */
#include "Request.h"
#include <string>

using std::string;

/**
 * @brief Determine whether the request has finished processing.
 *
 * A request is considered complete when its remaining time reaches zero
 * and its identifying fields are initialized.
 *
 * @return true If the request has completed.
 * @return false Otherwise.
 */
bool Request::isComplete()
{
    return timeRemaining == 0 && ipIn != "" && ipOut != "" && jobType != '\0';
}

/**
 * @brief Decrease the remaining processing time.
 *
 * @param amt Number of clock cycles to subtract from timeRemaining.
 */
void Request::decrementTime(int amt)
{
    timeRemaining -= amt;
}

/**
 * @brief Get the remaining processing time.
 *
 * @return int Number of cycles remaining before completion.
 */
int Request::getTimeRemaining()
{
    return timeRemaining;
}

/**
 * @brief Get the job type of the request.
 *
 * @return char 'P' for processing or 'S' for streaming.
 */
char Request::getJobType()
{
    return jobType;
}

/**
 * @brief Set the job type of the request.
 *
 * @param type Character representing the job type ('P' or 'S').
 */
void Request::setJobType(char type)
{
    jobType = type;
}

/**
 * @brief Set the source IP address of the request.
 *
 * @param ip IPv4 address string in dotted-decimal format.
 */
void Request::setIpIn(std::string ip)
{
    ipIn = ip;
}

/**
 * @brief Set the destination IP address of the request.
 *
 * @param ip IPv4 address string in dotted-decimal format.
 */
void Request::setIpOut(std::string ip)
{
    ipOut = ip;
}

/**
 * @brief Get the source IP address.
 *
 * @return std::string Source IPv4 address.
 */
std::string Request::getIpIn()
{
    return ipIn;
}

/**
 * @brief Get the destination IP address.
 *
 * @return std::string Destination IPv4 address.
 */
std::string Request::getIpOut()
{
    return ipOut;
}

/**
 * @brief Default constructor.
 *
 * Initializes a request with zero remaining time and empty fields.
 */
Request::Request()
{
    timeRemaining = 0;
    jobType = '\0';
    ipIn = "";
    ipOut = "";
}

/**
 * @brief Construct a fully initialized Request.
 *
 * @param time Initial processing time (in cycles).
 * @param type Job type ('P' for processing, 'S' for streaming).
 * @param ipIn Source IPv4 address.
 * @param ipOut Destination IPv4 address.
 */
Request::Request(int time, char type, string ipIn, string ipOut) : timeRemaining(time),
                                                                   jobType(type),
                                                                   ipIn(ipIn),
                                                                   ipOut(ipOut)
{
}