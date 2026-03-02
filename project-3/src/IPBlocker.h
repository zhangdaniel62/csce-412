/**
 * @file IPBlocker.h
 * @brief Firewall-style IP range blocker used by the load balancer simulation.
 *
 * The IPBlocker class maintains a list of blocked IPv4 address ranges.
 * Incoming request source IPs can be checked against this list to simulate
 * basic DOS prevention / firewall behavior.
 */
#pragma once

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

/**
 * @class IPBlocker
 * @brief Simulates a firewall by blocking configurable IPv4 address ranges.
 *
 * Ranges are stored internally as 32-bit integers for correct ordering and
 * efficient numeric comparison. Each stored range is inclusive: [start, end].
 */
class IPBlocker
{
public:
    /**
     * @brief Construct an empty IPBlocker.
     */
    IPBlocker();
    /**
     * @brief Destroy the IPBlocker.
     */
    ~IPBlocker();
    /**
     * @brief Determine whether a given IPv4 address is blocked.
     *
     * @param ip IPv4 address in dotted-decimal form (e.g., "192.168.0.1").
     * @return true If the IP is within any blocked range.
     * @return false Otherwise.
     */
    bool isBlocked(const std::string &ip) const;
    /**
     * @brief Add a blocked IPv4 range.
     *
     * The input pair represents (start, end) in dotted-decimal form.
     * If start > end, the bounds are swapped internally.
     *
     * @param ip Pair of strings representing the start and end IPv4 addresses.
     */
    void addIP(const std::pair<std::string, std::string> &ip);
    /**
     * @brief Add multiple blocked IPv4 ranges.
     *
     * @param ips Vector of (start, end) IPv4 address pairs to block.
     */
    void addIP(const std::vector<std::pair<std::string, std::string>> &ips);
    /**
     * @brief Remove a blocked IPv4 range.
     *
     * The specified range is removed if an exact matching range exists.
     *
     * @param ip Pair of strings representing the start and end IPv4 addresses.
     */
    void removeIP(const std::pair<std::string, std::string> &ip);
    /**
     * @brief Remove multiple blocked IPv4 ranges.
     *
     * @param ips Vector of (start, end) IPv4 address pairs to remove.
     */
    void removeIP(const std::vector<std::pair<std::string, std::string>> &ips);

private:
    /**
     * @brief Blocked IPv4 ranges stored as numeric values.
     *
     * Each pair represents (start, end) inclusive.
     */
    std::vector<std::pair<std::uint32_t, std::uint32_t>> blockedRanges;

    /**
     * @brief Convert a dotted-decimal IPv4 string to a 32-bit integer.
     *
     * @param ip IPv4 string in dotted-decimal form.
     * @return std::uint32_t Numeric representation of the IPv4 address.
     *
     * @throws std::invalid_argument If the input is malformed.
     */
    static std::uint32_t ipv4ToUint32(const std::string &ip);
};