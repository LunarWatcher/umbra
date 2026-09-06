#pragma once

#include "TestRunner.hpp"
#include "umbra/wrapper/XMLWrapper.hpp"

namespace umbra {

class Catch2Runner : public TestRunner {
private:
    bool build(const std::string& pathOrModule);
    TestCases parseXmlV3(
        XMLDocumentContext& context
    );
public:
    TestCases runTests(
        const std::string& pathOrModule,
        const std::vector<std::string>& extraArguments
    ) override;
};

}
