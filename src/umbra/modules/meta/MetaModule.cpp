#include "MetaModule.hpp"
#include <cpr/cpr.h>
#include "nlohmann/json.hpp"
#include "spdlog/spdlog.h"
#include "umbra/except/Exception.hpp"
#include "umbra/modules/Module.hpp"
#include "umbra/util/Parse.hpp"

namespace umbra {

MetaModule::MetaModule(const parse::CommonContext& commonCtx) : Module(parse::ParseContext {
    .commonContext = commonCtx
}) {

}

LoadInfo MetaModule::onLoadCLI(CLI::App& app) {

    auto* rootSubcommand = app.add_subcommand("meta");

    rootSubcommand->add_flag_callback("--check-update,-u", [this]() {
        this->checkUpdates();
    });

    return LoadInfo {
        .rootSubcommand = rootSubcommand
    };
}

void MetaModule::checkUpdates() {
    std::string currVersion = UMBRA_VERSION;
    if (currVersion.contains("DEV") || !currVersion.contains(".")) {
        std::cerr << "Cannot compare: you're running a development version, or a version built without git available. "
            << "Found version " << UMBRA_VERSION << ", but expected vX.Y.Z"
            << std::endl;
        return;
    }

    auto res = cpr::Get(
        cpr::Url { "https://api.github.com/repos/LunarWatcher/umbra/releases/latest" },
        cpr::Header {
            { "User-Agent", "LunarWatcher/umbra (update-check)" },
        }
    );
    if (res.status_code != 200) {
        throw Exception(
            "GitHub returned invalid response",
            res.text
        );
    }
    nlohmann::json json;
    try {
        json = nlohmann::json::parse(res.text);
    } catch (const nlohmann::json::exception& e) {
        spdlog::error("{}", e.what());
        throw Exception(
            "Failed to parse response from GitHub"
        );
    }
    std::string latest = json.at("name");

    if (currVersion != latest) {
        std::cout
            << "Out of date. You're on "
            << currVersion
            << ", latest is "
            << latest
            << std::endl;
    } else {
        std::cout << "Up to date" << std::endl;
    }

}

}
