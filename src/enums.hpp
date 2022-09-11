#pragma once

namespace Gorilla { namespace Graphics
{
    enum class CommandType
    {
        DIRECT = 0,
        COMPUTE,
        COPY,
        VIDEO_DECODE,
        VIDEO_ENCODE,
    };

    enum class DescriptorType
    {
        CONSTANT_BUFFER = 0,
        SHADER_RESOURCE,
        UNORDERED_ACCESS,
        SAMPLER,
        RENDER_TARGET,
        DEPTH_STENCIL,
    };

    enum class ResourceState
    {
        COMMON = 0,
        VERTEX_AND_CONSTANT_BUFFER,
        INDEX_BUFFER,
        RENDER_TARGET,
        UNORDERED_ACCESS,
        DEPTH_WRITE,
        DEPTH_READ,
        PIXEL_SHADER_RESOURCE,
        STREAM_OUT,
        INDIRECT_ARGUMENT,
        COPY_DEST,
        COPY_SOURCE,
        RAYTRACING_ACCELERATION_STRUCTURE,
        PRESENT,
        VIDEO_DECODE_READ,
        VIDEO_DECODE_WRITE,
        VIDEO_ENCODE_READ,
        VIDEO_ENCODE_WRITE
    };

    /*    namespace EResource
    {
        enum Type : uint8
        {
            Buffer = 0,
            Effect,
            Geometry,
            RenderState,
            RenderTarget,
            SamplerState,
            Shader,
            Swapchain,
            Texture2D,
            Texture3D,

            Count
        };
    };

    namespace ETechnique
    {
        enum Type : uint8
        {
            Default = 0,
            Instancing,

            Count
        };
    };

    namespace EShader
    {
        enum Type : uint8
        {
            Vertex = 0,
            Geometry,
            Pixel,
            Compute,

            Count
        };
    };

    namespace ESampler
    {
        enum Type
        {
        #ifdef RENDERER_API_D3D11
            Point = D3D11_FILTER_MIN_MAG_MIP_POINT,
            Linear = D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR,
            BiLinear = D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT,
            TriLinear = D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR,
            Anisotropic = D3D11_FILTER_ANISOTROPIC,
            Shadow = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR,
        #endif

            Count = 6
        };
    };

    namespace EAddressingMode
    {
        enum Type : uint8
        {
        #ifdef RENDERER_API_D3D11
            Wrap = D3D11_TEXTURE_ADDRESS_WRAP,
            Mirror = D3D11_TEXTURE_ADDRESS_MIRROR,
            Clamp = D3D11_TEXTURE_ADDRESS_CLAMP,
            Border = D3D11_TEXTURE_ADDRESS_BORDER,
        #endif

            Count = 4
        };
    };

    namespace EBind
    {
        enum Type
        {
        #ifdef RENDERER_API_D3D11
            VertexBuffer        = D3D11_BIND_VERTEX_BUFFER,
            IndexBuffer            = D3D11_BIND_INDEX_BUFFER,
            ConstantBuffer        = D3D11_BIND_CONSTANT_BUFFER,
            ShaderResource        = D3D11_BIND_SHADER_RESOURCE,
            StreamOutput        = D3D11_BIND_STREAM_OUTPUT,
            RenderTarget        = D3D11_BIND_RENDER_TARGET,
            DepthStencil        = D3D11_BIND_DEPTH_STENCIL,
            UnorderedAccess        = D3D11_BIND_UNORDERED_ACCESS,
            Decoder                = D3D11_BIND_DECODER,
            VideoEncoder        = D3D11_BIND_VIDEO_ENCODER
        #endif
        };
    };*/

    enum class ResourceUsage
    {
        DEFAULT = 0,
        IMMUTABLE,
        DYNAMIC,
        STAGING,
    };

    enum class PixelFormat
    {
        UNKNOWN = 0,
        R32G32B32A32_TYPELESS,
        R32G32B32A32_FLOAT,
        R32G32B32A32_UINT,
        R32G32B32A32_SINT,
        R32G32B32_TYPELESS,
        R32G32B32_FLOAT,
        R32G32B32_UINT,
        R32G32B32_SINT,
        R16G16B16A16_TYPELESS,
        R16G16B16A16_FLOAT,
        R16G16B16A16_UNORM,
        R16G16B16A16_UINT,
        R16G16B16A16_SNORM,
        R16G16B16A16_SINT,
        R32G32_TYPELESS,
        R32G32_FLOAT,
        R32G32_UINT,
        R32G32_SINT,
        R32G8X24_TYPELESS,
        D32_FLOAT_S8X24_UINT,
        R32_FLOAT_X8X24_TYPELESS,
        X32_TYPELESS_G8X24_UINT,
        R10G10B10A2_TYPELESS,
        R10G10B10A2_UNORM,
        R10G10B10A2_UINT,
        R11G11B10_FLOAT,
        R8G8B8A8_TYPELESS,
        R8G8B8A8_UNORM,
        R8G8B8A8_UNORM_SRGB,
        R8G8B8A8_UINT,
        R8G8B8A8_SNORM,
        R8G8B8A8_SINT,
        R16G16_TYPELESS,
        R16G16_FLOAT,
        R16G16_UNORM,
        R16G16_UINT,
        R16G16_SNORM,
        R16G16_SINT,
        R32_TYPELESS,
        D32_FLOAT,
        R32_FLOAT,
        R32_UINT,
        R32_SINT,
        R24G8_TYPELESS,
        D24_UNORM_S8_UINT,
        R24_UNORM_X8_TYPELESS,
        X24_TYPELESS_G8_UINT,
        R8G8_TYPELESS,
        R8G8_UNORM,
        R8G8_UINT,
        R8G8_SNORM,
        R8G8_SINT,
        R16_TYPELESS,
        R16_FLOAT,
        D16_UNORM,
        R16_UNORM,
        R16_UINT,
        R16_SNORM,
        R16_SINT,
        R8_TYPELESS,
        R8_UNORM,
        R8_UINT,
        R8_SNORM,
        R8_SINT,
        A8_UNORM,
        R1_UNORM,
        R9G9B9E5_SHAREDEXP,
        R8G8_B8G8_UNORM,
        G8R8_G8B8_UNORM,
        BC1_TYPELESS,
        BC1_UNORM,
        BC1_UNORM_SRGB,
        BC2_TYPELESS,
        BC2_UNORM,
        BC2_UNORM_SRGB,
        BC3_TYPELESS,
        BC3_UNORM,
        BC3_UNORM_SRGB,
        BC4_TYPELESS,
        BC4_UNORM,
        BC4_SNORM,
        BC5_TYPELESS,
        BC5_UNORM,
        BC5_SNORM,
        B5G6R5_UNORM,
        B5G5R5A1_UNORM,
        B8G8R8A8_UNORM,
        B8G8R8X8_UNORM,
        R10G10B10_XR_BIAS_A2_UNORM,
        B8G8R8A8_TYPELESS,
        B8G8R8A8_UNORM_SRGB,
        B8G8R8X8_TYPELESS,
        B8G8R8X8_UNORM_SRGB,
        BC6H_TYPELESS,
        BC6H_UF16,
        BC6H_SF16,
        BC7_TYPELESS,
        BC7_UNORM,
        BC7_UNORM_SRGB,
        AYUV,
        Y410,
        Y416,
        NV12,
        P010,
        P016,
        OPAQUE_420,
        YUY2,
        Y210,
        Y216,
        NV11,
        AI44,
        IA44,
        P8,
        A8P8,
        B4G4R4A4_UNORM,
    };

    /*namespace ETopology
    {
        enum Type : uint8
        {
        #ifdef RENDERER_API_D3D11
            PointList                = D3D_PRIMITIVE_TOPOLOGY_POINTLIST,
            LineList                = D3D_PRIMITIVE_TOPOLOGY_LINELIST,
            LineStrip                = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP,
            TriangleList            = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
            TriangleAdjacent        = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
            LineListAdjacent        = D3D_PRIMITIVE_TOPOLOGY_LINELIST_ADJ,
            LineStripAdjacent        = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ,
            TriangleListAdjacent    = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ,
            TriangleStripAdjacent    = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ,
            ControlPointList        = D3D_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST,
        #endif

            Count = 10,
        };
    };

    namespace EDepthComparaison
    {
        enum Type : uint8
        {
        #ifdef RENDERER_API_D3D11
            Never = D3D11_COMPARISON_NEVER,
            Less = D3D11_COMPARISON_LESS,
            Equal = D3D11_COMPARISON_EQUAL,
            LessEqual = D3D11_COMPARISON_LESS_EQUAL,
            Greater = D3D11_COMPARISON_GREATER,
            NotEqual = D3D11_COMPARISON_NOT_EQUAL,
            GeaterEqual = D3D11_COMPARISON_GREATER_EQUAL,
            Always = D3D11_COMPARISON_ALWAYS,
        #endif

            Count = 8,
        };
    };

    namespace EBlend
    {
        enum Type : uint8
        {
        #ifdef RENDERER_API_D3D11
            Zero                        = D3D11_BLEND_ZERO,
            One                            = D3D11_BLEND_ONE,
            SourceColor                    = D3D11_BLEND_SRC_COLOR,
            SourceColorInversed            = D3D11_BLEND_INV_SRC_COLOR,
            SourceAlpha                    = D3D11_BLEND_SRC_ALPHA,
            SourceAlphaInversed            = D3D11_BLEND_INV_SRC_ALPHA,
            DestinationAlpha            = D3D11_BLEND_DEST_ALPHA,
            DestinationAlphaInversed    = D3D11_BLEND_INV_DEST_ALPHA,
            DestinationColor            = D3D11_BLEND_DEST_COLOR,
            DestinationColorInversed    = D3D11_BLEND_INV_DEST_COLOR,
            SourceAlphaSaturated        = D3D11_BLEND_SRC_ALPHA_SAT,
        #endif

            Count = 11
        };
    };

    namespace EBlendOperation
    {
        enum Type : uint8
        {
        #ifdef RENDERER_API_D3D11
            Add                = D3D11_BLEND_OP_ADD,
            Substract        = D3D11_BLEND_OP_SUBTRACT,
            SubstractRev    = D3D11_BLEND_OP_REV_SUBTRACT,
            Min                = D3D11_BLEND_OP_MIN,
            Max                = D3D11_BLEND_OP_MAX,
        #endif

            Count = 5,
        };
    };

    namespace EDimension
    {
        enum Type : uint8
        {
        #ifdef RENDERER_API_D3D11
            Buffer        = D3D11_RESOURCE_DIMENSION_BUFFER,
            Texture1D    = D3D11_RESOURCE_DIMENSION_TEXTURE1D,
            Texture2D    = D3D11_RESOURCE_DIMENSION_TEXTURE2D,
            Texture3D    = D3D11_RESOURCE_DIMENSION_TEXTURE3D
        #endif
        };
    };

    namespace EMap
    {
        enum Type
        {
        #ifdef RENDERER_API_D3D11
            Read            = D3D11_MAP_READ,
            Write            = D3D11_MAP_WRITE,
            ReadWrite        = D3D11_MAP_READ_WRITE,
            WriteDiscard    = D3D11_MAP_WRITE_DISCARD,
        #endif
        };
    }

    namespace ELineStyle
    {
        enum Type
        {
        #ifdef RENDERER_API_D3D11
            Solid        = D2D1_DASH_STYLE_SOLID,
            Dash        = D2D1_DASH_STYLE_DASH,
            Dot            = D2D1_DASH_STYLE_DOT,
            DashDot        = D2D1_DASH_STYLE_DASH_DOT,
            DashDotDot    = D2D1_DASH_STYLE_DASH_DOT_DOT,
        #endif
        };
    };

    namespace EFontStyle
    {
        enum Type
        {
        #ifdef RENDERER_API_D3D11
            Normal        = DWRITE_FONT_STYLE_NORMAL,
            Oblique        = DWRITE_FONT_STYLE_OBLIQUE,
            Italic        = DWRITE_FONT_STYLE_ITALIC,
        #endif
        };
    };

    namespace EFontWeight
    {
        enum Type
        {
            Thin        = 100,
            Light        = 300,
            Normal        = 400,
            Medium        = 500,
            BoldSemi    = 600,
            Bold        = 700,
            BoldExtra    = 800,
        };
    };*/

}}
