#include "FilesystemExt.hpp"
#include <filesystem>
#include <iostream>
#include <vector>

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
        std::cerr << "Failed to load git root: " << stdout;
        return std::nullopt;
    }
    stdout.erase(
        std::remove(stdout.begin(), stdout.end(), '\n'),
        stdout.cend()
    );
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
