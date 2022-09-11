#pragma once

#include "color_rgba.hpp"
#include "enums.hpp"
#include "errors.hpp"

namespace Gorilla { namespace Graphics
{
    class CommandAllocator;
    class DescriptorHeap;
    class Texture2D;
}}

namespace Gorilla { namespace Graphics
{
    class CommandList
    {
        friend class Device;

    private:
        CommandList(void *handle, CommandType type);
        ~CommandList();

    public:
        inline void* get_handle();
        inline CommandType get_type() const;

        ErrorCode ResourceBarrier(CommandAllocator *command_allocator, DescriptorHeap *descriptor_heap, Texture2D *texture, ResourceState from, ResourceState to);
        ErrorCode Clear(Texture2D *texture_2d, ColorRGBA color);

        ErrorCode Reset(CommandAllocator *command_allocator);
        ErrorCode Close();

    private:
        void* m_handle;
        CommandType m_type;
    };

    void* CommandList::get_handle()
    {
        return m_handle;
    }

    CommandType CommandList::get_type() const
    {
        return m_type;
    }
}}
