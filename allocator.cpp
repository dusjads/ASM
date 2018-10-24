#include "allocator.h"
#include <sys/mman.h>

void **Allocator::current = nullptr;

void* Allocator::malloc(){
    if (current == nullptr){
        void* new_mem = mmap(nullptr, PAGE_SIZE * PAGE_AMOUNT, PROT_EXEC | PROT_READ | PROT_WRITE,
                        MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        current = (void**) new_mem;
        for (size_t i = 0; i < PAGE_AMOUNT * PAGE_SIZE; i += SIZE){
            char* next = (char *)new_mem + i;
            *((void**) next) = 0;
            if (i > 0) {
                *((void**) (next - SIZE)) = next;
            }
        }
    }
    void* ptr = current;
    current = (void **) *current;
    return ptr; 

}

void Allocator::free(void* ptr){
    *(void **) ptr = current;
    current = (void**) ptr;
}

