
#include "command_queue.hpp"
#include "command_list.hpp"
#include "resources/fence.hpp"
#include "resources/swapchain.hpp"

#include <d3d12.h>
#include <dxgi1_6.h>

namespace Gorilla { namespace Graphics
{
    CommandQueue::CommandQueue(void *handle)
        : m_handle(handle)
    {

    }

    CommandQueue::~CommandQueue()
    {

    }

    ErrorCode CommandQueue::execute(CommandList* command_list)
    {
        ID3D12CommandQueue *d3d12_command_queue = reinterpret_cast<ID3D12CommandQueue*>(m_handle);
        ID3D12CommandList *d3d12_command_list = reinterpret_cast<ID3D12CommandList*>(command_list->get_handle());
        d3d12_command_queue->ExecuteCommandLists(1, &d3d12_command_list);

        return ErrorCode::NONE;
    }

    bool g_VSync = true;
    bool g_TearingSupported = true;
    ErrorCode CommandQueue::present(SwapChain *swapchain)
    {
        IDXGISwapChain4* d3d12_swapchain = reinterpret_cast<IDXGISwapChain4*>(swapchain->get_handle());

        UINT syncInterval = g_VSync ? 1 : 0;
        UINT presentFlags = g_TearingSupported && !g_VSync ? DXGI_PRESENT_ALLOW_TEARING : 0;
        HRESULT result = d3d12_swapchain->Present(syncInterval, presentFlags);
        if (FAILED(result))
            return ErrorCode::FAIL;

        return ErrorCode::NONE;
    }

    ErrorCode CommandQueue::flush(Fence *fence, uint64_t id)
    {
        ErrorCode error = signal(fence, id);
        if (error != ErrorCode::NONE)
            return error;

        error = wait(fence, id);
        if (error != ErrorCode::NONE)
            return error;

        return ErrorCode::NONE;
    }

    ErrorCode CommandQueue::signal(Fence *fence, uint64_t value)
    {
        ID3D12CommandQueue *d3d12_command_queue = reinterpret_cast<ID3D12CommandQueue*>(m_handle);
        ID3D12Fence* d3d12_fence = reinterpret_cast<ID3D12Fence*>(fence->get_handle());

        HRESULT result = d3d12_command_queue->Signal(d3d12_fence, value);
        if (FAILED(result))
            return ErrorCode::FAIL;

        return ErrorCode::NONE;
    }

    ErrorCode CommandQueue::wait(Fence *fence, uint64_t value)
    {
        ID3D12CommandQueue *d3d12_command_queue = reinterpret_cast<ID3D12CommandQueue*>(m_handle);
        ID3D12Fence* d3d12_fence = reinterpret_cast<ID3D12Fence*>(fence->get_handle());

        HANDLE fenceEvent;
        fenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);

        HRESULT result = d3d12_fence->SetEventOnCompletion(value, fenceEvent);
        if (FAILED(result))
            return ErrorCode::FAIL;

        ::WaitForSingleObject(fenceEvent, INFINITE);

        return ErrorCode::NONE;
    }
}}
