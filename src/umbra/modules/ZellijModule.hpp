#include "Module.hpp"

namespace umbra {

class ZellijModule : public Module {
private:
    std::string layout;
    bool listLayouts;

    void moduleMain();
public:
    virtual CLI::App* onLoadCLI(CLI::App& app) override;
};

}
