#pragma once

#include "GUI/MiniGUI.h"
#include <string>
#include <functional>

/* Runs all the tests in the given file. If they all pass, then uses the
 * specified factory function to create a GUI. If not, returns a GUI that
 * reports that the GUI can't be run until the tests are all passing.
 */
std::shared_ptr<ProblemHandler>
ifPassedThen(GWindow& window,
             const std::string& filename,
             std::function<std::shared_ptr<ProblemHandler>(GWindow&)> constructor);
