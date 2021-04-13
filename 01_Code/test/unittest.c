#include <unity.h>

void test_function(void) {
    TEST_ASSERT_EQUAL(20, 40/2);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_function);
    UNITY_END();

    return 0;
}