#pragma once

#include "umbra/modules/Module.hpp"

namespace umbra {

class DevenvModule : public Module {
private:
    const std::vector<std::filesystem::path> lookups;
    CLI::App* subcommand;

    // Config options {{{
    std::string environment;
    std::string shell;

    bool trusted = false;
    bool list = false;
    bool lint = false;
    // }}}
public:
    DevenvModule(const parse::CommonContext& commonCtx);

    LoadInfo onLoadCLI(CLI::App& app) override;

    void moduleMain();
};

}
