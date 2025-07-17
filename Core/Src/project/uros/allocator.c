#include "project/uros/utility.h"

void *Uros_MallocImpl(size_t xWantedSize);
void Uros_FreeImpl(void *pv);
void *Uros_ReallocImpl(void *pv, size_t xWantedSize);
void *Uros_CallocImpl(size_t num, size_t xWantedSize);

void *Uros_Allocate(size_t size, void *state) {
    (void)state;
    return Uros_MallocImpl(size);
}

void Uros_Deallocate(void *pointer, void *state) {
    (void)state;
    Uros_FreeImpl(pointer);
}

void *Uros_Reallocate(void *pointer, size_t size, void *state) {
    (void)state;
    return Uros_ReallocImpl(pointer, size);
}

void *Uros_ZeroAllocate(const size_t number_of_elements, const size_t size_of_element, void *const state) {
    (void)state;
    return Uros_CallocImpl(number_of_elements, size_of_element);
}