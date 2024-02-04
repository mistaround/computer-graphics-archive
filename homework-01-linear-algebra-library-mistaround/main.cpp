// Base code written by Jan Allbeck, Chris Czyzewicz, Cory Boatright, Tiantian Liu, Benedict Brown, and Adam Mally
// University of Pennsylvania

// At least some C++ compilers do funny things
// to C's math.h header if you don't define this
#define _USE_MATH_DEFINES

#include "vec4.h"
#include "mat4.h"
#include <iostream>
#include <math.h>
using namespace std;

/*
write code that tests your implementation of the following functions in the vec4 class.
- All three constructors
- operator<<
- operator=
- operator==
- operator!=
- dot product
- cross product
- normalize
*/
void vec4ConstructorTest() {
	vec4 a = vec4();
	vec4 b = vec4(1.f, 2.f, 3.f, 4.f);
	vec4 c = vec4(b);
	std::cout << "------ Testing default constructor ------" << std::endl;
	std::cout << "vec4() expected value: (0.0, 0.0, 0.0, 0.0)" << std::endl;
	std::cout << "vec4() actual value: " << a << std::endl;
	std::cout << "------ Testing element constructor ------" << std::endl;
	std::cout << "vec4(float, float, float, float) expected value: (1.0, 2.0, 3.0, 4.0)" << std::endl;
	std::cout << "vec4(float, float, float, float) actual value: " << b << std::endl;
	std::cout << "------ Testing copy constructor ------" << std::endl;
	std::cout << "vec4(vec4) expected value: (1.0, 2.0, 3.0, 4.0)" << std::endl;
	std::cout << "vec4(vec4) actual value: " << c << std::endl;
}

void vec4OperatorTest() {
	vec4 a = vec4(1.f, 2.f, 3.f, 4.f);
	vec4 b = vec4(2.f, 4.f, 6.f, 8.f);
	std::cout << "------ Testing operator<< ------" << std::endl;
	std::cout << "vec4 expected value: (1.0, 2.0, 3.0, 4.0)" << std::endl;
	std::cout << "vec4 actual value: " << a << std::endl;
	std::cout << "------ Testing operator= ------" << std::endl;
	std::cout << "vec4 = vec4 expected value: (2.0, 4.0, 6.0, 8.0)" << std::endl;
	std::cout << "vec4 = vec4 actual value: " << (a = b) << std::endl;
	std::cout << "------ Testing operator== ------" << std::endl;
	std::cout << "vec4 == vec4 expected value: 1" << std::endl;
	std::cout << "vec4 == vec4 actual value: " << (a == b) << std::endl;
	std::cout << "------ Testing operator!= ------" << std::endl;
	std::cout << "vec4 != vec4 expected value: 0" << std::endl;
	std::cout << "vec4 != vec4 actual value: " << (a != b) << std::endl;
}

void vec4DotTest() {
	vec4 a = vec4(1.f, 2.f, 3.f, 4.f);
	vec4 b = vec4(2.f, 4.f, 6.f, 8.f);
	std::cout << "------ Testing dot product ------" << std::endl;
	std::cout << "vec4 dot vec4 expected value: 60.0" << std::endl;
	std::cout << "vec4 dot vec4 actual value: " << dot(a, b) << std::endl;
}

void vec4CrossTest() {
	vec4 a = vec4(1.f, 2.f, 3.f, 4.f);
	vec4 b = vec4(1.f, 5.f, 7.f, 8.f);
	std::cout << "------ Testing cross product ------" << std::endl;
	std::cout << "vec4 cross vec4 expected value: (-1.0, -4.0, 3.0, 0.0)" << std::endl;
	std::cout << "vec4 cross vec4 actual value: " << cross(a, b) << std::endl;
}

void vec4NormalizeTest() {
	vec4 a = vec4(1.f, 2.f, 3.f, 4.f);
	std::cout << "------ Testing normalization ------" << std::endl;
	std::cout << "vec4 normalize expected value: (0.182574, 0.365148, 0.547723, 0.730297)" << std::endl;
	std::cout << "vec4 normalize actual value: " << normalize(a) << std::endl;
}


void vec4AddTest() {
	vec4 a = vec4(1.f, 2.f, 3.f, 4.f);
	vec4 b = vec4(2.f, 4.f, 6.f, 8.f);
	std::cout << "------ Testing add ------" << std::endl;
	std::cout << "vec4 + vec4 expected value: (3.0, 6.0, 9.0, 12.0)" << std::endl;
	std::cout << "vec4 + vec4 actual value: " << (a + b) << std::endl;
}

/*
Test your implementation of the following functions in the mat4 class:
- All four constructors
- operator<<
- operator=
- operator==
- operator!=
- Rotate, with at least two different angles tested.
- Matrix-matrix multiplication
- Matrix-vector multiplication
- Vector-matrix multiplication
*/

void mat4ConstructorTest() {
	mat4 a = mat4();
	mat4 b = mat4(1.f);
	mat4 c = mat4(vec4(1.f, 2.f, 3.f, 4.f), vec4(5.f, 6.f, 7.f, 8.f), vec4(9.f, 10.f, 11.f, 12.f), vec4(13.f, 14.f, 15.f, 16.f));
	mat4 d = mat4(c);
	std::cout << "------ Testing default constructor ------" << std::endl;
	std::cout << "mat4() expected value: \n [(1.0, 0.0, 0.0, 0.0) \n (0.0, 1.0, 0.0, 0.0) \n (0.0, 0.0, 1.0, 0.0) \n (0.0, 0.0, 0.0, 1.0)]" << std::endl;
	std::cout << "mat4() actual value: " << a << std::endl;
	std::cout << "------ Testing diagonal constructor ------" << std::endl;
	std::cout << "mat4(float) expected value: \n [(1.0, 0.0, 0.0, 0.0) \n (0.0, 1.0, 0.0, 0.0) \n (0.0, 0.0, 1.0, 0.0) \n (0.0, 0.0, 0.0, 1.0)]" << std::endl;
	std::cout << "mat4(float) actual value: " << b << std::endl;
	std::cout << "------ Testing column constructor ------" << std::endl;
	std::cout << "mat4(vec4, vec4, vec4, vec4) expected value: \n [(1.0, 5.0, 9.0, 13.0) \n (2.0, 6.0, 10.0, 14.0) \n (3.0, 7.0, 11.0, 15.0) \n (4.0, 8.0, 12.0, 16.0)]" << std::endl;
	std::cout << "mat4(vec4, vec4, vec4, vec4) actual value: " << c << std::endl;
	std::cout << "------ Testing copy constructor ------" << std::endl;
	std::cout << "mat4(mat4) expected value: \n [(1.0, 5.0, 9.0, 13.0) \n (2.0, 6.0, 10.0, 14.0) \n (3.0, 7.0, 11.0, 15.0) \n (4.0, 8.0, 12.0, 16.0)]" << std::endl;
	std::cout << "mat4(mat4) actual value: " << d << std::endl;
}

void mat4OperatorTest() {
	mat4 a = mat4(vec4(1.f, 2.f, 3.f, 4.f), vec4(5.f, 6.f, 7.f, 8.f), vec4(9.f, 10.f, 11.f, 12.f), vec4(13.f, 14.f, 15.f, 16.f));
	mat4 b = mat4(vec4(2.f, 4.f, 6.f, 8.f), vec4(10.f, 12.f, 14.f, 16.f), vec4(18.f, 20.f, 22.f, 24.f), vec4(26.f, 28.f, 30.f, 32.f));
	std::cout << "------ Testing operator<< ------" << std::endl;
	std::cout << "mat4 expected value: \n [(1.0, 5.0, 9.0, 13.0) \n (2.0, 6.0, 10.0, 14.0) \n (3.0, 7.0, 11.0, 15.0) \n (4.0, 8.0, 12.0, 16.0)]" << std::endl;
	std::cout << "mat4 actual value: " << a << std::endl;
	std::cout << "------ Testing operator= ------" << std::endl;
	std::cout << "mat4 = mat4 expected value: \n [(2.0, 10.0, 18.0, 26.0) \n (4.0, 12.0, 20.0, 28.0) \n (6.0, 14.0, 22.0, 30.0) \n (8.0, 16.0, 24.0, 32.0)]" << std::endl;
	std::cout << "mat4 = mat4 actual value: " << (a = b) << std::endl;
	std::cout << "------ Testing operator== ------" << std::endl;
	std::cout << "mat4 == mat4 expected value: 1" << std::endl;
	std::cout << "mat4 == mat4 actual value: " << (a == b) << std::endl;
	std::cout << "------ Testing operator!= ------" << std::endl;
	std::cout << "mat4 != mat4 expected value: 0" << std::endl;
	std::cout << "mat4 != mat4 actual value: " << (a != b) << std::endl;
}

void mat4RotateTest() {
	std::cout << "------ Testing rotate ------" << std::endl;
	float angle1 = 90.f;
	float x1 = 1.f;
	float y1 = 0.f;
	float z1 = 0.f;
	std::cout << "mat4 rotate expected value: \n [(1.0, 0.0, 0.0, 0.0) \n (0.0, 0.0, -1.0, 0.0) \n (0.0, 1.0, 0.0, 0.0) \n (0.0, 0.0, 0.0, 1.0)]" << std::endl;
	std::cout << "mat4 rotate actual value: " << mat4::rotate(angle1, x1, y1, z1) << std::endl;
	float angle2 = 360.f;
	float x2 = 1.f;
	float y2 = 1.f;
	float z2 = -1.f;
	std::cout << "mat4 rotate expected value: \n [(1.0, 0.0, 0.0, 0.0) \n (0.0, 1.0, 0.0, 0.0) \n (0.0, 0.0, 1.0, 0.0) \n (0.0, 0.0, 0.0, 0.0)]" << std::endl;
	std::cout << "mat4 rotate actual value: " << mat4::rotate(angle2, x2, y2, z2) << std::endl;

}

void mat4MultiplyTest() {
	std::cout << "------ Testing Matrix-matrix multiplication ------" << std::endl;
	mat4 a1 = mat4(vec4(1.f, 3.f, 1.f, 3.f), vec4(2.f, 2.f, 2.f, 2.f), vec4(3.f, 1.f, 3.f, 1.f), vec4(1.f, 2.f, 3.f, 4.f));
	mat4 b1 = mat4(vec4(4.f, 6.f, 4.f, 1.f), vec4(5.f, 5.f, 6.f, 1.f), vec4(6.f, 4.f, 5.f, 1.f), vec4(-1.f, -2.f, -3.f, -4.f));
	mat4 result1 = mat4(vec4(29.f, 30.f, 31.f, 32.f), vec4(34.f, 33.f, 36.f, 35.f), vec4(30.f, 33.f, 32.f, 35.f), vec4(-18.f, -18.f, -26.f, -26.f));
	std::cout << "mat4 * mat4 expected value: " << result1 << std::endl;
	std::cout << "mat4 * mat4 actual value: " << (a1 * b1) << std::endl;
	std::cout << "------ Testing Matrix-vector multiplication ------" << std::endl;
	mat4 a2 = mat4(vec4(1.f, 3.f, 1.f, 3.f), vec4(2.f, 2.f, 2.f, 2.f), vec4(3.f, 1.f, 3.f, 1.f), vec4(1.f, 2.f, 3.f, 4.f));
	vec4 b2 = vec4(1.f, 2.f, 3.f, 4.f);
	vec4 result2 = vec4(18.f, 18.f, 26.f, 26.f);
	std::cout << "mat4 * vec4 expected value: " << result2 << std::endl;
	std::cout << "mat4 * vec4 actual value: " << (a2 * b2) << std::endl;
	std::cout << "------ Testing Vector-matrix multiplication ------" << std::endl;
	vec4 a3 = vec4(1.f, 2.f, 3.f, 4.f);
	mat4 b3 = mat4(vec4(1.f, 3.f, 1.f, 3.f), vec4(2.f, 2.f, 2.f, 2.f), vec4(3.f, 1.f, 3.f, 1.f), vec4(1.f, 2.f, 3.f, 4.f));
	vec4 result3 = vec4(22.f, 20.f, 18.f, 30.f);
	std::cout << "vec4 * mat4 expected value: " << result3 << std::endl;
	std::cout << "vec4 * mat4 actual value: " << (a3 * b3) << std::endl;
}


int main() {
	// Example test (won't work until you implement vec4's operator+ and operator<<)
	std::cout << "/////////////// Testing vec4 ///////////////" << std::endl;
	vec4AddTest();
	// Add your own tests here
	vec4ConstructorTest();
	vec4OperatorTest();
	vec4DotTest();
	vec4CrossTest();
	vec4NormalizeTest();
	std::cout << "/////////////// Testing mat4 ///////////////" << std::endl;
	mat4ConstructorTest();
	mat4OperatorTest();
	mat4RotateTest();
	mat4MultiplyTest();
}
