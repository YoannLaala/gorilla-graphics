#pragma once

#include <cstdint>

namespace Gorilla { namespace Graphics
{
    struct ColorRGBA
    {
        ColorRGBA();
        ColorRGBA(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255);
        ColorRGBA(uint32_t value);
        ~ColorRGBA();

        inline bool operator==(ColorRGBA color);
        inline bool operator!=(ColorRGBA color);
        inline operator uint32_t() const;

        uint32_t Alpha:8;
        uint32_t Blue:8;
        uint32_t Green:8;
        uint32_t Red:8;


        /*constexpr ColorRGBA Black = ColorRGBA(0, 0, 0, 255);
        using White = ColorRGBA(255, 255, 255, 255);
        using Red = ColorRGBA(255, 0, 0, 255);
        using Green = ColorRGBA(0, 255, 0, 255);
        using Blue = ColorRGBA(0, 0, 255, 255);*/
    };

    bool ColorRGBA::operator==(ColorRGBA color)
    {
        return *(uint32_t*)this == *(uint32_t*)&color;
    }

    bool ColorRGBA::operator!=(ColorRGBA color)
    {
        return *(uint32_t*)this != *(uint32_t*)&color;
    }

    ColorRGBA::operator uint32_t() const
    {
        return *(uint32_t*)this;
    }
}}
