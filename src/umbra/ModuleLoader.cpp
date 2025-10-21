#include "ModuleLoader.hpp"
#include "umbra/except/ErrorCode.hpp"
#include "umbra/except/Exception.hpp"
#include <stc/Colour.hpp>

#include "modules/include.hpp"

namespace umbra {
ModuleLoader::ModuleLoader() : app(
    "Magic terminal utilities"
) {

#define CREATE_MODULE(type) std::make_shared<type>()
    app.require_subcommand(true);
    std::vector<std::shared_ptr<Module>> modules = {
        CREATE_MODULE(ZellijModule)
    };  

    for (auto& mod : modules) {
        auto rootSubcommand = mod->onLoadCLI(app);
        if (rootSubcommand == nullptr) {
            [[unlikely]]
            throw std::runtime_error("Programmer error: onLoadCli returned nullptr");
        }
        this->modules[rootSubcommand] = mod;
    }
}

ErrorCode ModuleLoader::parse(int argc, const char* argv[]) {
    try {
        (this->app).parse(argc, argv);
    } catch (const CLI ::ParseError &e) {
        (this->app).exit(e);
        return ErrorCode::GENERIC_ERROR;
    } catch(const Exception& e) {
        std::cerr
            << stc::colour::fg<stc::colour::FourBitColour::RED>
            << stc::colour::use<stc::colour::Typography::BOLD>
            << "An error occurred. Message: "
            << stc::colour::reset;
        std::cerr << e.what() << std::endl;
        return e.errorCode;
    }

    return ErrorCode::OK;
}

}

