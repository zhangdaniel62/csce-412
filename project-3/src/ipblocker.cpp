/**
 * @file IPBlocker.cpp
 * @brief Implementation of the IPBlocker firewall component.
 *
 * This file implements the IPBlocker class, which simulates a firewall
 * by maintaining a list of blocked IPv4 address ranges. Incoming request
 * IP addresses are converted to 32-bit integers and checked against
 * these stored ranges.
 */
#include "IPBlocker.h"
#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <utility>

using std::pair;
using std::string;
using std::uint32_t;
using std::vector;

/**
 * @brief Construct an empty IPBlocker.
 *
 * Initializes the internal blocked range list to empty.
 */
IPBlocker::IPBlocker() = default;

/**
 * @brief Destroy the IPBlocker.
 *
 * No dynamic memory is owned directly by this class.
 */
IPBlocker::~IPBlocker() = default;

/**
 * @brief Convert an IPv4 address string to a 32-bit integer.
 *
 * The IPv4 address must be in dotted-decimal format (e.g., "192.168.0.1").
 * The conversion packs the four octets into a single unsigned 32-bit value
 * for efficient numeric comparison.
 *
 * @param ip IPv4 address string in dotted-decimal notation.
 * @return uint32_t Numeric representation of the IPv4 address.
 *
 * @throws std::invalid_argument If the string is not a valid IPv4 address.
 */
uint32_t IPBlocker::ipv4ToUint32(const string &ip)
{
    unsigned int a, b, c, d;
    char dot1, dot2, dot3;

    std::istringstream iss(ip);

    if (!(iss >> a >> dot1 >> b >> dot2 >> c >> dot3 >> d) || dot1 != '.' || dot2 != '.' || dot3 != '.')
    {
        throw std::invalid_argument("Invalid IPv4 address: " + ip);
    }

    return (uint32_t(a) << 24) | (uint32_t(b) << 16) | (uint32_t(c) << 8) | uint32_t(d);
}

/**
 * @brief Check whether an IP address is within any blocked range.
 *
 * The IP string is converted to its numeric representation and compared
 * against all stored blocked ranges.
 *
 * @param ip IPv4 address string to test.
 * @return true If the IP falls within a blocked range.
 * @return false Otherwise.
 */
bool IPBlocker::isBlocked(const string &ip) const
{
    uint32_t intIP = ipv4ToUint32(ip);
    auto it = std::find_if(blockedRanges.begin(), blockedRanges.end(), [&](const pair<uint32_t, uint32_t> &range)
                           { return intIP >= range.first && intIP <= range.second; });
    return it != blockedRanges.end();
}

/**
 * @brief Add a single blocked IP range.
 *
 * The provided pair represents the start and end of the range (inclusive).
 * If the start value is greater than the end value, they are automatically
 * swapped to ensure a valid range.
 *
 * @param ip Pair containing start and end IPv4 address strings.
 */
void IPBlocker::addIP(const pair<string, string> &ip)
{
    uint32_t start = ipv4ToUint32(ip.first);
    uint32_t end = ipv4ToUint32(ip.second);

    if (start > end)
    {
        std::swap(start, end);
    }

    blockedRanges.emplace_back(start, end);
}

/**
 * @brief Add multiple blocked IP ranges.
 *
 * Each pair in the vector represents a start and end IPv4 address range.
 * Internally calls addIP() for each range.
 *
 * @param ips Vector of IP range pairs to block.
 */
void IPBlocker::addIP(const std::vector<std::pair<std::string, std::string>> &ips)
{
    for (const auto &ip : ips)
    {
        addIP(ip);
    }
}

/**
 * @brief Remove a single blocked IP range.
 *
 * The specified start and end addresses are converted to numeric form.
 * If necessary, the range bounds are swapped to ensure correct ordering.
 * The exact matching numeric range is then removed from the internal list.
 *
 * @param ip Pair containing start and end IPv4 address strings.
 */
void IPBlocker::removeIP(const std::pair<std::string, std::string> &ip)
{
    uint32_t start = ipv4ToUint32(ip.first);
    uint32_t end = ipv4ToUint32(ip.second);

    if (start > end)
    {
        std::swap(start, end);
    }

    std::erase(blockedRanges, pair<uint32_t, uint32_t>(start, end));
}

/**
 * @brief Remove multiple blocked IP ranges.
 *
 * Internally calls removeIP() for each range in the vector.
 *
 * @param ips Vector of IP range pairs to remove from the block list.
 */
void IPBlocker::removeIP(const std::vector<std::pair<std::string, std::string>> &ips)
{
    for (const auto &ip : ips)
    {
        removeIP(ip);
    }
}