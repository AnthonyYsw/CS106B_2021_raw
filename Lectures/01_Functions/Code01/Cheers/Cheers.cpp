#include <iostream>
#include "console.h"
using namespace std;

/* Prototype for the cheer function. This serves as an "IOU" that
 * promises that, indeed, we will write this function.
 */
void cheer(int numTimes);

int main() {
    cheer(137);
    return 0;
}

/* The actual implementation of the cheer function. */
void cheer(int numTimes) {
    for (int i = 0; i < numTimes; i++) {
        cout << "Hip hip, hooray!" << endl;
    }
}
