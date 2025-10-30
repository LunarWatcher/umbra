#include "DevenvModule.hpp"
#include "spdlog/spdlog.h"
#include "stc/Environment.hpp"
#include "umbra/except/Exception.hpp"
#include "umbra/modules/devenv/ConfigSpec.hpp"

#include "umbra/util/FilesystemExt.hpp"
#include "umbra/util/JSONCParser.hpp"
#include "umbra/util/Parse.hpp"
#include "umbra/wranglers/Shell.hpp"
#include <nlohmann/json.hpp>
#include <ranges>

namespace umbra {

DevenvModule::DevenvModule(const parse::CommonContext& commonCtx)
    : Module(parse::ParseContext {
        .commonContext = commonCtx
    }), lookups(
        {
            getEnvWithTransform(
                "UMBRA_DEVENV_PRIVATE_SUBDIR",
                "{{git_root}}/.git/devenv"
            ),
                getEnvWithTransform(
                    "UMBRA_DEVENV_PUBLIC_SUBDIR",
                    "{{git_root}}/dev/devenv"
                ),
        }
    ) {}

LoadInfo DevenvModule::onLoadCLI(CLI::App& app) {
    subcommand = app.add_subcommand(
        "devenv",
        "Environment and command managing utility"
    )
        ->alias("env")
        ->usage(R"(
USAGE:
  # Load the default environment
  umbra env

  # Load an environment specified in a sourced .env.devenv file
  umbra env environment

  # Load a specific shell. Only useful if you don't want to run in your default shell
  umbra env --shell=bash

  # List available environments, along with where they're defined
  umbra env --list
)");

    autoConfVerbosity(subcommand);
    subcommand->callback([this]() {
        moduleMain();
    });

    subcommand->add_option(
        "environment",
        environment,
        "The name of an environment"
    )
        ->default_val("default")
        ->required(false);

    subcommand->add_option(
        "--shell,-s",
        this->shell,
        "The shell to use. Defaults to $SHELL"
    )
        ->default_val(
            stc::getEnv("SHELL", "__fail__")
        )
        ->required(false);

    subcommand->add_flag(
        "--trusted,-t",
        this->trusted,
        "Whether or not to automatically trust any sourced files. Strongly discouraged"
    )
        ->default_val(false)
        ->required(false);

    subcommand->add_flag(
        "--list,-l",
        this->list,
        "Lists available environments and their sources"
    )
        ->default_val(false)
        ->required(false);

    return {
        .rootSubcommand = subcommand,
    };
}

void DevenvModule::moduleMain() {
    if (shell == "__fail__") {
        throw Exception("Failed to identify shell to run");
    } else if (this->list) {
        std::cout << "IOU 1x list command" << std::endl;
        return;
    } else if (!stc::getEnv("UMBRA_DEVENV_ENVIRONMENT").empty()) {
        throw Exception(
            "umbra devenv should NOT be nested. You're already in a session. If you'd like to run several "
            "sessions at once, consider a terminal multiplexer (I strongly recommend zellij, largely because you "
            "can write layouts containing commands to invoke all the umbra sessions you want)"
        );
    }
    std::vector<std::filesystem::path> envLookups = lookups;
    std::reverse(envLookups.begin(), envLookups.end());

    auto shellFile = util::findMatchesInPaths(
        lookups,
        { ".shell.devenv" },
        true
    );
    auto envFile = util::findMatchesInPaths(
        envLookups,
        { ".env.devenv" },
        false
    );

    if (shellFile.empty() && envFile.empty()) {
        throw Exception("Found neither .env.devenv nor .shell.devenv");
    }


    if (envFile.empty() && this->environment != "default") {
        throw Exception(".env.devenv is missing, but using a non-default environment. Cannot continue");
    } else if (envFile.empty()) {
        spdlog::warn(".env.devenv is missing. The environment will not be modified");
    }

    std::vector<devenv::ConfigSpec> configFiles;
    for (auto& file : envFile) {
        spdlog::debug("Now loading {}", file.string());
        std::ifstream f(file);
        if (!f) {
            spdlog::error("{} exists, but could not be opened", file.string());
            continue;
        }
        devenv::ConfigSpec spec = parseJsonC(f).get<devenv::ConfigSpec>();
        configFiles.push_back(
            spec
        );
    }

    if (!configFiles.empty()) {
        auto legalEnvs = std::ranges::to<std::vector>(
            configFiles
            | std::views::transform([](const auto& confSpec) {
                return std::ranges::to<std::vector<std::string>>(
                    confSpec.envs | std::views::keys
                );
            })
            | std::views::join
        );

        if (std::find(legalEnvs.begin(), legalEnvs.end(), environment) == legalEnvs.end()) {
            spdlog::error(
                "The provided environment ({}) is not valid. Legal values: {}",
                environment,
                std::ranges::to<std::string>(std::views::join_with(legalEnvs, ", "))
            );
            throw Exception("Failed to resolve environment");
        }
    } else {
        if (!envFile.empty()) {
            throw Exception("Found .env files, but failed to load any; aborting");
        }

        if (environment != "default") {
            throw Exception("Only the default environment is allowed when no config files exist");
        }
    }

    stc::setEnv("UMBRA_DEVENV_ENVIRONMENT", environment.c_str());

    for (auto& configSpec : configFiles) { // NOLINT
        spdlog::info("IOU 1x env file loading system");
    }

    ShellWrangler wrangler;
    std::vector<const char*> command = {
        "/usr/bin/env",
        shell.c_str()
    };

    std::stringstream sourceCmd;
    if (!shellFile.empty()) {
        // converting a path implicitly like this adds quotes and everything. Not sure if it'll deal with paths
        // containing a quote, but I would assume so. Basically, this is pre-safe.
        sourceCmd << "source " << shellFile.at(0) << "\n";
    }

    wrangler.execInteractive(
        shell,
        command,
        sourceCmd.str()
    );

}

}
