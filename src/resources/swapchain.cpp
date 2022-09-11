#include "swapchain.hpp"

namespace Gorilla { namespace Graphics
{
    SwapChain::SwapChain(void *handle, const Vector<Texture2D*>& buffers)
        : m_handle(handle)
        , m_buffers(buffers)
    {

    }

    SwapChain::~SwapChain()
    {

    }
}}
