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
#include <optional>
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

LOOKUP ORDER:
  1. $UMBRA_DEVENV_PRIVATE_SUBDIR
  2. $UMBRA_DEVENV_PUBLIC_SUBDIR

  EXCEPT:
    1. The first matching `.shell.devenv` is loaded, while the rest are ignored
    2. `.env.devenv` is loaded in the opposite order, so the private files take priority over public files.

ENVIRONMENT VARIABLES:
  * `UMBRA_DEVENV_PRIVATE_SUBDIR` (default: `{{git_root}}/.git/devenv`; templates supported)
  * `UMBRA_DEVENV_PUBLIC_SUBDIR` (default: `{{git_root}}/dev/devenv`; templates supported)
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
        printList();
        return;
    } else if (this->lint) {
        std::cout << "IOU 1x lint command" << std::endl;
        return;
    } else if (!stc::getEnv("UMBRA_DEVENV_ENVIRONMENT").empty()) {
        throw Exception(
            "umbra devenv should NOT be nested. You're already in a session. If you'd like to run several "
            "sessions at once, consider a terminal multiplexer (I strongly recommend zellij, largely because you "
            "can write layouts containing commands to invoke all the umbra sessions you want)"
        );
    }
    auto shellFile = loadShellPaths();
    auto envFile = loadEnvPaths();

    if (shellFile.empty() && envFile.empty()) {
        throw Exception("Found neither .env.devenv nor .shell.devenv");
    } else if (envFile.empty() && this->environment != "default") {
        throw Exception(".env.devenv is missing, but using a non-default environment. Cannot continue");
    } else if (envFile.empty()) {
        spdlog::warn(".env.devenv is missing. The environment will not be modified");
    }

    auto configFiles = loadConfigFiles(
        envFile
    );
    std::string environment = configFiles.size() == 0 ? this->environment : resolveEnvironment(
        configFiles.at(0), this->environment
    );
    spdlog::info(
        "Resolved environment to {} (aliased: {})",
        environment,
        environment != this->environment ? "yes" : "no"
    );

    if (!configFiles.empty()) {
        auto legalEnvs = getLegalEnvironments(configFiles);
        if (legalEnvs.find(environment) == legalEnvs.end()) {
            spdlog::error(
                "The provided environment ({}) is not valid. Legal values: {}",
                environment,
                std::ranges::to<std::string>(std::views::join_with(legalEnvs, ", "))
            );
            throw Exception("Failed to resolve environment");
        }
    } else if (!envFile.empty()) {
        throw Exception("Found .env files, but failed to load any; aborting");
    } else if (environment != "default") {
        throw Exception("Only the default environment is allowed when no config files exist");
    }

    stc::setEnv("UMBRA_DEVENV_ENVIRONMENT", environment.c_str());

    for (auto& configSpec : configFiles) { // NOLINT
        loadEnvironment(
            configSpec, 
            environment
        );
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

void DevenvModule::loadEnvironment(
    const devenv::ConfigSpec& spec,
    const std::string& environment
) {
    auto& env = spec.envs.at(environment);
    for (const auto& [k, v] : env.vars) {
        stc::setEnv(k.c_str(), parse::parse(v, this->parseCtx).c_str());
    }
    for (const auto& [k, vals] : env.prepend) {
        std::string existing = stc::getEnv(k.c_str());
        std::stringstream ss;
        for (auto& val : vals) {
            if (ss.tellp() > 0) {
                ss << ":";
            }
            ss << parse::parse(val, this->parseCtx);
        }
        if (ss.tellp() > 0 && !existing.empty()) {
            ss << ":" << existing;
        }
        stc::setEnv(
            k.c_str(), 
            ss.str().c_str()
        );
    }
}

std::string DevenvModule::resolveEnvironment(
    const devenv::ConfigSpec& mainSpec,
    const std::string& inputEnvironment
) {
    if (inputEnvironment == "default") {
        return mainSpec.envs.at(inputEnvironment).aliasFor.value_or(inputEnvironment);
    } else {
        return inputEnvironment;
    }

}

void DevenvModule::printList() {
    std::cout 
        << "Note that the source order is reversed for .env.devenv files, and only the topmost shell file is sourced "
        << "directly by umbra. See the documentation (or --help) for more information.\n"
        << std::endl;
    auto list = util::listDirectoryPaths(this->lookups);

    std::set<std::string> allEnvs;
    for (auto& [dir, files] : list) {
        std::cout << dir.string() << std::endl;
        bool hasContents = false;
        for (const auto& file : files) {
            if (!file.ends_with(".devenv")) {
                continue;
            }

            if (file == ".env.devenv" || file == ".shell.devenv") {
                hasContents = true;
                std::cout << "\t" << file;
                if (file.starts_with(".env")) {
                    std::cout << " [env file]";
                } else {
                    std::cout << " [shell file]";
                }
                std::cout << " [trusted: " << "not implemented" << "]" << std::endl;

                if (file.starts_with(".env")) {
                    try {
                        auto spec = loadConfigFile(dir / file);
                        if (spec.has_value()) {
                            auto envs = getLegalEnvironments({spec.value()}, false);
                            std::cout << "\t    Declared environments: "
                                << std::ranges::to<std::string>(
                                    std::views::join_with(envs, ", ")
                                )
                                << std::endl;
                            allEnvs.insert(envs.begin(), envs.end());
                        }
                    } catch (const nlohmann::json::exception& err) {
                        spdlog::error("\t    JSON parse error: {}", err.what());
                    }
                }
            }
        }

        if (!hasContents) {
            std::cout << "\tNo config files found" << std::endl;
        }
        std::cout << "\n\n";

        if (allEnvs.size()) {
            std::cout << "All available environments: " << std::ranges::to<std::string>(
                std::views::join_with(allEnvs, ", ")
            ) << std::endl;

        }
    }
}

std::set<std::string> DevenvModule::getLegalEnvironments(
    const std::vector<devenv::ConfigSpec>& specFiles,
    bool flattenAliases
) {
    std::set<std::string> out;
    for (auto& confSpec : specFiles) {
        for (const auto& key : confSpec.envs | std::views::keys) {
            if (key == "default") {

                auto alias = confSpec.envs
                    .at(key)
                    .aliasFor
                    .value_or(key);
                if (alias == key || flattenAliases) {
                    out.insert(alias); // not aliased (key == alias), or flattening is enabled (key -> alias[target])

                } else {
                    out.insert(
                        key + " -> " + alias
                    );
                }
            } else {
                out.insert(key);
            }
        }
    }

    return out;
}

std::optional<devenv::ConfigSpec> DevenvModule::loadConfigFile(const std::filesystem::path& file) {
    std::ifstream f(file);
    if (!f) {
        spdlog::error("{} exists, but could not be opened", file.string());
        return std::nullopt;
    }
    return parseJsonC(f).get<devenv::ConfigSpec>();
}

std::vector<devenv::ConfigSpec> DevenvModule::loadConfigFiles(
    const std::vector<std::filesystem::path>& envFile
) {
    std::vector<devenv::ConfigSpec> configFiles;
    for (auto& file : envFile) {
    spdlog::debug("Now loading {}", file.string());
        auto spec = loadConfigFile(file);
        if (!spec.has_value()) {
            continue;
        }
        configFiles.push_back(
            spec.value()
        );
    }
    return configFiles;
}

std::vector<std::filesystem::path> DevenvModule::loadShellPaths() {
    return util::findMatchesInPaths(
        lookups,
        { ".shell.devenv" },
        true
    );

}
std::vector<std::filesystem::path> DevenvModule::loadEnvPaths() {
    std::vector<std::filesystem::path> envLookups = lookups;
    std::reverse(envLookups.begin(), envLookups.end());
    return util::findMatchesInPaths(
        envLookups,
        { ".env.devenv" },
        false
    );
}

}
