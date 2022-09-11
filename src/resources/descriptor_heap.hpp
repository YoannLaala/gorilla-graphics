#pragma once

#include "../enums.hpp"

namespace Gorilla { namespace Graphics
{
    class DescriptorHeap
    {
        friend class Device;

    private:
        DescriptorHeap(void *handle, DescriptorType type);
        ~DescriptorHeap();

    public:
        inline void* get_handle();
        inline DescriptorType get_type() const;

    private:
        void* m_handle;
        DescriptorType m_type;
    };

    void* DescriptorHeap::get_handle()
    {
        return m_handle;
    }

    DescriptorType DescriptorHeap::get_type() const
    {
        return m_type;
    }
}}
