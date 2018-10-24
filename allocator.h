#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <iostream>

using std::size_t;

struct Allocator {
    void* malloc();
    void free(void* ptr);

private:
    static const size_t PAGE_SIZE = 1;
    static const size_t PAGE_AMOUNT = 4096;
    static const size_t SIZE = 128;

    static void** current;
};

#endif // ALLOCATOR_H