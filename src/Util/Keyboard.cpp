#include "Keyboard.h"

Keyboard::Keyboard()
{
    reset();
}

void Keyboard::update(const sf::Event& event)
{

    if (auto key = event.getIf<sf::Event::KeyPressed>())
    {
        m_keys[(size_t)key->code] = true;
    }
    else if (auto key = event.getIf<sf::Event::KeyReleased>())
    {
        m_keys[(size_t)key->code] = false;
    }
}

bool Keyboard::is_key_down(sf::Keyboard::Key key) const
{
    return m_keys[(size_t)key];
}

void Keyboard::reset()
{
    std::fill(m_keys.begin(), m_keys.end(), false);
}