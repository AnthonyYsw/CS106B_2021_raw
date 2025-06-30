#pragma once

/* Checks whether the given condition is true. If so, nothing happens. If the condition
 * is false, then the test fails. For example:
 *
 *     EXPECT(vec.isEmpty());
 *     EXPECT(history.next() == "Dik-dik");
 */
#define EXPECT(condition) /* Something internal you shouldn't worry about. */

/* Checks whether the given expression called the error() handler. If so, nothing happens.
 * If not, then the test fails. You can use this to confirm that an expression that is
 * supposed to trigger an exception indeed does so. For example:
 *
 *     Vector<int> myVec;
 *     EXPECT_ERROR(myVec.get(0));    // Nothing happens, since myVec.get(0) does trigger error().
 *     EXPECT_ERROR(myVec.isEmpty()); // The test fails because no error would be generated here.
 */
#define EXPECT_ERROR(expression) /* Something internal you shouldn't worry about. */

/* Checks whether two objects are equal. If so, nothing happens. If not, then the test fails
 * and reports what the two values were. The expectation is that the first value should be
 * the student's value and the second should be the correct answer. For example:
 *
 *     EXPECT_EQUAL(studentValue, referenceValue)
 */
#define EXPECT_EQUAL(student, reference) /* Something internal you shouldn't worry about. */

/* Fails the given test, printing out the specified error message. For example:
 *
 *     SHOW_ERROR("Something went wrong.");
 */
#define SHOW_ERROR(message);

/* Defines a new test case. You can write whatever code you want inside of the test case,
 * but you'll likely want to use EXPECT and EXPECT_ERROR in your test cases, as they're
 * what actually perform tests.
 *
 * The syntax for defining a new test case is
 *
 *    STUDENT_TEST("Description of Test Case") {
 *       ... whatever code you'd like! ...
 *    }
 */
#define STUDENT_TEST(name) /* Something internal you shouldn't worry about. */













/******************************************************************************
 * Everything below this point is purely implementation details. You should   *
 * not need to reference or use anything below here.                          *
 ******************************************************************************/









#include "TextUtils.h"
#include "TestDriver.h"
#include "error.h"
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <functional>
#include <stdexcept>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <cfloat>
#include <algorithm>

namespace SimpleTest {
    namespace Internal {
        /* Type representing a key in the top-level testing map. It's the
         * name of the file in which the test was declared.
         */
        using TestKey = std::string;

        /* Type representing a test case. */
        struct TestCase {
            std::string name;
            TestType type;
            int lineNumber;
            std::function<void()> callback;
        };

        /* The top-level tests map. This is defined as a function rather than a static variable
         * to ensure that it plays well with static initializers.
         */
        std::map<TestKey, std::multimap<int, TestCase>>& rawTests();

        /* Exception type signifying that a test case failed. */
        class TestFailedException: public std::logic_error {
        public:
            TestFailedException(const std::string& message, std::size_t line);
        };

        /* Object whose sole purpose is to put a test case into a group of tests. */
        class TestCaseAdder {
        public:
            TestCaseAdder(const TestKey& key, int lineNumber, const std::string& name, TestType type, std::function<void()> test);
        };

        /**** Defines the macro that adds a new test case. ****/

        /* Generic test-adder, which is hooked into by STUDENT_TEST, PROVIDED_TEST, etc.
         * We need several levels of indirection here because of how the preprocessor works.
         * This first layer expands out to the skeleton of what we want.
         */
        #define ADD_TEST(name, type) DO_ADD_TEST(_testCase, _adder, name, type, __FILE__, __LINE__)

        /* This level of indirection exists so that line will be expanded to __LINE__ and
         * from there to the true line number. We still can't token-paste it here, since
         * the even further level of indirection.
         */
        #define DO_ADD_TEST(fn, adder, name, type, file, line)\
            static void JOIN(fn, line)();\
            static SimpleTest::Internal::TestCaseAdder JOIN(adder, line)(file, line, name, type, JOIN(fn, line));\
            static void JOIN(fn, line)()

        #define JOIN(X, Y) X##Y

        /***** Specific test case additions. *****/
        #undef STUDENT_TEST
        #define STUDENT_TEST(name) ADD_TEST(name, SimpleTest::TestType::STUDENT)
        #define PROVIDED_TEST(name) ADD_TEST(name, SimpleTest::TestType::PROVIDED)
        #define AUTOGRADER_TEST(name) ADD_TEST(name, SimpleTest::TestType::AUTOGRADER)
        #define MANUAL_TEST(name) ADD_TEST(name, SimpleTest::TestType::MANUAL)

        /***** Macros used to implement testing primitives. *****/
        [[ noreturn ]] void doFail(const std::string &message, std::size_t line);

        #undef EXPECT
        #define EXPECT(condition) SimpleTest::Internal::doExpect(condition, "EXPECT failed: " #condition " is not true.", __LINE__)
        void doExpect(bool condition, const std::string& expression, std::size_t line);

        #undef EXPECT_ERROR
        #define EXPECT_ERROR(condition) do {\
            try {\
                (void)(condition); \
                SimpleTest::Internal::doFail("EXPECT_ERROR: " #condition " did not call error().", __LINE__); \
            } catch (const ErrorException& ) { \
                /* Do nothing. */ \
            }\
        } while(0)

        /* * * * Pretty-Printers for EXPECT_EQUAL * * * */
        inline std::string debugFriendlyString(char c) {
            return quotedVersionOf(c);
        }

        /* Print strings in quotes. */
        inline std::string debugFriendlyString(const std::string& s) {
            return quotedVersionOf(s);
        }
        inline std::string debugFriendlyString(const char* s) {
            return debugFriendlyString(std::string(s));
        }
        template <size_t N> std::string debugFriendlyString(char (&s)[N]) {
            return debugFriendlyString((const char *)s);
        }

        /* Print bools as strings. */
        inline std::string debugFriendlyString(bool value) {
            return value? "true" : "false";
        }

        /* Print float/double with maximum precision. */
        inline std::string debugFriendlyString(float value) {
            std::ostringstream result;
            result << std::setprecision(FLT_DIG+1) << value << "f";
            return result.str();
        }

        inline std::string debugFriendlyString(double value) {
            std::ostringstream result;
            result << std::setprecision(DBL_DIG+1) << value << "d";
            return result.str();
        }

        /* Print nullptr as such. */
        inline std::string debugFriendlyString(std::nullptr_t) {
            return "nullptr";
        }

        template <typename T>
        inline std::string debugFriendlyString(T* ptr) {
            /* Print null pointers as such. */
            if (ptr == nullptr) return "nullptr";

            std::ostringstream converter;
            converter << "Memory address " << std::hex << ptr;
            return converter.str();
        }

        /* Print everything else using stream insertion. */
        template <typename T> std::string debugFriendlyString(const T& value) {
            std::ostringstream result;
            result << value;
            return result.str();
        }

        /* * * * Equality comparisons * * * */

        /* Equality comparisons are fuzzy for real numbers. */
        inline bool areEqual(double lhs, double rhs) {
            double tolerance = std::max(std::abs(lhs), std::abs(rhs)) * DBL_EPSILON;
            return std::abs(lhs - rhs) <= tolerance;
        }

        template <typename T1, typename T2> bool areEqual(const T1& lhs, const T2& rhs) {
            return lhs == rhs;
        }

        /* Truncate long strings */
        inline std::string abbreviate(const std::string& s, size_t maxLen = 300) {
            return s.length() < maxLen ? s : s.substr(0, maxLen) + " ...";
        }

        #undef EXPECT_EQUAL
        #define EXPECT_EQUAL(student, ...)                                                          \
            do {                                                                                    \
               auto _studentAnswer = (student);                                                     \
               decltype(_studentAnswer) _referenceAnswer = __VA_ARGS__;                             \
                                                                                                    \
               std::stringstream _expression;                                                       \
               _expression << std::boolalpha << "EXPECT_EQUAL failed: "                             \
                           << #student << " != " << #__VA_ARGS__ "\n"                               \
                           << "                " #student   " = "                                   \
                           << SimpleTest::Internal::abbreviate(SimpleTest::Internal::debugFriendlyString(_studentAnswer)) << '\n'                           \
                           << "                " #__VA_ARGS__ " = "                                 \
                           << SimpleTest::Internal::abbreviate(SimpleTest::Internal::debugFriendlyString(_referenceAnswer));                                \
               SimpleTest::Internal::doExpect(SimpleTest::Internal::areEqual(_studentAnswer, _referenceAnswer), _expression.str(), __LINE__); \
            } while (0)

        #undef SHOW_ERROR
        #define SHOW_ERROR(message) SimpleTest::Internal::showError(message, __LINE__)
        [[ noreturn ]] void showError(const std::string& message, std::size_t line);
    }
}
