#include <unity.h>
#include <scheduler.h>
#include <sarb.h>

void test_scheduler(void)
{
    TEST_ASSERT_EQUAL(10, 2*5);
};

void test_sarb(void)
{
   task_t TestTask;
   sarb_InitStruct(&TestTask);

   TEST_ASSERT_EQUAL_UINT8(0, TestTask.command);
};

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_scheduler);
    RUN_TEST(test_sarb);
    UNITY_END();

    return 0;
}