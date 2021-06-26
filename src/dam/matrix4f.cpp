#include <iostream>
#include <math.h>
#include "matrix4f.hpp"

std::array<float, 16> get_identity_data()
{
    std::array<float, 16> temp;
    temp.fill(0);

    temp[0] = 1;
    temp[5] = 1;
    temp[10] = 1;
    temp[15] = 1;

    return temp;
}

// TODO: check for exceptions? (e.g. size)

dam::Matrix4F::Matrix4F()
{
    m_rows = 4;
    m_columns = 4;
}

dam::Matrix4F::Matrix4F(std::array<float, 16> data)
{
    m_rows = 4;
    m_columns = 4;

    for (int i = 0; i < (int)m_data.size(); ++i) {
        m_data[i] = data[i];
    }
}

float dam::Matrix4F::get(unsigned int x, unsigned int y) const
{
    return m_data[y * m_columns + x];
}

void dam::Matrix4F::set(unsigned int x, unsigned int y, float value)
{
    m_data[y * m_columns + x] = value;
}

void dam::Matrix4F::set_data(std::array<float, 16> data)
{
    for (int i = 0; i < (int)m_data.size(); ++i) {
        m_data[i] = data[i];
    }
}

dam::Matrix4F dam::Matrix4F::identity()
{
    return dam::Matrix4F(get_identity_data());
}

dam::Matrix4F dam::Matrix4F::transpose(dam::Matrix4F matrix)
{
    std::array<float, 16> temp;

    temp[0] = matrix.data()[0];
    temp[1] = matrix.data()[4];
    temp[2] = matrix.data()[8];
    temp[3] = matrix.data()[12];
    temp[4] = matrix.data()[1];
    temp[5] = matrix.data()[5];
    temp[6] = matrix.data()[9];
    temp[7] = matrix.data()[13];
    temp[8] = matrix.data()[2];
    temp[9] = matrix.data()[6];
    temp[10] = matrix.data()[10];
    temp[11] = matrix.data()[14];
    temp[12] = matrix.data()[3];
    temp[13] = matrix.data()[7];
    temp[14] = matrix.data()[11];
    temp[15] = matrix.data()[15];

    return dam::Matrix4F(temp);
}

dam::Matrix4F dam::Matrix4F::add(dam::Matrix4F a, dam::Matrix4F b)
{
    auto temp = a.data();

    for (int i = 0; i < (int)temp.size(); i++) {
        temp[i] += b.data()[i];
    }

    return dam::Matrix4F(temp);
}

dam::Matrix4F dam::Matrix4F::subtract(dam::Matrix4F a, dam::Matrix4F b)
{
    auto temp = a.data();

    for (int i = 0; i < 16; i++) {
        temp[i] -= b.data()[i];
    }

    return dam::Matrix4F(temp);
}

dam::Matrix4F dam::Matrix4F::multiply(dam::Matrix4F a, dam::Matrix4F b)
{
    std::array<float, 16> temp;

    temp[0] =
        a.data()[0] * b.data()[0] +
        a.data()[1] * b.data()[4] +
        a.data()[2] * b.data()[8] +
        a.data()[3] * b.data()[12];
    temp[1] =
        a.data()[0] * b.data()[1] +
        a.data()[1] * b.data()[5] +
        a.data()[2] * b.data()[9] +
        a.data()[3] * b.data()[13];
    temp[2] =
        a.data()[0] * b.data()[2] +
        a.data()[1] * b.data()[6] +
        a.data()[2] * b.data()[10] +
        a.data()[3] * b.data()[14];
    temp[3] =
        a.data()[0] * b.data()[3] +
        a.data()[1] * b.data()[7] +
        a.data()[2] * b.data()[11] +
        a.data()[3] * b.data()[15];

    temp[4] =
        a.data()[4] * b.data()[0] +
        a.data()[5] * b.data()[4] +
        a.data()[6] * b.data()[8] +
        a.data()[7] * b.data()[12];
    temp[5] =
        a.data()[4] * b.data()[1] +
        a.data()[5] * b.data()[5] +
        a.data()[6] * b.data()[9] +
        a.data()[7] * b.data()[13];
    temp[6] =
        a.data()[4] * b.data()[2] +
        a.data()[5] * b.data()[6] +
        a.data()[6] * b.data()[10] +
        a.data()[7] * b.data()[14];
    temp[7] =
        a.data()[4] * b.data()[3] +
        a.data()[5] * b.data()[7] +
        a.data()[6] * b.data()[11] +
        a.data()[7] * b.data()[15];

    temp[8] =
        a.data()[8] * b.data()[0] +
        a.data()[9] * b.data()[4] +
        a.data()[10] * b.data()[8] +
        a.data()[11] * b.data()[12];
    temp[9] =
        a.data()[8] * b.data()[1] +
        a.data()[9] * b.data()[5] +
        a.data()[10] * b.data()[9] +
        a.data()[11] * b.data()[13];
    temp[10] =
        a.data()[8] * b.data()[2] +
        a.data()[9] * b.data()[6] +
        a.data()[10] * b.data()[10] +
        a.data()[11] * b.data()[14];
    temp[11] =
        a.data()[8] * b.data()[3] +
        a.data()[9] * b.data()[7] +
        a.data()[10] * b.data()[11] +
        a.data()[11] * b.data()[15];

    temp[12] =
        a.data()[12] * b.data()[0] +
        a.data()[13] * b.data()[4] +
        a.data()[14] * b.data()[8] +
        a.data()[15] * b.data()[12];
    temp[13] =
        a.data()[12] * b.data()[1] +
        a.data()[13] * b.data()[5] +
        a.data()[14] * b.data()[9] +
        a.data()[15] * b.data()[13];
    temp[14] =
        a.data()[12] * b.data()[2] +
        a.data()[13] * b.data()[6] +
        a.data()[14] * b.data()[10] +
        a.data()[15] * b.data()[14];
    temp[15] =
        a.data()[12] * b.data()[3] +
        a.data()[13] * b.data()[7] +
        a.data()[14] * b.data()[11] +
        a.data()[15] * b.data()[15];

    return dam::Matrix4F(temp);
}

dam::Matrix4F dam::Matrix4F::add_scalar(dam::Matrix4F matrix, float scalar)
{
    auto temp = matrix.data();

    for (int i = 0; i < 16; i++) {
        temp[i] += scalar;
    }

    return dam::Matrix4F(temp);
}

dam::Matrix4F dam::Matrix4F::subtract_scalar(dam::Matrix4F matrix, float scalar)
{
    auto temp = matrix.data();

    for (int i = 0; i < 16; i++) {
        temp[i] -= scalar;
    }

    return dam::Matrix4F(temp);
}

dam::Matrix4F dam::Matrix4F::multiply_scalar(dam::Matrix4F matrix, float scalar)
{
    auto temp = matrix.data();

    for (int i = 0; i < 16; i++) {
        temp[i] *= scalar;
    }

    return dam::Matrix4F(temp);
}

dam::Matrix4F dam::Matrix4F::divide_scalar(dam::Matrix4F matrix, float scalar)
{
    auto temp = matrix.data();

    for (int i = 0; i < 16; i++) {
        temp[i] /= scalar;
    }

    return dam::Matrix4F(temp);
}

dam::Matrix4F dam::Matrix4F::create_rotation_x(float angle)
{
    auto temp = get_identity_data();

    temp[5] = cos(angle);
    temp[6] = sin(angle);
    temp[9] = -sin(angle);
    temp[10] = cos(angle);

    return dam::Matrix4F(temp);
}

dam::Matrix4F dam::Matrix4F::create_rotation_y(float angle)
{
    auto temp = get_identity_data();

    temp[0] = cos(angle);
    temp[2] = -sin(angle);
    temp[8] = sin(angle);
    temp[10] = cos(angle);

    return dam::Matrix4F(temp);
}

dam::Matrix4F dam::Matrix4F::create_rotation_z(float angle)
{
    auto temp = get_identity_data();

    temp[0] = cos(angle);
    temp[1] = sin(angle);
    temp[4] = -sin(angle);
    temp[5] = cos(angle);

    return dam::Matrix4F(temp);
}

dam::Matrix4F dam::Matrix4F::create_translation(float x, float y, float z)
{
    auto temp = get_identity_data();

    temp[12] = x;
    temp[13] = y;
    temp[14] = z;

    return dam::Matrix4F(temp);
}

dam::Matrix4F dam::Matrix4F::create_scale(float x, float y, float z)
{
    auto temp = get_identity_data();

    temp[0] = x;
    temp[5] = y;
    temp[10] = z;

    return dam::Matrix4F(temp);
}

void dam::Matrix4F::debug()
{
    for (int y = 0; y < (int)m_columns; y++) {
        for (int x = 0; x < (int)m_rows; x++) {
            std::cout << get(x, y) << ", ";
        }
        std::cout << "\n";
    }
}

