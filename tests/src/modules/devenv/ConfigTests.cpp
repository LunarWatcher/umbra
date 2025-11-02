#include "catch2/catch_test_macros.hpp"
#include "umbra/modules/devenv/ConfigSpec.hpp"

TEST_CASE("The default environment should always exist", "[DevenvConfig]") {
    umbra::devenv::ConfigSpec spec;
    umbra::devenv::from_json(nlohmann::json::object(), spec);
    REQUIRE(spec.envs.size() == 1);
    REQUIRE(spec.envs.contains("default"));
}
