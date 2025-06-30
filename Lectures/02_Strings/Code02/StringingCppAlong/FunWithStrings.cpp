/* File: FunWithStrings.cpp
 *
 * A program for playing around with C++ strings!
 */
#include <iostream>
#include <string>
#include "simpio.h"
#include "console.h"
using namespace std;

string reverseOf(string input);

int main() {
    while (true) {
        string text = getLine("Enter some text: ");
        cout << "Behold, it is now \"" << reverseOf(text) << ".\"" << endl << endl;
    }
}

/* Given a string, returns the reverse of that string. */
string reverseOf(string input) {
    /* Base case: If the string is empty, it's its own reverse. */
    if (input == "") {
        return "";
    }
    /* Otherwise, the reverse of the string is formed by placing
     * the first character at the back and reversing the rest.
     */
    else {
        return reverseOf(input.substr(1)) + input[0];
    }
}
