#pragma once

#include <gorilla-core/container/vector.hpp>

#include "errors.hpp"
#include "enums.hpp"
#include "device_info.hpp"
#include <cstdint>
#include <vector>

namespace Gorilla { namespace Graphics
{
    class CommandAllocator;
    class CommandList;
    class CommandQueue;
    class DescriptorHeap;
    class Fence;
    class SwapChain;
    class Window;
}}

namespace Gorilla { namespace Graphics
{
    class Device
    {
    public:
        Device();
        ~Device();

        static Result<Vector<DeviceInfo>> list();

        ErrorCode initialize(uint64_t device_id = 0);
        ErrorCode release();

        Result<CommandQueue*> create_command_queue(CommandType type);
        Result<DescriptorHeap*> create_descriptor_heap(DescriptorType type, uint32_t descriptor_count);
        Result<CommandAllocator*> create_command_allocator(CommandType type);
        Result<CommandList*> create_command_list(CommandAllocator *allocator);

        Result<Fence*> create_fence(uint64_t value);

        Result<SwapChain*> create_swapchain(CommandQueue *command_queue, DescriptorHeap *descriptor_heap, Window *window, PixelFormat format, uint32_t buffer_count);

        // Texture2D* CreateTextureCube(uint32_t _uiWidth, uint32_t _uiHeight, uint32_t _uiMipCount, EFormat::Type _eFormat, uint32_t _eBindFlag, EUsage::Type _eUsage = EUsage::Default, const void* _pData = NULL);
        //Texture2D* CreateTexture2D(uint32_t _uiWidth, uint32_t _uiHeight, uint32_t _uiMipCount, EFormat::Type _eFormat, uint32_t _eBindFlag, EUsage::Type _eUsage = EUsage::Default, const void* _pData = NULL);
        Result<char> create_texture_2d(uint32_t width, uint32_t height, uint32_t mip_count, uint32_t count, PixelFormat format, uint32_t bind_flags, ResourceUsage usage, const void *dada, uint32_t flag);
        // Texture3D* CreateTexture3D(uint32_t _uiWidth, uint32_t _uiHeight, uint32_t _uiDepth, uint32_t _uiMipCount, EFormat::Type _eFormat, uint32_t _eBindFlag, EUsage::Type _eUsage = EUsage::Default, const void* _pData = NULL);
    private:
        void* m_handle;
    };
}}
