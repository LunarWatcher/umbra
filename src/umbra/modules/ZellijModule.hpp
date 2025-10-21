#include "Module.hpp"

namespace umbra {

class ZellijModule : public Module {
public:
    virtual CLI::App* onLoadCLI(CLI::App& app) override;
};

}
