#include "Parse.hpp"
#include "umbra/except/Exception.hpp"
#include <format>
#include <sstream>
#include <stc/StringUtil.hpp>

namespace umbra {

std::string parse::parse(const std::string& in, const ParseContext& context) {
    if (in.size() == 0) {
        return "";
    }

    std::stringstream out;
    size_t i;
    for (i = 0; i < in.size() - 1; ++i) {
        if (in.at(i) == '{' && in.at(i + 1) == '{') {
            auto end = in.find("}}", i);
            if (end == std::string::npos) {
                throw TemplateException(
                    "Mismatched brackets: found {{, but no }}"
                );
            }
            auto innerStart = i + 2;

            auto contents = in.substr(
                innerStart,
                end - innerStart
            );
            contents.erase(contents.find_last_not_of(' ') + 1);
            contents.erase(0, contents.find_first_not_of(' '));

            if (contents == "git_root") {
                if (!context.gitRoot.has_value()) {
                    out << "./";
                } else {
                    out << context.gitRoot.value();
                }
            } else {
                throw TemplateException(
                    std::format("Invalid template used: {}", contents)
                );
            }

            i = end + 2;
            continue;
        } else {
            out << in.at(i);
        }
    }
    if (i < in.size()) {
        out << in.at(i);
    }
    return out.str();
}

}
