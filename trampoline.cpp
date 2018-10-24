#include "allocator.h"
#include <cassert>

static Allocator allocator;

template <typename T>
struct Trampoline;

template <typename R, typename... Args>
struct Trampoline<R(Args...)> {

    template <typename F>
    Trampoline(F const &func) : func_obj(new F(func)), deleter(do_delete <F>) {
        code = allocator.malloc();
        code_ptr = (unsigned char*) code;

        // mov r12, func_obj
        
        *(code_ptr++) = 0x49;
        *(code_ptr++) = 0xBC;
        *((void**) code_ptr) = func_obj;
        code_ptr += 8;

        // mov rax, &do_call
        
        *(code_ptr++) = 0x48;
        *(code_ptr++) = 0xB8;
        *((void**) code_ptr) = (void*) &do_call<F>;
        code_ptr += 8;

        //jmp rax

        *(code_ptr++) = 0xFF;
        *(code_ptr++) = 0xE0;
    }

     ~Trampoline() {
        if (func_obj != nullptr) {
            deleter(func_obj);
        }
        allocator.free(code);
    }

    template <typename F>
    static R do_call(Args... args) {
        void* obj;

        __asm__ volatile (
            "movq %%r12, %0\n"
            :"=r"(obj)
            :"r"(obj)
            : "memory"
        );

        return (*((F*) obj))(args...);
    }

    template <typename F>
    static void do_delete(void* obj) {
        delete ((F*) obj);
    }

    R (*get() const )(Args... args) {
        return (R (*)(Args... args)) code;
    }

    Trampoline &operator=(const Trampoline &other) = delete;

private:
    void* func_obj;
    void* code;
    unsigned char* code_ptr;

    R (*caller)(void* obj, Args... args);

    void (*deleter)(void *);
};


int main() {
    std::cout << sizeof(Trampoline<void()>) << std::endl;

    Trampoline<int(double, int, float, int, int, double, double, float)>
        t([&](double p0, int p1, float p2, int p3, int p4, double p5, double p6, float p7) { 
            return p0 + p1 + p2 + p3 + p4 + p5 + p6 + p7; 
        });
    auto p = t.get();
    p(2, 3, 4, 5, 6, 7, 8, 9);
    assert(p(1, 2, 3, 4, 5, 6.8, -7.3, 8.8) == int(1 + 2 + 3 + 4 + 5 + 6.8 - 7.3 + 8.8));

    Trampoline<std::string(std::string)>
        t2([&](std::string s) { 
            return s; 
        });
    auto p2 = t2.get();
    std::cout << p2("OK\n");
}