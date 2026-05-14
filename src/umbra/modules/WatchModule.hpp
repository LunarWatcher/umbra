#pragma once

#include "Module.hpp"

namespace umbra {

class WatchModule : public Module {
private:
    std::vector<std::filesystem::path> watchDirs;
    CLI::App* subcommand;

    bool exitOnChange = false;
    std::optional<std::string> command;

    std::vector<std::filesystem::path> passthroughOpts;
public:
    WatchModule(
        const parse::CommonContext& commonCtx
    );

    LoadInfo onLoadCLI(CLI::App &app) override;

    void moduleMain();
};

}
