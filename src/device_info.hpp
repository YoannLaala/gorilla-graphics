#pragma once

#include <gorilla-core/string/string.hpp>

namespace Gorilla { namespace Graphics
{
    class DeviceInfo
    {
    public:
        DeviceInfo()
            : m_id((uint64_t)-1)
        {

        }

        DeviceInfo(uint64_t id, const char *label)
            : m_id(id)
            , m_label(label)
        {

        }

        ~DeviceInfo()
        {

        }

        inline uint64_t get_id() const;
        inline const char* get_label() const;

    private:
        uint64_t m_id;
        String m_label;
    };

    uint64_t DeviceInfo::get_id() const
    {
        return m_id;
    }

    const char* DeviceInfo::get_label() const
    {
        return m_label.get_buffer();
    }
}}
