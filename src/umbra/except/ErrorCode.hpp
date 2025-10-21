#pragma once

namespace umbra {

/**
 * Root class for error codes. 
 *
 * For now, only 0 (OK) and 1 (generic error) are implemented, but the idea here is to support additional standard error
 * codes for very specific kinds of errors. This primarily has value for scripting use.
 */
enum class ErrorCode {
    OK = 0,
    GENERIC_ERROR = 1,

    TEMPLATE_ERROR = 2,
};

}
