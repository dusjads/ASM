#include <iostream>
#include <emmintrin.h>

void* naive_memcpy( void* dest, const void* src, std::size_t count ){
    for (size_t i = 0; i < count; i++){
        *((char*)dest + i) = *((char *) src + i);
    }
    return dest;
}

void* memcpy1( void* dest, const void* src, std::size_t count ){
    __asm__ __volatile__ ( 
    "cld\n\t"                               // Clear Direction Flag
    "rep movsl\n\t"
    :
    : "S" (src), "D" (dest), "c" (count)
    );
    return dest;
}

void* memcpy8( void* dest, const void* src, std::size_t count ){
    size_t buf;
    for (size_t i = 0; i < count; i++){
        __asm__ __volatile__ ( 
            "cld\n\t"                               // Clear Direction Flag
            "mov (%1), %0\n"
            "mov %0, (%2)\n"
            : "=r"(buf)
            : "r"((char*)src + i), "r"((char*)dest + i)
            : "memory"
        );
    }
    return dest;
}

void* memcpy16( void* dest, const void* src, std::size_t count ){
    __m128i buf;
    for (size_t i = 0; i < count; i += 16){
        __asm__ __volatile__ ( 
            "cld\n\t"                               // Clear Direction Flag
            "movdqu (%1), %0\n"
            "movntdq %0, (%2)\n"
            : "=x"(buf)
            : "r"((char*)src + i), "r"((char*)dest + i)
            : "memory"
        );
    }
    return dest;
}

void* memcpy16_aligned( void* dest, const void* src, std::size_t count ){
    if (count < 32) return naive_memcpy(dest, src, count);
    size_t offset = (16 - (size_t)dest % 16) % 16;
    for (size_t i = 0; i < offset; i++)
        *((char*)dest + i) = *((char *) src + i);
    size_t middle = count - (count - offset) % 16;
    // std::cout << offset << ' ' << middle << ' ' << count << '\n';
    __m128i buf;
    for (size_t i = offset; i < middle; i += 16){
        __asm__ __volatile__ ( 
            "cld\n\t"                               // Clear Direction Flag
            "movdqu (%1), %0\n"
            "movntdq %0, (%2)\n"
            : "=x"(buf)
            : "r"((char*)src + i), "r"((char*)dest + i)
            : "memory"
        );
    }
    for (size_t i = middle; i < count; i++)
        *((char*)dest + i) = *((char *) src + i);
    return dest;
}


void print(char* src, size_t size){
    for (size_t i = 0; i < size; i++)
        std::cout << src[i];
    std::cout << '\n';
}

int main(){
    int n = 300;
    char str[n];
    for (int i = 0; i < n; i++)
        str[i] = (i % 26) + 'a';
    char* dest = new char[n];

    memcpy16_aligned(dest, str, n);
    print(dest, n);
    delete[] dest;
}