RUN_TESTS_MENU_OPTION()

MENU_OPTION("Stack Overflows", makeStackOverflowGUI)
MENU_OPTION("Only Connect", makeOnlyConnectGUI)
MENU_OPTION("Playing Fair", makePlayingFairGUI)
MENU_OPTION("Sandpiles", makeSandpilesGUI)
MENU_OPTION("Plotter", makePlotterGUI)

WINDOW_TITLE("Welcome to C++!")

TEST_ORDER("StackOverflow.cpp","OnlyConnect.cpp","PlayingFair.cpp","Sandpiles.cpp","Plotter.cpp")
