#pragma once

#include <gorilla-core/signal.hpp>

namespace Gorilla { namespace Graphics
{
    class Window
    {
    public:
        Window(const char *title, uint32_t width, uint32_t height);
        ~Window();

        inline void* get_handle();
        uint32_t get_width();
        uint32_t get_height();

        void show();
        void hide();

        void set_title(const char *title);

        Signal<bool> on_visibility_changed;
        Signal<uint32_t, uint32_t> on_size_changed;
        Signal<> on_closed;

    private:
        void* m_handle;
    };

    void* Window::get_handle()
    {
        return m_handle;
    }
}}
