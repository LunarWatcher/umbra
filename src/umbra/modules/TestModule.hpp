#pragma once

#include "Module.hpp"
#include "andromeda/component/component.hpp"
#include "andromeda/dom/elements.hpp"
#include "andromeda/screen/color.hpp"
#include "umbra/modules/test/TestRunner.hpp"
#include <stc/StringUtil.hpp>

namespace umbra {

enum class TestRunnerType {
    Auto,
    Catch2,
};

class TestModule : public Module {
private:
    CLI::App* subcommand;
    std::string pathOrModule;
    std::vector<std::string> extras;

    TestRunnerType runner = TestRunnerType::Auto;

    void moduleMain();

    void render(TestCases cases);
    andromeda::Color getColorForResult(TestResult result);
    std::string statusToText(TestResult result);

    /**
     * Cached active test case for dirty checks before refreshing test details
     */
    int lastTestCase = -1;
    void appendRunInformation(
        andromeda::Component& testDetailsRoot,
        const TestCases& cases,
        int& activeTestCase,
        int& activeRun
    );
    void refreshTestDetails(
        andromeda::Component& testDetailsRoot,
        const TestCases& cases,
        int& activeTestCase,
        int& activeRun
    );

public:
    TestModule(const parse::CommonContext& commonCtx);

    virtual LoadInfo onLoadCLI(CLI::App& app) override;
};

}
