#include "Exception.hpp"

namespace umbra {


Exception::Exception(const std::string& message, ErrorCode code) 
    : errorCode(code), errorMessage(message) {}

Exception::Exception(
    const std::string& message,
    const std::string& embeddedErrorMessage,
    ErrorCode code
)
    : errorCode(code), errorMessage(message), embeddedErrorMessage(embeddedErrorMessage) {}


const char* Exception::what() const noexcept {
    return errorMessage.c_str();
}

}
