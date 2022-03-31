#include <utils/byte_array.h>
#include <iostream>

using namespace std;

#define STRINGIFY(x) #x
#define test_assert(condition, result)     if (!( condition == result )) { cout << "ASSERTION FAILED : " << STRINGIFY(condition) << endl; }
#define assert_true(condition)             test_assert(condition, true)
#define assert_false(condition)            test_assert(condition, false)

int main(int argc, char ** argv)
{
    ByteArray ba = ByteArray::from_hex("010203aabbcc");

    printf("%d\n", ba.length());

    printf("%02x\n", ba(0));
    printf("%02x\n", ba(1));
    printf("%02x\n", ba(2));

    printf("%02x\n", ba(2));

    cout << ba.hex() << endl;

    cout << ba.slice(2, 3).hex() << endl;
    cout << ba(2, 3).hex() << endl;

    cout << ba.slice(0, 2).hex() + ba.slice(-2, 2).hex() << endl;

    cout << (ba.slice(0, 2) + ba.slice(-2, 2)).hex() << endl;

    cout << (ba.slice(0, 2) + 0xFF).hex() << endl;

    ba = ByteArray("hello");
    cout << ba.hex() << endl;

    assert_false(ByteArray::from_hex("00") < ByteArray::from_hex("00"));
    assert_true(ByteArray::from_hex("00") < ByteArray::from_hex("01"));
    assert_false(ByteArray::from_hex("0002") < ByteArray::from_hex("01"));
    assert_true(ByteArray::from_hex("00") < ByteArray::from_hex("0302"));
    assert_true(ByteArray::from_hex("03") < ByteArray::from_hex("0000"));
    assert_false(ByteArray::from_hex("0002") < ByteArray::from_hex("0002"));
    assert_true(ByteArray::from_hex("0002") < ByteArray::from_hex("0102"));
    assert_true(ByteArray::from_hex("000315") < ByteArray::from_hex("000400"));

    return 0;
}