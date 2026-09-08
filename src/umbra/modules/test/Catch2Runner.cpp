#include "Catch2Runner.hpp"
#include "libxml/parser.h"
#include "libxml/xmlstring.h"
#include "stc/unix/Process.hpp"

#include <minilog/minilog.hpp>
#include <nlohmann/json.hpp>

#include <string>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

namespace umbra {

namespace {

void iterate(
    xmlNode* root,
    const std::function<void(xmlNode* entry)>& callback
) {
    for (xmlNode* currNode = root->children; currNode != nullptr; currNode = currNode->next) {
        if (currNode->type == xmlElementType::XML_ELEMENT_NODE) {
            callback(currNode);
        }
    }
}

std::string safeGetContent(xmlNode* node) {
    auto* raw = xmlNodeGetContent(node);

    if (raw == nullptr) {
        throw std::runtime_error("No content!");
    }

    return std::string {
        (const char*) raw,
        xmlStrlen(raw)
    };
}

std::string safeGet(xmlNode* node, const xmlChar* propName) {
    // TODO: this will behave weirdly if null bytes are involved. Ignoring this for now because it's very much a
    // self-inflicted problem if it ever comes up, but would be nice to do it properly
    auto* raw = xmlGetProp(node, propName);
    if (raw == nullptr) {
        throw std::runtime_error("Unexpected null property: " + std::string((const char*) propName));
    }
    return std::string((const char*) raw);
}
std::optional<std::string> safeNullableGet(xmlNode* node, const xmlChar* propName) {
    auto* raw = xmlGetProp(node, propName);
    if (raw == nullptr) {
        return std::nullopt;
    }
    return std::string((const char*) raw);
}

size_t safeGetSize(xmlNode* node, const xmlChar* propName) {
    auto raw = safeGet(node, propName);
    return std::stoull(raw);
}

double safeGetDouble(xmlNode* node, const xmlChar* propName) {
    auto raw = safeGet(node, propName);
    return std::stod(raw);
}

bool safeGetBool(xmlNode* node, const xmlChar* propName) {
    auto raw = safeGet(node, propName);
    return raw == "true";
}

}

bool Catch2Runner::build(const std::string& identifier) {
    stc::Unix::Process proc({
        "/usr/bin/make",
        "-j", std::to_string(std::thread::hardware_concurrency()),
        identifier
    });

    if (proc.block() != 0) {
        minilog::error("Build failed");
        return false;
    }
    return true;
}

TestCases Catch2Runner::parseXmlV3(
    XMLDocumentContext& context
) {
    TestCases tests;

    tests.reserve(
        xmlChildElementCount(context.root)
    );
    iterate(
        context.root,
        [&](xmlNode* node) {
            std::string_view nodeName {reinterpret_cast<const char*>(node->name)};

            if (nodeName == "TestCase") {
                TestResult result = TestResult::Skipped;
                TestRuns children;
                std::optional<std::string> className = safeNullableGet(node, (const xmlChar*) "class-name");

                auto testName = safeGet(node, (const xmlChar*) "name");

                // Iterate test runs
                iterate(
                    node,
                    [&](xmlNode* testRunNode) {
                        auto nodeName = std::string_view { (const char*) testRunNode->name };

                        if (nodeName != "TestRun") {
                            // OverallResult is derived from all the tests, so we don't care about that particular
                            // node. Any other nodes appearing is unexpected
                            if (nodeName != "OverallResult") {
                                minilog::warn("Skipping {}", nodeName);
                            }
                            return;
                        }

                        TestRun run;
                        run.runNumber = safeGetSize(testRunNode, (const xmlChar*) "run-number");
                        iterate(
                            testRunNode,
                            [&](xmlNode* runDataNode) {
                                std::string_view nodeName {reinterpret_cast<const char*>(runDataNode->name)};
                                if (nodeName == "Section") {
                                    // TODO: would be nice to include sourceinfo here, but not sure how to best display
                                    // it yet
                                    run.extraNames.push_back(
                                        safeGet(runDataNode, (const xmlChar*) "name")
                                    );
                                } else if (nodeName == "Expression") {
                                    // TODO: we could in theory report every assertion here, but we currently don't
                                    // because I want to get the UI going, and I'm not convinced reporting passing
                                    // assertions is in any way useful

                                    if (!safeGetBool(runDataNode, (const xmlChar*) "success")) {
                                        auto original = context
                                            .xpath(runDataNode, (const xmlChar*) "./Original")
                                            .autoresolve();
                                        auto expanded = context
                                            .xpath(runDataNode, (const xmlChar*) "./Expanded")
                                            .autoresolve();

                                        run.result.evalResult = {
                                            .expression = safeGetContent(original),
                                            .expanded = safeGetContent(expanded),
                                        };
                                    }
                                } else if (nodeName == "Skip") {
                                    run.result.result = TestResult::Skipped; // TODO: redundant?
                                    run.result.message = safeGetContent(runDataNode);
                                } else if (nodeName == "Duration") {
                                    run.duration = safeGetDouble(
                                        runDataNode,
                                        (const xmlChar*) "durationInSeconds"
                                    );
                                } else if (nodeName == "OverallResults") {
                                    if (safeGetBool(runDataNode, (const xmlChar*) "skipped")) {
                                        run.result.result = TestResult::Skipped;
                                    } else if (safeGetBool(runDataNode, (const xmlChar*) "success")) {
                                        run.result.result = TestResult::Passed;
                                    } else {
                                        run.result.result = TestResult::Failed;
                                    }
                                    auto stdOut = context
                                        .xpath(runDataNode, (const xmlChar*) "./StdOut")
                                        .autoresolveNullable();
                                    auto stdErr = context
                                        .xpath(runDataNode, (const xmlChar*) "./StdErr")
                                        .autoresolveNullable();

                                    if (stdOut != nullptr) {
                                        run.stdOut = safeGetContent(stdOut);
                                    }
                                    if (stdErr != nullptr) {
                                        run.stdErr = safeGetContent(stdErr);
                                    }

                                } else if (nodeName == "Info") {
                                    // TODO: suboptimal
                                    if (!run.result.message) {
                                        run.result.message = "";
                                    }
                                    *run.result.message += safeGetContent(runDataNode);
                                } else {
                                    minilog::error("Unhandled node: {}", nodeName);
                                }
                            }
                        );
                        // Propagate result to parent test
                        if (static_cast<int>(run.result.result) > static_cast<int>(result)) {
                            result = run.result.result;
                        }

                        children.push_back(
                            std::move(run)
                        );
                    }
                );
                
                TestCase testCase {
                    std::move(testName),
                    className,
                    result,
                    children
                };

                tests.push_back(std::move(testCase));
                // NOLINTNEXTLINE(bugprone-branch-clone): empty branches are kept for doc purposes, as well as placeholders for future data expansion
            } else if (nodeName == "OverallResults") {
                // Assertions? Might be sections?
            } else if (nodeName == "OverallResultsCases") {
                // Overall test case count, not useful for our use
            }
        });

    return tests;
}

TestCases Catch2Runner::runTests(
    const std::string& pathOrModule,
    const std::vector<std::string>& extraArguments
) {
    const static std::string filename = "./.umbra-test-report-cache.xml";
    auto path = pathOrModule.empty() ? std::string{"./bin/tests"} : pathOrModule;
    if (!path.contains('/')) {
        minilog::info("Name supplied: running build");
        if (!build(path)) {
            return {};
        }
        // allow for example umbra_test as argument
        path = "./bin/" + path;
    } else {
        minilog::info("Can't do build because path was provided");
    }

    if (!std::filesystem::exists(path)) {
        minilog::error("Path {} does not exist! Do you need to manually supply -p?", path);
        return {};
    }

    std::vector<std::string> args = {
        path,
        "--reporter", "AugmentedXML",
        "-o", filename,
    };
    args.insert(args.end(), extraArguments.begin(), extraArguments.end());

    stc::Unix::Process proc(
        args
    );
    int code = proc.block();

    minilog::info("Tests {}", code == 0 ? "passed" : "failed");

    return parseFile(filename);
}

TestCases Catch2Runner::parseFile(const std::string& filename) {
    XMLDocumentContext doc(filename);
    auto versionStr = xmlGetProp(doc.root, (const xmlChar*) "xml-format-version");

    if (versionStr == nullptr) {
        minilog::error(
            "Failed to resolve XML format version! Is this actually a catch2 test suite with stc::testutil linked?"
        );

        // TODO: sounds like a fine use for std::expected
        return {};
    }

    int version = std::strtoul(
        (const char*) versionStr,
        nullptr,
        10
    );

    minilog::debug("Catch2 format version is {}", version);

    switch(version) {
    case 3:
        return parseXmlV3(doc);
    default:
        minilog::error("The format json has been bumped and is not recognised. Found version {}", version);
        return {};
    }
}

}
