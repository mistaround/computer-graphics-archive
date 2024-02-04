#include "mat4.h"

///----------------------------------------------------------------------
/// Constructors
///----------------------------------------------------------------------

/// Default Constructor.  Initialize to identity matrix.
mat4::mat4(): 
    data({vec4(1, 0, 0, 0), vec4(0, 1, 0, 0), vec4(0, 0, 1, 0), vec4(0, 0, 0, 1)}) {}

/// Initializes the diagonal values of the matrix to diag. All other values are 0.
mat4::mat4(float diag) : 
    data({vec4(diag, 0, 0, 0), vec4(0, diag, 0, 0), vec4(0, 0, diag, 0), vec4(0, 0, 0, diag)}) {}

/// Initializes matrix with each vector representing a column in the matrix
mat4::mat4(const vec4 &col0, const vec4 &col1, const vec4 &col2, const vec4& col3) :
    data({col0, col1, col2, col3}) {}

/// copy constructor
mat4::mat4(const mat4 &m2) :
    data({m2[0], m2[1], m2[2], m2[3]}) {}

///----------------------------------------------------------------------
/// Getters
///----------------------------------------------------------------------	

/// Returns the values of the column at the index
vec4 mat4::operator[](unsigned int index) const {
    return data[index];
}

/// Returns a reference to the column at the index
vec4& mat4::operator[](unsigned int index) {
    return data[index];
}

///----------------------------------------------------------------------
/// Static Initializers
///----------------------------------------------------------------------

/// Creates a 3-D rotation matrix.
/* 
    Takes an angle in degrees and an axis represented by its xyz components, and outputs a 4x4 rotation matrix
    Use Rodrigues' formula to implement this method 
*/
mat4 mat4::rotate(float angle, float x, float y, float z) {
    vec4 axis = normalize(vec4(x, y, z, 0));
    angle = angle * M_PI / 180.f;
    mat4 K = mat4(vec4(0, axis[2], -axis[1], 0), vec4(-axis[2], 0, axis[0], 0), vec4(axis[1], -axis[0], 0, 0), vec4(0, 0, 0, 0));
    mat4 R = identity() + sin(angle) * K + (1 - cos(angle)) * (K * K);
    return R;
}

/// Takes an xyz displacement and outputs a 4x4 translation matrix
mat4 mat4::translate(float x, float y, float z) {
    return mat4(vec4(1, 0, 0, x), vec4(0, 1, 0, y), vec4(0, 0, 1, z), vec4(0, 0, 0, 1));
}

/// Takes an xyz scale and outputs a 4x4 scale matrix
mat4 mat4::scale(float x, float y, float z) {
    return mat4(vec4(x, 0, 0, 0), vec4(0, y, 0, 0), vec4(0, 0, z, 0), vec4(0, 0, 0, 1));
}

/// Generates a 4x4 identity matrix
mat4 mat4::identity() {
    return mat4();
}

///----------------------------------------------------------------------
/// Operator Functions
///----------------------------------------------------------------------

/// Assign m2's data to this's data and return this
mat4& mat4::operator=(const mat4 &m2) {
    this->data[0] = m2[0];
    this->data[1] = m2[1];
    this->data[2] = m2[2];
    this->data[3] = m2[3];
    return *this;
}

/// Test for equality
bool mat4::operator==(const mat4 &m2) const {
    return (data[0] == m2[0] && data[1] == m2[1] && data[2] == m2[2] && data[3] == m2[3]);
}

/// Test for inequality
bool mat4::operator!=(const mat4 &m2) const {
    return (data[0] != m2[0] || data[1] != m2[1] || data[2] != m2[2] || data[3] != m2[3]);
}

/// Element-wise arithmetic
/// e.g. += adds the elements of m2 to this and returns this (like regular +=)
///      +  returns a new matrix whose elements are the sums of this and v2
mat4& mat4::operator+=(const mat4 &m2) {
    this->data[0] += m2[0];
    this->data[1] += m2[1];
    this->data[2] += m2[2];
    this->data[3] += m2[3];
    return *this;
}

mat4& mat4::operator-=(const mat4 &m2) {
    this->data[0] -= m2[0];
    this->data[1] -= m2[1];
    this->data[2] -= m2[2];
    this->data[3] -= m2[3];
    return *this;
}

// multiplication by a scalar
mat4& mat4::operator*=(float c) {
    this->data[0] *= c;
    this->data[1] *= c;
    this->data[2] *= c;
    this->data[3] *= c;
    return *this;
}

// division by a scalar
mat4& mat4::operator/=(float c) {
    if (c == 0) {
        std::cout << "Error: division by 0" << std::endl;
        return *this;
    }
    this->data[0] /= c;
    this->data[1] /= c;
    this->data[2] /= c;
    this->data[3] /= c;
    return *this;
}

mat4 mat4::operator+(const mat4 &m2) const {
    return mat4(this->data[0] + m2[0], this->data[1] + m2[1], this->data[2] + m2[2], this->data[3] + m2[3]);
}

mat4 mat4::operator-(const mat4 &m2) const {
    return mat4(this->data[0] - m2[0], this->data[1] - m2[1], this->data[2] - m2[2], this->data[3] - m2[3]);
}

// multiplication by a scalar
mat4 mat4::operator*(float c) const {
    return mat4(this->data[0] * c, this->data[1] * c, this->data[2] * c, this->data[3] * c);
}

// division by a scalar
mat4 mat4::operator/(float c) const {
    if (c == 0) {
        std::cout << "Error: division by 0" << std::endl;
        return *this;
    }
    return mat4(this->data[0] / c, this->data[1] / c, this->data[2] / c, this->data[3] / c);
}

/// Matrix multiplication (this->data * m2)
mat4 mat4::operator*(const mat4 &m2) const {
    mat4 m;
    mat4 m1 = transpose(*this);
    for (int i = 0; i < 4; i++) {
        m[i] = vec4(dot(m1[0], m2[i]), dot(m1[1], m2[i]), dot(m1[2], m2[i]), dot(m1[3], m2[i]));
    }
    return m;
}

/// Matrix/vector multiplication (m * v)
/// Assume v is a column vector (ie. a 4x1 matrix)
vec4 mat4::operator*(const vec4 &v) const {
    mat4 m = transpose(*this);
    return vec4(dot(m[0], v), dot(m[1], v), dot(m[2], v), dot(m[3], v));
}

///----------------------------------------------------------------------
/// Matrix Operations
///----------------------------------------------------------------------	
/// Returns the transpose of the input matrix (v_ij == v_ji)
mat4 transpose(const mat4 &m) {
    return mat4(row(m, 0), row(m, 1), row(m, 2), row(m, 3));
}

/// Returns a row of the input matrix
vec4 row(const mat4 &m, unsigned int index) {
    return vec4(m[0][index], m[1][index], m[2][index], m[3][index]);
}

/// Scalar multiplication (c * m)
mat4 operator*(float c, const mat4 &m) {
    return mat4(m[0] * c, m[1] * c, m[2] * c, m[3] * c);
}
	
/// Vector/matrix multiplication (v * m)
/// Assume v is a row vector (ie. a 1x4 matrix)
vec4 operator*(const vec4 &v, const mat4 &m) {
    return vec4(dot(v, m[0]), dot(v, m[1]), dot(v, m[2]), dot(v, m[3]));
}

/// Prints the matrix to a stream in a nice format
std::ostream &operator<<(std::ostream &o, const mat4 &m) {
    // print each row of the matrix
    o << std::endl << "[";
    for (int i = 0; i < 4; i++) {
        if (i != 3) {
            o << row(m, i) << std::endl;
        } else {
            o << row(m, i) << "]" << std::endl;
        }
    }
    return o;
}