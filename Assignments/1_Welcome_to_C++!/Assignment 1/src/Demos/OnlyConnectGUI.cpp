#include "../OnlyConnect.h"
#include "../GUI/MiniGUI.h"
#include "ginteractors.h"
using namespace std;

namespace {
    class OnlyConnectGUI: public ProblemHandler {
    public:
        OnlyConnectGUI(GWindow& window);
        void actionPerformed(GObservable* source) override;

    private:
        Temporary<GLabel>     inputLabel;
        Temporary<GTextField> inputText;
        Temporary<GButton>    go;

        Temporary<GColorConsole> display;

        /* Button to clear the console. */
        Temporary<GButton> clear;
    };

    OnlyConnectGUI::OnlyConnectGUI(GWindow& window) : ProblemHandler(window) {
        /* Standard buttons. */
        clear = Temporary<GButton>(new GButton("Clear Console"),  window, "WEST");

        /* Raw input. */
        inputLabel = Temporary<GLabel>(new GLabel("Enter text to convert: "), window, "SOUTH");

        inputText = Temporary<GTextField>(new GTextField(), window, "SOUTH");
        go        = Temporary<GButton>(new GButton("Go!"), window, "SOUTH");

        /* Display. */
        display   = Temporary<GColorConsole>(new GColorConsole(), window, "CENTER");
    }

    void processString(GColorConsole& out, const string& text) {
        out << "Input:  " << text << endl;
        out << "Output: ";

        out.doWithStyle("#000080", GColorConsole::BOLD, [&] {
            out << onlyConnectize(text) << endl << endl;
        });
    }

    void OnlyConnectGUI::actionPerformed(GObservable* source) {
        if (source == clear) {
            display->clear();
        } else if (source == go || source == inputText) {
            processString(*display, inputText->getText());
            inputText->setText("");
        }
    }
}

shared_ptr<ProblemHandler> makeOnlyConnectGUI(GWindow& window) {
    return make_shared<OnlyConnectGUI>(window);
}
