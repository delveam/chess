#ifndef MATRIX4_HPP
#define MATRIX4_HPP
#include <array>

namespace dam {
class Matrix4F;
}

class dam::Matrix4F {
public:
    Matrix4F();
    Matrix4F(std::array<float, 16>);
    unsigned int rows;
    unsigned int columns;
    std::array<float, 16> data;
    float get(unsigned int x, unsigned int y);
    void set(unsigned int x, unsigned int y, float value);
    void set_data(std::array<float, 16>);
    void debug();

    static Matrix4F identity();
    static Matrix4F transpose(Matrix4F matrix);
    static Matrix4F add(Matrix4F a, Matrix4F b);
    static Matrix4F subtract(Matrix4F a, Matrix4F b);
    static Matrix4F multiply(Matrix4F a, Matrix4F b);
    static Matrix4F add_scalar(Matrix4F matrix, float scalar);
    static Matrix4F subtract_scalar(Matrix4F matrix, float scalar);
    static Matrix4F multiply_scalar(Matrix4F matrix, float scalar);
    static Matrix4F divide_scalar(Matrix4F matrix, float scalar);
    static Matrix4F create_rotation_x(float angle);
    static Matrix4F create_rotation_y(float angle);
    static Matrix4F create_rotation_z(float angle);
    static Matrix4F create_translation(float x, float y, float z);
    static Matrix4F create_scale(float x, float y, float z);
};
#endif
