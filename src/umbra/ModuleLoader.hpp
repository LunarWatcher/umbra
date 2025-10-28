#pragma once

#include "CLI/CLI.hpp"
#include "umbra/modules/Module.hpp"
#include "umbra/util/Parse.hpp"
#include <memory>

namespace umbra {

class ModuleLoader {
public:
    CLI::App app;
    parse::CommonContext cc;

    std::map<
        CLI::App* /* moduleRootSubcommand */,
        std::shared_ptr<Module>
    > modules;

    ModuleLoader();

    /**
     * Parses command line arguments using `CLI11_PARSE()`, and adds error handling that turns exceptions into better
     * output.
     * 
     * \returns an ErrorCode. ErrorCode::OK means no error, all other values means an error happened.
     */
    int parse(int argc, const char* argv[]);
};

}
