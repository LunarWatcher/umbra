#include "WatchModule.hpp"
#include "umbra/sys/FSWatcher.hpp"
#include <thread>
#include <stc/unix/Process.hpp>

namespace umbra {

WatchModule::WatchModule(
    const parse::CommonContext& commonCtx
): Module(parse::ParseContext {
    .commonContext = commonCtx
}) {}

LoadInfo WatchModule::onLoadCLI(CLI::App& app) {
    subcommand = app.add_subcommand("watch");
    subcommand
        ->alias("w")
        ->description("File watcher")
        ->allow_extras(true)
        ->usage(R"(
USAGE:
  # Watch folders
  umbra w -c 'make -j $(nproc)' -- ../src ../tests

  # Watch files
  umbra w -c 'make -j $(nproc)' -- ../CMakeLists.txt

  # Exit on match
  # This is not recommended due to the overhead of recreating the watch tree every time umbra starts, but if that's what
  # you want, this does technically work
  while umbra w --exit -- ../src; do make -j $(nproc); done

  # Chaining with `umbra pty`
  umbra watch --command "umbra pty -- make -j $(nproc) test | less -r" -- ../src
)");

    autoConfVerbosity(subcommand);
    subcommand->callback([this]() {
        moduleMain();
    });

    auto exitOpt = subcommand->add_flag(
        "--exit,-e",
        exitOnChange,
        "Whether or not to exit when a change happens. This is discouraged in favour of --command"
    )
        ->default_val(exitOnChange);

    auto execOpt = subcommand->add_option(
        "--command,-c",
        command,
        "An optional command to execute. Incompatible with -e"
    )
        ->default_val(std::nullopt);
    execOpt->excludes(exitOpt);

    subcommand->add_option(
        "paths",
        passthroughOpts,
        "Files and directories to watch"
    )
        ->required(true)
        ->allow_extra_args(true);

    return {
        .rootSubcommand = subcommand
    };
}

void WatchModule::moduleMain() {
    if (passthroughOpts.size() == 0) {
        spdlog::error("You must provide at least one path to watch");
        return;
    }

    FSWatcher watcher(passthroughOpts);

    watcher.block([this](
            EventKind,
            const std::filesystem::path& path
    ) {
        std::cout << path.string() << std::endl;
        if (this->command.has_value()) {
            // NOLINTNEXTLINE(bugprone-command-processor): it's a feature, not a bug
            std::system(this->command.value().c_str());
        }
        return !this->exitOnChange;
    });
}

}
