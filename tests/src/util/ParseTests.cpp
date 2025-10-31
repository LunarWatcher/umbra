#include "umbra/util/FilesystemExt.hpp"
#include "umbra/util/Parse.hpp"
#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <format>
#include <format>

static umbra::parse::CommonContext cc;

TEST_CASE("Normal strings are handled correctly", "[Parse]") {
    SECTION("Emtpy string is empty") {
        REQUIRE(umbra::parse::parse("", {cc}) == "");
    }
    SECTION("Standard text is not modified") {
        std::string normal = "trans rights are human rights";
        std::string bracketed = "Hi {there}";

        REQUIRE(umbra::parse::parse(normal, {cc}) == normal);
        REQUIRE(umbra::parse::parse(bracketed, {cc}) == bracketed);
    }
    SECTION("Double brackets standalone don't result in special handling") {
        REQUIRE(
            umbra::parse::parse("trans rights are human rights}}", {cc})
            ==
            "trans rights are human rights}}"
        );
    }
}

TEST_CASE("Brackets are handled correctly", "[Parse]") {
    const auto referenceRoot = umbra::util::getGitRoot();
    REQUIRE(referenceRoot.has_value());
    REQUIRE(cc.gitRoot.has_value());
    REQUIRE(referenceRoot.value() == cc.gitRoot.value());

    SECTION("Git root is correctly expanded") {
        REQUIRE(umbra::parse::parse("{{ git_root }}", {cc}) == referenceRoot.value());
    }

    SECTION("curr_folder_name is correctly expanded") {
        const auto cwd = std::filesystem::current_path().filename().string();
        REQUIRE(umbra::parse::parse("{{curr_folder_name}}", {cc}) == cwd);
    }

    SECTION("Other characters are not eaten") {
        REQUIRE(
            umbra::parse::parse("hi{{git_root}}hi", {cc})
            ==
            std::format("hi{}hi", referenceRoot.value())
        );
    }
}

TEST_CASE("{{cwd}}", "[Parse]") {
    REQUIRE(
        umbra::parse::parse("{{cwd}}", {cc})
        ==
        std::filesystem::current_path().string()
    );
}
