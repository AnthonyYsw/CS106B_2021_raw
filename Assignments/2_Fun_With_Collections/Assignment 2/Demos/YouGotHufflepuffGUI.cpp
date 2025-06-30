#include "YouGotHufflepuff.h"
#include "GUI/MiniGUI.h"
#include "Demos/BarrierGUI.h"
#include "gthread.h"
#include "gtimer.h"
#include "filelib.h"
#include <fstream>
#include <iomanip>
#include <thread>
#include <mutex>
#include <condition_variable>
using namespace std;
using namespace MiniGUI;

namespace {
    /* Window padding to content area. */
    const double kPadding = 20;

    /* Fraction of the window allocated to the different sections. */
    const double kTitleFraction = 0.4;   // Of the whole window
    const double kMessageFraction = 0.5; // Of what's left

    /* Directory where data files live. */
    const string kBaseDir = "res/";

    /* File containing the personality questions. */
    const string kQuestionsFile = "AB5C.questions";

    /* Text parameters. */
    const Font kTitleFont  (FontFamily::SERIF, FontStyle::ITALIC, 24, "#808080");
    const Font kMessageFont(FontFamily::SERIF, FontStyle::BOLD,   48, "blue");
    const Font kFooterFont (FontFamily::SERIF, FontStyle::ITALIC, 24, "#808080");

    /* Number of questions to ask, by default. */
    const int kDefaultQuestionCount = 10;

    /* "Not selected" option. */
    const string kNotSelected = "-";

    const string kQuestionTitle = "How much do you agree with this statement?";

    class QuizGUI: public ProblemHandler, public enable_shared_from_this<QuizGUI> {
    public:
        QuizGUI(GWindow& window);
        ~QuizGUI();

        /* Hooks for student code. */
        void setMessage(const string& message);
        void setFooter(const string& message);
        int  askPersonalityQuestion(const Question& question);

        /* "Timer" is our hacky proxy way of saying "something happened." It's really
         * a user event, and honestly we should consider adding those in. :-)
         */
        void timerFired() override;

        /* Respond to button presses and the like. */
        void actionPerformed(GObservable* source) override;

    protected:
        void repaint() override;

    private:
        /* Bounds in which to draw. */
        GRectangle titleBounds;
        GRectangle messageBounds;
        GRectangle footerBounds;
        void computeBounds();

        /* The selected chrome option. */
        GContainer* currentContainer = nullptr;
        void installContainer(GContainer* container);

        void tryStartQuiz();
        void tryAcceptAnswer();

        bool quizStarted = false;

        /* Menu container: Shows a chooser for the questions file, the person
         * file, and the number of questions to ask.
         */
        GContainer* menuContainer   = nullptr;
        GComboBox*  personChooser   = nullptr;
        GCheckBox*  showFactors     = nullptr;
        GTextField* questionCount   = nullptr;
        GButton*    startQuiz       = nullptr;
        void makeMenuContainer();


        /* Likert container - shows a 1-5 Likert scale. */
        GContainer*   likertContainer = nullptr;
        GRadioButton* likertOptions[5] = { nullptr, nullptr, nullptr, nullptr, nullptr };

        /* Dummy radio button that, if selected, will deselect everything else. Otherwise,
         * we can't clear which one is checked.
         */
        GRadioButton* dummyButton = nullptr;

        GButton*      likertAccept = nullptr;
        void makeLikertContainer();

        /* Current title / message / footer to draw. This is guarded by dataLock. */
        string title;
        string message;
        string footer;

        void setFooter(const string& message, unique_lock<mutex>&);

        /* We communicate data into the student thread only through the interface that
         * lets us respond to a question. Here, we have the place where we write that
         * data, along with a mutex/condition variable guarding it.
         */
        int    mDataToStudent;
        bool   mDataReady = false;
        mutex  mDataLock;
        condition_variable mDataCV;

        /* Hacky hacky hacky: We use this timer as a way of generating events to wake us up
         * when the GUI needs to change. What we really need is some sort of "user event"
         * option that we could put into the event queue.
         */
        GTimer wakeUpTimer{1};
        void triggerRepaint();
    };


    /* Draws centered, multiline text. */
    void drawCenteredText(GWindow& window,
                          const string& text,
                          const GRectangle& bounds,
                          const Font& font) {
        /* Do two renders: one to position, and one to draw. */
        auto render = TextRender::construct(text, bounds, font);
        render->alignCenterHorizontally();
        render->draw(window);
    }

    /* Builds a file chooser for all files with the given extension. */
    GComboBox* fileChooserFor(const string& extension) {
        auto* result = new GComboBox();
        result->addItem(kNotSelected); // Initially, nothing shows
        for (const string& file: listDirectory(kBaseDir)) {
            if (endsWith(file, extension)) {
                result->addItem(file);
            }
        }
        result->setEditable(false);

        return result;
    }

    string factorsToCategories(const Map<char, int>& factors) {
        ostringstream builder;
        for (char ch: factors) {
            if (factors[ch] == -1) {
                builder << '-' << ch << ' ';
            } else if (factors[ch] == +1) {
                builder << '+' << ch << ' ';
            } else {
                builder << '(' << factors[ch] << ')' << ch << ' ';
            }
        }
        return builder.str();
    }

    /**
     * Asks the given personality question, returning a number between 1 and 5
     * indicating where on the scale from "Strongly Disagree" (1) to "Strongly
     * Agree" (5) the user lies.
     * <p>
     * We've implemented this function for you. You don't need to write any
     * code for this.
     *
     * @param question The question to ask.
     * @return A number between 1 and 5 indicating the user's preference.
     */
    int askPersonalityQuestion(const Question& question);



    /* Displays the user's OCEAN scores on the screen. The message will
     * persist until another set of OCEAN scores are displayed.
     */
    void displayScores(const Map<char, int>& scores);

    /* Displays the given message on the screen. The message will persist
     * until either another message is displayed or a question is asked.
     */
    void displayMessage(const std::string& message);

    /* Administers questions for the given personality quiz, returning the user's
     * answers.
     */
    Map<Question, int> giveQuestions(const Set<Question>& questions, int numQuestions) {
        Map<Question, int> result;

        Set<Question> available = questions;
        for (int i = 0; i < numQuestions; i++) {
            Question q = randomQuestionFrom(available);
            result[q] = askPersonalityQuestion(q);
        }

        return result;
    }

    /* Administers a personality quiz, reporting which person matches the user most
     * closely.
     */
    void administerQuiz(const Set<Question>& questions,
                        int numQuestions,
                        const Set<Person>& people) {
        auto answers = giveQuestions(questions, numQuestions);
        auto results = scoresFrom(answers);

        if (all_of(results.begin(), results.end(), [&](char ch) {
            return results[ch] == 0;
        })) {
            displayMessage("We don't have enough information to make a determination. Sorry!");
        } else {
            auto match   = mostSimilarTo(results, people);
            displayMessage("You got " + match.name + "!");
        }
        displayScores(results);
    }

    /* Lock around the GUI. It's really, really bad if there are many instances of
     * the GUI running at once.
     */
    mutex* theGUILock = new mutex; // Use a pointer so this isn't cleaned up at shutdown.
    QuizGUI* theGUI;

    QuizGUI::QuizGUI(GWindow& window) : ProblemHandler(window) {
        /* Lock should be held at this point; no need to guard. */
        theGUI = this;

        makeMenuContainer();
        makeLikertContainer();

        installContainer(menuContainer);
        computeBounds();
    }

    QuizGUI::~QuizGUI() {
        unique_lock<mutex> lock(*theGUILock);
        theGUI = nullptr;

        if (currentContainer) {
            window().remove(currentContainer);
        }

        delete likertContainer;
        delete menuContainer;
        delete dummyButton;
    }

    void QuizGUI::repaint() {
        clearDisplay(window(), "white");

        /* We have to lock the state so that it can't be changed during the redraw. */
        unique_lock<mutex> lock(mDataLock);
        drawCenteredText(window(), title,   titleBounds,   kTitleFont);
        drawCenteredText(window(), message, messageBounds, kMessageFont);
        drawCenteredText(window(), footer,  footerBounds,  kFooterFont);
    }

    void QuizGUI::setMessage(const string& message) {
        unique_lock<mutex> lock(mDataLock);

        /* Set the message. */
        title = "";
        this->message = message;
        triggerRepaint();
    }

    void QuizGUI::setFooter(const string& text) {
        unique_lock<mutex> lock(mDataLock);
        setFooter(text, lock);
    }

    void QuizGUI::setFooter(const string& text, unique_lock<mutex>&) {
        footer = text;
        triggerRepaint();
    }

    void QuizGUI::timerFired() {
        requestRepaint();
        wakeUpTimer.stop();
    }

    void QuizGUI::triggerRepaint() {
        requestRepaint();
        wakeUpTimer.start();
    }

    void QuizGUI::makeLikertContainer() {
        likertContainer = new GContainer(GContainer::LAYOUT_GRID);

        likertOptions[0] = new GRadioButton("Strongly disagree", "likert");
        likertOptions[1] = new GRadioButton("Disagree", "likert");
        likertOptions[2] = new GRadioButton("Neutral", "likert");
        likertOptions[3] = new GRadioButton("Agree", "likert");
        likertOptions[4] = new GRadioButton("Strongly agree", "likert");

        /* This dummy button exists purely so we can deselect things. */
        dummyButton = new GRadioButton("(dummy)", "likert");

        for (int i = 0; i < 5; i++) {
            likertContainer->addToGrid(likertOptions[i], 0, i);
        }

        likertAccept = new GButton("Select");
        likertContainer->addToGrid(likertAccept, 1, 2);
    }

    void QuizGUI::actionPerformed(GObservable* source) {
        if (source == startQuiz) {
            tryStartQuiz();
        } else if (source == likertAccept) {
            tryAcceptAnswer();
        }
    }

    void QuizGUI::tryStartQuiz() {
        /* If we're in a quiz, don't start another! */
        if (quizStarted) return;

        /* Validate data. */
        if (personChooser->getSelectedItem() == kNotSelected) return;

        int numQs;
        try {
            numQs = stringToInteger(questionCount->getText());
        } catch (const exception &) {
            return;
        }

        ifstream qFile(kBaseDir + kQuestionsFile);
        ifstream pFile(kBaseDir + personChooser->getSelectedItem());
        if (!qFile) error("Cannot open file " + kQuestionsFile + " for reading.");
        if (!pFile) error("Cannot open file " + personChooser->getSelectedItem() + " for reading.");

        Set<Question> questions;
        Set<Person> people;
        for (Question q; qFile >> q; questions += q);
        for (Person p;   pFile >> p; people += p   );

        if (questions.size() < numQs) return;

        /* We now are ready to start the quiz. */
        installContainer(nullptr);

        /* Because of the tricky multithreading issues involved here, we need to
         * lock down the rest of the option buttons on the top of the window.
         */
        setDemoOptionsEnabled(false);

        /* Mark that a quiz is beginning so we can't do this again. */
        quizStarted = true;

        /* Run the student code in its own detached thread. We make this thread
         * detached because this GUI can be destructed in one of two ways:
         *
         * 1. The student's code has finished running normally, and we're done. In that
         *    case, the thread's already finished, and nothing happens.
         * 2. The program is being terminated. In that case, we don't want to block until
         *    the thread finishes running, because that will hang the program.
         */
        thread([questions, people, numQs] {
            administerQuiz(questions, numQs, people);
            GThread::runOnQtGuiThread([] {
                setDemoOptionsEnabled(true);
            });
        }).detach();
    }

    void QuizGUI::tryAcceptAnswer() {
        /* See which button, if any, is selected. */
        int selected = 0;
        for (int i = 0; i < 5; i++) {
            if (likertOptions[i]->isSelected()) {
                selected = i+1; // 1- versus 0-indexing.
            }
        }

        if (selected != 0) {
            {
                /* Get the data lock so we don't trash anything. */
                unique_lock<mutex> lock(mDataLock);
                mDataToStudent = selected;
                mDataReady = true;
            }
            installContainer(nullptr);
            mDataCV.notify_one();
        }
    }

    /* Compute the bounds of the window regions. */
    void QuizGUI::computeBounds() {
        /* Get the main bounding rectangle. */
        GRectangle all = {
            kPadding, kPadding,
            window().getCanvasWidth() - 2 * kPadding,
            window().getCanvasHeight() - 2 * kPadding
        };

        titleBounds = {
            all.x, all.y,
            all.width, all.height * kTitleFraction
        };

        double remainingHeight = all.height - titleBounds.height;
        messageBounds = {
            all.x, titleBounds.y + titleBounds.height,
            all.width, remainingHeight * kMessageFraction
        };

        double footerHeight = remainingHeight - messageBounds.height;
        footerBounds = {
            all.x, messageBounds.y + messageBounds.height,
            all.width, footerHeight
        };
    }

    int askPersonalityQuestion(const Question& question) {
        unique_lock<mutex> guiLock(*theGUILock);
        if (!theGUI) {
            /* If this is triggered, it means there's no GUI, so this function was called
             * from somewhere it wasn't supposed to be called from.
             */
            error("The askPersonalityQuestion function only works when you select the\n"
                  "\"You Got Hufflepuff!\" option from the main window. If you're\n"
                  "seeing this error, it probably means that either (1) you tried\n"
                  "calling askPersonalityQuestion from Rising Tides, which isn't supported,\n"
                  "or (2) you tried calling askPersonalityQuestion from a test case,\n"
                  "which isn't supported.");
        }

        return theGUI->askPersonalityQuestion(question);
    }

    int QuizGUI::askPersonalityQuestion(const Question& question) {
        {
            unique_lock<mutex> lock(mDataLock);

            title = kQuestionTitle;
            message = question.questionText;

            /* Data has not yet been received. */
            mDataReady = false;

            if (showFactors->isChecked()) {
                setFooter(factorsToCategories(question.factors), lock);
            } else {
                setFooter("", lock);
            }
        }

        /* Clean and install the Likert scale. This cannot be done when holding the
         * data lock because it acquires the GUI lock, and we have to enforce the
         * ordering that the GUI lock can get the data lock but not the other way
         * around.
         *
         * That's okay, though, because we'll just be chilling and waiting for data
         * to come in.
         */
        dummyButton->setChecked(true);
        installContainer(likertContainer);
        triggerRepaint();

        /* Wait for data to be available. */
        int result;
        {
            unique_lock<mutex> lock(mDataLock);
            mDataCV.wait(lock, [this] {
                return mDataReady;
            });
            result = mDataToStudent;
            title = message = "";
        }
        triggerRepaint();

        return result;
    }

    void QuizGUI::installContainer(GContainer* container) {
        if (currentContainer) window().remove(currentContainer);
        currentContainer = container;
        if (currentContainer) window().addToRegion(currentContainer, "SOUTH");
    }

    void displayMessage(const string& message) {
        unique_lock<mutex> guiLock(*theGUILock);
        if (!theGUI) {
            /* If this is triggered, it means there's no GUI, so this function was called
             * from somewhere it wasn't supposed to be called from.
             */
            error("The displayMessage function only works when you select the\n"
                  "\"You Got Hufflepuff!\" option from the main window. If you're\n"
                  "seeing this error, it probably means that either (1) you tried\n"
                  "calling displayMessage from Rising Tides, which isn't supported,\n"
                  "or (2) you tried calling displayMessage from a test case,\n"
                  "which isn't supported.");
        }

        theGUI->setMessage(message);
    }

    void displayScores(const Map<char, int>& scores) {
        unique_lock<mutex> guiLock(*theGUILock);
        if (!theGUI) {
            /* If this is triggered, it means there's no GUI, so this function was called
             * from somewhere it wasn't supposed to be called from.
             */
            error("The displayScores function only works when you select the\n"
                  "\"You Got Hufflepuff!\" option from the main window. If you're\n"
                  "seeing this error, it probably means that either (1) you tried\n"
                  "calling displayScores from Rising Tides, which isn't supported,\n"
                  "or (2) you tried calling displayScores from a test case,\n"
                  "which isn't supported.");
        }

        ostringstream builder;
        builder << "Your Scores:" << '\n';
        for (char ch: string("OoCcEeAaNn")) {
            if (scores.containsKey(ch)) {
                builder << ch << ": " << scores[ch] << '\n';
            }
        }

        theGUI->setFooter(builder.str());
    }

    void QuizGUI::makeMenuContainer() {
        personChooser   = fileChooserFor(".people");
        questionCount   = new GTextField(to_string(kDefaultQuestionCount));
        showFactors     = new GCheckBox("Show Factors on Questions");
        startQuiz       = new GButton("Start Quiz");

        menuContainer = new GContainer(GContainer::LAYOUT_GRID);

        menuContainer->addToGrid(new GLabel("People File: "), 0, 0);
        menuContainer->addToGrid(personChooser, 0, 1);
        menuContainer->addToGrid(new GLabel("Number of Questions: "), 0, 2);
        menuContainer->addToGrid(questionCount, 0, 3);
        menuContainer->addToGrid(showFactors, 0, 4);
        menuContainer->addToGrid(startQuiz, 1, 2);
    }
}

/* There should only ever be a single GUI, so if we need to make a copy and one already
 * exists, we should just hand that one back instead.
 */
shared_ptr<ProblemHandler> makeQuizGUI(GWindow& window) {
    return ifPassedThen(window, "YouGotHufflepuff.cpp", [](GWindow& window) {
        unique_lock<mutex> lock(*theGUILock);
        if (theGUI) return theGUI->shared_from_this();

        /* Sets theGUI internally. */
        return make_shared<QuizGUI>(window);
    });
}
