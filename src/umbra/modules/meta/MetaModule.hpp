#pragma once

#include "umbra/modules/Module.hpp"
#include "umbra/util/Parse.hpp"
namespace umbra {

class MetaModule : public Module {
public:
    MetaModule(const parse::CommonContext& commonCtx);

    LoadInfo onLoadCLI(CLI::App &app) override;

    void moduleMain();

    void checkUpdates();
};

}
