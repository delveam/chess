#ifndef COORDINATES_HPP
#define COORDINATES_HPP
#include <optional>
#include <string>

class Coordinates {
public:
    unsigned int x() const
    {
        return m_x;
    }
    unsigned int y() const
    {
        return m_y;
    }

    std::string to_string() const;

    static std::optional<Coordinates> create(unsigned int x, unsigned int y);
    static std::optional<Coordinates> from_string(std::string notation);
private:
    // Coordinates' constructors are private because it is possible to create invalid coordinates.
    // The only way to create a Coordinates object is to use `Coordinates::create`.
    Coordinates() = default;
    Coordinates(unsigned int x, unsigned int y) :
        m_x(x),
        m_y(y)
    {
    }

    unsigned int m_x { 0 };
    unsigned int m_y { 0 };
};
#endif
