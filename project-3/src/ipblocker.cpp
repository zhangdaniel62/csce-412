#include "IPBlocker.hpp"
#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <utility>

using std::pair;
using std::string;
using std::uint32_t;
using std::vector;

IPBlocker::IPBlocker() = default;

IPBlocker::~IPBlocker() = default;

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

bool IPBlocker::isBlocked(const string &ip) const
{
    uint32_t intIP = ipv4ToUint32(ip);
    auto it = std::find_if(blockedRanges.begin(), blockedRanges.end(), [&](const pair<uint32_t, uint32_t> &range)
                           { return intIP >= range.first && intIP <= range.second; });
    return it != blockedRanges.end();
}

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

void IPBlocker::addIP(const std::vector<std::pair<std::string, std::string>> &ips)
{
    for (const auto &ip : ips)
    {
        addIP(ip);
    }
}

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

void IPBlocker::removeIP(const std::vector<std::pair<std::string, std::string>> &ips)
{
    for (const auto &ip : ips)
    {
        removeIP(ip);
    }
}