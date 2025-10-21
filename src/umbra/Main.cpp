#include "umbra/ModuleLoader.hpp"

int main(int argc, const char* argv[]) {
    umbra::ModuleLoader loader;

    auto errCode = loader.parse(argc, argv);
    return static_cast<int>(errCode);
}
