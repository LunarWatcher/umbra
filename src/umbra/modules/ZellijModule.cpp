
#include "CLI/CLI.hpp"
#include "umbra/except/Exception.hpp"
#include "ZellijModule.hpp"

namespace umbra {

CLI::App* ZellijModule::onLoadCLI(CLI::App& app) {
    auto subcommand = app.add_subcommand("zellij");
    subcommand->alias("z");

    subcommand->callback([]() {
        throw Exception("Hi");
    });

    return subcommand;
}

}
