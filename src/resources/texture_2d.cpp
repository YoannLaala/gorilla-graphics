#include "texture_2d.hpp"

namespace Gorilla { namespace Graphics
{
    ResourceView::ResourceView()
        : m_descriptor_heap(nullptr)
        , m_offset(0)
    {

    }

    ResourceView::ResourceView(DescriptorHeap *descriptor_heap, uint64_t offset)
        : m_descriptor_heap(descriptor_heap)
        , m_offset(offset)
    {

    }

    ResourceView::~ResourceView()
    {

    }

    Texture2D::Texture2D(void *handle)
        : m_handle(handle)
    {

    }

    Texture2D::~Texture2D()
    {

    }
}}
