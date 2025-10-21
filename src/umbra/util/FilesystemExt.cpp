#include "FilesystemExt.hpp"
#include <filesystem>
#include <iostream>

namespace umbra {

std::optional<std::string> util::getGitRoot() {
    int code = 0;
    auto stdout = stc::syscommand(std::vector {
        "/usr/bin/env",
        "git", 
        "rev-parse",
        "--show-toplevel"
    }, &code);
    if (code != 0) {
        std::cerr << stdout << std::endl;
        return std::nullopt;
    }

    return stdout;
}

std::vector<std::filesystem::path> util::findMatchesInPaths(
    const std::vector<std::filesystem::path>& paths,
    const std::vector<std::string>& filenames,
    bool stopAfterFirst
) {
    std::vector<std::filesystem::path> out;
    for (auto& path : paths) {
        for (auto& filename : filenames) {
            auto expectedPath = path / filename;
            if (std::filesystem::exists(expectedPath) && !std::filesystem::is_directory(expectedPath)) {
                out.push_back(expectedPath);
                if (stopAfterFirst) {
                    goto stop;
                }
            }
        }
    }
stop:
    return out;
}

}
