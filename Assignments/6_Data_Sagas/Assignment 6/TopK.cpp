#include "GUI/SimpleTest.h"
#include "TopK.h"
#include "HeapPQueue.h"
using namespace std;

/* TODO: Refer to TopK.h for more information about what this function does, then
 * delete this comment.
 */
Vector<DataPoint> topK(istream& stream, int k) {
    /* TODO: Delete the next few lines and implement this. */
    (void) stream;
    (void) k;
    return {};
}





/* * * * * * Test Cases Below This Point * * * * * */

/* Helper function that, given a list of data points, produces a stream from them. */
stringstream asStream(const Vector<DataPoint>& dataPoints) {
    stringstream result;
    for (const auto& pt: dataPoints) {
        result << pt;
    }
    return result;
}

/* TODO: Add your own custom tests here! */














/* * * * * Provided Tests Below This Point * * * * */

/* Constant used for sizing the tests below this point. */
const int kMany = 100000;

PROVIDED_TEST("Stream 0 elements, ask for top 0") {
    auto stream = asStream({});
    Vector<DataPoint> expected = {};
    EXPECT_EQUAL(topK(stream, 0), expected);
}

PROVIDED_TEST("Stream 0 elements, ask for top 1") {
    auto stream = asStream({});
    Vector<DataPoint> expected = {};
    EXPECT_EQUAL(topK(stream, 1), expected);
}

PROVIDED_TEST("Stream 0 elements, ask for top many") {
    auto stream = asStream({});
    Vector<DataPoint> expected = {};
    EXPECT_EQUAL(topK(stream, kMany), expected);
}

PROVIDED_TEST("Stream 1 element, ask for top 0") {
    auto stream = asStream({ { "A" , 1 } });
    Vector<DataPoint> expected = {};
    EXPECT_EQUAL(topK(stream, 0), expected);
}

PROVIDED_TEST("Stream 1 element, ask for top 1") {
    auto stream = asStream({ { "A" , 1 } });
    Vector<DataPoint> expected = { { "A" , 1 } };
    EXPECT_EQUAL(topK(stream, 1), expected);
}

PROVIDED_TEST("Stream 1 element, ask for top many") {
    auto stream = asStream({ { "A" , 1 } });
    Vector<DataPoint> expected = { { "A" , 1 } };
    EXPECT_EQUAL(topK(stream, kMany), expected);
}

PROVIDED_TEST("Works in a simple case.") {
    /* Build a stream with some simple elements in it. */
    auto stream = asStream({
        { "A", 1 }, { "B", 2 }, { "C", 3 }, { "D", 4 }
    });

    /* What we should see. */
    Vector<DataPoint> expected = {
        { "D", 4 }, { "C", 3 }, { "B", 2 }
    };

    EXPECT(topK(stream, 3) == expected);
}

PROVIDED_TEST("Stream contains duplicate elements") {
    Vector<DataPoint> vec = {
        { "" , 1 },
        { "" , 3 },
        { "" , 2 },
        { "" , 1 },
        { "" , 5 },
        { "" , 4 },
        { "" , 2 },
        { "" , 3 },
        { "" , 4 },
        { "" , 5 },
    };
    auto stream = asStream(vec);
    Vector<DataPoint> expected = { vec[4] };
    EXPECT_EQUAL(topK(stream, 1), expected);

    stream = asStream(vec);
    expected = { vec[4], vec[4] };
    EXPECT_EQUAL(topK(stream, 2), expected);

    stream = asStream(vec);
    expected = { vec[4], vec[4], vec[5] };
    EXPECT_EQUAL(topK(stream, 3), expected);

    stream = asStream(vec);
    expected = { vec[4], vec[4], vec[5], vec[5] };
    EXPECT_EQUAL(topK(stream, 4), expected);

    stream = asStream(vec);
    expected = { vec[4], vec[4], vec[5], vec[5], vec[1] };
    EXPECT_EQUAL(topK(stream, 5), expected);
}

PROVIDED_TEST("Stream contains negative elements") {
    Vector<DataPoint> vec = {
        { "" , 1 },
        { "" , 3 },
        { "" , 2 },
        { "" , -1 },
        { "" , -5 },
        { "" , 4 },
        { "" , -2 },
        { "" , 3 },
        { "" , -4 },
        { "" , 5 },
    };
    auto stream = asStream(vec);
    Vector<DataPoint> expected = { vec[9] };
    EXPECT_EQUAL(topK(stream, 1), expected);

    stream = asStream(vec);
    expected = { vec[9], vec[5] };
    EXPECT_EQUAL(topK(stream, 2), expected);

    stream = asStream(vec);
    expected = { vec[9], vec[5], vec[1] };
    EXPECT_EQUAL(topK(stream, 3), expected);

    stream = asStream(vec);
    expected = { vec[9], vec[5], vec[1], vec[1] };
    EXPECT_EQUAL(topK(stream, 4), expected);

    stream = asStream(vec);
    expected = { vec[9], vec[5], vec[1], vec[1], vec[2] };
    EXPECT_EQUAL(topK(stream, 5), expected);
}

PROVIDED_TEST("Stream many elements, ask for top 0") {
    Vector<DataPoint> vec;
    for (int i = 0; i < kMany; i++) vec.add({ "", i });
    auto stream = asStream(vec);
    Vector<DataPoint> expected = {};
    EXPECT_EQUAL(topK(stream, 0), expected);
}

PROVIDED_TEST("Stream many elements, ask for top 1") {
    Vector<DataPoint> vec;
    for (int i = 0; i < kMany; i++) vec.add({ "", i });
    auto stream = asStream(vec);
    Vector<DataPoint> expected = { { "" , kMany - 1 } };
    EXPECT_EQUAL(topK(stream, 1), expected);
}

PROVIDED_TEST("Stream many elements, ask for top 5") {
    Vector<DataPoint> vec;
    for (int i = 0; i < kMany; i++) vec.add({ "", i });
    auto stream = asStream(vec);
    Vector<DataPoint> expected = {
        { "" , kMany - 1 },
        { "" , kMany - 2 },
        { "" , kMany - 3 },
        { "" , kMany - 4 },
        { "" , kMany - 5 }
    };
    EXPECT_EQUAL(topK(stream, 5), expected);
}

PROVIDED_TEST("Stress test (should take at most a second or two).") {
    Vector<int> sorted;
    Vector<DataPoint> points;
    for (int i = 0; i < 10000; i++) {
        int weight = randomInteger(0, 100000);
        sorted.add(weight);
        points.add({ "" , weight});
    }

    auto stream = asStream(points);
    sort(sorted.begin(), sorted.end(), greater<int>());
    auto result = topK(stream, 10);

    EXPECT_EQUAL(result.size(), 10);
    for (int i = 0; i < 10; i++) {
        EXPECT_EQUAL(result[i].weight, sorted[i]);
    }
}
