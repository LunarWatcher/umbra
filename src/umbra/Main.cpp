#include "spdlog/spdlog.h"
#include "umbra/ModuleLoader.hpp"

int main(int argc, const char* argv[]) {

#ifdef UMBRA_DEBUG
    spdlog::set_level(spdlog::level::debug);
    spdlog::set_pattern("%^[%H:%M:%S.%f] [%-8l]%$ %v");
#else
    spdlog::set_level(spdlog::level::info);
    spdlog::set_pattern("%^%8l%$ %v");
#endif

    umbra::ModuleLoader loader;
    return loader.parse(argc, argv);
}
