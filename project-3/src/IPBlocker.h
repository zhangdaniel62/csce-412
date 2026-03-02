#pragma once

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

class IPBlocker
{
public:
    IPBlocker();
    ~IPBlocker();
    bool isBlocked(const std::string &ip) const;
    void addIP(const std::pair<std::string, std::string> &ip);
    void addIP(const std::vector<std::pair<std::string, std::string>> &ips);
    void removeIP(const std::pair<std::string, std::string> &ip);
    void removeIP(const std::vector<std::pair<std::string, std::string>> &ips);

private:
    // Store IPv4 ranges as numeric values for correct ordering/comparison.
    // Each range is inclusive: [start, end].
    std::vector<std::pair<std::uint32_t, std::uint32_t>> blockedRanges;

    // Converts dotted-quad IPv4 (e.g. "192.168.0.1") to a 32-bit integer.
    // Throws std::invalid_argument on malformed input.
    static std::uint32_t ipv4ToUint32(const std::string &ip);
};