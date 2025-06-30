#include "GUI/MiniGUI.h"
#include "DoctorsWithoutOrdersParser.h"
#include <fstream>
#include <memory>
#include <string>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <vector>
#include "filelib.h"
#include "simpio.h"
#include "strlib.h"
#include "gthread.h"
using namespace std;
using namespace MiniGUI;

namespace {
    /* File constants. */
    const string kProblemSuffix = ".dwo";
    const string kBasePath = "res/";

    const string kBackgroundColor = "White";

    /* Text box information. */
    const double kBoxWidth       = 250;
    const double kBoxHeight      = 45;

    /* Global top/bottom padding. */
    const double kVerticalPadding = kBoxHeight / 2.0;

    /* Spacing between columns. */
    const double kColumnSpacing = 200;

    /* Different colors for matched and unmatched boxes. */
    const string kMatchedBoxColor       = "#F3E5AB"; // Vanilla
    const string kMatchedBoxBorderColor = "Black";
    const Font   kMatchedBoxFont(FontFamily::SANS_SERIF, FontStyle::BOLD, 13, "Black");

    const string kUnusedBoxColor       = "White";
    const string kUnusedBoxBorderColor = "Gray";
    const Font   kUnusedBoxFont(FontFamily::SANS_SERIF, FontStyle::NORMAL, 13, "Gray");

    /* Line information. */
    const string kUnusedLineColor      = "#C0C0C0";
    const string kMatchedLineColor     = "Blue";
    const double kMatchedLineThickness = 5;

    /* Lays out a single column. The X coordinate of the left side of the boxes is given
     * as input, along with the total window height.
     */
    template <typename PersonType>
    void layOutColumn(const Vector<PersonType>& nodes, Map<string, GRectangle>& result,
                      double leftX, double height) {
        /* Edge case: If there are no nodes, don't do anything. */
        if (nodes.size() == 0) return;

        /* Edge case: If there is one node, center it vertically. */
        if (nodes.size() == 1) {
            result[nodes[0].name] = { leftX, (height - kBoxHeight) / 2.0, kBoxWidth, kBoxHeight };
            return;
        }

        /* Adjust for the top/bottom margins. */
        double columnHeight = height - 2 * kVerticalPadding;

        /* Compute the spacing between boxes. */
        double boxSpacing = (columnHeight - kBoxHeight * nodes.size()) / (nodes.size() - 1);

        /* The base Y coordinate is the quantity that centers all these boxes. */
        double topY = (height - columnHeight) / 2.0;

        /* Lay out all the nodes. */
        int index = 0;
        for (const auto& node: nodes) {
            result[node.name] = { leftX, topY + (kBoxHeight + boxSpacing) * index, kBoxWidth, kBoxHeight };
            index++;
        }
    }

    /* Given a graph, determines the location of each node. */
    Map<string, GRectangle> layOutNodes(GWindow& window, const HospitalTestCase& hospital) {
        /* Determine the locations of the left and right columns. The entire
         * arrangement will be centered in the window with the appropriate
         * buffer spacing between.
         */
        double centerX = window.getCanvasWidth() / 2.0;
        double height  = window.getCanvasHeight();

        Map<string, GRectangle> result;
        layOutColumn(hospital.doctors,  result, centerX - kColumnSpacing / 2.0 - kBoxWidth, height);
        layOutColumn(hospital.patients, result, centerX + kColumnSpacing / 2.0, height);

        return result;
    }

    /* Draw edges with the specified graphics properties. */
    void drawEdges(GWindow& window, const Map<string, Set<string>>& edges,
                   const Map<string, GRectangle>& nodeLocations,
                   const string& color, double thickness) {
        GLine line(0, 0, 0, 0);
        line.setLineWidth(thickness);
        line.setColor(color);

        for (const auto& src: edges) {
            for (const auto& dst: edges[src]) {
                /* Line originates on the right-hand side of the doctor node and is vertically
                 * centered.
                 */
                auto doctorPos = nodeLocations[src];
                double x0 = doctorPos.x + doctorPos.width;
                double y0 = doctorPos.y + doctorPos.height / 2.0;

                /* Line ends on the left-hand side of the patient node and is vertically
                 * centered.
                 */
                auto patientPos = nodeLocations[dst];
                double x1 = patientPos.x;
                double y1 = patientPos.y + patientPos.height / 2.0;

                line.setLocation(x0, y0);
                line.setEndPoint(x1, y1);
                window.draw(&line);
            }
        }
    }

    /* Draws each node in a nice pretty box. */
    void drawNodes(GWindow& window,
                   const Set<string>& toDraw,
                   const Map<string, GRectangle>& nodeLocations,
                   const Map<string, string>& nodeModifiers,
                   const string& boxColor, const string& borderColor,
                   const Font& font) {
        for (const auto& node: toDraw) {
            auto location = nodeLocations[node];

            /* Draw the background. */
            window.setColor(boxColor);
            window.fillRect(location);

            /* Draw the border. */
            window.setColor(borderColor);
            window.drawRect(location);

            /* Draw the text itself. */
            auto text = TextRender::construct(node + "\n" + nodeModifiers[node], {
                                             location.x, location.y,
                                             location.width, location.height
                                         }, font);
            text->alignCenterVertically();
            text->alignCenterHorizontally();
            text->draw(window);
        }
    }

    /* Converts a list of doctors or patients into a set. */
    template <typename Container>
    Set<string> asSet(const Container& c) {
        Set<string> result;
        for (const auto& elem: c) {
            result += elem.name;
        }
        return result;
    }

    /* Returns a list of all sample problems associated with this problem type. */
    vector<string> sampleProblems(const string& basePath) {
        vector<string> result;
        for (const auto& file: listDirectory(basePath)) {
            if (endsWith(file, kProblemSuffix)) {
                result.push_back(file);
            }
        }
        return result;
    }

    /* Problem handler to play around with the Doctors Without Orders
     * problem on some synthetic data sets.
     */
    class DoctorGUI: public ProblemHandler {
    public:
        DoctorGUI(GWindow& window);

        void actionPerformed(GObservable* source) override;
        void changeOccurredIn(GObservable* source) override;

    protected:
        void repaint() override;

    private:
        /* Dropdown of all the problems to choose from. */
        Temporary<GComboBox> mProblems;

        /* Button to trigger the solver. */
        Temporary<GButton> mSolve;

        /* Current network and solution. */
        HospitalTestCase mHospital;
        Map<string, Set<string>> mAssignment;
        bool mIsSolvable;

        /* Loads the world with the given name. */
        void loadWorld(const string& filename);

        /* Computes an optimal solution. */
        void solve();
    };

    DoctorGUI::DoctorGUI(GWindow& window) : ProblemHandler(window) {
        GComboBox* choices = new GComboBox();
        for (const string& file: sampleProblems(kBasePath)) {
            choices->addItem(file);
        }
        choices->setEditable(false);

        mProblems = Temporary<GComboBox>(choices, window, "SOUTH");
        mSolve    = Temporary<GButton>(new GButton("Solve"), window, "SOUTH");

        loadWorld(choices->getSelectedItem());
    }

    void DoctorGUI::actionPerformed(GObservable* source) {
        if (source == mSolve) {
            solve();
        }
    }

    void DoctorGUI::changeOccurredIn(GObservable* source) {
        if (source == mProblems) {
            loadWorld(mProblems->getSelectedItem());
        }
    }

    void DoctorGUI::repaint() {
        /* Clear display. */
        clearDisplay(window(), kBackgroundColor);

        /* Map each node to its bounding box. */
        auto nodeLocations = layOutNodes(window(), mHospital);

        /* See which patients are covered. */
        Set<string> matchedNodes;
        for (const auto& doctor: mAssignment) {
            for (const auto& patient: mAssignment[doctor]) {
                matchedNodes += patient;
            }
            matchedNodes += doctor;
        }

        /* These modifiers are suffixed onto the node names and contain information about the
         * number of hours needed.
         */
        Map<string, string> nodeModifiers;
        for (const auto& doctor: mHospital.doctors) {
            nodeModifiers[doctor.name] = "(" + pluralize(doctor.hoursFree, "hour") + " free)";
        }
        for (const auto& patient: mHospital.patients) {
            nodeModifiers[patient.name] = "(" + pluralize(patient.hoursNeeded, "hour") + " needed)";
        }

        /* Draw lines connecting patients to their doctor. */
        drawEdges(window(), mAssignment, nodeLocations, kMatchedLineColor, kMatchedLineThickness);

        /* Draw matched and unmatched nodes separately for efficiency (minimize graphics calls). */
        drawNodes(window(), (asSet(mHospital.doctors) + asSet(mHospital.patients)) - matchedNodes,
                  nodeLocations,
                  nodeModifiers,
                  kUnusedBoxColor, kUnusedBoxBorderColor, kUnusedBoxFont);
        drawNodes(window(), matchedNodes, nodeLocations, nodeModifiers,
                  kMatchedBoxColor, kMatchedBoxBorderColor, kMatchedBoxFont);
    }

    void DoctorGUI::loadWorld(const string& filename) {
        ifstream input(kBasePath + filename);
        if (!input) error("Cannot open file.");

        mHospital = loadHospitalTestCase(input);
        mAssignment.clear();
        requestRepaint();
    }

    void DoctorGUI::solve() {
        mAssignment.clear();

        /* Convert from list of Doctors and Patients into maps. */
        Map<string, int> doctors, patients;
        for (Doctor doc: mHospital.doctors) {
            doctors[doc.name] = doc.hoursFree;
        }
        for (Patient p: mHospital.patients) {
            patients[p.name] = p.hoursNeeded;
        }

        mIsSolvable = canAllPatientsBeSeen(doctors, patients, mAssignment);
        if (mIsSolvable) {
            requestRepaint();
        } else {
            GOptionPane::showMessageDialog(&window(), "Sorry, there's no way for everyone to be seen.", "No Solution");
        }
    }

    /* Pluralizes the given word based on its quantity. */
    string pluralize(int quantity, string singular, string plural) {
        return to_string(quantity) + " " + (quantity == 1? singular : plural);
    }

    string pluralize(int quantity, string word) {
        return pluralize(quantity, word, word + "s");
    }

    /* Displays all the doctors and patients in a hospital. */
    void printHospital(const HospitalTestCase& hospital) {
        cout << "There are " << pluralize(hospital.doctors.size(), "doctor") << ":" << endl;
        for (Doctor d: hospital.doctors) {
            cout << "  " << d.name << " (" << pluralize(d.hoursFree, "hour") << " free)" << endl;
        }

        cout << "There are " << pluralize(hospital.patients.size(), "patient") << ":" << endl;
        for (Patient p: hospital.patients) {
            cout << "  " << p.name << " (" << pluralize(p.hoursNeeded, "hour") << " needed)" << endl;
        }
    }

    /* Displays a schedule of patients per doctor. */
    void printSchedule(const Map<string, Set<string>>& schedule) {
        for (string doctor: schedule) {
            cout << "  " << doctor << " sees " << pluralize(schedule[doctor].size(), "patient") << endl;
            for (string patient: schedule[doctor]) {
                cout << "    " << patient << endl;
            }
        }
    }

    void demoDoctorsWithoutOrders() {
        cout << "Doctors Without Orders" << endl;
        do {
            const string filename = makeFileSelection(".dwo", kBasePath);

            ifstream input(filename);
            if (!input) error("Cannot open file.");

            auto hospital = loadHospitalTestCase(input);
            printHospital(hospital);

            Map<string, int> doctors, patients;
            for (Doctor d: hospital.doctors) {
                doctors[d.name] = d.hoursFree;
            }
            for (Patient p: hospital.patients) {
                patients[p.name] = p.hoursNeeded;
            }

            Map<string, Set<string>> schedule;
            cout << "Running your code to find a schedule... " << flush;
            bool result = canAllPatientsBeSeen(doctors, patients, schedule);
            cout << "done!" << endl;

            if (result) {
                cout << "It's possible to schedule everyone! Here's one way to do so." << endl;
                printSchedule(schedule);
            } else {
                cout << "Unfortunately, it's not possible for everyone to be seen." << endl;
            }

        } while (getYesOrNo("Try another demo file? "));
    }
}

GRAPHICS_HANDLER("Doctors Without Orders", GWindow& window) {
    return make_shared<DoctorGUI>(window);
}

CONSOLE_HANDLER("Doctors Without Orders") {
    /* TODO: Implement a console-based demo. */
    demoDoctorsWithoutOrders();
}
