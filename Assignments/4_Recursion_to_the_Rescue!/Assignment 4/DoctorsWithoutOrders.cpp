#include "DoctorsWithoutOrders.h"
#include "GUI/SimpleTest.h"
using namespace std;

/* TODO: Refer to DoctorsWithoutOrders.h for more information about this function.
 * Then, delete this comment.
 */
bool canAllPatientsBeSeen(const Map<string, int>& doctors,
                          const Map<string, int>& patients,
                          Map<string, Set<string>>& schedule) {
    /* TODO: Delete the next few lines and implement this function. */
    (void) doctors;
    (void) patients;
    (void) schedule;
    return false;
}



/* * * * * * Test Cases Below This Point * * * * * */

/* TODO: Add your own custom tests here! */














/* * * * * Provided Tests Below This Point * * * * */

PROVIDED_TEST("Can't schedule if a patient requires more hours than any doctor has.") {
    Map<string, Set<string>> schedule;

    Map<string, int> doctors = {
        { "Dr. Zhivago", 8 },
        { "Dr. Strange", 8 },
        { "Dr. Horrible", 8 },
    };
    Map<string, int> patients = {
        { "You Poor Soul", 9 },  // Needs more time than any one doctor can provide
    };

    EXPECT(!canAllPatientsBeSeen(doctors, patients, schedule));
}

PROVIDED_TEST("Can schedule if doctor has way more time than patient needs.") {
    Map<string, Set<string>> schedule;

    Map<string, int> doctors = {
        { "Dr. Wheelock", 12 },
    };
    Map<string, int> patients = {
        { "Lucky Patient", 8 },
    };

    EXPECT(canAllPatientsBeSeen(doctors, patients, schedule));
}

PROVIDED_TEST("Can schedule if there's one doctor and one patient with the same hours.") {
    Map<string, Set<string>> schedule;

    Map<string, int> doctors = {
        { "Dr. Wheelock", 8 },
    };
    Map<string, int> patients = {
        { "Lucky Patient", 8 },
    };

    EXPECT(canAllPatientsBeSeen(doctors, patients, schedule));
}

PROVIDED_TEST("Schedule for one doctor and one patient is correct.") {
    Map<string, Set<string>> schedule;

    Map<string, int> doctors = {
        { "Dr. Wheelock", 8 },
    };
    Map<string, int> patients = {
        { "Lucky Patient", 8 },
    };

    EXPECT(canAllPatientsBeSeen(doctors, patients, schedule));

    Map<string, Set<string>> expectedSchedule = {
        { "Dr. Wheelock", { "Lucky Patient" } }
    };
    EXPECT_EQUAL(schedule, expectedSchedule);
}

PROVIDED_TEST("Single doctor can see many patients.") {
    Map<string, Set<string>> schedule;

    Map<string, int> doctors = {
        { "Dr. House", 7 },
    };
    Map<string, int> patients = {
        { "Patient A", 4 },
        { "Patient B", 2 },
        { "Patient C", 1 }
    };

    EXPECT(canAllPatientsBeSeen(doctors, patients, schedule));

    Map<string, Set<string>> expectedSchedule = {
        { "Dr. House", { "Patient A", "Patient B", "Patient C" } }
    };
    EXPECT_EQUAL(schedule, expectedSchedule);
}

PROVIDED_TEST("Multiple doctors can see multiple patients.") {
    Map<string, Set<string>> schedule;

    Map<string, int> doctors = {
        { "Dr. House", 7  },
        { "AutoDoc",   70 }
    };
    Map<string, int> patients = {
        { "Patient A", 4  },
        { "Patient B", 2  },
        { "Patient C", 1  },
        { "Patient D", 40 },
        { "Patient E", 20 },
        { "Patient F", 10 }
    };

    EXPECT(canAllPatientsBeSeen(doctors, patients, schedule));

    Map<string, Set<string>> expectedSchedule = {
        { "Dr. House", { "Patient A", "Patient B", "Patient C" } },
        { "AutoDoc",   { "Patient D", "Patient E", "Patient F" } },
    };
    EXPECT_EQUAL(schedule, expectedSchedule);
}

PROVIDED_TEST("Doesn't necessarily assign neediest patient to most available doctor.") {
    Map<string, Set<string>> schedule;

    Map<string, int> doctors = {
        { "Doctor Workaholic", 10 },
        { "Doctor Average",    8 },
        { "Doctor Lazy",       6 },
    };
    Map<string, int> patients = {
        { "Patient EightHour", 8 },
        { "Patient SixHour",   6 },
        { "Patient FiveHour1", 5 },
        { "Patient FiveHour2", 5 }
    };

    /* You can't make this work if you assign Patient EightHour to Doctor Workaholic.
     * The only way for this setup to work is if you give the two five-hour patients
     * to Doctor Workaholic.
     */
    Map<string, Set<string>> expectedSchedule = {
        { "Doctor Workaholic", { "Patient FiveHour1", "Patient FiveHour2" } },
        { "Doctor Average",    { "Patient EightHour" } },
        { "Doctor Lazy",       { "Patient SixHour" } }
    };

    EXPECT(canAllPatientsBeSeen(doctors, patients, schedule));
    EXPECT_EQUAL(schedule, expectedSchedule);
}

PROVIDED_TEST("Can't schedule patients if there are no doctors.") {
    Map<string, int> patients = {
        { "You Poor Soul", 8 },
    };
    Map<string, Set<string>> schedule;
    EXPECT(!canAllPatientsBeSeen({}, patients, schedule));
}

PROVIDED_TEST("Agreement in total hours doesn't mean a schedule exists (1).") {
    Map<string, int> doctors = {
        { "Doctor A", 3 },
        { "Doctor B", 3 },
    };
    Map<string, int> patients = {
        { "Patient X", 2 },
        { "Patient Y", 2 },
        { "Patient Z", 2 },
    };

    /* Notice the the total hours free (6) matches the total hours needed (6), but it's set
     * up in a way where things don't align properly.
     */
    Map<string, Set<string>> schedule;
    EXPECT(!canAllPatientsBeSeen(doctors, patients, schedule));
}

PROVIDED_TEST("Agreement in total hours doesn't mean a schedule exists (2).") {
    Map<string, int> doctors = {
        { "Doctor A", 8 },
        { "Doctor B", 8 },
        { "Doctor C", 8 },
    };
    Map<string, int> patients = {
        { "Patient U", 5 },
        { "Patient V", 5 },
        { "Patient W", 5 },
        { "Patient X", 4 },
        { "Patient Y", 3 },
        { "Patient Z", 2 },
    };

    /* Notice the the total hours free (24) matches the total number of hours needed
     * (24), but the way those hours are divvied up makes things impossible. Specifically,
     * no doctor can see two of the patients who each need five hours, so they need to be
     * spread around the three doctors evenly, but then there isn't enough time for
     * anyone to see the patient who needs four hours.
     */
    Map<string, Set<string>> schedule;
    EXPECT(!canAllPatientsBeSeen(doctors, patients, schedule));
}

PROVIDED_TEST("Stress test: Explores each option once (should take at most a few seconds).") {
    /* In this test, there are two doctors and seventeen patients. Each patient
     * requires two hours of time to be seen. Each doctor has 17 free hours.
     * This means that there is no way for each patient to be seen; after each
     * doctor has seen eight patients, they each have one free hour, meaning
     * neither has the time to see the last patient.
     *
     * There are a total of 218,790* different ways you can try assigning the
     * patients to the doctors assuming that you
     *
     *  (1) never leave patients who could be assigned unassigned, and
     *  (2) never generate the same arrangement twice.
     *
     * If your recursive solution has these two properties, then your code should
     * be able to pass this test in a few seconds. However, if you do generate
     * the same assignments multiple times, or you consider options in which a
     * patient was intentionally never assigned to a doctor, then this test will
     * take a very long time to complete.
     *
     * * Wonder where that math comes from? This is the multinomial coefficient
     *   (7, 7, 1), since that's the number of ways of assigning seven patients
     *   to one doctor, seven to another, and leaving one unassigned.
     */
    Map<string, int> doctors = {
        { "Doctor A", 17 },
        { "Doctor B", 17 },
    };
    Map<string, int> patients = {
        { "Patient 01", 2 },
        { "Patient 02", 2 },
        { "Patient 03", 2 },
        { "Patient 04", 2 },
        { "Patient 05", 2 },
        { "Patient 06", 2 },
        { "Patient 07", 2 },
        { "Patient 08", 2 },
        { "Patient 09", 2 },
        { "Patient 10", 2 },
        { "Patient 11", 2 },
        { "Patient 12", 2 },
        { "Patient 13", 2 },
        { "Patient 14", 2 },
        { "Patient 15", 2 },
        { "Patient 16", 2 },
        { "Patient 17", 2 },
    };

    Map<string, Set<string>> schedule;
    EXPECT(!canAllPatientsBeSeen(doctors, patients, schedule));
}
