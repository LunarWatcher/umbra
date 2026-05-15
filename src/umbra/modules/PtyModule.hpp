#pragma once

#include "umbra/modules/Module.hpp"
namespace umbra {

class PtyModule : public Module {
private:
    CLI::App* subcommand;

    std::vector<std::string> passthroughOpts;
public:
    PtyModule(
        const parse::CommonContext& commonCtx
    );

    LoadInfo onLoadCLI(CLI::App& app) override;

    void moduleMain();
    bool isModuleQuiet() override { return true; }
};

}
