#include "Exception.hpp"

namespace umbra {


Exception::Exception(const std::string& message) 
    : errorMessage(message) {}

Exception::Exception(
    const std::string& message,
    const std::string& embeddedErrorMessage
)
    : errorMessage(message), embeddedErrorMessage(embeddedErrorMessage) {}


const char* Exception::what() const noexcept {
    return errorMessage.c_str();
}

}
