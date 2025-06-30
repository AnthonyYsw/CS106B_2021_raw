#ifndef DoctorsWithoutOrdersParser_Included
#define DoctorsWithoutOrdersParser_Included

#include "DoctorsWithoutOrders.h"
#include "vector.h"
#include <istream>

/* * * * * Helper Routines * * * * */


/**
 * A type representing a doctor and the number of hours they can work.
 */
struct Doctor {
    std::string name; // Their name, for testing purposes.
    int hoursFree;    // How many hours they have free to work.
};

/**
 * A type representing a patient and the number of hours they need to be seen for.
 */
struct Patient {
    std::string name; // Their name, for testing purposes.
    int hoursNeeded;  // How many hours they must be seen for.
};

/**
 * Type representing a test case for the Doctors Without Orders problem.
 */
struct HospitalTestCase {
    Vector<Doctor>  doctors;
    Vector<Patient> patients;
};

/**
 * Given a stream containing a hospital test case file, reads the
 * data from that file and returns a HospitalTestCase containing
 * the relevant information.
 *
 * @param source The stream to read from.
 * @return The test case from that file.
 * @throws ErrorException If the test case is malformed.
 */
HospitalTestCase loadHospitalTestCase(std::istream& source);

#endif
