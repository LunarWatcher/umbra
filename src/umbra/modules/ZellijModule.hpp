#include "Module.hpp"

namespace umbra {

struct FilesystemLookupDescriptor {
    std::filesystem::path folder;
    std::vector<std::string> contents;
};

class ZellijModule : public Module {
private:
    std::vector<std::filesystem::path> lookupPaths;
    CLI::App* subcommand;

    // Config options {{{
    std::string layout;
    bool listLayouts;

    std::vector<std::string> passthroughArgs;
    // }}}

    void moduleMain();

    void printList();
public:
    ZellijModule(const parse::CommonContext& commonCtx);

    virtual LoadInfo onLoadCLI(CLI::App& app) override;

    /**
     * Takes a string believed to be a layout name, and converts it to a path.
     * Does nothing if the input already is a path, or if the name isn't found anywhere.
     *
     * \returns An absolute path to a layout file, or the name unmodified if it's a path, or the name couldn't be found.
     *
     */
    std::string resolvePathFromName(const std::string& name);
    std::vector<FilesystemLookupDescriptor> listLayoutsAndDirs();
};

}
