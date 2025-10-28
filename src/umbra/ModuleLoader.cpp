#include "ModuleLoader.hpp"
#include "umbra/except/Exception.hpp"
#include <memory>
#include <stc/Colour.hpp>

#include "modules/include.hpp"
#include "umbra/modules/DevenvModule.hpp"

namespace umbra {
ModuleLoader::ModuleLoader() : app(
    "Magic terminal utilities"
) {

#define CREATE_MODULE(type) std::make_shared<type>( \
                                cc \
                            )
    app.require_subcommand(-1);
    std::vector<std::shared_ptr<Module>> modules = {
        CREATE_MODULE(ZellijModule),
        CREATE_MODULE(DevenvModule),
    };

    for (auto& mod : modules) {
        auto loadInfo = mod->onLoadCLI(app);
        if (loadInfo.rootSubcommand == nullptr) {
            [[unlikely]]
            throw std::runtime_error("Programmer error: onLoadCli returned nullptr");
        }
        this->modules[loadInfo.rootSubcommand] = mod;
    }

    // TODO: optimally, this flag and the subcommand should be mutually exclusive
    app.add_flag_callback(
        "--version,-v",
        []() {
            std::cout << "umbra "
                << (UMBRA_VERSION[0] == 'v' ? "" : "DEV ")
                << UMBRA_VERSION << std::endl;
        },
        "Prints the version and exits"
    );

    app
        .footer(
            R"(Umbra is open-source and licensed under the MIT License.
    https://codeberg.org/LunarWatcher/umbra/src/branch/master/LICENSE

Code available on GitHub and Codeberg:
* https://codeberg.org/LunarWatcher/umbra
* https://github.com/LunarWatcher/umbra)");
}

int ModuleLoader::parse(int argc, const char* argv[]) {
    try {
        (this->app).parse(argc, argv);
    } catch (const CLI ::ParseError &e) {
        (this->app).exit(e);
        return 1;
    } catch(const Exception& e) {
        // TODO: exception handling for the custom exceptions could be a lot better than this, but I'm not currently
        // sure what I could add that's actually useful rather than just a gimmick
        std::cerr
            << stc::colour::fg<stc::colour::FourBitColour::RED>
            << stc::colour::use<stc::colour::Typography::BOLD>
            << "An error occurred. Message: "
            << stc::colour::reset;
        std::cerr << e.what() << std::endl;
        return 2;
    }

    return 0;
}

}

