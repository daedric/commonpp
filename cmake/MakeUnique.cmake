MACRO(CHECK_HAVE_MAKE_UNIQUE VARIABLE)
    UNSET(${VARIABLE})
    INCLUDE (CheckCXXSourceCompiles)
    SET(SAVE ${CMAKE_REQUIRED_FLAGS})
    SET(CMAKE_REQUIRED_FLAGS "-std=c++11")
    CHECK_CXX_SOURCE_COMPILES("
#include <memory>

int main(int, char**)
{
    auto i = std::make_unique(123);
    return 0;
}
"
${VARIABLE})
    SET(CMAKE_REQUIRED_FLAGS ${SAVE})
ENDMACRO()
