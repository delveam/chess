#ifndef TWEENABLE_HPP
#define TWEENABLE_HPP
#include "../dam/vector2f.hpp"

class Tweenable {
public:
    Tweenable() = default;
    Tweenable(dam::Vector2F start, dam::Vector2F end, float duration) :
        m_start(start),
        m_end(end),
        m_duration(duration)
    {
    }

    dam::Vector2F position() const
    {
        return dam::Vector2F::lerp_precise(m_start, m_end, m_t);
    }

    bool done() const
    {
        return m_t >= 1;
    }

    void update(float delta_time);
private:
    dam::Vector2F m_start { 0, 0 };
    dam::Vector2F m_end { 0, 0 };
    float m_duration { 0 };
    float m_t { 0 };
};
#endif
