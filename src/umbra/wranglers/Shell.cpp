#include "umbra/wranglers/Shell.hpp"
#include "spdlog/spdlog.h"
#include "stc/Environment.hpp"
#include "stc/StringUtil.hpp"
#include "umbra/except/Exception.hpp"
#include <filesystem>

namespace umbra {
 
ShellType ShellWrangler::identifyShell(const std::string& shellOrPathName) {
    auto split = stc::string::split(shellOrPathName, '/').back();

    if (split == "zsh") {
        return ShellType::ZSH;
    } else if (split == "bash") {
        return ShellType::BASH;
    } else {
        throw Exception("Invalid shell. Expected bash or zsh, found " + split);
    }
}

void ShellWrangler::execInteractive(
    const std::string& shellPathOrName,
    std::vector<const char*> args,
    const std::string& commands
) {
    auto type = identifyShell(shellPathOrName);
    std::filesystem::path currPath = stc::executablePath();
    auto wranglerPath = std::filesystem::canonical(
        currPath.parent_path() / ".." / "share" / "umbra" / "wrangler"
    );
    spdlog::debug("wranglerPath is {}", wranglerPath.string());
    spdlog::debug("Command to inject is: {}", commands);

    // Required to avoid UB
    std::vector<std::string> strs;

    switch (type) {
    case ShellType::ZSH: {
        auto zdotdir = stc::getEnv("ZDOTDIR");
        if (zdotdir.empty()) {
            stc::setEnv(
                "UMBRA_SHELLWRANGLER_ZDOTDIR",
                zdotdir.c_str()
            );
        }
        stc::setEnv(
            "ZDOTDIR",
            wranglerPath.string().c_str()
        );
    } break;
    case ShellType::BASH:
        args.push_back("--init-file");
        strs.push_back(
            std::filesystem::canonical(
                wranglerPath / ".bashrc"
            ).string()
        );
        args.push_back(strs.back().c_str());
        break;
    }

    stc::setEnv(
        "UMBRA_SHELLWRANGLER_COMMANDS",
        commands.c_str()
    );

    for (auto& arg : args) {
        spdlog::debug("Arg: {}", arg);
    }

    int code;
    stc::syscommandNoCapture(
        args,
        &code
    );

    // TODO: figure out what the code for ctrl-d is
    if (code != 0) {
        throw Exception(
            "Shell returned non-zero exit code: " + std::to_string(code)
        );
    }
}

}
