
#include "CLI/CLI.hpp"
#include "spdlog/spdlog.h"
#include "stc/Environment.hpp"
#include "umbra/except/Exception.hpp"
#include "umbra/util/FilesystemExt.hpp"
#include <filesystem>
#include "ZellijModule.hpp"

namespace umbra {

ZellijModule::ZellijModule(const parse::CommonContext& ctx) : Module(parse::ParseContext {
    .commonContext = ctx
}) {}

CLI::App* ZellijModule::onLoadCLI(CLI::App& app) {
    subcommand = app.add_subcommand("zellij");
    subcommand
        ->alias("z")
        ->allow_extras(true)
        ->description("Adds features on top of Zellij. Requires `zellij` installed.")
        ->usage(R"(
USAGE:
  # Open the default layout
  umbra z

  # Only open the layout
  umbra z layout

  # Forward flags to zellij
  umbra z layout -- zellij-flags

  # List layouts and layout directories in prioritised order
  umbra z --list

LAYOUT LOOKUP ORDER:
  1. $UMBRA_ZELLIJ_PRIVATE_SUBDIR
  2. $UMBRA_ZELLIJ_PUBLIC_SUBDIR
  3. Passthrough to zellij for check against `layout_dir` in zellij

ENVIRONMENT VARIABLES:
* UMBRA_ZELLIJ_PRIVATE_SUBDIR (default: `{{git_root}}/.git/zellij`; supports templates)
    Folder for privately shared (non-committed) zellij layouts
* UMBRA_ZELLIJ_PUBLIC_SUBDIR (default: `{{git_root}}/.git/zellij`; supports templates)
    Folder for publicly shared (committed) zellij layouts
* UMBRA_ZELLIJ_NAME_CONVENTION (default: `default.kdl`; supports templates)
    What name to use for the layout if none is provided. This can technically be a full path.)");

    autoConfVerbosity(subcommand);
    subcommand->callback([this]() {
        moduleMain();
    });
    subcommand->add_option(
        "layout",
        layout,
        "Path or name of the layout. Defaults to $UMBRA_ZELLIJ_NAME_CONVENTION"
    )
        ->default_val(stc::getEnv("UMBRA_ZELLIJ_NAME_CONVENTION", "default.kdl"))
        ->transform(parseTransform)
        ->required(false);

    subcommand->add_flag(
        "--list,-l",
        listLayouts,
        "Lists the layout directories and the contents within them. This does not include anything in zellij's layout_dir"
    )
        ->default_val(false);

    subcommand->add_option(
        "zellij-args",
        passthroughArgs,
        "Args to pass through directly to zellij. This option must start with a -- to be used to prevent umbra from "
        "evaluating the flags as part of umbra's argument parsing. See the USAGE section."
    );

    return subcommand;
}

void ZellijModule::moduleMain() {
    if (listLayouts) {
        printList();
        return;
    }

    if (layout.empty()) {
        throw Exception(
            "No layout provided directly or implicitly; cannot load"
        );
    }

    auto resolvedLayout = resolvePathFromName(layout);
    std::vector<const char*> command = {
        "/usr/bin/env",
        "zellij",
        "-l",
        resolvedLayout.c_str()
    };

    for (auto& cmd : passthroughArgs) {
        command.push_back(cmd.c_str());
    }

    int code = 0;
    stc::syscommandNoCapture(command, &code);

    if (code != 0) {
        throw Exception("Failed to load zellij");
    }


}

std::string ZellijModule::resolvePathFromName(const std::string& name) {
    if (name.contains('/') || name.contains('\\')) {
        return name;
    }

    auto resolvedPath = util::findMatchesInPaths(lookupPaths, {
        name.ends_with(".kdl") ? name : name + ".kdl"
    }, true);

    std::string resolvedLayout = name;
    if (!resolvedPath.empty()) {
        resolvedLayout = resolvedPath.at(0);
        spdlog::info("Resolved path to {}", resolvedLayout);
    } else {
        spdlog::warn("Failed to resolve path, passing {} directly as -l", resolvedLayout);
    }
    return resolvedLayout;
}

// TODO: this should become a standard function in FilesystemExt
std::vector<FilesystemLookupDescriptor> ZellijModule::listLayoutsAndDirs() {
    std::vector<FilesystemLookupDescriptor> out;
    for (auto& dir : lookupPaths) {
        FilesystemLookupDescriptor desc = {
            .folder = dir,
            .contents = {}
        };
        if (std::filesystem::exists(dir)) {
            for (auto& path : std::filesystem::directory_iterator(dir)) {
                if (std::filesystem::is_directory(path)) {
                    continue;
                }

                desc.contents.push_back(
                    std::filesystem::relative(path.path(), dir).string()
                );
            }
        }
        out.push_back(desc);
    }
    return out;
}

// TODO: this should probably become a standard function in FilesystemExt (maybe Module, though with an extra arg)
void ZellijModule::printList() {
    std::cout << "Folders are ordered by priority, so layouts with the same name are picked from the first "
        << "available folder. Zellij's `layout_dir` is not listed here, but non-matching layouts are passed directly "
        << "to zellij, so layout_dir and absolute and relative paths are also in play. Keeping track of these is left "
        << "as an exercise to the user." << std::endl;
    auto list = listLayoutsAndDirs();

    for (auto& [dir, files] : list) {
        std::cout << dir.string() << std::endl;
        bool hasLiveTemplates = false;
        for (auto& file : files) {
            if (!file.ends_with(".kdl")) {
                continue;
            }

            std::cout << "\t" << file << std::endl;
            hasLiveTemplates = true;
        }

        if (!hasLiveTemplates) {
            std::cout << "\t" << "No templates found" << std::endl;
        }
        std::cout << "\n\n";
    }
}



}
