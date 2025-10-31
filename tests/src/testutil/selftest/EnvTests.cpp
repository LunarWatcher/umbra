#include "catch2/catch_test_macros.hpp"

#include "stc/Environment.hpp"
#include "testutil/Env.hpp"
#include <cstdlib>
#include <stdexcept>

TEST_CASE("Test environment deletion", "[testutil::Env]") {
    const static auto VARIABLE_NAME = "TEST_VARIABLE";
    bool control = false;
    SECTION("It should just work") {
        control = true;
        test::EnvVar var { VARIABLE_NAME };
        stc::setEnv(VARIABLE_NAME, "boo");
        REQUIRE(std::getenv(VARIABLE_NAME) != nullptr);
    }
    SECTION("It should work with exceptions") {
        control = true;
        REQUIRE_THROWS([]() {
            test::EnvVar var { VARIABLE_NAME };
            stc::setEnv(VARIABLE_NAME, "boo");
            REQUIRE(std::getenv(VARIABLE_NAME) != nullptr);
            throw std::runtime_error("Boom");
        }());
    }
    
    INFO(
        "If the control check fails, sections can no longer be used for teardown. "
        "(or you did a dumdum and forgot to set the variable in a new section)"
    );
    REQUIRE(control);
    REQUIRE(std::getenv(VARIABLE_NAME) == nullptr);
}
