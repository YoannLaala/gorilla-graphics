#include "command_allocator.hpp"

namespace Gorilla { namespace Graphics
{
    CommandAllocator::CommandAllocator(void *handle, CommandType type)
        : m_handle(handle)
        , m_type(type)
    {

    }

    CommandAllocator::~CommandAllocator()
    {

    }
}}
