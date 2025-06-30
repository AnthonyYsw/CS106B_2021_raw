#include "GUI/MiniGUI.h"
#include "HeapPQueue.h"
#include "TopK.h"
#include "Utilities/CSV.h"
#include "ginteractors.h"
#include "simpio.h"
#include "hashmap.h"
#include "vector.h"
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <fstream>
#include "strlib.h"
#include "filelib.h"
#include "set.h"
using namespace std;
using namespace MiniGUI;

namespace {
    const string kBaseDirectory = "res/";
    const int kGraphicsResultsDisplayed = 16;
    const int kConsoleResultsDisplayed = 5;

    /* Background and foreground colors are designed to look like water. */
    const char kBackgroundColor[] = "#007FFF"; // Azure
    const char kForegroundColor[] = "#E1EBEE"; // Marian Blue

    /* General text description. */
    const string kDescription =
        "This visualization shows the progression in times for the Women's 800m Freestyle "
        "swimming event over time. As you change which year is displayed, you "
        "can see the " + to_string(kGraphicsResultsDisplayed) + " fastest times recorded up to "
        "the given year at a collection of major sporting events. You may recognize some "
        "of the names that come up in this list!\n"
        "Data is taken from the International Olympic Committee and FINA.";
    const Font kDescriptionFont(FontFamily::SANS_SERIF, FontStyle::BOLD, 16, kForegroundColor);
    const double kDescriptionHeight = 275;

    const double kWindowPadding = 15;
    const double kResultsPadding = 5;

    /* Graphics information for displaying results. */
    const char kGoldColor[]   = "#FFD700";
    const char kSilverColor[] = "#C0C0C0";
    const char kBronzeColor[] = "#CD7F32";
    const char kOtherColor[]  = "#555D50"; // Ebony

    const string kResultColor = kForegroundColor;
    const string kBorderColor = kForegroundColor;
    const Font kResultFont(FontFamily::SANS_SERIF, FontStyle::BOLD, 20, kGoldColor);

    /* Type representing a race time. */
    struct RaceTime {
        int minutes;
        int seconds;
        int hundredths;
    };

    /* Type representing a swimmer's time. */
    struct SwimResult {
        int year;
        std::string event;
        std::string swimmer;
        std::string country;
        RaceTime time;
    };

    /* Given a string of the form MM:SS.HH, parses it into minutes, seconds, and hundredths. */
    RaceTime parseTime(const string& time) {
        istringstream extractor(time);
        char colon, dot;

        RaceTime result;
        extractor >> result.minutes >> colon >> result.seconds >> dot >> result.hundredths;

        if (!extractor || colon != ':' || dot != '.') error("Invalid time: " + time);

        char leftover;
        if (extractor >> leftover) error("Unexpected character: " + string(1, leftover));

        return result;
    }

    /* Give a base directory, returns all the swim records from the CSV files in that
     * directory.
     */
    Vector<SwimResult> parseCSVsIn(const string& baseDir) {
        Vector<SwimResult> allData;

        /* Pull up all CSV files from the base directory. */
        for (string filename: listDirectory(baseDir)) {
            /* If this isn't a CSV file, skip it. */
            if (!endsWith(filename, ".800m.csv")) continue;

            /* Otherwise, pull our data. */
            CSV data = CSV::parseFile(baseDir + filename);

            Vector<SwimResult> result;
            for (size_t row = 0; row < data.numRows(); row++) {
                result.add({
                    stringToInteger(data[row]["Year"]),
                    data[row]["Event"],
                    data[row]["Athlete"],
                    data[row]["Country"],
                    parseTime(data[row]["Time"])
                });
            }

            allData += result;
        }
        return allData;
    }

    /* Data points encoding all the results. */
    struct DataSet {
        /* Master data set, sorted by year. */
        Vector<SwimResult> results;

        /* Data points corresponding to the swim results. Weights are years, keys are
         * indices into mResults.
         */
        Vector<DataPoint> yearPoints;

        /* Data points corresponding to the swim times. Weights are times measured in centiseconds,
         * keys are indices into mResults.
         */
        Vector<DataPoint> timePoints;
    };


    /* Given a RaceTime, expresses that RaceTime in centiseconds. */
    int toCentiseconds(const RaceTime& time) {
        return time.hundredths +
               100 * time.seconds +
               100 * 60 * time.minutes;
    }

    DataSet loadData(const string& baseDir) {
        /* All the data points we have. */
        Vector<SwimResult> allData = parseCSVsIn(baseDir);

        /* We how have a bunch of time series. We can sort them by year by dropping
         * them all in a priority queue (key = index, value = year) and pulling them
         * back out.
         */
        HeapPQueue pq;
        for (int i = 0; i < allData.size(); i++) {
            pq.enqueue({ to_string(i), allData[i].year });
        }

        /* Build our result from the priority queue data. */
        DataSet result;
        while (!pq.isEmpty()) {
            result.results += allData[stringToInteger(pq.dequeue().name)];
        }

        /* Build searchable lists. */
        for (int i = 0; i < result.results.size(); i++) {
            result.yearPoints.add({ to_string(i), result.results[i].year });

            /* Notice that we store the negated times, which means that lower times are
             * considered better than higher times.
             */
            result.timePoints.add({ to_string(i), -toCentiseconds(result.results[i].time) });
        }
        return result;
    }

    /* Comparison function for use in lower_bound. */
    struct YearComp {
        bool operator()(const DataPoint& lhs, const DataPoint& rhs) {
            return lhs.weight < rhs.weight;
        }
    };

    /* Problem handler that lets the user progressions in the Women's 800m Freestyle
     * swimming event at a collection of sporting events.
     */
    class Womens800MGUI: public ProblemHandler {
    public:
        Womens800MGUI(GWindow& window);

        void changeOccurredIn(GObservable* source) override;

    protected:
        void repaint() override;

    private:
        /* Graphics components. */
        Temporary<GSlider> mYearSlider;

        /* Actively-displayed swim results. */
        Vector<SwimResult> mShown;

        /* Currently-displayed year. */
        int mYear = -1;

        /* Underlying data. */
        DataSet data;

        /* Updates the display to show the best results up through a given year. */
        void showBestTimesThrough(int year);
    };

    Womens800MGUI::Womens800MGUI(GWindow& window): ProblemHandler(window) {
        data = loadData(kBaseDirectory);

        mYearSlider = makeYearSlider(window, data.results.first().year, data.results.last().year);

        /* Update the display. */
        showBestTimesThrough(data.results.first().year);
    }

    void Womens800MGUI::changeOccurredIn(GObservable* source) {
        if (source == mYearSlider) {
            showBestTimesThrough(mYearSlider->getValue());
        }
    }

    Vector<SwimResult> bestTimesThrough(const DataSet& data, int year, int numResults) {
        /* Find the first event that occurs after the indicated year. That will be our
         * stop point.
         */
        int endPoint = lower_bound(data.yearPoints.begin(), data.yearPoints.end(), DataPoint{ "", year + 1 }, YearComp()) - data.yearPoints.begin();

        stringstream stream;
        for (int i = 0; i < endPoint; i++) {
            stream << data.timePoints[i];
        }

        /* Find the winners. */
        Vector<SwimResult> result;
        for (auto point: topK(stream, numResults)) {
            result += data.results[stringToInteger(point.name)];
        }
        return result;
    }

    void Womens800MGUI::showBestTimesThrough(int year) {
        /* If the year hasn't changed, don't do anything. */
        if (year == mYear) return;
        mYear = year;

        mShown = bestTimesThrough(data, year, kGraphicsResultsDisplayed);
        requestRepaint();
    }

    vector<string> displayViewOf(const Vector<SwimResult>& results) {
        vector<string> swimmerList;
        for (auto result: results) {
            ostringstream builder;
            builder << result.year << ": "
                    << setfill('0') << setw(2) << result.time.minutes << ":"
                    << setw(2) << result.time.seconds << "."
                    << setw(2) << result.time.hundredths
                    << " by " << result.swimmer << " (" << result.country << ")"
                    << " at the " << result.event << endl;
            swimmerList.push_back(builder.str());
        }
        return swimmerList;
    }

    void Womens800MGUI::repaint() {
        /* Clear the display. */
        clearDisplay(window(), kBackgroundColor);

        /* Draw header text. */
        auto header = TextRender::construct(kDescription + "\n\nYear: " + to_string(mYear), {
                                                kWindowPadding, kWindowPadding,
                                                window().getCanvasWidth() - 2 * kWindowPadding,
                                                kDescriptionHeight - 2 * kWindowPadding
                                            }, kDescriptionFont);
        header->draw(window());

        /* Assemble the list of results to display. */
        vector<string> swimmerList = displayViewOf(mShown);

        /* Assemble a list of colors for the markers. */
        vector<string> colorList = { kGoldColor, kSilverColor, kBronzeColor };
        while (colorList.size() < swimmerList.size()) {
            colorList.push_back(kOtherColor);
        }

        /* Draw the result. */
        auto bounds = header->bounds();
        double baseY = bounds.y + bounds.height + kResultsPadding;
        LegendRender::construct(swimmerList, colorList, {
                                    bounds.x, baseY,
                                    bounds.width, window().getCanvasHeight() - baseY - kResultsPadding
                                }, colorList, kResultFont, kBorderColor)->draw(window());
    }
}

GRAPHICS_HANDLER("Womens 800m Freestyle", GWindow& window) {
    return make_shared<Womens800MGUI>(window);
}

CONSOLE_HANDLER("Womens 800m Freestyle") {
    cout << kDescription << endl;
    auto data = loadData(kBaseDirectory);
    int lowYear  = data.yearPoints.first().weight;
    int highYear = data.yearPoints.last().weight;

    do {
        cout << "We have data ranging from " << lowYear << " to " << highYear << "." << endl;
        int year = getIntegerBetween("Show data up through which year?", lowYear, highYear);

        auto results = bestTimesThrough(data, year, kConsoleResultsDisplayed);

        cout << "Fastest times recorded up through the year " << year << ": " << endl;
        for (string text: displayViewOf(results)) {
            cout << text << endl;
        }

    } while (getYesOrNo("See results from another year? "));
}
