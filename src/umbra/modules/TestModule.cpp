#include "TestModule.hpp"
#include "andromeda/component/component_options.hpp"
#include "andromeda/dom/elements.hpp"
#include "minilog/minilog.hpp"
#include "umbra/modules/Module.hpp"
#include "umbra/modules/TestModule.hpp"
#include "umbra/modules/test/Catch2Runner.hpp"
#include "umbra/modules/test/TestRunner.hpp"
#include <utility>

#include "andromeda/component/screen_interactive.hpp"
#include "andromeda/component/component.hpp"
#include "andromeda/component/loop.hpp"

namespace umbra {

TestModule::TestModule(
    const parse::CommonContext& ctx
) : Module(parse::ParseContext {
        .commonContext = ctx
    }) {}

void TestModule::moduleMain() {
    switch (runner) {
    case TestRunnerType::Auto: {
        // TODO: make use of the `test` function? Not really happy with how I structured it, it really should be static
        // with some kind of registry (same setup for init) so we don't initialize more than we have to just to test if
        // the TestRunner matches.
        // Constructor methods might make sense here?
        if (std::filesystem::exists("Makefile")) {
            minilog::info(
                "Found Makefile. Assuming catch2 test suite"
            );
            runner = TestRunnerType::Catch2;
            moduleMain();
        } else {
            minilog::error(
                "Found no auto-resolution markers. Supply runner manually with -r, or double-check your cwd."
            );
        }
    } return;
    case TestRunnerType::Catch2: {
        TestCases cases = Catch2Runner().runTests(
            pathOrModule,
            extras
        );

        // Debug:
        // for (auto& testCase : cases) {
        //     minilog::info("{} (status: {})", testCase.name, static_cast<int>(testCase.overallResult));
        //     for (auto& run : testCase.runs) {
        //         minilog::info("\t{} (status: {})", run.runNumber, static_cast<int>(run.result.result));
        //     }
        // }
        render(cases);
    } break;
    default:
        std::unreachable();
    }
}

andromeda::Color TestModule::getColorForResult(TestResult result) {
    switch(result) {
    case TestResult::Skipped:
        return andromeda::Color::Palette256::Grey30;
        break;
    case TestResult::Passed:
        return andromeda::Color::Palette256::DarkGreen;
        break;
    case TestResult::Failed:
        return andromeda::Color::Palette256::Red1;
        break;
    }
    throw std::runtime_error("Illegal TestResult");
}

std::string TestModule::statusToText(TestResult result) {
    // TODO: include icons?
    switch(result) {
    case TestResult::Skipped:
        return "Skipped";
        break;
    case TestResult::Passed:
        return "Passed";
        break;
    case TestResult::Failed:
        return "Failed";
        break;
    }
    throw std::runtime_error("Illegal TestResult");
}

void TestModule::render(TestCases cases) {
    // Filter failing test cases first so they get more attention
    std::sort(cases.begin(), cases.end(), [](const TestCase& a, const TestCase& b) {
        return static_cast<int>(a.overallResult) > static_cast<int>(b.overallResult);
    });
    // TODO:
    // * It would be nice if you could just like press F5 and have the build run again
    //   * This requires showing build output in the UI, which is a :afoxblobsweats: whole thing

    // Temporary to accelerate the feedback loop. When the UI is more persistent, this should be removed and be added to
    // the UI
    if (cases.empty()) {
        minilog::error("No test cases to show!");
        return;
    }

    auto screen = andromeda::ScreenInteractive::Fullscreen();

    int selectedTest = 0;
    std::vector<std::string> testNames;

    andromeda::Elements children;

    auto menu = andromeda::Container::Vertical(
        {},
        &selectedTest
    );

    size_t failed = 0, passed = 0, skipped = 0;

    for (size_t i = 0; i < cases.size(); ++i) {
        auto& testCase = cases.at(i);
        switch (testCase.overallResult) {
        case TestResult::Skipped:
            ++skipped;
            break;
        case TestResult::Passed:
            ++passed;
            break;
        case TestResult::Failed:
            ++failed;
            break;
        }
        menu->Add(
            andromeda::MenuEntry(andromeda::MenuEntryOption {
                    .label = testCase.name,
                    .transform = [this, testCase](const andromeda::EntryState& state) {
                        andromeda::Color color = getColorForResult(testCase.overallResult);

                        auto box = andromeda::color(
                            color,
                            andromeda::hbox({
                                    andromeda::text(testCase.name)
                                })
                        );

                        return (state.focused) ?
                            box | andromeda::inverted | andromeda::bold | andromeda::focus
                            : (state.active ? box | andromeda::bold : box);
                    }
                })
        );
    }

    int selectedTestRun = 0;

    auto testDetailsRoot = andromeda::Container::Vertical({});

    int splitSize = 20;

    auto root =
        andromeda::Container::Vertical({
                andromeda::Renderer([&]() {
                    return andromeda::text(
                        std::format("Test results ({} passed, {} failed, {} skipped)", passed, failed, skipped)
                    ) | andromeda::center;
                }),
                andromeda::ResizableSplitLeft(
                    menu | andromeda::flex
                    | andromeda::vscroll_indicator
                    | andromeda::yframe,
                    testDetailsRoot | andromeda::flex | andromeda::vscroll_indicator | andromeda::yframe | andromeda::border,
                    &splitSize
                ) | andromeda::flex_grow
            })
        | andromeda::flex_grow;
    
    try {
        andromeda::Loop loop(&screen, root);

        while (!loop.HasQuitted()) {
            refreshTestDetails(
                testDetailsRoot,
                cases,
                selectedTest,
                selectedTestRun
            );

            loop.RunOnce();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
    } catch (...) {
        screen.ExitLoopClosure();
        throw;
    }
}


void TestModule::refreshTestDetails(
    andromeda::Component& testDetailsRoot,
    const TestCases& cases,
    int& activeTestCase,
    int& activeRun
) {
    if (lastTestCase == activeTestCase) {
        return;
    }
    lastTestCase = activeTestCase;
    testDetailsRoot->DetachAllChildren();
        
    auto& test = cases.at(activeTestCase);

    if ((size_t) activeRun >= test.runs.size()) {
        activeRun = 0;
    }

    auto runContainer = andromeda::Container::Vertical({}, &activeRun);
    for (auto& run : test.runs) {
        runContainer->Add(
            andromeda::MenuEntry(andromeda::MenuEntryOption {
                    .label = "",
                    .transform = [this, run](const andromeda::EntryState& state) -> andromeda::Element {
                        andromeda::Color color = getColorForResult(run.result.result);

                        auto box = andromeda::color(
                            color,
                            andromeda::hbox({
                                    andromeda::text(std::to_string(run.runNumber))
                                })
                        );

                        return (state.focused) ?
                            box | andromeda::inverted | andromeda::bold | andromeda::focus
                            : (state.active ? box | andromeda::bold : box);
                    }
                })
        );
    }

    auto split = [](const std::string& value) {
        andromeda::Elements elems;

        auto lines = stc::string::split(value, '\n');
        for (auto& line : lines) {
            elems.push_back(andromeda::text(line));
        }

        return andromeda::vbox(elems);
    };

    auto testDetails = andromeda::Renderer([&]() {
        auto& runData = test.runs.at(activeRun);

        andromeda::Elements messages;
        
        if (runData.result.evalResult) {
            messages.push_back(andromeda::vbox({
                        andromeda::text("Expression:"),
                        split(runData.result.evalResult->expression),
                        andromeda::separator(),
                        andromeda::text("Expanded:"),
                        split(runData.result.evalResult->expanded),
                    }) | andromeda::flex_grow
                );
        }
        if (runData.result.message) {
            messages.push_back(
                andromeda::vbox({
                        andromeda::text("Message"),
                        split(*runData.result.message)
                    }) | andromeda::flex_grow
            );
        }
        return andromeda::vbox({
                andromeda::color(
                    andromeda::Color::Palette256::BlueViolet,
                    andromeda::text(
                        std::format(
                            "Run {}{}{}",
                            activeRun,
                            runData.extraNames.empty() ? "" : ": ",
                            runData.joinNames()
                        )
                    )
                ) | andromeda::center,
                andromeda::color(
                    getColorForResult(runData.result.result),
                    andromeda::text(statusToText(runData.result.result))
                ) | andromeda::center,
                andromeda::hbox(messages),
                andromeda::hbox(
                    {
                        andromeda::vbox({
                                andromeda::text("Stdout") | andromeda::center,
                                split(runData.stdOut.value_or(std::string("")))
                                | andromeda::vscroll_indicator | andromeda::yframe | andromeda::border
                            }) | andromeda::flex_grow,
                        andromeda::vbox({
                                andromeda::text("Stderr") | andromeda::center,
                                split(runData.stdErr.value_or(std::string("")))
                                | andromeda::vscroll_indicator | andromeda::yframe | andromeda::border
                            }) | andromeda::flex_grow,
                    }
                ) | andromeda::flex
            }) | andromeda::flex_grow;
    });
    testDetailsRoot->Add(
        runContainer | andromeda::vscroll_indicator | andromeda::yframe | andromeda::border
    );
    testDetailsRoot->Add(
        testDetails | andromeda::vscroll_indicator | andromeda::yframe
    );
}

LoadInfo TestModule::onLoadCLI(CLI::App& app) {
    subcommand = app.add_subcommand("test");
    subcommand
        ->alias("t")
        ->allow_extras(true)
        ->description(
            "Utility for running tests"
        );

    subcommand->callback(
        [this]() {
            moduleMain();
        }
    );

    std::map<std::string, TestRunnerType> strToTestRunner {
        {"auto", TestRunnerType::Auto},
        {"catch2", TestRunnerType::Catch2},
    };

    subcommand->add_option(
        "-r,--runner",
        runner,
        "Which runner to use."
    )
        ->default_val(runner)
        ->transform(
            CLI::CheckedTransformer(strToTestRunner, CLI::ignore_case)
        );

    subcommand->add_option(
        "-p,--path,-m,--module",
        pathOrModule,
        "A path or module. Can be auto-resolved in certain cases. Path or module depends on "
        "whether or not you use a native runner, but since they internally are treated the same way, it does not "
        "matter which flag you use."
    );

    subcommand->add_option(
        "paths",
        this->extras,
        "Extra args to pass through to the test binary"
    )
        ->required(false)
        ->allow_extra_args(true);

    return {
        .rootSubcommand = subcommand,
    };
}

}
