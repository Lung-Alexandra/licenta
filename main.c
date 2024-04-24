#include "allocator.c"
#include "tests.c"

int main() {
    init();
//    de ce a ramane cu pointer catre adresa lui c?
//    trebuie facut de mana null

// TESTS
//    test_basic_allocation_and_fill();
    test_multiple_page_allocation_and_fill();
//    test_multiple_page_allocation_fill_and_free();
    return 0;
}