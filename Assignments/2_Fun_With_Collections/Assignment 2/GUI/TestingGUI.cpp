#include "ProblemHandler.h"
#include "TemporaryComponent.h"
#include "GColorConsole.h"
#include "GUIMain.h"
#include "TextUtils.h"
#include "MemoryDiagnostics.h"
#include "SimpleTest.h"
#include "TestDriver.h"
#include "error.h"
using namespace std;

namespace {
    /* Test type --> String */
    string to_string(SimpleTest::TestType type) {
        switch (type) {
            case SimpleTest::TestType::STUDENT: return "Student Test";
            case SimpleTest::TestType::PROVIDED: return "Provided Test";
            case SimpleTest::TestType::AUTOGRADER: return "Autograder Test";
            case SimpleTest::TestType::MANUAL: return "Manual Test";
            default: error("Unknown test type.");
        }
    }

    /* Test --> Display Name */
    string displayNameOf(const SimpleTest::Test& test) {
        return to_string(test.type) + ": " + test.name;
    }

    /* Displays all the results from the given test group. */
    void displayResults(GColorConsole& console, const Vector<SimpleTest::TestGroup>& testGroups) {
        console.clearDisplay();

        for (const auto& group: testGroups) {
            /* Draw Header */
            console.doWithStyle("#000080", GColorConsole::BOLD, [&] {
                console << "==== Tests for " << group.name << " ====" << '\n';
            });

            /* Draw each test */
            for (const auto& test: group.tests) {
                if (test.result == SimpleTest::TestResult::WAITING) {
                    console.doWithStyle("#A0A0A0", GColorConsole::ITALIC, [&] {
                        console << "         " << displayNameOf(test) << '\n';
                    });
                } else if (test.result == SimpleTest::TestResult::RUNNING) {
                    console.doWithStyle("#0000FF", GColorConsole::ITALIC, [&] {
                        console << "running: " << displayNameOf(test) << '\n';
                    });
                } else if (test.result == SimpleTest::TestResult::PASS) {
                    console.doWithStyle("#404040", GColorConsole::ITALIC, [&] {
                        console << "   pass: " << displayNameOf(test) << '\n';
                    });
                } else if (test.result == SimpleTest::TestResult::FAIL) {
                    console.doWithStyle("#800000", GColorConsole::BOLD, [&] {
                        console << "   FAIL: " << displayNameOf(test) << '\n';
                        console << "    " << test.detailMessage << '\n';
                    });
                } else if (test.result == SimpleTest::TestResult::EXCEPTION) {
                    console.doWithStyle("#FF0000", GColorConsole::BOLD, [&] {
                        console << "   FAIL: " << displayNameOf(test) << '\n';
                        console << "    " << test.detailMessage << '\n';
                    });
                } else if (test.result == SimpleTest::TestResult::LEAK) {
                    console.doWithStyle("#FF0000", GColorConsole::BOLD, [&] {
                        console << "   FAIL: " << displayNameOf(test) << '\n';
                        console << "    " << "Test defined on line " << test.lineNumber << "." << '\n';
                        console << "    " << test.detailMessage << '\n';
                    });
                } else {
                    error("Internal error: Unknown test result. Contact the course staff.");
                }
            }

            /* Draw the overall result. */
            if (group.numPassed == group.numTests && group.numTests > 0) {
                console.doWithStyle("#0000FF", GColorConsole::ITALIC, [&] {
                    console << "All tests in this section passed!" << '\n';
                    console << '\n';
                    console << '\n';
                });
            } else {
                console.doWithStyle("#400000", GColorConsole::BOLD, [&] {
                    console << "Summary: " << group.numPassed << " / " << group.numTests
                         << " test" << (group.numTests == 1? "" : "s") << " passed." << '\n';
                    console << '\n';
                    console << '\n';
                });
            }
        }

        console.flush();
    }

    class TestingGUI: public ProblemHandler {
    public:
        TestingGUI(GWindow& window);

        void settingUp() override;

    private:
        Temporary<GColorConsole> console;
    };

    /* Initialize chrome. */
    TestingGUI::TestingGUI(GWindow& window) : ProblemHandler(window) {
        console = Temporary<GColorConsole>(new GColorConsole(), window, "CENTER");
        setDemoOptionsEnabled(false);
    }

    /* Actually run all the tests. */
    void TestingGUI::settingUp() {
        /* Order in which to run tests. */
        const vector<string> kTestOrder = {
            #define MENU_OPTION(name, constructor)
            #define RUN_TESTS_MENU_OPTION()
            #define WINDOW_TITLE(title)
            #define TEST_ORDER(...) __VA_ARGS__
            #include "../Demos/GUIConfig.h"
            #undef WINDOW_TITLE
            #undef GUI_HANDLER
            #undef MENU_OPTION
            #undef TEST_ORDER
        };

        SimpleTest::TestGroupComparator comparator = [&](const string& lhs, const string& rhs) {
            /* Compare indices of the two strings. */
            return find(kTestOrder.begin(), kTestOrder.end(), lhs) <
                    find(kTestOrder.begin(), kTestOrder.end(), rhs);
        };

        SimpleTest::TestReporter reporter = [this](const Vector<SimpleTest::TestGroup>& groups) {
            displayResults(*console, groups);
        };

        SimpleTest::run(reporter, comparator);

        setDemoOptionsEnabled(true);
    }
}

std::shared_ptr<ProblemHandler> makeTestingGUI(GWindow& window) {
    return make_shared<TestingGUI>(window);
}
