#include "IPBlocker.hpp"
#include <algorithm>

using std::pair;
using std::string;
using std::vector;

IPBlocker::IPBlocker()  = default;

IPBlocker::~IPBlocker() = default;

bool IPBlocker::isBlocked(const string& ip) const {
    auto it = std::find_if(blockedRanges.begin(), blockedRanges.end(), [&ip](const pair<string, string>& range) {
        return ip >= range.first && ip <= range.second;
    });
    return it != blockedRanges.end();
}