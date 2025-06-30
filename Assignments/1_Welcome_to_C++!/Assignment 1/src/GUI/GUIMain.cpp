#include "ProblemHandler.h"
#include "EmptyProblemHandler.h"
#include "gwindow.h"
#include "gevent.h"
#include "gbutton.h"
#include "goptionpane.h"
#include "gthread.h"
#include "map.h"
#include <chrono>
#include <cstdlib>
using namespace std;

namespace {
    /* Constructor type: Given a GWindow, build a problem handler. */
    using Constructor = std::function<std::shared_ptr<ProblemHandler> (GWindow &)>;

    /* An entry in the top-level GUI control bar. */
    struct GUIOption {
        std::string name;
        Constructor constructor;
    };
}

/* Use X Macro Trick to define all the menu options. */
static vector<GUIOption> kGUIOptions = {
    #define MENU_OPTION(name, constructor) {                           \
        name, [] (GWindow& window) {                                   \
            std::shared_ptr<ProblemHandler> constructor(GWindow &);    \
            return constructor(window);                                \
        }                                                              \
    },
    #define RUN_TESTS_MENU_OPTION() MENU_OPTION("Run Tests", makeTestingGUI)
    #define WINDOW_TITLE(title)
    #define TEST_ORDER(...)

    #include "../Demos/GUIConfig.h"

    #undef WINDOW_TITLE
    #undef RUN_TESTS_MENU_OPTION
    #undef MENU_OPTION
    #undef TEST_ORDER
};

/* Use X Macro Trick a second time to define the window title. */
static string kWindowTitle =
    #define MENU_OPTION(name, constructor)
    #define RUN_TESTS_MENU_OPTION()
    #define WINDOW_TITLE(title) title
    #define TEST_ORDER(...)
    #include "../Demos/GUIConfig.h"
    #undef WINDOW_TITLE
    #undef GUI_HANDLER
    #undef MENU_OPTION
    #undef TEST_ORDER
;

namespace {

    /* Window graphics constants. */
    const double kWindowWidth  = 1000;
    const double kWindowHeight = 800;

    /* Milliseconds before a timer event is considered "stale." */
    const long kTimelyCutoff = 100;

    /* Type representing all state necessary to make the graphics work. */
    struct Graphics {
        GWindow window{kWindowWidth, kWindowHeight}; // The window
        shared_ptr<ProblemHandler> handler;          // Current task handler
        Map<GObservable*, Constructor> constructors; // Map from buttons to constructors.
    };

    /* Creates the graphics window and associated state. */
    Graphics* makeGraphics() {
        auto* result = new Graphics();

        /* Basic window setup. */
        result->window.setTitle(kWindowTitle);
        result->window.setCloseOperation(GWindow::CLOSE_DO_NOTHING);

        /* Basic graphics setup. */
        result->window.setRepaintImmediately(false);
        result->window.setCanvasSize(kWindowWidth, kWindowHeight);

        /* Default handler. */
        result->handler = make_shared<EmptyProblemHandler>(result->window);

        /* Problem handlers. */
        for (const auto& entry: kGUIOptions) {
            auto* button = new GButton(entry.name);
            result->window.addToRegion(button, "NORTH");

            result->constructors[button] = entry.constructor;
        }
        return result;
    }

    /* Sets the active problem. */
    void setProblem(Graphics* graphics, GObservable* source) {
        /* See whether to abort the shutdown. */
        if (graphics->handler && !graphics->handler->shuttingDown()) {
            return;
        }

        auto constructor = graphics->constructors.get(source);
        if (!constructor) {
            error("No constructor for that GObservable?");
        }

        /* Dispose of the previous handler, clear the display, and
         * set up the new handler. This is done on the Qt GUI thread
         * so that there isn't any flicker.
         */
        GThread::runOnQtGuiThread([&] {
            graphics->handler.reset();
            graphics->window.clearCanvas();
            graphics->handler = constructor(graphics->window);
        });
        graphics->handler->settingUp();
    }

    Graphics* theGraphics;
    bool theOptionsEnabled = true;
}

/* Enable/disable all demo buttons. */
void setDemoOptionsEnabled(bool isEnabled) {
    for (GObservable* option: theGraphics->constructors) {
        if (auto* button = dynamic_cast<GButton*>(option)) {
            button->setEnabled(isEnabled);
        }
    }
    theOptionsEnabled = isEnabled;
}

int main() {
    theGraphics = makeGraphics();

    while (true) {
        /* Update the window (no-op if nothing needs to be redrawn.) */
        theGraphics->handler->draw();

        GEvent e = waitForEvent(MOUSE_EVENT | ACTION_EVENT | CHANGE_EVENT | TIMER_EVENT | WINDOW_EVENT);
        if (e.getEventClass() == ACTION_EVENT) {
            auto source = GActionEvent(e).getSource();

            /* We are responsible for the problem buttons. */
            if (theGraphics->constructors.containsKey(source)) {
                if (theOptionsEnabled) setProblem(theGraphics, source);
            }
            /* Any other event is the responsible of the problem handler. */
            else {
                theGraphics->handler->actionPerformed(source);
            }
        } else if (e.getEventClass() == CHANGE_EVENT) {
            theGraphics->handler->changeOccurredIn(GChangeEvent(e).getSource());
        } else if (e.getEventClass() == TIMER_EVENT) {
            /* If one of the handlers is responding slowly, it can cause a positive
             * feedback loop where timer events don't get pulled from the queue at the
             * rate at which they're generated, starving out higher-priority events.
             * To address this, if we pull a timer event out and it hasn't happened
             * sufficiently recently, we're going to assume we're running behind and
             * just swallow that event without processing it.
             */
            long now = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
            if (now - e.getTime() < kTimelyCutoff) {
                theGraphics->handler->timerFired();
            }
        } else if (e.getEventClass() == MOUSE_EVENT) {
            /* Make sure the event was generated from the canvas, not from one of the
             * interactors that happens to be in the window.
             */
            if (e.getSource() == theGraphics->window.getCanvas()) {
                if (e.getEventType() == MOUSE_MOVED) {
                    theGraphics->handler->mouseMoved(e.getX(), e.getY());
                } else if (e.getEventType() == MOUSE_PRESSED) {
                    theGraphics->handler->mousePressed(e.getX(), e.getY());
                } else if (e.getEventType() == MOUSE_DRAGGED) {
                    theGraphics->handler->mouseDragged(e.getX(), e.getY());
                } else if (e.getEventType() == MOUSE_RELEASED) {
                    theGraphics->handler->mouseReleased(e.getX(), e.getY());
                } else if (e.getEventType() == MOUSE_EXITED) {
                    theGraphics->handler->mouseExited();
                } else if (e.getEventType() == MOUSE_CLICKED) {
                    theGraphics->handler->mouseClicked(e.getX(), e.getY());
                } else if (e.getEventType() == MOUSE_DOUBLE_CLICKED) {
                    theGraphics->handler->mouseDoubleClicked(e.getX(), e.getY());
                }
            }
        } else if (e.getEventClass() == WINDOW_EVENT) {
            /* If this isn't for our window, we don't care about it. */
            if (e.getSource() == &theGraphics->window) {
                if (e.getEventType() == WINDOW_MAXIMIZED ||
                    e.getEventType() == WINDOW_RESIZED   ||
                    e.getEventType() == WINDOW_RESTORED) {
                    theGraphics->handler->windowResized();
                } else if (e.getEventType() == WINDOW_CLOSING && theGraphics->handler->shuttingDown()) {
                    theGraphics->handler.reset();
                    break;
                }
            }
        }
    }

    /* Tell the system to shut down when the window closes, then close the window. */
    theGraphics->window.setExitOnClose(true);
    theGraphics->window.close();
    return 0;
}
