#include <gorilla-core/log/log.hpp>
#include <gorilla-core/process/application.hpp>
#include "command_list.hpp"
#include "command_queue.hpp"
#include "resources/swapchain.hpp"
#include "window.hpp"
#include "device.hpp"

using namespace Gorilla;
using namespace Gorilla::Graphics;

#define TEST(_condition_) LOG_ASSERT(_condition_, #_condition_);

void test_device()
{
    auto device_infos_result = Device::list();
    TEST(device_infos_result == true);
    TEST(device_infos_result.Value.get_size() > 0);

    auto* device = new Device();
    TEST(device->initialize() == ErrorCode::NONE);
    TEST(device->release() == ErrorCode::NONE);

    TEST(device->initialize() == ErrorCode::NONE);
}

struct test_window_receiver
{
    uint32_t width = 0;
    uint32_t height = 0;
    bool visibility = false;

    void on_size_changed(uint32_t w, uint32_t h)
    {
        width = w;
        height = h;
    }

    void on_visibility_changed(bool v)
    {
        visibility = v;
    }

    void on_closed()
    {
        Application::quit();
    }
};

struct GraphicsContext
{
    Device* device;
    CommandQueue* command_queue;
    DescriptorHeap* descriptor_heap;
    CommandAllocator* command_allocator;
    CommandList* command_list;
    Fence* fence;
    uint64_t frame_id;

    SwapChain* swapchain;
};

int32_t test_window_loop(void *user_data)
{
    GraphicsContext *graphics_context = reinterpret_cast<GraphicsContext*>(user_data);

    uint32_t frame_index = graphics_context->frame_id % graphics_context->swapchain->get_buffer_count();
    graphics_context->command_list->Reset(graphics_context->command_allocator);
    graphics_context->command_list->ResourceBarrier(graphics_context->command_allocator, nullptr, graphics_context->swapchain->get_buffer(frame_index), ResourceState::PRESENT, ResourceState::RENDER_TARGET);
    graphics_context->command_list->Clear(graphics_context->swapchain->get_buffer(frame_index), ColorRGBA(255, 0, 0));
    graphics_context->command_list->ResourceBarrier(graphics_context->command_allocator, nullptr, graphics_context->swapchain->get_buffer(frame_index), ResourceState::RENDER_TARGET, ResourceState::PRESENT);
    graphics_context->command_list->Close();
    graphics_context->command_queue->execute(graphics_context->command_list);

    graphics_context->command_queue->present(graphics_context->swapchain);

    graphics_context->command_queue->flush(graphics_context->fence, ++graphics_context->frame_id);

    return 0;
}

void test_window()
{
    Window *window = new Window("foo", 1280, 720);
    TEST(window->get_handle() != nullptr);

    test_window_receiver receiver;
    window->on_size_changed.connect(&receiver, &test_window_receiver::on_size_changed);
    window->on_visibility_changed.connect(&receiver, &test_window_receiver::on_visibility_changed);
    window->on_closed.connect(&receiver, &test_window_receiver::on_closed);

    window->show();
    TEST(receiver.width == 1280);
    TEST(receiver.height == 720);
    TEST(receiver.width == window->get_width());
    TEST(receiver.height == window->get_height());
    TEST(receiver.visibility == true);

    window->hide();
    TEST(receiver.visibility == false);

    window->show();

    Device *device = new Device();
    device->initialize();

    Result<CommandQueue*> command_queue_result = device->create_command_queue(CommandType::DIRECT);
    Result<DescriptorHeap*> descriptor_heap_result = device->create_descriptor_heap(DescriptorType::RENDER_TARGET, 3);
    Result<CommandAllocator*> command_allocator_result = device->create_command_allocator(CommandType::DIRECT);
    Result<CommandList*> command_list_result = device->create_command_list(command_allocator_result.Value);
    Result<Fence*> fence_result = device->create_fence(0);
    Result<SwapChain*> swapchain_result = device->create_swapchain(command_queue_result.Value, descriptor_heap_result.Value, window, PixelFormat::R8G8B8A8_UNORM, 3);

    GraphicsContext context =
    {
        device,
        command_queue_result.Value,
        descriptor_heap_result.Value,
        command_allocator_result.Value,
        command_list_result.Value,
        fence_result.Value,
        0,
        swapchain_result.Value,
    };

    int32_t result = Application::run(test_window_loop, &context);
    TEST(result == 0);

    device->release();
    delete device;
}

int32_t main(int32_t argc, const char *argv[])
{
    static const struct
    {
        const char *name;
        void (*callback)();
    } tests[] =
    {
        {"device",    &test_device},
        {"window",    &test_window},
    };

    const char *unit_test_name = argc < 2 ? nullptr: argv[1];
    const uint32_t test_count = sizeof(tests) / sizeof(*tests);
    for(uint32_t i = 0; i < test_count; i++)
    {
        bool execute_test = unit_test_name == nullptr || strcmp(tests[i].name, unit_test_name) == 0;
        if(execute_test)
        {
            tests[i].callback();
            if(unit_test_name)
                return 0;
        }
    }

    return 0;
}
