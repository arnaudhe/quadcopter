#pragma once

#include <iostream>

using namespace std;

#define STRINGIFY(x) #x

#define test_assert(condition, result)     if ( (condition) == result ) { cout << "PASS" << endl; } else { cout << "ASSERTION FAILED : " << STRINGIFY(condition) << endl; }

#define assert_true(condition)             test_assert(condition, true)

#define assert_false(condition)            test_assert(condition, false)
