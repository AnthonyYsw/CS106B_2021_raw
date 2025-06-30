#pragma once

#include "GUI/MemoryDiagnostics.h"

struct DataPoint;

namespace DataPointUtils {
    void* dataPointAlloc(size_t size,  bool isVector);
    void dataPointFree (void* memory, bool isVector);
}

template <> struct MemoryDiagnostics::Allocator<DataPoint> {
    static void* scalarAlloc(std::size_t bytes) {
        return DataPointUtils::dataPointAlloc(bytes, false);
    }

    static void* vectorAlloc(std::size_t bytes) {
        return DataPointUtils::dataPointAlloc(bytes, true);
    }

    static void scalarFree(void* memory) {
        DataPointUtils::dataPointFree(memory, false);
    }

    static void vectorFree(void* memory) {
        DataPointUtils::dataPointFree(memory, true);
    }
};
