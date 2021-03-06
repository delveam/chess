#ifndef DAM_MATRIX4_HPP
#define DAM_MATRIX4_HPP
#include <array>

namespace dam {
class Matrix4F;
}

class dam::Matrix4F {
public:
    Matrix4F() = default;
    Matrix4F(std::array<float, 16> data) :
        m_data(data)
    {
    }

    unsigned int rows() const
    {
        return m_rows;
    }
    unsigned int columns() const
    {
        return m_columns;
    }
    const std::array<float, 16>& data() const
    {
        return m_data;
    }

    float get(unsigned int x, unsigned int y) const;
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
private:
    const unsigned int m_rows { 4 };
    const unsigned int m_columns { 4 };
    std::array<float, 16> m_data;
};
#endif
