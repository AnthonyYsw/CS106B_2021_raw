WINDOW_TITLE("Recursion to the Rescue!")

RUN_TESTS_MENU_OPTION()
MENU_ORDER("TowersOfHanoiGUI.cpp",
           "PermutationGUI.cpp",
           "DoctorGUI.cpp",
           "DisasterGUI.cpp")
           
TEST_ORDER("DoctorsWithoutOrders.cpp",
           "DisasterPlanning.cpp")
           
TEST_BARRIER("DoctorGUI.cpp", "DoctorsWithoutOrders.cpp")
TEST_BARRIER("DisasterGUI.cpp", "DisasterPlanning.cpp")
