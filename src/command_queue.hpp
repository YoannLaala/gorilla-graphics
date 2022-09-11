#pragma once

#include "enums.hpp"
#include "errors.hpp"

#include <cstdint>

namespace Gorilla { namespace Graphics
{
    class CommandList;
    class Fence;
    class SwapChain;
}}

namespace Gorilla { namespace Graphics
{
    class CommandQueue
    {
        friend class Device;

    private:
        CommandQueue(void *handle);
        ~CommandQueue();

    public:
        inline void* get_handle();

        ErrorCode execute(CommandList *command_list);
        ErrorCode present(SwapChain *swapchain);
        ErrorCode flush(Fence *fence, uint64_t id);

        ErrorCode signal(Fence *fence, uint64_t id);
        ErrorCode wait(Fence *fence, uint64_t id);

    private:
        void* m_handle;
    };

    void* CommandQueue::get_handle()
    {
        return m_handle;
    }
}}
