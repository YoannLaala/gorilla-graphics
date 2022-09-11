#include "descriptor_heap.hpp"

namespace Gorilla { namespace Graphics
{
    DescriptorHeap::DescriptorHeap(void *handle, DescriptorType type)
        : m_handle(handle)
        , m_type(type)
    {

    }

    DescriptorHeap::~DescriptorHeap()
    {

    }
}}
