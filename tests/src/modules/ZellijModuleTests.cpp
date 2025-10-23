#include "umbra/util/FilesystemExt.hpp"
#include "umbra/util/Parse.hpp"
#include "umbra/util/Parse.hpp"
#include <catch2/catch_test_macros.hpp>

#include <umbra/modules/ZellijModule.hpp>

TEST_CASE("Non-existent paths are not resolved", "[ZellijModule]") {
    umbra::parse::CommonContext cc;
    umbra::ZellijModule mod{cc};
    REQUIRE(mod.resolvePathFromName("__testreserved__") == "__testreserved__");
}

TEST_CASE("Umbra's own public default.kdl is resolved", "[ZellijModule]") {
    umbra::parse::CommonContext cc;
    umbra::ZellijModule mod{cc};
    auto gitRoot = umbra::util::getGitRoot();
    REQUIRE(gitRoot.has_value());
    auto expectedPath = std::filesystem::path(gitRoot.value()) / "dev/zellij/default.kdl";
    REQUIRE(mod.resolvePathFromName("default.kdl") == expectedPath.string());
}

TEST_CASE("Paths are not modified", "[ZellijModule]") {
    umbra::parse::CommonContext cc;
    umbra::ZellijModule mod{cc};

    REQUIRE(mod.resolvePathFromName("./a/b") == "./a/b");
    REQUIRE(mod.resolvePathFromName("a/b") == "a/b");
    REQUIRE(mod.resolvePathFromName("/a/b") == "/a/b");
}
