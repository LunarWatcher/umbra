#include "Parse.hpp"
#include "umbra/except/Exception.hpp"
#include <filesystem>
#include <format>
#include <sstream>
#include <stc/StringUtil.hpp>

namespace umbra {

std::string parse::parse(const std::string& in, const ParseContext& context) {
    if (in.empty()) {
        return "";
    }
    auto gitRoot = context.commonContext.gitRoot;

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

            // TODO: This probably needs to be replaced with a map or something instead.
            // This will not scale well
            if (contents == "git_root") {
                if (!gitRoot.has_value()) {
                    out << "./";
                } else {
                    out << gitRoot.value();
                }
            } else if (contents == "curr_folder_name") {
                out << std::filesystem::current_path().filename().string();
            } else if (contents == "cwd") {
                out << std::filesystem::current_path().string();
            } else {
                throw TemplateException(
                    std::format("Invalid template used: {}", contents)
                );
            }

            // Technically +2, but another +1 happens when the for loop cycles, so +2 eats a character immediately after
            // the {{template}}
            i = end + 1;
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
