#include "stc/minilog.hpp"
#include "umbra/ModuleLoader.hpp"

int main(int argc, const char* argv[]) {

#ifdef UMBRA_DEBUG
    minilog::config().level = minilog::Debug;
#else
    minilog::config().level = minilog::Info;
#endif

    umbra::ModuleLoader loader;
    return loader.parse(argc, argv);
}
