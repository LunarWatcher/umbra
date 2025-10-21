#pragma once

#include "umbra/except/ErrorCode.hpp"
#include <exception>
#include <string>

namespace umbra {

class Exception : public std::exception {
public:
    const ErrorCode errorCode;
    const std::string errorMessage;

    Exception(const std::string& message, ErrorCode code = ErrorCode::GENERIC_ERROR);
    virtual const char* what() const noexcept override;
};

class TemplateException : public Exception {
public:
    TemplateException(const std::string& message) : Exception(message, ErrorCode::TEMPLATE_ERROR) {}
};

}
