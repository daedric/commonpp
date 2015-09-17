macro(CHECK_HAVE_THREAD_LOCAL_SPECIFIER VARIABLE)
    unset(${VARIABLE})
    include(CheckCXXSourceCompiles)
    set(SAVE ${CMAKE_REQUIRED_FLAGS})
    set(CMAKE_REQUIRED_FLAGS "-std=c++11")
    CHECK_CXX_SOURCE_COMPILES("
#include <memory>

struct st
{
    st()
    {
        if (!myptr)
        {
            myptr.reset(new st);
        }
    }

    static thread_local std::unique_ptr<st> myptr;
};

thread_local std::unique_ptr<st> st::myptr;

int main(int, char**)
{

    thread_local int i;
    st _;
    return 0;
}
"
${VARIABLE})
    set(CMAKE_REQUIRED_FLAGS ${SAVE})
endmacro()
