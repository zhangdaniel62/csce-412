/**
 * @file Request.h
 * @brief Declaration of the Request data structure used in the simulation.
 *
 * A Request represents a single simulated web job handled by the load
 * balancer. Each request tracks its source and destination IP addresses,
 * job type, and remaining processing time (measured in clock cycles).
 */
#pragma once

#include <string>

/**
 * @class Request
 * @brief Represents a single web request in the load balancer simulation.
 *
 * A Request encapsulates all information required to simulate processing,
 * including job type ('P' for processing, 'S' for streaming), remaining
 * time, and IP address information.
 */
class Request
{
public:
    /**
     * @brief Default constructor.
     *
     * Initializes an empty request with zero remaining time.
     */
    Request();
    /**
     * @brief Construct a fully initialized Request.
     *
     * @param time Initial processing time (in cycles).
     * @param type Job type ('P' for processing or 'S' for streaming).
     * @param ipIn Source IPv4 address.
     * @param ipOut Destination IPv4 address.
     */
    Request(int time, char type, std::string ipIn, std::string ipOut);
    /**
     * @brief Check whether the request has completed processing.
     *
     * @return true If timeRemaining has reached zero.
     * @return false Otherwise.
     */
    bool isComplete();
    /**
     * @brief Decrease the remaining processing time.
     *
     * @param amt Number of cycles to subtract from timeRemaining.
     */
    void decrementTime(int amt);
    /**
     * @brief Get the source IP address.
     *
     * @return std::string Source IPv4 address.
     */
    std::string getIpIn();
    /**
     * @brief Get the destination IP address.
     *
     * @return std::string Destination IPv4 address.
     */
    std::string getIpOut();
    /**
     * @brief Get the remaining processing time.
     *
     * @return int Number of cycles remaining before completion.
     */
    int getTimeRemaining();
    /**
     * @brief Get the job type.
     *
     * @return char 'P' for processing or 'S' for streaming.
     */
    char getJobType();
    /**
     * @brief Set the source IP address.
     *
     * @param ip IPv4 address string.
     */
    void setIpIn(std::string ip);
    /**
     * @brief Set the destination IP address.
     *
     * @param ip IPv4 address string.
     */
    void setIpOut(std::string ip);
    /**
     * @brief Set the job type.
     *
     * @param type Character representing the job type.
     */
    void setJobType(char type);

private:
    /** Source IPv4 address of the request. */
    std::string ipIn;
    /** Destination IPv4 address of the request. */
    std::string ipOut;
    /** Remaining processing time in clock cycles. */
    int timeRemaining;
    /** Job type identifier ('P' for processing, 'S' for streaming). */
    char jobType;
};