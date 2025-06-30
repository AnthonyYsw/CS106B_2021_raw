/* File: DigitalDigits.cpp
 *
 * A program that explores properties of digits of
 * numbers!
 */
#include <iostream>
#include "simpio.h"
#include "console.h"
using namespace std;

int sumOfDigitsOf(int n);
int digitalRootOf(int n);

int main() {
    while (true) {
        int val = getInteger("For this trick, I need a number! ");
        cout << "Its digital root is " << digitalRootOf(val) << endl << endl;
    }
}

/* Returns the digital root of the input number. The digital root of a number
 * is formed by repeatedly summing the digits of the number until it's down to
 * a single digit.
 *
 * There is a faster way to compute digital roots that works by looking at
 * the number modulo nine. Can you see what it is?
 */
int digitalRootOf(int n) {
    /* Base case: Any single digit is its own digital root. */
    if (n < 10) {
        return n;
    }
    /* Otherwise, the digital root of the number is formed by summing up
     * its digits and taking the digital root of that.
     */
    else {
        return digitalRootOf(sumOfDigitsOf(n));
    }
}

/* Given a number n, returns the sum of the digits of n. */
int sumOfDigitsOf(int n) {
    if (n < 10) {
        return n;
    } else {
        return sumOfDigitsOf(n / 10) + (n % 10);
    }
}











