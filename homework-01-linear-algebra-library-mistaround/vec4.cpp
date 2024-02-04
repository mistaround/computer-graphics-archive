#include "vec4.h"

///----------------------------------------------------------------------
/// Constructors
///----------------------------------------------------------------------

// Default constructor. Initializes all elements to 0.
vec4::vec4(): data({0, 0, 0, 0}) {}

// Initializes to (x, y, z, w)
vec4::vec4(float x, float y, float z, float w): data({x, y, z, w}) {}

// copy constructor, sets each element of this->data to v2's corresponding data
vec4::vec4(const vec4 &v2): data({v2[0], v2[1], v2[2], v2[3]}) {}

///----------------------------------------------------------------------
/// Getters/Setters
///----------------------------------------------------------------------	
	
/// Returns the value at index
float vec4::operator[](unsigned int index) const {
    return data[index];
}

float& vec4::operator[](unsigned int index) {
    return data[index];
}

///----------------------------------------------------------------------
/// Operator Functions
///----------------------------------------------------------------------		

/// Sets each element of this->data to v2's corresponding data, then returns this
vec4& vec4::operator=(const vec4 &v2) {
    data[0] = v2[0];
    data[1] = v2[1];
    data[2] = v2[2];
    data[3] = v2[3];
    return *this;
}

/// Test for equality
bool vec4::operator==(const vec4 &v2) const {
    return (data[0] == v2[0] && data[1] == v2[1] && data[2] == v2[2] && data[3] == v2[3]);
}

/// Test for inequality
bool vec4::operator!=(const vec4 &v2) const {
    return (data[0] != v2[0] || data[1] != v2[1] || data[2] != v2[2] || data[3] != v2[3]);
}

/// Arithmetic

// addition
vec4& vec4::operator+=(const vec4 &v2) {
    data[0] += v2[0];
    data[1] += v2[1];
    data[2] += v2[2];
    data[3] += v2[3];
    return *this;
}

// subtraction
vec4& vec4::operator-=(const vec4 &v2) {
    data[0] -= v2[0];
    data[1] -= v2[1];
    data[2] -= v2[2];
    data[3] -= v2[3];
    return *this;
}

// multiplication by a scalar
vec4& vec4::operator*=(float c) {
    data[0] *= c;
    data[1] *= c;
    data[2] *= c;
    data[3] *= c;
    return *this;
}

// division by a scalar
vec4& vec4::operator/=(float c) {
    if (c == 0) {
        std::cout << "Error: cannot divide by 0" << std::endl;
        return *this;
    }
    data[0] /= c;
    data[1] /= c;
    data[2] /= c;
    data[3] /= c;
    return *this;
}

// addition const version
vec4 vec4::operator+(const vec4 &v2) const {
    return vec4(data[0] + v2[0], data[1] + v2[1], data[2] + v2[2], data[3] + v2[3]);
}

// subtraction const version
vec4 vec4::operator-(const vec4 &v2) const {
    return vec4(data[0] - v2[0], data[1] - v2[1], data[2] - v2[2], data[3] - v2[3]);
}

// multiplication by a scalar const version
vec4 vec4::operator*(float c) const {
    return vec4(data[0] * c, data[1] * c, data[2] * c, data[3] * c);
}

// division by a scalar const version
vec4 vec4::operator/(float c) const {
    if (c == 0) {
        std::cout << "Error: cannot divide by 0" << std::endl;
        return *this;
    }
    return vec4(data[0] / c, data[1] / c, data[2] / c, data[3] / c);
}

/// Dot Product
float dot(const vec4 &v1, const vec4 &v2) {
    return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2] + v1[3] * v2[3];
}

/// Cross Product
/*  
    Compute the result of v1 x v2 using only their X, Y, and Z elements. 
    In other words, treat v1 and v2 as 3D vectors, not 4D vectors. 
    The fourth element of the resultant vector should be 0. 
*/ 
vec4 cross(const vec4 &v1, const vec4 &v2) {
    return vec4(v1[1] * v2[2] - v1[2] * v2[1], 
                v1[2] * v2[0] - v1[0] * v2[2], 
                v1[0] * v2[1] - v1[1] * v2[0], 
                0);
}

/// Returns the geometric length of the input vector
float length(const vec4 &v) {
    return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3]);
}

/// Scalar Multiplication (c * v)
vec4 operator*(float c, const vec4 &v) {
    return vec4(c * v[0], c * v[1], c * v[2], c * v[3]);
}

/// Normalize v and return the result as a new vector
vec4 normalize(const vec4 &v) {
    float len = length(v);
    if (len == 0) {
        std::cout << "Error: cannot normalize a zero vector" << std::endl;
        return vec4(v);
    }
    return vec4(v[0] / len, v[1] / len, v[2] / len, v[3] / len);
}

/// Prints the vector to a stream in a nice format
std::ostream &operator<<(std::ostream &o, const vec4 &v) {
    o.precision(4);
    o << "(" << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << ")";
    return o;
}