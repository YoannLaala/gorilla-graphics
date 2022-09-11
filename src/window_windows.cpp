#include "window.hpp"
#include <gorilla-core/log/log.hpp>

#include <Windows.h>

#define WINDOW_CLASS_NAME "Gorilla"

namespace Gorilla { namespace Graphics
{
    LRESULT WindowProcess(HWND hwnd, UINT message, WPARAM param, LPARAM lparam)
    {
        switch (message)
        {
            case WM_CREATE:
            {
                Window *window = (Window*)((LPCREATESTRUCT)lparam)->lpCreateParams;
                ::SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)window); 
                break;
            }

            case WM_DESTROY:
            {
                Window* window = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
                window->on_closed.send();
                break;
            }

            case WM_SHOWWINDOW:
            {
                Window* window = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
                window->on_visibility_changed.send(param != 0);
                break;
            }

            case WM_SIZE:
            {
                switch(param)
                {
                    case SIZE_RESTORED:
                    case SIZE_MAXIMIZED:
                    {
                        Window* window = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
                        window->on_size_changed.send(window->get_width(), window->get_height());
                        break;
                    }
                }
                break;
            }

            case WM_EXITSIZEMOVE:
            {
                Window* window = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
                window->on_size_changed.send(window->get_width(), window->get_height());
                break;
            }

            default:
            {
                return ::DefWindowProc(hwnd, message, param, lparam);
            }
        }

        return 0;
    }

    bool g_registered = false;
    Window::Window(const char *title, uint32_t width, uint32_t height)
    {
        HINSTANCE instance = ::GetModuleHandle(nullptr);
        if(!g_registered)
        {
            // Define the class
            WNDCLASSEX wnd_class;
            wnd_class.cbSize = sizeof(WNDCLASSEX);
            wnd_class.cbClsExtra = 0;
            wnd_class.cbWndExtra = 0;
            wnd_class.style = CS_HREDRAW | CS_VREDRAW;
            wnd_class.hIcon = LoadIcon(instance, IDI_APPLICATION);
            wnd_class.hIconSm = LoadIcon(instance, IDI_APPLICATION);
            wnd_class.hCursor = LoadCursor(instance, IDC_ARROW);
            wnd_class.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
            wnd_class.lpszMenuName = NULL;
            wnd_class.hInstance = instance;
            wnd_class.lpfnWndProc = (WNDPROC)WindowProcess;
            wnd_class.lpszClassName = WINDOW_CLASS_NAME;

            // register the class
            LOG_ASSERT(::RegisterClassEx(&wnd_class), "RegisterClassEx failed!");
            g_registered = true;
        }

        // compute real size (including border& & title)
        RECT rectangle;
        ZeroMemory(&rectangle, sizeof(RECT));
        rectangle.right = width;
        rectangle.bottom = height;
        const DWORD style = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN;
        ::AdjustWindowRect(&rectangle, style, false);

        // create window
        uint32_t adjusted_width = rectangle.right - rectangle.left;
        uint32_t adjusted_height = rectangle.bottom - rectangle.top;

        m_handle = ::CreateWindowEx(NULL, WINDOW_CLASS_NAME, title, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, adjusted_width, adjusted_height, NULL, NULL, instance, this);
    }

    Window::~Window()
    {
        LOG_ASSERT(::DestroyWindow(reinterpret_cast<HWND>(m_handle)), "DestroyWindow failed!");
    }

    uint32_t Window::get_width()
    {
        RECT rectangle;
        GetClientRect(reinterpret_cast<HWND>(m_handle), &rectangle);
        return rectangle.right - rectangle.left;
    }

    uint32_t Window::get_height()
    {
        RECT rectangle;
        GetClientRect(reinterpret_cast<HWND>(m_handle), &rectangle);
        return rectangle.bottom - rectangle.top;
    }

    void Window::show()
    {
        ::ShowWindow(reinterpret_cast<HWND>(m_handle), SW_NORMAL);
    }

    void Window::hide()
    {
        ::ShowWindow(reinterpret_cast<HWND>(m_handle), SW_HIDE);
    }

    void Window::set_title(const char* _szTitle)
    {
        ::SetWindowText(reinterpret_cast<HWND>(m_handle), _szTitle);
    }
}}