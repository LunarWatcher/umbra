#include "Exception.hpp"

namespace umbra {


Exception::Exception(const std::string& message, ErrorCode code) 
    : errorCode(code), errorMessage(message) {}

const char* Exception::what() const noexcept {
    return errorMessage.c_str();
}

}
