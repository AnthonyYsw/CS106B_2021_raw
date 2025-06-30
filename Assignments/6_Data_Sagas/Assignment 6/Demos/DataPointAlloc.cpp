#include "Demos/DataPointAlloc.h"
#include "Demos/DataPoint.h"
#include <iostream>
using namespace std;

namespace DataPointUtils {
    /* When we do an allocation, we prepend space for one DataPoint before and one DataPoint
     * after the allocated region. Those regions are not actually going to hold DataPoints,
     * and instead will hold the following struct.
     */
    struct BlockBoundary {
        size_t blockSize;
        int type;
    };
    static_assert(sizeof(BlockBoundary) <= sizeof(DataPoint), "Internal error: contact course staff.");

    /* Constants used to track whether we did vector or scalar initialization. */
    const int kIsVector    = 0xA110C2;
    const int kIsScalar    = 0xA110C1;
    const int kDeallocated = 0xA110C0;

    /* Allocates a block of memory that contains one more DataPoint than requested.
     * That extra DataPoint is a sentinel that tells us whether we used the scalar
     * or vector alloc/dealloc function.
     */
    void* dataPointAlloc(size_t space, bool isVector) {
        /* Get more space than we need. Specifically, we want what we need, plus space
         * for the header and footer.
         */
        char* fullBlock  = static_cast<char *>(operator new(2 * sizeof(DataPoint) + space));
        char* headerAddr = fullBlock;
        char* footerAddr = fullBlock + sizeof(DataPoint) + space; // Skip header, skip payload
        void* result     = fullBlock + sizeof(DataPoint);

        /* Construct header and footer. */
        ::new (headerAddr) BlockBoundary{space, isVector? kIsVector : kIsScalar};
        ::new (footerAddr) BlockBoundary{space, isVector? kIsVector : kIsScalar};

        return result;
    }

    /* Deallocates a block of memory, checking to make sure the type of deallocation
     * performed was the right one.
     */
    void dataPointFree(void* memory, bool isVector) {\
        /* Get the header. It's DataPoint before the block of memory. */
        BlockBoundary* header = reinterpret_cast<BlockBoundary *>(((DataPoint*) memory) - 1);

        /* Confirm this one has the correct type. */
        if (header->type == kIsVector) {
            if (!isVector) {
                cerr << "You are attempting to deallocate a block of memory that you allocated with "
                        "new[] using the delete operator. This will cause memory errors. Instead, "
                        "use the delete[] operator (with square brackets)."
                        "\n"
                        "Run your program with the debugger enabled and use the call stack to see "
                        "where this error occurred." << endl;
                abort();
            }
        } else if (header->type == kIsScalar) {
            if (isVector) {
                cerr << "You are attempting to deallocate a block of memory that you allocated with "
                        "new using the delete[] operator. This will cause memory errors. Instead,"
                        "use the delete operator (without square brackets)."
                        "\n"
                        "Run your program with the debugger enabled and use the call stack to see "
                        "where this error occurred." << endl;
                abort();
            }
        } else if (header->type == kDeallocated) {
            cerr << "You are attempting to delete memory that you have already deleted."
                    "\n"
                    "Run your program with the debugger enabled and use the call stack to see"
                    "where this error occurred." << endl;
            abort();
        } else {
            cerr << "Something went wrong when you tried to deallocate memory. This could mean that you "
                    "deallocated memory you didn't allocate, or that you used the wrong deallocation "
                    "operator (for example, mixing up delete and delete[])."
                    "\n"
                    "Run your program with the debugger enabled and use the call stack to see "
                    "where this error occurred." << endl;
            abort();
        }

        /* Find the footer. */
        BlockBoundary* footer = reinterpret_cast<BlockBoundary*>(static_cast<char *>(memory) + header->blockSize);

        /* Confirm that the footer matches the header. */
        if (footer->blockSize != header->blockSize || footer->type != header->type) {
            cerr << "Something went wrong when you tried to deallocate memory. Specifically, the "
                    "memory right after the end of the allocated space has been modified since when "
                    "it was created. This might indicate writing off the end of an array, or could "
                    "be due to deallocating memory that wasn't allocated."
                    "\n"
                    "Run your program with the debugger enabled and use the call stack to see "
                    "where this error occurred." << endl;
            abort();
        }

        /* Clear the header and footer in case we use it again in the future. */
        header->type = footer->type = kDeallocated;
        header->blockSize = footer->blockSize = kDeallocated;

        /* Destroy the header and footer. */
        header->~BlockBoundary();
        footer->~BlockBoundary();

        /* Free the memory. */
        operator delete(header);
    }
}
