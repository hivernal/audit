#ifndef IMGUI_GLFW_VULKAN_IMGUI_GLFW_VULKAN_H_
#define IMGUI_GLFW_VULKAN_IMGUI_GLFW_VULKAN_H_

#include <array>
#include <cstdint>
#include <optional>
#include <string_view>
#include <vector>

#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_vulkan.h"
#include "imgui/imgui.h"

#define GLFW_INCLUDE_VULKAN
#include "glfw/include/GLFW/glfw3.h"

class ImGuiGlfwVulkan {
 public:
  ImGuiGlfwVulkan(std::string_view name = "Window", int width = 800,
                  int height = 600);
  ImGuiGlfwVulkan(const ImGuiGlfwVulkan&) = default;
  ImGuiGlfwVulkan& operator=(const ImGuiGlfwVulkan&) = default;
  ImGuiGlfwVulkan(ImGuiGlfwVulkan&&) = default;
  ImGuiGlfwVulkan& operator=(ImGuiGlfwVulkan&&) = default;
  virtual ~ImGuiGlfwVulkan();
  void Run();

 private:
  struct QueueFamilyIndices {
    std::optional<std::uint32_t> graphics_family{};
    std::optional<std::uint32_t> present_family{};
    bool IsComplete() {
      return graphics_family.has_value() && present_family.has_value();
    }
  };

  struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> present_modes;
    bool IsComplete() { return !formats.empty() && !present_modes.empty(); }
  };

  void InitWindow(const char* name);
  void InitVulkan(const char* name);
  void CreateInstance(const char* name);
  bool CheckValidationLayerSupport();
  std::vector<const char*> GetRequiredExtensions();
  void SetupDebugMessenger();
  void CreateSurface();
  void PickPhysicalDevice();
  VkPhysicalDevice SelectPhysicalDevice(
      const std::vector<VkPhysicalDevice>& devices);
  int RateDeviceSuitability(const VkPhysicalDevice& device);
  ImGuiGlfwVulkan::QueueFamilyIndices FindQueueFamilies(
      const VkPhysicalDevice& device);
  bool CheckDeviceExtensionSupport(const VkPhysicalDevice& device);
  ImGuiGlfwVulkan::SwapChainSupportDetails QuerySwapChainSupport(
      const VkPhysicalDevice& device);
  void CreateLogicalDevice();
  VkResult CreateDescriptorPool();
  void InitImGui();
  void SetupImGuiWindow();
  virtual void Render() = 0;
  void FrameRender(ImDrawData* draw_data);
  VkSemaphore GetImageAcquiredSemaphore();
  VkSemaphore GetRenderCompleteSemaphore();
  VkResult BeginCommandBuffer(const ImGui_ImplVulkanH_Frame* fd);
  void CmdBeginRenderPass(const ImGui_ImplVulkanH_Frame* fd);
  VkResult QueueSubmit(const ImGui_ImplVulkanH_Frame* fd,
                       const VkSemaphore* image_semaphore,
                       const VkSemaphore* render_semaphore);
  void FramePresent();

  GLFWwindow* window_{nullptr};
  int window_width_{800};
  int window_height_{600};
  VkInstance instance_{};
  static constexpr std::array<const char*, 1> kValidationLayers = {
      "VK_LAYER_KHRONOS_validation"};
  static constexpr std::array<const char*, 1> kDeviceExtensions = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME};
#ifdef NDEBUG
  static constexpr bool kEnableValidationLayers{false};
#else
  static constexpr bool kEnableValidationLayers{true};
#endif
  VkDebugUtilsMessengerEXT debug_messenger_{};
  VkPhysicalDevice physical_device_{VK_NULL_HANDLE};
  VkDevice device_{};
  ImGuiGlfwVulkan::QueueFamilyIndices queue_family_{};
  VkQueue graphics_queue_{};
  VkQueue present_queue_{};
  VkSurfaceKHR surface_{};
  VkDescriptorPool descriptor_pool_{VK_NULL_HANDLE};
  ImGui_ImplVulkanH_Window window_data_{};
  std::uint32_t min_image_count_{2};
  bool swap_chain_rebuild_{false};
  ImGuiIO* io_{nullptr};
};

#endif  // IMGUI_GLFW_VULKAN_IMGUI_GLFW_VULKAN_H_
