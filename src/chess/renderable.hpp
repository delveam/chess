#ifndef RENDERABLE_HPP
#define RENDERABLE_HPP
#include <optional>
#include "../dam/vector2f.hpp"

class Renderable {
public:
    Renderable() = default;
    Renderable(dam::Vector2F value) :
        m_current(value)
    {
    }

    dam::Vector2F interpolate(float alpha) const;
    void set(dam::Vector2F value);
private:
    std::optional<dam::Vector2F> m_previous { std::nullopt };
    dam::Vector2F m_current { 0, 0 };
};
#endif
