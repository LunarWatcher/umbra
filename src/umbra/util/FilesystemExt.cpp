#include "FilesystemExt.hpp"
#include "spdlog/spdlog.h"
#include <filesystem>
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
        spdlog::error("Failed to load git root: {}. `{{git_root}}` templates will default to ./ instead.", stdout);
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

std::vector<util::FilesystemLookupDescriptor> util::listDirectoryPaths(
    const std::vector<std::filesystem::path>& lookupPaths
) {
    std::vector<FilesystemLookupDescriptor> out;
    for (auto& dir : lookupPaths) {
        FilesystemLookupDescriptor desc = {
            .folder = dir,
            .contents = {}
        };
        if (std::filesystem::exists(dir)) {
            for (auto& path : std::filesystem::directory_iterator(dir)) {
                if (std::filesystem::is_directory(path)) {
                    continue;
                }

                desc.contents.push_back(
                    std::filesystem::relative(path.path(), dir).string()
                );
            }
        }
        out.push_back(desc);
    }
    return out;
}

}
