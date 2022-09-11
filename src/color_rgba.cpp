#include "color_rgba.hpp"

namespace Gorilla { namespace Graphics
{
    ColorRGBA::ColorRGBA()
        : Alpha(255)
    {

    }

    ColorRGBA::ColorRGBA(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha /*= 255*/)
    {
        Red = red;
        Green = green;
        Blue = blue;
        Alpha = alpha;
    }

    ColorRGBA::ColorRGBA(uint32_t value)
        // : m_uiValue(_uiColor)
    {

    }

    ColorRGBA::~ColorRGBA()
    {

    }
}}
