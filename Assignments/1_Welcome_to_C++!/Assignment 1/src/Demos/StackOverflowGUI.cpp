#include "../StackOverflow.h"
#include "../GUI/MiniGUI.h"
#include "goptionpane.h"
using namespace std;

namespace {
    const string kWarningMessage =
R"(Warning! This is going to crash your program.
Make sure you ran your program using the "Debug" option,
the green triangle with the little bug icon on it.)";

    /* This is an unusual one - it doesn't actually visualize anything, and instead
     * either (1) crashes or (2) aborts with an exception.
     */
    class StackOverflowGUI: public ProblemHandler {
    public:
        StackOverflowGUI(GWindow& window) : ProblemHandler(window) {
            GOptionPane::showMessageDialog(kWarningMessage, "Warning!");
            triggerStackOverflow(137);
        }
    };
}

shared_ptr<ProblemHandler> makeStackOverflowGUI(GWindow& window) {
    return make_shared<StackOverflowGUI>(window);
}
