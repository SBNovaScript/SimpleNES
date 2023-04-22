#include "Controller.h"

namespace sn
{
    // Constants for default key bindings
    constexpr sf::Keyboard::Key DEFAULT_KEY_BINDINGS[] = {
        sf::Keyboard::J,
        sf::Keyboard::K,
        sf::Keyboard::RShift,
        sf::Keyboard::Return,
        sf::Keyboard::W,
        sf::Keyboard::S,
        sf::Keyboard::A,
        sf::Keyboard::D
    };

    Controller::Controller() :
        m_keyStates(0),
        m_keyBindings(DEFAULT_KEY_BINDINGS, DEFAULT_KEY_BINDINGS + TotalButtons)
    {
    }

    void Controller::setKeyBindings(const std::vector<sf::Keyboard::Key>& keys)
    {
        if (keys.size() == TotalButtons)
        {
            m_keyBindings = keys;
        }
    }

    void Controller::strobe(Byte b)
    {
        m_strobe = (b & 1);
        if (!m_strobe)
        {
            m_keyStates = 0;
            for (size_t button = 0; button < TotalButtons; ++button)
            {
                m_keyStates |= (sf::Keyboard::isKeyPressed(m_keyBindings[button]) << button);
            }
        }
    }

    Byte Controller::read()
    {
        Byte ret;
        if (m_strobe)
        {
            ret = sf::Keyboard::isKeyPressed(m_keyBindings[A]);
        }
        else
        {
            ret = (m_keyStates & 1);
            m_keyStates >>= 1;
        }
        return ret | 0x40;
    }

}
