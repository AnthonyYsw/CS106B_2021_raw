RUN_TESTS_MENU_OPTION()

MENU_OPTION("Rising Tides", makeWaterGUI)
MENU_OPTION("You Got Hufflepuff", makeQuizGUI)

WINDOW_TITLE("Fun With Collections")

TEST_ORDER("RisingTides.cpp", "YouGotHufflepuff.cpp")
