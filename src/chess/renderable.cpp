#include "renderable.hpp"

dam::Vector2F Renderable::interpolate(float alpha) const
{
    if (!m_previous.has_value()) {
        return m_current;
    }

    return dam::Vector2F::lerp_precise(m_previous.value(), m_current, alpha);
}

void Renderable::set(dam::Vector2F value)
{
    m_previous = m_current;
    m_current = value;
}
