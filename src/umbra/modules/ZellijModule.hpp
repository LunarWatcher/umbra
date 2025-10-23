#include "Module.hpp"

namespace umbra {

class ZellijModule : public Module {
private:
    std::string layout;
    bool listLayouts;

    void moduleMain();
public:
    virtual CLI::App* onLoadCLI(CLI::App& app) override;

    /**
     * Takes a string believed to be a layout name, and converts it to a path.
     * Does nothing if the input already is a path, or if the name isn't found anywhere.
     *
     * \returns An absolute path to a layout file, or the name unmodified if it's a path, or the name couldn't be found.
     *
     */
    std::string resolvePathFromName(const std::string& name);
};

}
