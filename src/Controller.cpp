#include "Controller.h"

namespace sn
{
    // Constants for default key bindings
    const sf::Keyboard::Key DEFAULT_A_KEY = sf::Keyboard::J;
    const sf::Keyboard::Key DEFAULT_B_KEY = sf::Keyboard::K;
    const sf::Keyboard::Key DEFAULT_SELECT_KEY = sf::Keyboard::RShift;
    const sf::Keyboard::Key DEFAULT_START_KEY = sf::Keyboard::Return;
    const sf::Keyboard::Key DEFAULT_UP_KEY = sf::Keyboard::W;
    const sf::Keyboard::Key DEFAULT_DOWN_KEY = sf::Keyboard::S;
    const sf::Keyboard::Key DEFAULT_LEFT_KEY = sf::Keyboard::A;
    const sf::Keyboard::Key DEFAULT_RIGHT_KEY = sf::Keyboard::D;

    Controller::Controller() :
        m_keyStates(0),
        m_keyBindings(TotalButtons)
    {
        // Set default key bindings
        m_keyBindings[A] = DEFAULT_A_KEY;
        m_keyBindings[B] = DEFAULT_B_KEY;
        m_keyBindings[Select] = DEFAULT_SELECT_KEY;
        m_keyBindings[Start] = DEFAULT_START_KEY;
        m_keyBindings[Up] = DEFAULT_UP_KEY;
        m_keyBindings[Down] = DEFAULT_DOWN_KEY;
        m_keyBindings[Left] = DEFAULT_LEFT_KEY;
        m_keyBindings[Right] = DEFAULT_RIGHT_KEY;
    }

    /**
     * Set custom key bindings for the controller.
     * @param keys A vector of sf::Keyboard::Key values for custom key bindings.
     */
    void Controller::setKeyBindings(const std::vector<sf::Keyboard::Key>& keys)
    {
        m_keyBindings = keys;
    }

    /**
     * Update the internal state of the controller based on key presses.
     * @param b A byte value representing the current state of the buttons.
     */
    void Controller::strobe(Byte b)
    {
        m_strobe = (b & 1);
        if (!m_strobe)
        {
            m_keyStates = 0;
            int shift = 0;
            for (int button = A; button < TotalButtons; ++button)
            {
                m_keyStates |= (sf::Keyboard::isKeyPressed(m_keyBindings[static_cast<Buttons>(button)]) << shift);
                ++shift;
            }
        }
    }

    /**
     * Read the current state of the controller.
     * @return A byte value representing the current state of the controller.
     */
    Byte Controller::read()
    {
        Byte ret;
        if (m_strobe)
            ret = sf::Keyboard::isKeyPressed(m_keyBindings[A]);
        else
        {
            ret = (m_keyStates & 1);
            m_keyStates >>= 1;
        }
        return ret | 0x40;
    }

}
