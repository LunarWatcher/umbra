#include "catch2/catch_test_macros.hpp"
#include "stc/Environment.hpp"
#include "stc/Environment.hpp"
#include "testutil/Env.hpp"
#include "umbra/modules/DevenvModule.hpp"
#include "umbra/modules/DevenvModule.hpp"
#include "umbra/modules/devenv/ConfigSpec.hpp"
#include <filesystem>
#include <filesystem>

TEST_CASE("Verify environment loading", "[DevenvModule]") {
    umbra::devenv::ConfigSpec spec {
        .envs = {
            {
                "default",
                umbra::devenv::EnvConfig {
                    .aliasFor = std::nullopt,
                    .vars = {
                        {"TRANS_RIGHTS_ARE_HUMAN_RIGHTS", "true"}
                    },
                    .prepend = {
                        {
                            "TEST_PREPEND",
                            { "A", "B", "C", "D" }
                        }
                    }
                }
            }
        }
    };
    test::EnvVar v("TRANS_RIGHTS_ARE_HUMAN_RIGHTS"),
        v2("TEST_PREPEND");

    umbra::parse::CommonContext cc;
    auto mod = umbra::DevenvModule{cc};

    std::string expectedStandard = "true";
    std::string expectedPrepend;

    SECTION("Without pre-defined TEST_PREPEND") {
        stc::setEnv("TEST_PREPEND", nullptr);
        expectedPrepend = "A:B:C:D";
    }

    SECTION("With pre-defined TEST_PREPEND") {
        stc::setEnv("TEST_PREPEND", "For the horde:For the alliance");
        expectedPrepend = "A:B:C:D:For the horde:For the alliance";
    }

    REQUIRE_NOTHROW(mod.loadEnvironment(spec, "default"));

    REQUIRE(std::getenv("TRANS_RIGHTS_ARE_HUMAN_RIGHTS") != nullptr);
    REQUIRE(std::getenv("TEST_PREPEND") != nullptr);

    REQUIRE(stc::getEnv("TRANS_RIGHTS_ARE_HUMAN_RIGHTS") == expectedStandard);
    REQUIRE(stc::getEnv("TEST_PREPEND") == expectedPrepend);
}

TEST_CASE("Environment loading should support environment variables", "[DevenvModule]") {
    umbra::devenv::ConfigSpec spec {
        .envs = {
            {
                "default",
                umbra::devenv::EnvConfig {
                    .aliasFor = std::nullopt,
                    .vars = {
                        {"TRANS_RIGHTS_ARE_HUMAN_RIGHTS", "{{git_root}}"}
                    },
                    .prepend = {
                        {
                            "TEST_PREPEND",
                            { "{{curr_folder_name}}" }
                        }
                    }
                }
            }
        }
    };
    test::EnvVar v("TRANS_RIGHTS_ARE_HUMAN_RIGHTS"),
        v2("TEST_PREPEND");

    umbra::parse::CommonContext cc;
    auto mod = umbra::DevenvModule{cc};

    std::string expectedStandard = umbra::util::getGitRoot().value();
    std::string expectedPrepend = std::filesystem::current_path().filename();

    SECTION("Without pre-defined TEST_PREPEND") {
        stc::setEnv("TEST_PREPEND", nullptr);
    }

    SECTION("With pre-defined TEST_PREPEND") {
        stc::setEnv("TEST_PREPEND", "{{git_root}}:the factory must grow");
        // This tests that templates in existing prepended variables aren't expanded.
        // This should avoid multiple categories of clusterfuck bugs in weird variables
        expectedPrepend += ":{{git_root}}:the factory must grow";
    }

    REQUIRE_NOTHROW(mod.loadEnvironment(spec, "default"));

    REQUIRE(std::getenv("TRANS_RIGHTS_ARE_HUMAN_RIGHTS") != nullptr);
    REQUIRE(std::getenv("TEST_PREPEND") != nullptr);

    REQUIRE(stc::getEnv("TRANS_RIGHTS_ARE_HUMAN_RIGHTS") == expectedStandard);
    REQUIRE(stc::getEnv("TEST_PREPEND") == expectedPrepend);
}
