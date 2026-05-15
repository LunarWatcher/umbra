#include "PtyModule.hpp"
#include "stc/Environment.hpp"
#include "stc/StringUtil.hpp"
#include "stc/unix/Process.hpp"
#include "umbra/except/Exception.hpp"
#include <filesystem>

namespace umbra {

PtyModule::PtyModule(
    const parse::CommonContext& commonCtx
): Module(parse::ParseContext {
    .commonContext = commonCtx
}) {}

LoadInfo PtyModule::onLoadCLI(CLI::App& app) {
    subcommand = app.add_subcommand("pty");
    subcommand
        ->description(
            "Script helper for forcing sub-scripts to think they're running in a PTY. Similar to "
            "`script`, but recording is not supported by this command. This command pretty much exclusively exists "
            "to be able to `| tee` other scripts, and have them render in colour. Can be used in conjunction with "
            "umbra watch"
        )
        ->usage(R"(
USAGE:
  # To show make with colours in less
  umbra pty -- make -j $(nproc) | less -r

  # Chaining with `umbra watch` (command mode)
  umbra watch --command "umbra pty -- make -j $(nproc) test | less -r" -- ../src
)");

    autoConfVerbosity(subcommand);
    subcommand->callback([this]() {
        moduleMain();
    });

    subcommand->add_option(
        "command",
        passthroughOpts,
        "Files and directories to watch"
    )
        ->required(true)
        ->allow_extra_args(true);

    return {
        .rootSubcommand = subcommand
    };
}

void PtyModule::moduleMain() {
    std::string shell = stc::getEnv(
        "SHELL",
        "/usr/bin/bash"
    );

    std::stringstream ss;
    for (auto& arg : passthroughOpts) {
        ss << (ss.tellp() == 0 ? "" : " ")
           << std::quoted(arg);
    }
    
    stc::Unix::Process proc {
        {
            shell,
            "-c",
            ss.str()
        },
        std::make_shared<stc::Unix::PTY>(),
        std::nullopt,
        {},
        stc::Unix::ReadHandlers::stdStreamRedirect(false)
    };
    int code = proc.block();
    if (code != 0) {
        throw Exception(
            "Non-zero exit code: " + std::to_string(code)
        );
    }
}


}
