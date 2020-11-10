# Tests for IFJ Compiler 2020

ifj20compiler uses the [gtest framework](https://github.com/google/googletest)

To learn more about the framework, you can read the [documentation](https://github.com/google/googletest/blob/master/googletest/docs/primer.md).

## Basic Concepts

> This text was taken from a section in gtest's [documentation](https://github.com/google/googletest/blob/master/googletest/docs/primer.md#basic-concepts)

When using googletest, you start by writing assertions, which are statements
that check whether a condition is true. An assertion's result can be success,
nonfatal failure, or fatal failure. If a fatal failure occurs, it aborts the
current function; otherwise the program continues normally.

Tests use assertions to verify the tested code's behavior. If a test crashes or
has a failed assertion, then it fails; otherwise it succeeds.

A test suite contains one or many tests. You should group your tests into test
suites that reflect the structure of the tested code. When multiple tests in a
test suite need to share common objects and subroutines, you can put them into a
test fixture class.

A test program can contain multiple test suites.

## Examples and How to:

**Create a test:**

```c
TEST(TestSuiteName, TestName) {
    ... test body ...
}
```

**Assertions:**

``ASSERT_TRUE(condition);``

``ASSERT_FALSE(condition);``

**Binary Assertions:**

``ASSERT_EQ(val1, val2);`` -- means ``val1 == val2``

``ASSERT_NE(val1, val2);`` -- means ``val1 != val2``

``ASSERT_LT(val1, val2);`` -- means ``val1 < val2``

``ASSERT_LE(val1, val2);`` -- means ``val1 > val2``

``ASSERT_GT(val1, val2);`` -- means ``val1 > val2``

``ASSERT_GE(val1, val2);`` -- means ``val1 >= val2``

**String Assertions:**

``ASSERT_STREQ(str1,str2);`` -- expect 2 strings to be equal

``ASSERT_STRNE(str1,str2);`` -- expect 2 strings not to be equal

``ASSERT_STRCASEEQ(str1,str2);`` -- expect 2 strings to be equal, ignoring the case

``ASSERT_STRCASENE(str1,str2); `` -- expect 2 strings not to be equal, ignoring the case

And so, an example of a basic test that compares if *1 == 1*:

```c
TEST(Basic_Equation_tests, Test_number_1) {
    ASSERT_EQ(1,1);
}
