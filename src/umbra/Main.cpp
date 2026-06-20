#include "minilog/minilog.hpp"
#include "umbra/ModuleLoader.hpp"

int main(int argc, const char* argv[]) {

#ifdef UMBRA_DEBUG
    minilog::setLevel(minilog::Level::Debug);
#else
    minilog::setLevel(minilog::Level::Info);
#endif

    umbra::ModuleLoader loader;
    return loader.parse(argc, argv);
}
