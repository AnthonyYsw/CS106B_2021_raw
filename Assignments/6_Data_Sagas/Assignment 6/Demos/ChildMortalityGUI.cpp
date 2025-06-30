#include "GUI/MiniGUI.h"
#include "DataPoint.h"
#include "ginteractors.h"
#include "hashmap.h"
#include "simpio.h"
#include "TopK.h"
#include "Utilities/CSV.h"
#include "vector.h"
#include <algorithm>
#include <functional>
#include <vector>
#include <iomanip>
#include <sstream>
#include <ctime>
#include "urlstream.h"
#include "strlib.h"
#include "gthread.h"
using namespace std;
using namespace MiniGUI;

namespace {
    const string kDataFile = "res/UN-ChildMortality.csv";
    constexpr int kNumDisplayedCountries = 7;

    const char kBackgroundColor[] = "#FFFFFF"; // White
    const char kForegroundColor[] = "#5b92e5"; // United Nations blue

    /* The mortality rates are stored as doubles, but data points require integer values.
     * We'll fix this by using this conversion factor, which scales up the real numbers
     * to a range where they could operate as integers without loss in precision.
     */
    const double kIntegerScalar = 1000.0;

    /* Header constants. */
    const string kHeaderText =
        "As part of its Millennium Development Goals, the United Nations set a goal of reducing "
        "child mortality (defined as mortality for children ages 0 - 5) by 2015 to a level "
        "two-thirds that of the levels in 1990.\n"
        "This tool plots child mortality rates by country as of 2013, the most recent year "
        "for which the United Nations has released data.\n"
        "Numbers are expressed in child mortality per 1,000 live births. Data is taken from the "
        "United Nations.";

    const Font kHeaderFont(FontFamily::SERIF, FontStyle::BOLD_ITALIC, 16, kForegroundColor);

    /* Window boundary padding. */
    const double kWindowPadding = 10;

    /* Padding between bottom of the header and top of the graph. */
    const double kHeaderPadding = 10;

    /* Height of the heading. */
    const double kHeaderHeight = 200;

    /* Spacing from the graph bounding box to the axes. */
    const double kYAxisPadding = 10;
    const double kXAxisPadding = 10;

    /* Increments to use going up the side. */
    const int kMajorTickSize  = 50;
    const int kNumMinorTicks = 4;
    const Font kAxisFont(FontFamily::MONOSPACE, FontStyle::BOLD, 13, kForegroundColor);

    /* Max and min values for the side. */
    const int kMaxYValue      = 350;

    /* Plotted line constants. */
    const vector<string> kPlottedLineColors = {
        "#343434", // Jet
        "#B53389", // Fandango
        "#007BA7", // Cerulean
        "#008000", // Office Green
        "#FFC40C", // Mikado Yellow
        "#EE7F2D", // Princeton Orange
        "#CC0000", // Rosso Corsa
    };

    /* Legend Constants */
    const double kLegendWidth         = 300;
    const double kLegendHeight        = 200;
    const char kLegendFontColor[]     = "#2D4972"; // Darker United Nations blue
    const Font kLegendFont(FontFamily::SERIF, FontStyle::ITALIC, 14, kLegendFontColor);

    const string kLegendBorderColor   = kLegendFontColor;

    /* Loaded data from the CSV file. */
    struct DataSet {
        /* Start/end year. */
        int lowYear;
        int highYear;

        /* Country -> Year past lowYear -> Child mortality rates. */
        HashMap<string, Vector<double>> data;
    };

    /* Name of a country plus data set -> numeric score for that country. */
    using Scorer = function<int (const string&, const DataSet& data)>;

    /* Formatter function: takes in a DataPoint and returns a human-readable string
     * explaining that data point.
     */
    using Formatter = function<string (const DataPoint &, const DataSet& data)>;

    /* Loads the mortality rate data. */
    DataSet loadData(const string& filename) {
        ifstream input(filename);
        if (!input) error("Can't open data file " + filename);

        CSV data = CSV::parse(input);
        DataSet result;

        /* First column is country name, rest are data points. */
        for (size_t row = 0; row < data.numRows(); row++) {
            Vector<double> entries;
            for (size_t col = 1; col < data.numCols(); col++) {
                entries += stringToReal(data[row][col]);
            }
            result.data[data[row][0]] = entries;
        }

        auto headers = data.headers();
        result.lowYear  = stringToInteger(headers[1]);
        result.highYear = stringToInteger(headers[headers.size() - 1]);

        return result;
    }

    struct PlotInformation {
        string name;
        Scorer scorer;
        Formatter formatter;

        /* Export this one to the console? */
        bool forConsole;
    };

    const vector<PlotInformation> kAllPlots = {
        {
            "Lowest as of 2013",
            [](const string& country, const DataSet& data) {
                return -kIntegerScalar * data.data[country].last();
            },
            [] (const DataPoint& point, const DataSet&) {
                ostringstream builder;
                builder << point.name << " (" << -point.weight / kIntegerScalar << ")";
                return builder.str();
            },
            true
        },
        {
            "Highest as of 2013",
            [] (const string& country, const DataSet& data) {
                return kIntegerScalar * data.data[country].last();
            },
            [] (const DataPoint& point, const DataSet&) {
                ostringstream builder;
                builder << point.name << " (" << point.weight / kIntegerScalar << ")";
                return builder.str();
            },
            true
        },
        {
            "Most Improved (Absolute)",
            [] (const string& country, const DataSet& data) {
                return kIntegerScalar * (data.data[country].first() - data.data[country].last());
            },
            [] (const DataPoint& point, const DataSet&) {
                ostringstream builder;
                builder << point.name << " (-" << point.weight / kIntegerScalar << ")";
                return builder.str();
            },
            true
        },
        {
            "Most Improved (Relative)",
            [] (const string& country, const DataSet& data) {
                double now  = data.data[country].last();
                double then = data.data[country].first();
                return kIntegerScalar * (then - now) / then;
            },
            [] (const DataPoint& point, const DataSet& data) {
                double now  = data.data[point.name].last();
                double then = data.data[point.name].first();
                double improvement = 100.0 * (then - now) / then;

                ostringstream builder;
                builder << point.name << " (-" << fixed << setprecision(2) << improvement << "%)";
                return builder.str();
            },
            true
        },
        {
            "Randomly Selected Countries",
            [] (const string &, const DataSet &) {
                return rand();
            },
            [] (const DataPoint& point, const DataSet& ) {
                return point.name;
            },
            false
        }
    };

    /* Problem handler that lets the user see how child mortality rates have improved
     * since 1990.
     */
    class ChildMortalityGUI: public ProblemHandler {
    public:
        ChildMortalityGUI(GWindow& window);

        void actionPerformed(GObservable* source) override;

    protected:
        void repaint() override;

    private:
        /* Graphics components. */
        vector<Temporary<GButton>> buttons;
        Map<GObservable*, const PlotInformation*> plots;

        /* Actively-displayed countries. */
        Vector<DataPoint> mCountries;

        Formatter mFormatter;
        DataSet data;

        /* Renderer for the header. We cache this to avoid recomputing it unnecessarily. */
        std::shared_ptr<TextRender> mHeader;

        void drawGraph(GWindow& window, const GRectangle& bounds) const;
        void drawPlot(GWindow& window, const GRectangle& bounds) const;
        void drawLegend(GWindow& window, const GRectangle& bounds) const;
    };


    ChildMortalityGUI::ChildMortalityGUI(GWindow& window) : ProblemHandler(window){
        for (const auto& plot: kAllPlots) {
            buttons.push_back(make_temporary<GButton>(window, "SOUTH", plot.name));
            plots[buttons.back().get()] = &plot;
        }

        data = loadData(kDataFile);

        /* Set up graphics. */
        mHeader = TextRender::construct(kHeaderText, {
                                            kWindowPadding, kWindowPadding,
                                            window.getCanvasWidth() - 2 * kWindowPadding, kHeaderHeight - 2 * kWindowPadding
                                        }, kHeaderFont);
    }

    /* Given a scorer, return the best countries as scored by that metric. */
    Vector<DataPoint> bestCountriesBy(const DataSet& data, Scorer scorer) {
        /* Form a data stream from the list of all countries that populates data points
         * with the criterion.
         */
        stringstream stream;
        for (const auto& countryName: data.data) {
            stream << DataPoint{ countryName, scorer(countryName, data) };
        }

        /* Find the best countries by this metric. */
        return topK(stream, kNumDisplayedCountries);
    }

    void ChildMortalityGUI::actionPerformed(GObservable* source) {
        if (plots.containsKey(source)) {
            mFormatter = plots[source]->formatter;
            mCountries = bestCountriesBy(data, plots[source]->scorer);
            requestRepaint();
        }
    }

    void ChildMortalityGUI::repaint() {
        clearDisplay(window(), kBackgroundColor);

        /* Draw the header message. */
        mHeader->draw(window());

        /* From that, compute the bounding rectangle for our graph. */
        double headerBottom = mHeader->bounds().y + mHeader->bounds().height;
        GRectangle graphArea = {
            kWindowPadding, headerBottom + kHeaderPadding,
            window().getCanvasWidth() - 2 * kWindowPadding,
            window().getCanvasHeight() - kWindowPadding - headerBottom - kHeaderPadding
        };

        drawGraph(window(), graphArea);
    }

    void ChildMortalityGUI::drawGraph(GWindow& window, const GRectangle& bounds) const {
        /* Compute the graph content area. This is the area that the lines in the graph will
         * be plotted in.
         */
        GRectangle graphContent = {
            bounds.x + kXAxisPadding,
            bounds.y + kYAxisPadding,
            bounds.width  - 2 * kXAxisPadding,
            bounds.height - 2 * kYAxisPadding
        };

        drawPlot(window, graphContent);
        drawLegend(window, graphContent);
    }

    void ChildMortalityGUI::drawPlot(GWindow& window, const GRectangle& bounds) const {
        /* Assemble our axis labels. */
        vector<string> xAxisLabels = { "" }; // Don't plot first year
        for (int year = data.lowYear + 1; year <= data.highYear; ++year) {
            xAxisLabels.push_back("'" + to_string(year % 100));
        }
        vector<string> yAxisLabels;
        for (int value = 0; value <= kMaxYValue; value += kMajorTickSize) {
            yAxisLabels.push_back(to_string(value));
        }

        /* Assemble our lines to draw. */
        vector<vector<GPoint>> lines;
        for (const auto& country: mCountries) {
            vector<GPoint> line;
            for (int i = 0; i < data.data[country.name].size(); i++) {
                /* Coordinates should be in [0, 0] x [1, 1]. */
                double x = double(i) / (data.data[country.name].size() - 1);
                double y = data.data[country.name][i] / kMaxYValue;
                line.push_back({ x, y });
            }
            lines.push_back(line);
        }

        /* Draw everything! */
        LineGraphRender::construct(lines, xAxisLabels, yAxisLabels, 0, kNumMinorTicks, bounds,
                                   kAxisFont, kAxisFont, kPlottedLineColors, kForegroundColor)->draw(window);
    }

    void ChildMortalityGUI::drawLegend(GWindow& window, const GRectangle& bounds) const {
        /* If there's nothing selected, don't draw anything. */
        if (mCountries.isEmpty()) return;

        vector<string> labels;
        for (const auto& entry: mCountries) {
            labels.push_back(mFormatter(entry, data));
        }

        auto legend = LegendRender::construct(labels, kPlottedLineColors, {
                                    bounds.x + bounds.width - kLegendWidth, bounds.y,
                                    kLegendWidth, kLegendHeight
                                }, kLegendFont, kLegendBorderColor);
        window.setColor(kBackgroundColor);
        window.fillRect(legend->computedBounds());
        legend->draw(window);
    }
}

GRAPHICS_HANDLER("Child Mortality", GWindow& window) {
    return make_shared<ChildMortalityGUI>(window);
}

CONSOLE_HANDLER("Child Mortality") {
    Vector<string> options;
    for (const auto& plot: kAllPlots) {
        if (plot.forConsole) options += plot.name;
    }

    cout << kHeaderText << endl;

    auto data = loadData(kDataFile);
    do {
        int option = makeSelectionFrom("Which countries do you want to explore by child mortality rate?", options);

        /* Run the plot. */
        auto plot = kAllPlots[option];
        auto best = bestCountriesBy(data, plot.scorer);

        /* Print out the most recent data points we have for those countries.
         * TODO: We could print the whole time series, but that seems like it might
         * overrun the console?
         */
        for (const auto& pt: best) {
            cout << plot.formatter(pt, data) << endl;
        }
    } while (getYesOrNo("Explore more countries? "));
}
