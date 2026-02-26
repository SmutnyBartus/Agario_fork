#include "../src/add.h"
#include "../unity/src/unity.h"

void setUp() {};
void tearDown() {};

void test_add() {
    int output = add(10, 20);
    TEST_ASSERT_EQUAL(30, output);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_add);
    UNITY_END();

    return 0;
}
