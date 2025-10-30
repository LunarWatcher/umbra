#include "umbra/util/JSONCParser.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Test that comments are identified", "[JSONC]") {
    SECTION("All comments") {
        std::stringstream ss;
        ss << "// Hewwo\n"
            << " // Hewwo with spaces\n"
            << "\t// hewwo with tabs";
        std::stringstream outBuf;

        umbra::parseCommentsFromString(ss, outBuf);

        REQUIRE(
            outBuf.str() == ""
        );
    }
}

TEST_CASE("Parse full JSON", "[JSONC]") {
    std::stringstream ss;
    std::stringstream outBuf;
    ss << R"(
{
    // Some comment
    "test": "hi",
    "trans_rights_are_human_rights": true
}
)";

    SECTION("Check raw parsing") {
        REQUIRE_NOTHROW(
            umbra::parseCommentsFromString(ss, outBuf)
        );
    }

    SECTION("Validate with JSON parsing") {
        nlohmann::json j = umbra::parseJsonC(ss);
        REQUIRE(j.contains("trans_rights_are_human_rights"));
        REQUIRE(j.at("trans_rights_are_human_rights") == true);
    }
}
