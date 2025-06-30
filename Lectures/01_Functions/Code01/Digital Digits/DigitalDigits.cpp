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

int main() {
    int n = getInteger("Enter an integer: ");
    int digitSum = sumOfDigitsOf(n);
    cout << n << " sums to " << digitSum << endl;

    return 0;
}

/* Earlier, iterative version:

   int sumOfDigitsOf(int n) {
       int result = 0;
       
       while (n > 0) {
           result += (n % 10);
           n /= 10;
       }
       
       return result;
   }
   
*/

int sumOfDigitsOf(int n) {
    /* Base case: Any single-digit number is its own digit sum. */
    if (n < 10) {
        return n;
    }
    /* Recursive case: The sum of the digits of n is the last digit, plus
     * the sum of the remaining digits.
     */
    else {
        return sumOfDigitsOf(n / 10) + (n % 10);
    }
}

