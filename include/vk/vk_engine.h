#pragma once

#include "vk/vk_types.h"
#include "vk/vk_descriptors.h"
#include "vk/vk_pipelines.h"
#include <deque>
#include <vulkan/vulkan_core.h>

/*
 * Doing callbacks like this is **inneficient** at scale, because we are storing 
 * whole std::functions for every object we are deleting, which is not going 
 * to be optimal. For the amount of objects we will use in this tutorial, its 
 * going to be fine. but if you need to delete thousands of objects and want 
 * them deleted faster, a better implementation would be to store arrays of 
 * vulkan handles of various types such as VkImage, VkBuffer, and so on. And 
 * then delete those from a loop.
 */
struct DeletionQueue {
    std::deque<std::function<void()>> deletors;

    void push_function(std::function<void()>&& function) {
        deletors.push_back(function);
    }

    void flush() {
        for (auto it = deletors.rbegin(); it != deletors.rend(); it++) {
            (*it)();
        }

        deletors.clear();
    }
};

struct FrameData {
    VkCommandPool _commandPool;
    VkCommandBuffer _mainCommandBuffer;

    VkSemaphore _swapchainSemaphore, _renderSemaphore;
    VkFence _renderFence;

    DeletionQueue _deletionQueue;
};

// Double-Buffering
constexpr unsigned int FRAME_OVERLAP = 2;

class VulkanEngine {
public:
    // Window variables
    bool _isInitialized {false};
    int _frameNumber {0};
    bool stop_rendering {false};

    int WIDTH {1440};
    int HEIGHT {810};
    VkExtent2D _windowsExtent;
    struct GLFWwindow* _window {nullptr};

    // Vulkan Variables
    VkInstance _instance;
    VkDebugUtilsMessengerEXT _debug_messenger;
    VkPhysicalDevice _chosenGPU;
    VkDevice _device;
    VkSurfaceKHR _surface;

    // VK Swapchain
    VkSwapchainKHR _swapchain;
    VkFormat _swapchainImageFormat;

    std::vector<VkImage> _swapchainImages;
    std::vector<VkImageView> _swapchainImageViews;
    VkExtent2D _swapchainExtent;

    // Memory Allocator
    VmaAllocator _allocator;

    // Frame Data variables
    FrameData _frames[FRAME_OVERLAP];
    FrameData& get_current_frame() { return _frames[_frameNumber % FRAME_OVERLAP]; }

    // Command Queue
    VkQueue _graphicsQueue;
    uint32_t _graphicsQueueFamily;

    // Drawing Resources
    AllocatedImage _drawImage;
    VkExtent2D _drawExtent;


    DeletionQueue _mainDeletionQueue;

    // Descriptor Layouts and Allocators
    DescriptorAllocator globalDescriptorAllocator;

    VkDescriptorSet _drawImageDescriptors;
    VkDescriptorSetLayout _drawImageDescriptorLayout;

    // Pipelines
    VkPipeline _gradientPipeline;
    VkPipelineLayout _gradientPipelineLayout;

    // Global Functions
    static VulkanEngine& Get();
    void init();
    void cleanup();
    void draw();
    void run();

private:
    // GLFW Callback functions
    static void window_iconify_callback(GLFWwindow* window, int iconified);

    // Vulkan Inits
    void init_vulkan();
    void init_swapchain();
    void init_commands();
    void init_sync_structures();
    void init_descriptors();
    void init_pipelines();
    void init_background_pipelines();

    // Swapchain
    void create_swapchain(uint32_t width, uint32_t height);
    void destroy_swapchain();

    // Drawing function
    void draw_background(VkCommandBuffer cmd);
};
