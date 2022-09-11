#pragma once

namespace Gorilla { namespace Graphics
{
    class Fence
    {
        friend class Device;

    private:
        Fence(void *handle);
        ~Fence();

    public:
        inline void* get_handle();

    private:
        void* m_handle;
    };

    void* Fence::get_handle()
    {
        return m_handle;
    }
}}
