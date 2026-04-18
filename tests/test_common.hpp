#ifndef TEST_COMMON_HPP
#define TEST_COMMON_HPP

#include <iostream>
#include <string>

namespace darija_tests {

inline bool failed = false;

#define ASSERT_EQ(expected, actual) do { \
    if ((expected) != (actual)) { \
        std::cerr << "Assertion failed at " << __FILE__ << ":" << __LINE__ \
                  << ": Expected " << (expected) << " but got " << (actual) << std::endl; \
        failed = true; \
    } \
} while(0)

#define ASSERT_TRUE(condition) do { \
    if (!(condition)) { \
        std::cerr << "Assertion failed at " << __FILE__ << ":" << __LINE__ \
                  << ": " #condition << std::endl; \
        failed = true; \
    } \
} while(0)

#define ASSERT_NE_NULL(ptr) do { \
    if ((ptr) == nullptr) { \
        std::cerr << "Assertion failed at " << __FILE__ << ":" << __LINE__ \
                  << ": Expected non-null pointer" << std::endl; \
        failed = true; \
    } \
} while(0)

} // namespace darija_tests

#endif // TEST_COMMON_HPP
