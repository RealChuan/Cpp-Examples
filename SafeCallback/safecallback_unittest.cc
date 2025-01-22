#include "safecallback.hpp"

#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <thread>

class MyClass
{
public:
    void notify(int value)
    {
        std::cout << "Received value: " << value << std::endl;
        received_value = value;
    }

    int received_value = -1;
};

int free_function_value = -1;

void freeFunction(int value)
{
    std::cout << "Free function received value: " << value << std::endl;
    free_function_value = value;
}

class SafeCallbackTest : public ::testing::Test
{
protected:
    void SetUp() override { free_function_value = -1; }
};

TEST_F(SafeCallbackTest, MemberFunctionCallback)
{
    auto obj = std::make_shared<MyClass>();
    SafeCallback callback = makeSafeCallback(obj, &MyClass::notify);

    int test_value = 42;
    callback(test_value);

    EXPECT_EQ(obj->received_value, test_value);
}

TEST_F(SafeCallbackTest, MemberFunctionCallbackAfterObjectDestroyed)
{
    auto obj = std::make_shared<MyClass>();
    SafeCallback callback = makeSafeCallback(obj, &MyClass::notify);

    int initial_value = obj->received_value;
    obj.reset();
    int test_value = 42;
    callback(test_value);

    EXPECT_EQ(initial_value, -1);
    EXPECT_EQ(free_function_value, -1);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
