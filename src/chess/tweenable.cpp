#include "tweenable.hpp"

void Tweenable::update(float delta_time)
{
    if (done()) {
        return;
    }

    m_t += delta_time / m_duration;
}
