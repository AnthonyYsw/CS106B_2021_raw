#include "strlib.h"
#include <string>
#include <fstream>
#include <sstream>
#include "vector.h"
#include "DoctorsWithoutOrdersParser.h"
using namespace std;

/* Everything in here is private to the file. */
namespace {
    /**
     * Given a line of the form
     *   HEADER Name: Number
     * parses the data out of that line and fills it into a struct.
     */
    template <typename Entity, int Entity::* hourField>
    Entity parse(const string& line, const std::string& header) {
        if (!startsWith(line, header)) {
            error("Line should start with '" + header + "'");
        }

        Entity result;

        /* Skip the header and split at the colon. */
        auto components = stringSplit(line.substr(header.length()), ":");
        if (components.size() != 2) {
            error("Line should have exactly two components.");
        }

        result.name       = header + " " + trim(components[0]);
        result.*hourField = stringToInteger(trim(components[1]));

        return result;
    }

    /* Forward to the common parsing logic. */
    Doctor parseDoctor(const string& line) {
        return parse<Doctor, &Doctor::hoursFree>(line, "Doctor");
    }
    Patient parsePatient(const string& line) {
        return parse<Patient, &Patient::hoursNeeded>(line, "Patient");
    }
}

HospitalTestCase loadHospitalTestCase(istream& input) {
    HospitalTestCase result;

    for (string line; getline(input, line); ) {
        /* Skip blank lines or lines starting with hash marks. */
        if (trim(line).empty() || line[0] == '#') continue;

        /* See what this line is. */
        if (startsWith(line, "Doctor ")) {
            result.doctors.add(parseDoctor(line));
        } else if (startsWith(line, "Patient ")) {
            result.patients.add(parsePatient(line));
        } else {
            error("Not sure how to handle this line: " + line);
        }
    }

    return result;
}
