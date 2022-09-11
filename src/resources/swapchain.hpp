#pragma once

#include <gorilla-core/container/vector.hpp>

namespace Gorilla { namespace Graphics
{
    class Texture2D;
}}

namespace Gorilla { namespace Graphics
{
    class SwapChain
    {
        friend class Device;

    private:
        SwapChain(void *handle, const Vector<Texture2D*>& buffers);
        ~SwapChain();

    public:
        inline void* get_handle();
        inline uint32_t get_buffer_count() const;
        inline Texture2D* get_buffer(uint32_t index);

    private:
        void* m_handle;
        Vector<Texture2D*> m_buffers;
    };

    void* SwapChain::get_handle()
    {
        return m_handle;
    }

    uint32_t SwapChain::get_buffer_count() const
    {
        return m_buffers.get_size();
    }

    Texture2D* SwapChain::get_buffer(uint32_t index)
    {
        return m_buffers[index];
    }
}}
