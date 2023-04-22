#include "VirtualScreen.h"

namespace sn
{
    VirtualScreen::VirtualScreen(unsigned int w, unsigned int h, float pixel_size, sf::Color color)
    {
        create(w, h, pixel_size, color);
    }

    void VirtualScreen::create(unsigned int w, unsigned int h, float pixel_size, sf::Color color)
    {
        m_vertices.resize(w * h * 6);
        m_screenSize = {w, h};
        m_vertices.setPrimitiveType(sf::Triangles);
        m_pixelSize = pixel_size;

        // Initialize the vertices for each pixel in the virtual screen.
        for (std::size_t x = 0; x < w; ++x)
        {
            for (std::size_t y = 0; y < h; ++y)
            {
                setPixelVertices(x, y, color);
            }
        }
    }

    void VirtualScreen::setPixelVertices(std::size_t x, std::size_t y, sf::Color color)
    {
        std::size_t index = (x * m_screenSize.y + y) * 6;
        sf::Vector2f coord2d(x * m_pixelSize, y * m_pixelSize);
        sf::Vector2f offset;

        // Set the positions and colors of the vertices.
        for (int i = 0; i < 6; ++i)
        {
            offset.x = (i == 1 || i == 2 || i == 3) * m_pixelSize;
            offset.y = (i > 1 && i != 5) * m_pixelSize;

            m_vertices[index + i].position = coord2d + offset;
            m_vertices[index + i].color = color;
        }
    }

    void VirtualScreen::setPixel(std::size_t x, std::size_t y, sf::Color color)
    {
        // Check if the coordinates are within the screen dimensions.
        if (x >= m_screenSize.x || y >= m_screenSize.y)
            return;

        setPixelVertices(x, y, color);
    }

    sf::Color VirtualScreen::getPixel(std::size_t x, std::size_t y) const
    {
        // Check if the coordinates are within the screen dimensions.
        if (x >= m_screenSize.x || y >= m_screenSize.y)
            return sf::Color::Transparent;

        std::size_t index = (x * m_screenSize.y + y) * 6;
        return m_vertices[index].color;
    }

    void VirtualScreen::clear(sf::Color color)
    {
        for (std::size_t x = 0; x < m_screenSize.x; ++x)
        {
            for (std::size_t y = 0; y < m_screenSize.y; ++y)
            {
                setPixelVertices(x, y, color);
            }
        }
    }

    void VirtualScreen::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        target.draw(m_vertices, states);
    }
}
