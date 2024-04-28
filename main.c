#include "allocator.c"
#include "tests.c"
#include "test_random.c"

int main() {
    init();

//      TESTS

//    test_basic_allocation();
//    test_basic_allocation_and_fill();
//    test_multiple_page_allocation_and_fill();
//    test_fill_and_moveToFree_moveToFull();
//    test_fill_and_moveToFree_more_pages_moveToFul();
//    test_large();
    test();
    return 0;
}