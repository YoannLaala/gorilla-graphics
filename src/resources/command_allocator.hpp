#pragma once

#include "enums.hpp"

namespace Gorilla { namespace Graphics
{
    class CommandAllocator
    {
        friend class Device;

    private:
        CommandAllocator(void *handle, CommandType type);
        ~CommandAllocator();

    public:
        inline void* get_handle();
        inline CommandType get_type() const;

    private:
        void* m_handle;
        CommandType m_type;
    };

    void* CommandAllocator::get_handle()
    {
        return m_handle;
    }

    CommandType CommandAllocator::get_type() const
    {
        return m_type;
    }
}}
