#include "umbra/util/FilesystemExt.hpp"
#include "umbra/util/Parse.hpp"
#include <catch2/catch_test_macros.hpp>
#include <format>
#include <format>

TEST_CASE("Normal strings are handled correctly", "[ParseTests]") {
    SECTION("Emtpy string is empty") {
        REQUIRE(umbra::parse::parse("", {}) == "");
    }
    SECTION("Standard text is not modified") {
        std::string normal = "trans rights are human rights";
        std::string bracketed = "Hi {there}";

        REQUIRE(umbra::parse::parse(normal, {}) == normal);
        REQUIRE(umbra::parse::parse(bracketed, {}) == bracketed);
    }
    SECTION("Double brackets standalone don't result in special handling") {
        REQUIRE(
            umbra::parse::parse("trans rights are human rights}}", {})
            ==
            "trans rights are human rights}}"
        );
    }
}

TEST_CASE("Brackets are handled correctly", "[ParseTests]") {
    const auto referenceRoot = umbra::util::getGitRoot();
    REQUIRE(referenceRoot.has_value());

    SECTION("Git root is correctly expanded") {
        REQUIRE(umbra::parse::parse("{{ git_root }}", {}) == referenceRoot.value());
    }

    SECTION("Other characters are not eaten") {
        REQUIRE(
            umbra::parse::parse("hi{{git_root}}hi", {})
            ==
            std::format("hi{}hi", referenceRoot.value())
        );
    }
}
