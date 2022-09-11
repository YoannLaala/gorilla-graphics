#pragma once

#include "command_list.hpp"
#include "enums_d3d12.hpp"
#include "resources/command_allocator.hpp"
#include "resources/descriptor_heap.hpp"
#include "resources/texture_2d.hpp"
#include <d3d12.h>

namespace Gorilla { namespace Graphics
{
    CommandList::CommandList(void *handle, CommandType type)
        : m_handle(handle)
        , m_type(type)
    {

    }

    CommandList::~CommandList()
    {

    }

    ErrorCode CommandList::ResourceBarrier(CommandAllocator *command_allocator, DescriptorHeap *descriptor_heap, Texture2D *texture, ResourceState from, ResourceState to)
    {
        ID3D12GraphicsCommandList *d3d12_command_list =  reinterpret_cast<ID3D12GraphicsCommandList*>(m_handle);

        D3D12_RESOURCE_BARRIER d3d12_resource_barrier = {};
        d3d12_resource_barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        d3d12_resource_barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        d3d12_resource_barrier.Transition = 
        {
            (ID3D12Resource*)texture->get_handle(),
            0,
            map_enum(from),
            map_enum(to),
        };
        d3d12_command_list->ResourceBarrier(1, &d3d12_resource_barrier);

        return ErrorCode::NONE;
    }

    ErrorCode CommandList::Clear(Texture2D *texture_2d, ColorRGBA color)
    {
        ID3D12GraphicsCommandList *d3d12_command_list =  reinterpret_cast<ID3D12GraphicsCommandList*>(m_handle);

        ResourceView resource_view = texture_2d->get_view(0);
        D3D12_CPU_DESCRIPTOR_HANDLE d3d12_cpu_descripor_handle = { resource_view.get_offset() };

        FLOAT d3d12_color[] = { color.Red / 255.0f, color.Green / 255.0f, color.Blue / 255.0f, color.Alpha / 255.0f };
        d3d12_command_list->ClearRenderTargetView(d3d12_cpu_descripor_handle, d3d12_color, 0, nullptr);

        return ErrorCode::NONE;
    }

    ErrorCode CommandList::Reset(CommandAllocator *command_allocator)
    {
        ID3D12GraphicsCommandList *d3d12_command_list =  reinterpret_cast<ID3D12GraphicsCommandList*>(m_handle);

        ID3D12CommandAllocator *d3d12_command_allocator = reinterpret_cast<ID3D12CommandAllocator*>(command_allocator->get_handle());
        HRESULT result = d3d12_command_allocator->Reset();
        if (FAILED(result))
            return ErrorCode::FAIL;

        result = d3d12_command_list->Reset(d3d12_command_allocator, nullptr);
        if (FAILED(result))
            return ErrorCode::FAIL;

        return ErrorCode::NONE;
    }

    ErrorCode CommandList::Close()
    {
        ID3D12GraphicsCommandList *d3d12_command_list =  reinterpret_cast<ID3D12GraphicsCommandList*>(m_handle);

        HRESULT result = d3d12_command_list->Close();
        if (FAILED(result))
            return ErrorCode::FAIL;

        return ErrorCode::NONE;
    }
}}
