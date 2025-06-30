#include "GUI/SimpleTest.h"
#include "GUI/MemoryDiagnostics.h"
using namespace std;

PROVIDED_TEST("Sample Test Case") {
    EXPECT_EQUAL(2 + 2, 2 * 2);
    EXPECT_NOT_EQUAL(3 + 3, 3 * 3);
    EXPECT_ERROR(error("Aaaah!"));
    EXPECT(true || false);
}
