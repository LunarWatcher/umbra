#pragma once

#include "nlohmann/json.hpp"
#include <istream>
#include <stc/StdFix.hpp>

namespace umbra {

template <typename CharT>
inline void parseCommentsFromString(std::basic_istream<CharT>& ss, std::stringstream& tmpBuf) {
    std::string buf;
    while (stc::StdFix::getline(ss, buf)) {
        auto nonspace = buf.find_first_not_of(" \t");
        if (nonspace == std::string::npos) {
            // No whitespace
            tmpBuf << buf << "\n";
            continue;
        }

        if (
            buf.size() > nonspace + 1
            && buf[nonspace] == '/'
            && buf[nonspace + 1] == '/'
        ) {
            // Line is a comment
            continue;
        }

        tmpBuf << buf;
    }

}

/**
 * Naive implementation for parsing pseudo-JSONC to standard JSON and nlohmann::json.
 */
template <typename CharT>
inline nlohmann::json parseJsonC(std::basic_istream<CharT>& ss) {
    std::stringstream tmpBuf;
    parseCommentsFromString(ss, tmpBuf);
    nlohmann::json json;
    tmpBuf >> json;
    return json;
}

}
