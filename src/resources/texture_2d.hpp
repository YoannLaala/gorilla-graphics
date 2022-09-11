#pragma once

#include <gorilla-core/container/vector.hpp>

namespace Gorilla { namespace Graphics
{
    class DescriptorHeap;
}}

namespace Gorilla { namespace Graphics
{
    class ResourceView
    {
        friend class Device;

    private:
        ResourceView(DescriptorHeap *descriptor_heap, uint64_t offset);

    public:
        ResourceView();
        ~ResourceView();

    public:
        inline const DescriptorHeap* get_descriptor_heap() const;
        inline uint64_t get_offset() const;

    private:
        DescriptorHeap* m_descriptor_heap;
        uint64_t m_offset;
    };

    const DescriptorHeap* ResourceView::get_descriptor_heap() const
    {
        return m_descriptor_heap;
    }

    uint64_t ResourceView::get_offset() const
    {
        return m_offset;
    }

    class Texture2D
    {
        friend class Device;

    private:
        Texture2D(void *handle);
        ~Texture2D();

    public:
        inline void* get_handle();

        inline uint32_t get_view_count() const;
        inline void add_view(ResourceView resource_view);
        inline ResourceView get_view(uint32_t index);

    private:
        void* m_handle;
        Vector<ResourceView> m_views;
    };

    void* Texture2D::get_handle()
    {
        return m_handle;
    }

    uint32_t Texture2D::get_view_count() const
    {
        return m_views.get_size();
    }

    void Texture2D::add_view(ResourceView resource_view)
    {
        m_views.add(resource_view);
    }

    ResourceView Texture2D::get_view(uint32_t index)
    {
        return m_views[index];
    }
}}
