#pragma once

#include <exception>
#include <optional>
#include <string>

namespace umbra {

class Exception : public std::exception {
public:
    const std::string errorMessage;
    const std::optional<std::string> embeddedErrorMessage;

    Exception(const std::string& message);
    Exception(
        const std::string& message,
        const std::string& embeddedErrorMessage
    );
    virtual const char* what() const noexcept override;
};

class TemplateException : public Exception {
public:
    TemplateException(const std::string& message) : Exception(message) {}
};

}
