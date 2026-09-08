#include "umbra/modules/test/Catch2Runner.hpp"
#include "umbra/util/FilesystemExt.hpp"
#include <catch2/catch_test_macros.hpp>

namespace {

struct ReferenceData {
    std::string name;
    size_t runs;
};

TEST_CASE("Umbra's test suite (cached) should be correctly parsed") {
    umbra::Catch2Runner runner;

    auto gitRoot = umbra::util::getGitRoot().or_else([]() -> std::optional<std::string> {
        FAIL("Git root failed");
    }).value();

    auto filename = std::filesystem::path(gitRoot) / "tests" / "assets" / "umbra-suite.xml";
    INFO("Filename is " << filename);
    REQUIRE(std::filesystem::exists(filename));

    umbra::TestCases cases;
    REQUIRE_NOTHROW(
        cases = runner.parseFile(filename.c_str())
    );
    REQUIRE(cases.size() == 12);
    std::vector<ReferenceData> runs = {
        { "{{cwd}}", 1 },
        {"Non-existent paths are not resolved", 1},
        {"Test that comments are identified", 1},
        {"The default environment should always exist", 1},
        {"Normal strings are handled correctly", 3},
        {"Test environment deletion", 2},
        {"Environment loading should support environment variables", 2},
        {"Verify environment loading", 2},
        {"Umbra's own public default.kdl is resolved", 1},
        {"Parse full JSON", 2},
        {"Brackets are handled correctly", 3},
        {"Paths are not modified", 1},
    };

    for (size_t i = 0; i < cases.size(); ++i) {
        auto& testCase = cases.at(i);
        auto& ref = runs.at(i);

        INFO("Checking (source name) " << testCase.name);

        {
            INFO("Failing the name check means the order is not maintained");
            REQUIRE(testCase.name == ref.name);
        }
        REQUIRE(testCase.runs.size() == ref.runs);
    }
}

}
