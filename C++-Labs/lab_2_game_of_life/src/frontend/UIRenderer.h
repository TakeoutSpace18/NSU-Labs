#ifndef LAB_2_GAME_OF_LIFE_UIRENDERER_H
#define LAB_2_GAME_OF_LIFE_UIRENDERER_H

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN

#include <imgui_impl_vulkan.h>
#include <GLFW/glfw3.h>

#include <memory>
#include <lab_2_game_of_life/third-party/imgui/imgui.h>

#include "core/Universe.h"

class UIRenderer {
public:

    struct Config {
        int window_width;
        int window_height;
        std::string window_title;
    };

    UIRenderer() = default;
    int run(Config config);

    virtual void onFrameUpdate() = 0;

    virtual ~UIRenderer();

private:

    int initialize();
    int mainLoop();

    void SetupVulkan(ImVector<const char*> instance_extensions);
    VkPhysicalDevice SetupVulkan_SelectPhysicalDevice();
    void SetupVulkanWindow(ImGui_ImplVulkanH_Window* wd, VkSurfaceKHR surface, int width, int height);
    void FrameRender(ImGui_ImplVulkanH_Window* wd, ImDrawData* draw_data);
    void FramePresent(ImGui_ImplVulkanH_Window* wd);


    Config config_;

    GLFWwindow *window_ = nullptr;
    VkAllocationCallbacks*   allocator_ = nullptr;
    VkInstance               vulkan_instance_ = VK_NULL_HANDLE;
    VkPhysicalDevice         physical_device_ = VK_NULL_HANDLE;
    VkDevice                 device_ = VK_NULL_HANDLE;
    uint32_t                 queue_family_ = (uint32_t)-1;
    VkQueue                  queue_ = VK_NULL_HANDLE;
    VkDebugReportCallbackEXT debug_report_ = VK_NULL_HANDLE;
    VkPipelineCache          pipeline_cache_ = VK_NULL_HANDLE;
    VkDescriptorPool         descriptor_pool_ = VK_NULL_HANDLE;
    ImGui_ImplVulkanH_Window main_window_data_;
    int                      min_image_count_ = 2;
    bool                     swap_chain_rebuild_ = false;
};

#endif //LAB_2_GAME_OF_LIFE_UIRENDERER_H
