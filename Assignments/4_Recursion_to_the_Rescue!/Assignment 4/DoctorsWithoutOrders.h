#ifndef DoctorsWithoutOrders_Included
#define DoctorsWithoutOrders_Included

#include <string>
#include "set.h"
#include "map.h"

/**
 * Given a list of doctors and a list of patients, determines whether all the patients can
 * be seen. If so, this function fills in the schedule outparameter with a map from doctors
 * to the set of patients that doctor would see.
 *
 * @param doctors  The list of the doctors available to work.
 * @param patients The list of the patients that need to be seen.
 * @param schedule An outparameter that will be filled in with the schedule, should one exist.
 * @return Whether or not a schedule was found.
 */
bool canAllPatientsBeSeen(const Map<std::string, int>& doctors,
                          const Map<std::string, int>& patients,
                          Map<std::string, Set<std::string>>& schedule);


#endif
