#include "BarrierGUI.h"
#include "GUI/TestDriver.h"
using namespace std;
using namespace MiniGUI;

namespace {
    const string kTestRunningColor = "white";
    const string kTestFailedColor  = "#800000";

    const Font kTestRunningFont(FontFamily::SERIF, FontStyle::ITALIC, 24, "blue");
    const Font kTestFailedFont (FontFamily::SERIF, FontStyle::ITALIC, 24, "white");

    const string kTestRunningMessage = "Running tests in %s...";
    const string kTestFailedMessage  = "Not all tests in %s passed. Correct any errors in those tests and select this option again.";

    const double kPadding = 20;

    /* Graphics bounds. */
    GRectangle boundsFor(GWindow& window) {
        return { kPadding, kPadding, window.getCanvasWidth() - 2 * kPadding, window.getCanvasHeight() - 2 * kPadding };
    }

    class BarrierGUI: public ProblemHandler {
    public:
        BarrierGUI(GWindow& window, const string& filename) : ProblemHandler(window) {
            message = format(kTestFailedMessage, filename);
        }

    protected:
        void repaint() {
            clearDisplay(window(), kTestFailedColor);

            auto render = TextRender::construct(message, boundsFor(window()), kTestFailedFont);
            render->alignCenterVertically();
            render->alignCenterHorizontally();

            render->draw(window());
        }

    private:
        string message;
    };

    bool allTestsPassedIn(const string& filename) {
        Vector<SimpleTest::TestGroup> lastTests;
        SimpleTest::run([&](const Vector<SimpleTest::TestGroup>& tests) {
            lastTests = tests;
        }, [&](const string& testFile, const SimpleTest::Test &) {
            return filename == testFile;
        });

        return all_of(lastTests.begin(), lastTests.end(), [&](const SimpleTest::TestGroup& group) {
            /* forall g. (RightGroup(g) -> PassedAllTestsIn(g)) */
            return group.name != filename || group.numPassed == group.numTests;
        });
    }
}

shared_ptr<ProblemHandler>
ifPassedThen(GWindow& window,
             const string& filename,
             function<shared_ptr<ProblemHandler>(GWindow&)> constructor) {
    clearDisplay(window, kTestRunningColor);
    auto text = TextRender::construct(format(kTestRunningMessage, filename),
                                      boundsFor(window),
                                      kTestRunningFont);
    text->alignCenterHorizontally();
    text->alignCenterVertically();
    text->draw(window);
    window.repaint();

    /* Run the tests and see if they all passed. */
    if (allTestsPassedIn(filename)) {
        return constructor(window);
    } else {
        return make_shared<BarrierGUI>(window, filename);
    }
}
