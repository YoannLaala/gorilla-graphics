/******************************************************************************
**  Includes
******************************************************************************/
#include "device.hpp"
#include "command_queue.hpp"
#include "enums_d3d12.hpp"
#include "resources/command_allocator.hpp"
#include "command_list.hpp"
#include "resources/descriptor_heap.hpp"
#include "resources/fence.hpp"
#include "resources/swapchain.hpp"
#include "resources/texture_2d.hpp"
#include "window.hpp"

#include <memory>
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>

using namespace Microsoft::WRL;

namespace Gorilla { namespace Graphics
{
    struct Handle
    {
        ComPtr<ID3D12Device8> d3d12_device;
        ComPtr<IDXGIFactory6> dxgi_factory;
    };

    Result<ComPtr<IDXGIFactory6>> create_dxgi_factory()
    {
        UINT dxgi_factory_flags = 0;
#if defined(_DEBUG)
        dxgi_factory_flags = DXGI_CREATE_FACTORY_DEBUG;
#endif

        ComPtr<IDXGIFactory6> dxgi_factory = nullptr;
        HRESULT result = CreateDXGIFactory2(dxgi_factory_flags, IID_PPV_ARGS(&dxgi_factory));
        if (FAILED(result) || !dxgi_factory)
            return ErrorCode::FAIL;

        return dxgi_factory;
    }

    Result<Vector<ComPtr<IDXGIAdapter1>>> list_dxgi_adapters(ComPtr<IDXGIFactory6>& dxgi_factory)
    {
        Vector<ComPtr<IDXGIAdapter1>> dxgi_adapters;

        // https://docs.microsoft.com/en-us/windows/win32/api/dxgi1_6/nf-dxgi1_6-idxgifactory6-enumadapterbygpupreference
        UINT i = 0;
        HRESULT result = S_OK;
        ComPtr<IDXGIAdapter1> dxgi_adapter = nullptr;
        do
        {
            result = dxgi_factory->EnumAdapterByGpuPreference(i++, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&dxgi_adapter));
            if (SUCCEEDED(result))
            {
                DXGI_ADAPTER_DESC1 dxgi_adapter_desc;
                HRESULT result = dxgi_adapter->GetDesc1(&dxgi_adapter_desc);
                if (FAILED(result) || dxgi_adapter_desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
                    continue;
                dxgi_adapters.add(dxgi_adapter);
            }

        } while (result != DXGI_ERROR_NOT_FOUND);

        if (dxgi_adapters.is_empty())
            return ErrorCode::FAIL;

        return dxgi_adapters;
    }

    Device::Device()
        : m_handle(nullptr)
    {

    }

    Device::~Device()
    {
        release();
    }

    Result<Vector<DeviceInfo>> Device::list()
    {
        Result<ComPtr<IDXGIFactory6>> dxgi_factory_result = create_dxgi_factory();
        if (!dxgi_factory_result)
            return dxgi_factory_result.Code;

        Result<Vector<ComPtr<IDXGIAdapter1>>> dxgi_adapters_result = list_dxgi_adapters(dxgi_factory_result.Value);
        if (!dxgi_adapters_result)
            return dxgi_adapters_result.Code;

        Vector<DeviceInfo> device_infos;
        const uint32_t adapter_count = dxgi_adapters_result.Value.get_size();
        for (uint32_t i = 0; i < adapter_count; ++i)
        {
            ComPtr<IDXGIAdapter1>& dxgi_adapter = dxgi_adapters_result.Value[i];

            DXGI_ADAPTER_DESC1 dxgi_adapter_desc;
            HRESULT result = dxgi_adapter->GetDesc1(&dxgi_adapter_desc);
            if (FAILED(result))
                continue;

            device_infos.add(DeviceInfo(dxgi_adapter_desc.DeviceId, (char*)dxgi_adapter_desc.Description));
        }

        if (device_infos.is_empty())
            return ErrorCode::FAIL;

        return device_infos;
    }

    ErrorCode Device::initialize(uint64_t device_id /*= 0*/)
    {
        if (m_handle)
            return ErrorCode::FAIL;
        
        Result<ComPtr<IDXGIFactory6>> dxgi_factory_result = create_dxgi_factory();
        if (!dxgi_factory_result)
            return dxgi_factory_result.Code;

        std::unique_ptr<Handle> handle = std::make_unique<Handle>();
        handle->dxgi_factory = dxgi_factory_result.Value;

        auto dxgi_adapters_result = list_dxgi_adapters(handle->dxgi_factory);
        if (!dxgi_adapters_result)
            return dxgi_adapters_result.Code;

        ComPtr<IDXGIAdapter1> dxgi_adapter_selected = nullptr;
        const uint32_t adapter_count = dxgi_adapters_result.Value.get_size();
        for (uint32_t i = 0; i < adapter_count; ++i)
        {
            ComPtr<IDXGIAdapter1>& dxgi_adapter = dxgi_adapters_result.Value[i];

            DXGI_ADAPTER_DESC1 dxgi_adapter_desc;
            HRESULT result = dxgi_adapter->GetDesc1(&dxgi_adapter_desc);
            if (SUCCEEDED(result) && dxgi_adapter_desc.DeviceId == device_id)
            {
                dxgi_adapter_selected = dxgi_adapter;
                break;
            }
        }

        if (!dxgi_adapter_selected)
            dxgi_adapter_selected = dxgi_adapters_result.Value[0];

#if defined(_DEBUG)
        // enable debug layer in debug mode
        ComPtr<ID3D12Debug> d3d12_debug = nullptr;
        HRESULT result = D3D12GetDebugInterface(IID_PPV_ARGS(&d3d12_debug));
        if (FAILED(result))
            return ErrorCode::FAIL;
        d3d12_debug->EnableDebugLayer();
#endif

        result = D3D12CreateDevice(dxgi_adapter_selected.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&handle->d3d12_device));
        if (FAILED(result))
            ErrorCode::FAIL;

        ComPtr<ID3D12InfoQueue> d3d12_info_queue = nullptr;
        result = handle->d3d12_device.As(&d3d12_info_queue);
        if (FAILED(result))
            ErrorCode::FAIL;

        result = d3d12_info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
        if (FAILED(result))
            ErrorCode::FAIL;

        result = d3d12_info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
        if (FAILED(result))
            ErrorCode::FAIL;

        m_handle = handle.release();

        return ErrorCode::NONE;
    }

    ErrorCode Device::release()
    {
        if(m_handle)
        {
            auto* handle = static_cast<Handle*>(m_handle);
            delete handle;
        }
        m_handle = nullptr;

        return ErrorCode::NONE;
    }

    Result<CommandQueue*> Device::create_command_queue(CommandType type)
    {
        Handle *handle = reinterpret_cast<Handle*>(m_handle);

        D3D12_COMMAND_QUEUE_DESC d3d12_command_queue_desc = {};
        d3d12_command_queue_desc.Type = map_enum(type);
        d3d12_command_queue_desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
        d3d12_command_queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        d3d12_command_queue_desc.NodeMask = 0;

        ID3D12CommandQueue *d3d12_command_queue = nullptr;
        HRESULT result = handle->d3d12_device->CreateCommandQueue(&d3d12_command_queue_desc, IID_PPV_ARGS(&d3d12_command_queue));
        if (FAILED(result))
            return ErrorCode::FAIL;

        CommandQueue *command_queue = new CommandQueue(d3d12_command_queue);
        return command_queue;
    }

    Result<DescriptorHeap*> Device::create_descriptor_heap(DescriptorType type, uint32_t descriptor_count)
    {
        Handle *handle = reinterpret_cast<Handle*>(m_handle);

        D3D12_DESCRIPTOR_HEAP_DESC d3d12_descriptor_heap_desc = {};
        d3d12_descriptor_heap_desc.NumDescriptors = descriptor_count;
        d3d12_descriptor_heap_desc.Type = map_enum(type);
        d3d12_descriptor_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        d3d12_descriptor_heap_desc.NodeMask = 0;

        ID3D12DescriptorHeap *d3d12_descriptor_heap = nullptr;
        HRESULT result = handle->d3d12_device->CreateDescriptorHeap(&d3d12_descriptor_heap_desc, IID_PPV_ARGS(&d3d12_descriptor_heap));
        if (FAILED(result))
            return ErrorCode::FAIL;

        DescriptorHeap *descriptor_heap = new DescriptorHeap(d3d12_descriptor_heap, type);
        return descriptor_heap;
    }

    Result<CommandAllocator*> Device::create_command_allocator(CommandType type)
    {
        Handle *handle = reinterpret_cast<Handle*>(m_handle);

        ID3D12CommandAllocator *d3d12_command_allocator = nullptr;
        HRESULT result = handle->d3d12_device->CreateCommandAllocator(map_enum(type), IID_PPV_ARGS(&d3d12_command_allocator));
        if (FAILED(result))
            return ErrorCode::FAIL;

        CommandAllocator *command_allocator = new CommandAllocator(d3d12_command_allocator, type);
        return command_allocator;
    }

    Result<CommandList*> Device::create_command_list(CommandAllocator *allocator)
    {
        Handle *handle = reinterpret_cast<Handle*>(m_handle);
        ID3D12CommandAllocator *d3d12_command_allocator = reinterpret_cast<ID3D12CommandAllocator*>(allocator->get_handle());

        ID3D12GraphicsCommandList *d3d12_graphics_command_list = nullptr;
        HRESULT result = handle->d3d12_device->CreateCommandList(0, map_enum(allocator->get_type()), d3d12_command_allocator, nullptr, IID_PPV_ARGS(&d3d12_graphics_command_list));
        if (FAILED(result))
            return ErrorCode::FAIL;

        result = d3d12_graphics_command_list->Close();
        if (FAILED(result))
            return ErrorCode::FAIL;

        CommandList *command_list = new CommandList(d3d12_graphics_command_list, allocator->get_type());
        return command_list;
    }

    Result<Fence*> Device::create_fence(uint64_t value)
    {
        Handle *handle = reinterpret_cast<Handle*>(m_handle);

        ID3D12Fence *d3d12_fence = nullptr;
        HRESULT result = handle->d3d12_device->CreateFence(value, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&d3d12_fence));
        if (FAILED(result))
            return ErrorCode::FAIL;

        Fence *fence = new Fence(d3d12_fence);
        return fence;
    }

    Result<SwapChain*> Device::create_swapchain(CommandQueue *command_queue, DescriptorHeap *descriptor_heap, Window *window, PixelFormat format, uint32_t buffer_count)
    {
        Handle *handle = reinterpret_cast<Handle*>(m_handle);

        // check tearing support
        BOOL tearing_support = FALSE;
        handle->dxgi_factory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &tearing_support, sizeof(tearing_support));

        DXGI_SWAP_CHAIN_DESC1 dxgi_swapchain_desc = {};
        dxgi_swapchain_desc.Width = window->get_width();
        dxgi_swapchain_desc.Height = window->get_height();
        dxgi_swapchain_desc.Format = map_enum(format);
        dxgi_swapchain_desc.Stereo = FALSE;
        dxgi_swapchain_desc.SampleDesc = { 1, 0 };
        dxgi_swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        dxgi_swapchain_desc.BufferCount = buffer_count;
        dxgi_swapchain_desc.Scaling = DXGI_SCALING_STRETCH;
        dxgi_swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        dxgi_swapchain_desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        dxgi_swapchain_desc.Flags = tearing_support == TRUE ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

        ComPtr<IDXGISwapChain1> dxgi_swapchain = nullptr;
        HRESULT result = handle->dxgi_factory->CreateSwapChainForHwnd((ID3D12CommandQueue*)command_queue->get_handle(), (HWND)window->get_handle(), &dxgi_swapchain_desc, nullptr, nullptr, &dxgi_swapchain); 
        if (FAILED(result))
            return ErrorCode::FAIL;

        result = handle->dxgi_factory->MakeWindowAssociation((HWND)window->get_handle(), DXGI_MWA_NO_ALT_ENTER);
        if (FAILED(result))
            return ErrorCode::FAIL;

        ComPtr<IDXGISwapChain4> dxgi_swapchain4 = nullptr;
        result = dxgi_swapchain.As(&dxgi_swapchain4);
        if (FAILED(result))
            return ErrorCode::FAIL;

        UINT d3d12_descriptor_handle_size = handle->d3d12_device->GetDescriptorHandleIncrementSize(map_enum(descriptor_heap->get_type()));
        if (FAILED(result))
            return ErrorCode::FAIL;

        ID3D12DescriptorHeap *d3d12_descriptor_heap = reinterpret_cast<ID3D12DescriptorHeap*>(descriptor_heap->get_handle());
        D3D12_CPU_DESCRIPTOR_HANDLE d3d12_cpu_descripor_handle = d3d12_descriptor_heap->GetCPUDescriptorHandleForHeapStart();

        Vector<Texture2D*> buffers;
        buffers.reserve(buffer_count);
        for (uint32_t i = 0; i < buffer_count; ++i)
        {
            ComPtr<ID3D12Resource> d3d12_resource = nullptr;
            result = dxgi_swapchain4->GetBuffer(i, IID_PPV_ARGS(&d3d12_resource));
            if (FAILED(result))
                return ErrorCode::FAIL;

            handle->d3d12_device->CreateRenderTargetView(d3d12_resource.Get(), nullptr, d3d12_cpu_descripor_handle);

            Texture2D *texture_2d = new Texture2D(d3d12_resource.Detach());
            texture_2d->add_view({descriptor_heap, (uint64_t)d3d12_cpu_descripor_handle.ptr});
            buffers.add(texture_2d);

            d3d12_cpu_descripor_handle.ptr += d3d12_descriptor_handle_size;
        }

        SwapChain *swapchain = new SwapChain(dxgi_swapchain4.Detach(), buffers);
        return swapchain;
    }

    //Texture2D* Renderer::CreateTextureCube(uint32_t _uiWidth, uint32_t _uiHeight, uint32_t _uiMipCount, EFormat::Type _eFormat, uint32_t _eBindFlag, EUsage::Type _eUsage /*= EUsage::Default*/, const void* _pData /*= NULL*/)
    //{
    //    return CreateTexture2D(_uiWidth, _uiHeight, _uiMipCount, 6, _eFormat, _eBindFlag, _eUsage, _pData, D3D11_RESOURCE_MISC_TEXTURECUBE);
    //}

    //Texture2D* Renderer::CreateTexture2D(uint32_t _uiWidth, uint32_t _uiHeight, uint32_t _uiMipCount, EFormat::Type _eFormat, uint32_t _eBindFlag, EUsage::Type _eUsage /*= EUsage::Default*/, const void* _pData /*= NULL*/)
    //{
    //    return CreateTexture2D(_uiWidth, _uiHeight, _uiMipCount, 1, _eFormat, _eBindFlag, _eUsage, _pData, 0);
    //}

    Result<char> Device::create_texture_2d(uint32_t width, uint32_t height, uint32_t mip_count, uint32_t count, PixelFormat format, uint32_t bind_flags, ResourceUsage usage, const void *dada, uint32_t flag)
    {
        //// Prepare D3D11 Descriptor
        //D3D11_TEXTURE2D_DESC kD3D11TextureDesc;
        //ZeroMemory(&kD3D11TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));
        //kD3D11TextureDesc.Width = _uiWidth;
        //kD3D11TextureDesc.Height = _uiHeight;
        //kD3D11TextureDesc.BindFlags = _eBindFlag;
        //kD3D11TextureDesc.CPUAccessFlags = (_eUsage == EUsage::Dynamic) ? D3D11_CPU_ACCESS_WRITE : 0;
        //kD3D11TextureDesc.Format = (DXGI_FORMAT)_eFormat;
        //kD3D11TextureDesc.Usage = (D3D11_USAGE)_eUsage;
        //kD3D11TextureDesc.MipLevels = _uiMipCount;
        //kD3D11TextureDesc.ArraySize = _uiCount;
        //kD3D11TextureDesc.SampleDesc.Count = 1;
        //kD3D11TextureDesc.SampleDesc.Quality = 0;
        //kD3D11TextureDesc.MiscFlags = _eFlag;

        //// Affect only the subResource pointer in case we have some data in _pData
        //D3D11_SUBRESOURCE_DATA* pD3D11SubResource = NULL;
        //if(_pData)
        //{
        //    uint8* pDataStart = (uint8*)_pData;
        //    uint8* pDataEnd = NULL;

        //    uint32_t uiIndex = 0;
        //    pD3D11SubResource = new D3D11_SUBRESOURCE_DATA[kD3D11TextureDesc.MipLevels * kD3D11TextureDesc.ArraySize];
        //    for(uint32_t uiArray = 0; uiArray < kD3D11TextureDesc.ArraySize; ++uiArray)
        //    {
        //        for(uint32_t uiMipMap = 0; uiMipMap < kD3D11TextureDesc.MipLevels; ++uiMipMap)
        //        {
        //            // Retrieve inforamtion on the current mipmap
        //            uint32_t uiSurfaceByte;
        //            memcpy_s(&uiSurfaceByte, sizeof(uint32_t), pDataStart, sizeof(uint32_t));
        //            pDataStart += sizeof(uint32_t);

        //            uint32_t uiRowByte;
        //            memcpy_s(&uiRowByte, sizeof(uint32_t), pDataStart, sizeof(uint32_t));
        //            pDataStart += sizeof(uint32_t);
        //            pDataEnd = pDataStart + uiSurfaceByte;

        //            // Fill SubResources
        //            pD3D11SubResource[uiIndex].pSysMem = pDataStart;
        //            pD3D11SubResource[uiIndex].SysMemPitch = uiRowByte;
        //            pD3D11SubResource[uiIndex].SysMemSlicePitch = uiSurfaceByte;

        //            // Buffer Overrun
        //            pDataStart += uiSurfaceByte;
        //            if (pDataStart > pDataEnd)
        //            {
        //                LOG_INTERNAL_ERROR("[Renderer] CreateTexture2D failed - incoherency in MipMap size");
        //                return NULL;
        //            }

        //            ++uiIndex;
        //        }
        //    }
        //}

        //// Create D3D11 Specific Texture2D
        //ID3D11Texture2D* pD3D11Texture = NULL;
        //ID3D11Device* pD3D11Device = static_cast<ID3D11Device*>(m_pDevice);
        //HRESULT hResult = pD3D11Device->CreateTexture2D(&kD3D11TextureDesc, pD3D11SubResource, &pD3D11Texture);

        //// Delete Sub resource in all case
        //if(pD3D11SubResource)
        //{
        //    SAFE_DELETE_ARRAY(pD3D11SubResource);
        //}

        //// Check result
        //if(FAILED(hResult))
        //{
        //    LOG_INTERNAL_ERROR("[Renderer] CreateTexture2D failed (HRESULT = 0x%0X)", hResult);
        //    return NULL;
        //}

        //// Create Texture2D
        //Texture2D* pTexture = new Texture2D();
        //pTexture->Initialize(_uiWidth, _uiHeight, pD3D11Texture);

        //// A View will be needed since we want to use it as ShaderResource
        //if((_eBindFlag & EBind::ShaderResource) != 0)
        //{
        //    ID3D11ShaderResourceView* pD3D11ShaderResourceView = static_cast<ID3D11ShaderResourceView*>(CreateShaderResourceView(pTexture, _uiMipCount, _uiCount, _eFormat, _eFlag));
        //    pTexture->SetView(Texture2D::EView::ShaderResource, pD3D11ShaderResourceView);
        //}

        //if((_eBindFlag & EBind::RenderTarget) != 0)
        //{
        //    ID3D11RenderTargetView* pD3D11RenderTargetView = static_cast<ID3D11RenderTargetView*>(CreateRenderTargetView(pD3D11Texture));
        //    pTexture->SetView(Texture2D::EView::RenderTarget, pD3D11RenderTargetView);
        //}

        //if((_eBindFlag & EBind::DepthStencil) != 0)
        //{
        //    ID3D11DepthStencilView* pD3D11DepthStencilView = static_cast<ID3D11DepthStencilView*>(CreateDepthStenciltView(pTexture));
        //    pTexture->SetView(Texture2D::EView::DepthStencil, pD3D11DepthStencilView);
        //}

        //if((_eBindFlag & EBind::UnorderedAccess) != 0)
        //{
        //    ID3D11UnorderedAccessView* pD3D11UnorderedAccessView = static_cast<ID3D11UnorderedAccessView*>(CreateUnorderedAccessView(pTexture, _eFormat));
        //    pTexture->SetView(Texture2D::EView::UnorderedAccess, pD3D11UnorderedAccessView);
        //}

        //return pTexture;
        return ErrorCode::NONE;
    }

    //Texture3D* Renderer::CreateTexture3D(uint32_t _uiWidth, uint32_t _uiHeight, uint32_t _uiDepth, uint32_t _uiMipCount, EFormat::Type _eFormat, uint32_t _eBindFlag, EUsage::Type _eUsage /*= EUsage::Default*/, const void* _pData /*= NULL*/)
    //{
    //    // Prepare D3D11 Descriptor
    //    D3D11_TEXTURE3D_DESC kD3D11TextureDesc;
    //    ZeroMemory(&kD3D11TextureDesc, sizeof(D3D11_TEXTURE3D_DESC));
    //    kD3D11TextureDesc.Width = _uiWidth;
    //    kD3D11TextureDesc.Height = _uiHeight;
    //    kD3D11TextureDesc.Depth = _uiDepth;
    //    kD3D11TextureDesc.BindFlags = _eBindFlag;
    //    kD3D11TextureDesc.CPUAccessFlags = (_eUsage == EUsage::Dynamic) ? D3D11_CPU_ACCESS_WRITE : 0;
    //    kD3D11TextureDesc.Format = (DXGI_FORMAT)_eFormat;
    //    kD3D11TextureDesc.Usage = (D3D11_USAGE)_eUsage;
    //    kD3D11TextureDesc.MipLevels = _uiMipCount;
    //    kD3D11TextureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

    //    // Affect only the subResource pointer in case we have some data in _pData
    //    D3D11_SUBRESOURCE_DATA* pD3D11SubResource = NULL;
    //    if(_pData)
    //    {
    //        uint8* pDataStart = (uint8*)_pData;
    //        uint8* pDataEnd = NULL;
    //        UNUSED(pDataStart, pDataEnd);
    //        uint32_t uiIndex = 0;
    //        pD3D11SubResource = new D3D11_SUBRESOURCE_DATA[kD3D11TextureDesc.MipLevels * 6];
    //        for(uint32_t uiArray = 0; uiArray < 6; ++uiArray)
    //        {
    //            for(uint32_t uiMipMap = 0; uiMipMap < kD3D11TextureDesc.MipLevels; ++uiMipMap)
    //            {
    //                // Retrieve inforamtion on the current mipmap
    //                uint32_t uiSurfaceByte;
    //                memcpy_s(&uiSurfaceByte, sizeof(uint32_t), pDataStart, sizeof(uint32_t));
    //                pDataStart += sizeof(uint32_t);

    //                uint32_t uiRowByte;
    //                memcpy_s(&uiRowByte, sizeof(uint32_t), pDataStart, sizeof(uint32_t));
    //                pDataStart += sizeof(uint32_t);
    //                pDataEnd = pDataStart + uiSurfaceByte;

    //                // Fill SubResources
    //                pD3D11SubResource[uiIndex].pSysMem = pDataStart;
    //                pD3D11SubResource[uiIndex].SysMemPitch = uiRowByte;
    //                pD3D11SubResource[uiIndex].SysMemSlicePitch = uiSurfaceByte;

    //                // Buffer Overrun
    //                pDataStart += uiSurfaceByte;
    //                if (pDataStart > pDataEnd)
    //                {
    //                    LOG_INTERNAL_ERROR("[Renderer] CreateTexture2D failed - incoherency in MipMap size");
    //                    return NULL;
    //                }

    //                ++uiIndex;
    //            }
    //        }
    //    }

    //    // Create D3D11 Specific Texture2D
    //    ID3D11Texture3D* pD3D11Texture = NULL;
    //    ID3D11Device* pD3D11Device = static_cast<ID3D11Device*>(m_pDevice);
    //    HRESULT hResult = pD3D11Device->CreateTexture3D(&kD3D11TextureDesc, pD3D11SubResource, &pD3D11Texture);

    //    // Delete Sub resource in all case
    //    if(pD3D11SubResource)
    //    {
    //        SAFE_DELETE_ARRAY(pD3D11SubResource);
    //    }

    //    // Check result
    //    if(FAILED(hResult))
    //    {
    //        LOG_INTERNAL_ERROR("[Renderer] CreateTexture3D failed (HRESULT = 0x%0X)", hResult);
    //        return NULL;
    //    }

    //    // Create Texture2D
    //    Texture3D* pTexture = new Texture3D();
    //    pTexture->Initialize(_uiWidth, _uiHeight, pD3D11Texture);

    //    // A View will be needed since we want to use it as ShaderResource
    //    if((_eBindFlag & EBind::ShaderResource) != 0)
    //    {
    //        ID3D11ShaderResourceView* pD3D11ShaderResourceView = static_cast<ID3D11ShaderResourceView*>(CreateShaderResourceView(pTexture, _uiMipCount, _eFormat));
    //        pTexture->SetView(Texture3D::EView::ShaderResource, pD3D11ShaderResourceView);
    //    }

    //    if((_eBindFlag & EBind::RenderTarget) != 0)
    //    {
    //        ID3D11RenderTargetView* pD3D11RenderTargetView = static_cast<ID3D11RenderTargetView*>(CreateRenderTargetView(pD3D11Texture));
    //        pTexture->SetView(Texture3D::EView::RenderTarget, pD3D11RenderTargetView);
    //    }

    //    if((_eBindFlag & EBind::UnorderedAccess) != 0)
    //    {
    //        ID3D11UnorderedAccessView* pD3D11UnorderedAccessView = static_cast<ID3D11UnorderedAccessView*>(CreateUnorderedAccessView(pTexture, _eFormat));
    //        pTexture->SetView(Texture3D::EView::UnorderedAccess, pD3D11UnorderedAccessView);
    //    }

    //    return pTexture;
    //}
}}
