#pragma once

#include "umbra/except/ErrorCode.hpp"
#include <exception>
#include <optional>
#include <string>

namespace umbra {

class Exception : public std::exception {
public:
    const ErrorCode errorCode;
    const std::string errorMessage;
    const std::optional<std::string> embeddedErrorMessage;

    Exception(const std::string& message, ErrorCode code = ErrorCode::GENERIC_ERROR);
    Exception(
        const std::string& message,
        const std::string& embeddedErrorMessage,
        ErrorCode code = ErrorCode::GENERIC_ERROR
    );
    virtual const char* what() const noexcept override;
};

class TemplateException : public Exception {
public:
    TemplateException(const std::string& message) : Exception(message, ErrorCode::TEMPLATE_ERROR) {}
};

}
