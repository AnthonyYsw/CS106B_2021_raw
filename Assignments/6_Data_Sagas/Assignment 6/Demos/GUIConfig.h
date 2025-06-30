RUN_TESTS_MENU_OPTION()

MENU_ORDER("PerformanceGUI.cpp",
           "ExploreArraysGUI.cpp",
           "InteractivePQueueGUI.cpp",
           "ChildMortalityGUI.cpp",
           "EarthquakeGUI.cpp",
           "Womens800MGUI.cpp",
           "NPSGUI.cpp")
           
TEST_ORDER("HeapPQueue.cpp",
           "TopK.cpp")
           
TEST_BARRIER("ChildMortalityGUI.cpp", "HeapPQueue.cpp", "TopK.cpp")
TEST_BARRIER("EarthquakeGUI.cpp", "HeapPQueue.cpp", "TopK.cpp")
TEST_BARRIER("Womens800MGUI.cpp", "HeapPQueue.cpp", "TopK.cpp")
TEST_BARRIER("NPSGUI.cpp", "HeapPQueue.cpp", "TopK.cpp")

WINDOW_TITLE("Data Sagas")
