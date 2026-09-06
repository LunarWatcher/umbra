#pragma once

#include <optional>
#include <string>
#include <sstream>
#include <vector>

namespace umbra {

/**
 * Note: this enum is sorted by priority. When nodes have to be merged, 
 */
enum class TestResult {
    Skipped,
    Passed,
    Failed,
};

struct ExpectedPair {
    std::string expression;
    std::string expanded;
};

struct ResultData {
    TestResult result;
    /**
     * Shorthand message for the status. This will usually be empty for Passed, and non-empty for Skipped and Failed.  
     */
    std::optional<std::string> message;
    std::optional<ExpectedPair> evalResult;
};

struct TestRun {
    size_t runNumber;
    /**
     * Extra names for the test run. This is in addition to the test case's name, which is omitted from this list. This
     * is > 1 when a TestCase has more than one run, as this requires sections to be the case.  
     */
    std::vector<std::string> extraNames;

    std::optional<std::string> stdOut;
    std::optional<std::string> stdErr;

    double duration;

    ResultData result;

    std::string joinNames() const {
        std::stringstream ss;
        for (size_t i = 0; i < extraNames.size(); ++i) {
            if (i != 0) {
                ss << " / ";
            }
            ss << extraNames.at(i);
        }
        return ss.str();
    }
};
using TestRuns = std::vector<TestRun>;

struct TestCase {
    std::string name;
    std::optional<std::string> className;
    
    TestResult overallResult;

    TestRuns runs = {};
};
using TestCases = std::vector<TestCase>;

class TestRunner {
public:
    virtual ~TestRunner() = default;

    /**
     * Runs the rests and returns a list of test cases for display.
     * May alternatively print results directly if using a test framework that cannot be configured to trivially report
     * the test output outside, for example, pre-generated .html documents (for example for a hypothetical
     * implementation for a gradle test runner)
     *
     * \param pathOrModule The path to the executable (native tests) or the module name (non-native tests) to run tests
     *                     for. Non-native test runners should assume that pathOrModule == "" means all tests, while
     *                     native test runners should treat it as an error if `!std::filesystem::exists(pathOrModule)`.
     */
    virtual TestCases runTests(
        const std::string& pathOrModule,
        const std::vector<std::string>& extraArguments
    ) = 0;

    /**
     * Returns whether or not the test runner can be automatically matched. This is invoked when TestRunnerType ==
     * Auto. The pathOrModule or other parts of the working directory or similar may be used for matching.
     */
    virtual bool test(
        const std::string& /* pathOrModule */
    ) {
        return false;
    }
};

}
