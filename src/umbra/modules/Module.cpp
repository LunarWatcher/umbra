    #include "Module.hpp"
    #include "stc/minilog.hpp"
    #include "stc/Environment.hpp"
    #include "umbra/except/Exception.hpp"

    namespace umbra {

    Module::Module(const parse::ParseContext& parseCtx)
        : parseCtx(parseCtx), parseTransform(parseCtx) {}

    std::string Module::getEnvWithTransform(
        const std::string& name,
        const std::string& defaultVal
    ) {
        auto env = stc::getEnv(name.c_str(), defaultVal);
        return parse::parse(
            env,
            this->parseCtx
        );
    }

    void Module::autoConfVerbosity(CLI::App* subcommand) {
        using namespace std::literals;
        auto* group = subcommand->add_option_group(
            "Verbosity",
            "Overrides the default verbosity. The default level for this module is "s
            + (isModuleQuiet() ? "<LOGGING DISABLED>" : "info")
            + ". These options do not affect standard output, and certain kinds of error output from umbra itself. "
            + "It also does not affect output from nested programs, if applicable."
        );
        auto* v = group->add_flag(
            "-v",
            [this](int64_t count) {
                if (!isModuleQuiet()) {
                    count += 1;
                }
                if (count == 1) {
                    minilog::config().level = minilog::Level::Info;
                } else if (count >= 2) {
                    minilog::config().level = minilog::Level::Debug;
                }
            },
            "Increases verbosity level. Up to -vvv has an effect (-vv for non-quiet modules)"
        );
        group->add_flag(
            "-q",
            [v](int64_t count) {
                if (v->count() != 0) {
                    throw Exception("Passing both -q and -v is illegal");
                }
                if (count == 1) {
                minilog::config().level = minilog::Level::Error;
                } else if (count >= 2) {
                    minilog::config().level = minilog::Level::Critical; // TODO: replace with Off once Off is a thing
                }
            },
            "Decreases verbosity level. -q is error, -qq is off"
        );
        // preparse_callback is called before add_flag callbacks
        subcommand->preparse_callback([this](size_t /* totalArgs */) {
            if (isModuleQuiet()) {
                minilog::config().level = minilog::Level::Critical; // TODO: replace with Off once Off is a thing
            } else if (minilog::config().level != minilog::Level::Debug) { // Prevent overriding from debug to info
                minilog::config().level = minilog::Level::Info;
            }
    });
}

}
