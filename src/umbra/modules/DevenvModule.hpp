#pragma once

#include "umbra/modules/Module.hpp"
#include "umbra/modules/devenv/ConfigSpec.hpp"

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
    void printList();

    void loadEnvironment(
        const devenv::ConfigSpec& spec,
        const std::string& inputEnvironment
    );
    std::string resolveEnvironment(
        const devenv::ConfigSpec& mainSpec,
        const std::string& inputEnvironment
    );

    std::vector<std::string> getLegalEnvironments(
        const std::vector<devenv::ConfigSpec>& specFiles
    );

    std::vector<std::filesystem::path> loadShellPaths();
    std::vector<std::filesystem::path> loadEnvPaths();

    std::optional<devenv::ConfigSpec> loadConfigFile(const std::filesystem::path& file);
    std::vector<devenv::ConfigSpec> loadConfigFiles(
        const std::vector<std::filesystem::path>& envFile
    );
};

}
