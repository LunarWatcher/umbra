#pragma once

#include "CLI/CLI.hpp"
#include "umbra/cli/ParseTransformValidator.hpp"
#include "umbra/util/Parse.hpp"
#include <functional>

namespace umbra {

using HookCallback = std::function<void()>;

/**
 * Contains details about a loaded module.
 */
struct LoadInfo {
    /**
     * Required: pointer to the CLI::App representing this module's root subcommand.
     */
    CLI::App* rootSubcommand;
};

class Module {
protected:
    parse::ParseContext parseCtx;
    ParseTransformValidator parseTransform;
public:
    Module(const parse::ParseContext& ctx);
    virtual ~Module() = default;

    /**
     * Invoked as part of the CLI app setup.
     * Should always be used to create at least one subcommand, and the root subcommand must be returned. This is
     * primarily used to support certain kinds of tests, though I don't know if it's necessary at the time of writing,
     * so this may be refactored later.
     *
     * It's assumed the implementing module calls `callback` on the subcommand, as umbra core doesn't have any logic for
     * checking what ended up matching. This is suggested handled with `callback()`.
     *
     * \param app The app for the root umbra command
     */
    virtual LoadInfo onLoadCLI(CLI::App& app) = 0;

    /**
     * Whether or not the module is quiet by default.
     *
     * By default, this is false, as most modules are expected to have some kind of output. This should be set to true
     * if the module is expected to shut the fuck up or not.
     * This acts as a forced `-qq` flag (when using autoConfVerbosity).
     */
    virtual bool isModuleQuiet() { return false; }

    /**
     * Sets up standard config for -q and -v, and sets the default verbosity level based on isModuleQuiet if the app is
     * parsed.
     */
    void autoConfVerbosity(CLI::App* subcommand);

    std::string getEnvWithTransform(
        const std::string& name,
        const std::string& defaultVal = ""
    );
};

}
