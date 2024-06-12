#include "imgui_glfw_vulkan.h"

#include <algorithm>
#include <array>
#include <cstring>
#include <iostream>
#include <limits>
#include <map>
#include <set>
#include <stdexcept>

VkResult CreateDebugUtilsMessengerExt(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* create_info,
    const VkAllocationCallbacks* allocator,
    VkDebugUtilsMessengerEXT* debug_messenger) {
  auto func{reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
      vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"))};
  if (func != nullptr) {
    return func(instance, create_info, allocator, debug_messenger);
  } else {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
}

void DestroyDebugUtilsMessengerExt(VkInstance instance,
                                   VkDebugUtilsMessengerEXT debug_messenger,
                                   const VkAllocationCallbacks* allocator) {
  auto func{reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
      vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"))};
  if (func != nullptr) {
    func(instance, debug_messenger, allocator);
  }
}

ImGuiGlfwVulkan::ImGuiGlfwVulkan(std::string_view name, int width, int height)
    : window_width_{width}, window_height_{height}, window_data_{} {
  InitWindow(name.data());
  InitVulkan(name.data());
  InitImGui();
}

void ImGuiGlfwVulkan::InitWindow(const char* name) {
  if (!glfwInit()) {
    throw std::runtime_error("failed to init glfw!");
  }
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  window_ =
      glfwCreateWindow(window_width_, window_height_, name, nullptr, nullptr);
  if (!window_) {
    throw std::runtime_error("failed to create window!");
  }
}

ImGuiGlfwVulkan::~ImGuiGlfwVulkan() {
  vkDeviceWaitIdle(device_);
  ImGui_ImplVulkan_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  ImGui_ImplVulkanH_DestroyWindow(instance_, device_, &window_data_, nullptr);
  vkDestroyDescriptorPool(device_, descriptor_pool_, nullptr);
  vkDestroyDevice(device_, nullptr);
  if (kEnableValidationLayers) {
    DestroyDebugUtilsMessengerExt(instance_, debug_messenger_, nullptr);
  }
  vkDestroyInstance(instance_, nullptr);
  glfwDestroyWindow(window_);
  glfwTerminate();
}

void ImGuiGlfwVulkan::InitVulkan(const char* name) {
  CreateInstance(name);
  SetupDebugMessenger();
  CreateSurface();
  PickPhysicalDevice();
  CreateLogicalDevice();
}

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
    [[maybe_unused]] VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT message_type,
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
    [[maybe_unused]] void* user_data) {
  std::cerr << "validation layer: " << callback_data->pMessage << std::endl;
  return VK_FALSE;
}

void PopulateDebugMessengerCreateInfo(
    VkDebugUtilsMessengerCreateInfoEXT* create_info) {
  *create_info = {};
  create_info->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  create_info->messageSeverity =
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  create_info->messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  create_info->pfnUserCallback = DebugCallback;
}

void ImGuiGlfwVulkan::CreateInstance(const char* name) {
  if (kEnableValidationLayers && !CheckValidationLayerSupport()) {
    throw std::runtime_error("validation layers requested, but not available!");
  }
  VkApplicationInfo app_info{};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName = name;
  app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.pEngineName = "No Engine";
  app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.apiVersion = VK_API_VERSION_1_0;
  VkInstanceCreateInfo create_info{};
  VkDebugUtilsMessengerCreateInfoEXT debug{};
  if (kEnableValidationLayers) {
    create_info.enabledLayerCount =
        static_cast<std::uint32_t>(kValidationLayers.size());
    create_info.ppEnabledLayerNames = kValidationLayers.data();
    PopulateDebugMessengerCreateInfo(&debug);
    create_info.pNext = const_cast<VkDebugUtilsMessengerCreateInfoEXT*>(&debug);
  } else {
    create_info.enabledLayerCount = 0;
    create_info.pNext = nullptr;
  }
  auto extensions{GetRequiredExtensions()};
  create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
  create_info.ppEnabledExtensionNames = extensions.data();
  create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  create_info.pApplicationInfo = &app_info;
  if (vkCreateInstance(&create_info, nullptr, &instance_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create instance!");
  }
}

bool ImGuiGlfwVulkan::CheckValidationLayerSupport() {
  std::uint32_t layer_count;
  vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
  std::vector<VkLayerProperties> available_layers(layer_count);
  vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());
  for (const char* layer_name : kValidationLayers) {
    bool layer_found{false};
    for (const auto& layer_properties : available_layers) {
      if (strcmp(layer_name, layer_properties.layerName) == 0) {
        layer_found = true;
        break;
      }
    }
    if (!layer_found) {
      return false;
    }
  }
  return true;
}

std::vector<const char*> ImGuiGlfwVulkan::GetRequiredExtensions() {
  uint32_t glfw_extension_count;
  const char** glfw_extensions{nullptr};
  glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
  std::vector<const char*> extensions(glfw_extensions,
                                      glfw_extensions + glfw_extension_count);
  if (kEnableValidationLayers) {
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }
  return extensions;
}

void ImGuiGlfwVulkan::SetupDebugMessenger() {
  if (!kEnableValidationLayers) return;
  VkDebugUtilsMessengerCreateInfoEXT create_info{};
  PopulateDebugMessengerCreateInfo(&create_info);
  if (CreateDebugUtilsMessengerExt(instance_, &create_info, nullptr,
                                   &debug_messenger_) != VK_SUCCESS) {
    throw std::runtime_error("failed to set up debug messenger!");
  }
}

void ImGuiGlfwVulkan::CreateSurface() {
  if (glfwCreateWindowSurface(instance_, window_, nullptr, &surface_) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create window surface!");
  }
}

void ImGuiGlfwVulkan::PickPhysicalDevice() {
  std::uint32_t device_count{0};
  vkEnumeratePhysicalDevices(instance_, &device_count, nullptr);
  if (device_count == 0) {
    throw std::runtime_error("failed to find GPUs with Vulkan support!");
  }
  std::vector<VkPhysicalDevice> devices(device_count);
  vkEnumeratePhysicalDevices(instance_, &device_count, devices.data());
  physical_device_ = SelectPhysicalDevice(devices);
  if (physical_device_ == VK_NULL_HANDLE) {
    throw std::runtime_error("failed to find a suitable GPU!");
  }
}

VkPhysicalDevice ImGuiGlfwVulkan::SelectPhysicalDevice(
    const std::vector<VkPhysicalDevice>& devices) {
  std::multimap<int, VkPhysicalDevice> candidates;
  for (const auto& device : devices) {
    int score{RateDeviceSuitability(device)};
    candidates.insert(std::make_pair(score, device));
  }
  if (candidates.rbegin()->first > 0) return candidates.rbegin()->second;
  return VK_NULL_HANDLE;
}

int ImGuiGlfwVulkan::RateDeviceSuitability(const VkPhysicalDevice& device) {
  auto indices{FindQueueFamilies(device)};
  if (!indices.IsComplete()) return 0;
  auto extensions_supported{CheckDeviceExtensionSupport(device)};
  if (!extensions_supported) return 0;
  auto swap_chain_support{QuerySwapChainSupport(device)};
  if (!swap_chain_support.IsComplete()) return 0;
  VkPhysicalDeviceProperties device_properties;
  VkPhysicalDeviceFeatures device_feauteres;
  vkGetPhysicalDeviceProperties(device, &device_properties);
  vkGetPhysicalDeviceFeatures(device, &device_feauteres);
  int score{0};
  if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
    score += 1000;
  }
  score += static_cast<int>(device_properties.limits.maxImageDimension2D);
  if (!device_feauteres.geometryShader) {
    return 0;
  }
  return score;
}

ImGuiGlfwVulkan::QueueFamilyIndices ImGuiGlfwVulkan::FindQueueFamilies(
    const VkPhysicalDevice& device) {
  QueueFamilyIndices indices;
  std::uint32_t queue_family_count{0};
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count,
                                           nullptr);
  std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count,
                                           queue_families.data());
  std::uint32_t i{0};
  for (const auto& queue_family : queue_families) {
    if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphics_family = i;
    }
    VkBool32 present_support{false};
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface_, &present_support);
    if (present_support) {
      indices.present_family = i;
    }
    if (indices.IsComplete()) break;
    i++;
  }
  return indices;
}

bool ImGuiGlfwVulkan::CheckDeviceExtensionSupport(
    const VkPhysicalDevice& device) {
  std::uint32_t extension_count;
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count,
                                       nullptr);
  std::vector<VkExtensionProperties> available_extensions(extension_count);
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count,
                                       available_extensions.data());
  std::set<std::string> required_extensions(kDeviceExtensions.begin(),
                                            kDeviceExtensions.end());
  for (const auto& extension : available_extensions) {
    required_extensions.erase(extension.extensionName);
  }
  return required_extensions.empty();
}

ImGuiGlfwVulkan::SwapChainSupportDetails ImGuiGlfwVulkan::QuerySwapChainSupport(
    const VkPhysicalDevice& device) {
  SwapChainSupportDetails details{};
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface_,
                                            &details.capabilities);
  std::uint32_t format_count;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &format_count,
                                       nullptr);
  if (format_count != 0) {
    details.formats.resize(format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &format_count,
                                         details.formats.data());
  }
  std::uint32_t present_mode_count;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_,
                                            &present_mode_count, nullptr);
  if (present_mode_count != 0) {
    details.present_modes.resize(present_mode_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        device, surface_, &present_mode_count, details.present_modes.data());
  }

  return details;
}

void ImGuiGlfwVulkan::CreateLogicalDevice() {
  queue_family_ = FindQueueFamilies(physical_device_);
  std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
  std::set<std::uint32_t> unique_queue_families = {
      queue_family_.graphics_family.value(),
      queue_family_.present_family.value()};
  auto queue_priority{1.0f};
  for (std::uint32_t queue_family : unique_queue_families) {
    VkDeviceQueueCreateInfo queue_create_info{};
    queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_info.queueFamilyIndex = queue_family;
    queue_create_info.queueCount = 1;
    queue_create_info.pQueuePriorities = &queue_priority;
    queue_create_infos.push_back(queue_create_info);
  }
  VkPhysicalDeviceFeatures device_features{};
  VkDeviceCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  create_info.pEnabledFeatures = &device_features;
  create_info.enabledExtensionCount =
      static_cast<std::uint32_t>(kDeviceExtensions.size());
  create_info.ppEnabledExtensionNames = kDeviceExtensions.data();
  create_info.queueCreateInfoCount =
      static_cast<std::uint32_t>(queue_create_infos.size());
  create_info.pQueueCreateInfos = queue_create_infos.data();
  if (kEnableValidationLayers) {
    create_info.enabledLayerCount =
        static_cast<std::uint32_t>(kValidationLayers.size());
    create_info.ppEnabledLayerNames = kValidationLayers.data();
  } else {
    create_info.enabledLayerCount = 0;
  }
  if (vkCreateDevice(physical_device_, &create_info, nullptr, &device_) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create logical device!");
  }
  vkGetDeviceQueue(device_, queue_family_.graphics_family.value(), 0,
                   &graphics_queue_);
  vkGetDeviceQueue(device_, queue_family_.present_family.value(), 0,
                   &present_queue_);
  if (CreateDescriptorPool() != VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor pool!");
  }
  auto swap_chain_support{QuerySwapChainSupport(physical_device_)};
  min_image_count_ = swap_chain_support.capabilities.minImageCount + 1;
  if (swap_chain_support.capabilities.maxImageCount > 0 &&
      min_image_count_ > swap_chain_support.capabilities.maxImageCount) {
    min_image_count_ = swap_chain_support.capabilities.maxImageCount;
  }
}

VkResult ImGuiGlfwVulkan::CreateDescriptorPool() {
  constexpr std::array pool_sizes{
      VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1},
  };
  VkDescriptorPoolCreateInfo pool_info{};
  pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
  pool_info.maxSets = 1;
  pool_info.poolSizeCount = pool_sizes.size();
  pool_info.pPoolSizes = pool_sizes.data();
  return vkCreateDescriptorPool(device_, &pool_info, nullptr,
                                &descriptor_pool_);
}

void ImGuiGlfwVulkan::InitImGui() {
  SetupImGuiWindow();
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  io_ = &ImGui::GetIO();
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForVulkan(window_, true);
  ImGui_ImplVulkan_InitInfo init_info{};
  init_info.Instance = instance_;
  init_info.PhysicalDevice = physical_device_;
  init_info.Device = device_;
  init_info.QueueFamily = queue_family_.graphics_family.value();
  init_info.Queue = graphics_queue_;
  init_info.PipelineCache = VK_NULL_HANDLE;
  init_info.DescriptorPool = descriptor_pool_;
  init_info.RenderPass = window_data_.RenderPass;
  init_info.Subpass = 0;
  init_info.MinImageCount = min_image_count_;
  init_info.ImageCount = window_data_.ImageCount;
  init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
  init_info.Allocator = nullptr;
  ImGui_ImplVulkan_Init(&init_info);
}

void ImGuiGlfwVulkan::SetupImGuiWindow() {
  window_data_.Surface = surface_;
  constexpr std::array<VkFormat, 4> surface_image_format{
      VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM,
      VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM};
  const VkColorSpaceKHR surface_color_space{VK_COLORSPACE_SRGB_NONLINEAR_KHR};
  window_data_.SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(
      physical_device_, window_data_.Surface, surface_image_format.data(), 2,
      surface_color_space);
#ifdef APP_USE_UNLIMITED_FRAME_RATE
  constexpr std::array<VkPresentModeKHR, 3> present_modes{
      VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR,
      VK_PRESENT_MODE_FIFO_KHR};
#else
  constexpr std::array<VkPresentModeKHR, 1> present_modes{
      VK_PRESENT_MODE_FIFO_KHR};
#endif
  window_data_.PresentMode = ImGui_ImplVulkanH_SelectPresentMode(
      physical_device_, window_data_.Surface, present_modes.data(),
      present_modes.size());
  ImGui_ImplVulkanH_CreateOrResizeWindow(
      instance_, physical_device_, device_, &window_data_,
      queue_family_.graphics_family.value(), nullptr, window_width_,
      window_height_, min_image_count_);
}

void ImGuiGlfwVulkan::Run() {
  while (!glfwWindowShouldClose(window_)) {
    glfwPollEvents();
    int width{window_width_}, height{window_height_};
    glfwGetFramebufferSize(window_, &window_width_, &window_height_);
    if (width != window_width_ || height != window_height_) {
      swap_chain_rebuild_ = true;
    }
    if (swap_chain_rebuild_) {
      if (window_width_ > 0 && window_height_ > 0) {
        ImGui_ImplVulkan_SetMinImageCount(min_image_count_);
        ImGui_ImplVulkanH_CreateOrResizeWindow(
            instance_, physical_device_, device_, &window_data_,
            queue_family_.graphics_family.value(), nullptr, window_width_,
            window_height_, min_image_count_);
        window_data_.FrameIndex = 0;
        swap_chain_rebuild_ = false;
      }
    }
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    Render();
    ImGui::Render();
    ImDrawData* draw_data = ImGui::GetDrawData();
    const bool is_minimized(draw_data->DisplaySize.x <= 0.0f ||
                            draw_data->DisplaySize.y <= 0.0f);
    if (!is_minimized) {
      FrameRender(draw_data);
      FramePresent();
    }
  }
}

void ImGuiGlfwVulkan::FrameRender(ImDrawData* draw_data) {
  VkResult err;
  VkSemaphore image_acquired_semaphore{GetImageAcquiredSemaphore()};
  VkSemaphore render_complete_semaphore{GetRenderCompleteSemaphore()};
  err = vkAcquireNextImageKHR(device_, window_data_.Swapchain, UINT64_MAX,
                              image_acquired_semaphore, VK_NULL_HANDLE,
                              &window_data_.FrameIndex);
  if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR) {
    swap_chain_rebuild_ = true;
    return;
  }
  const ImGui_ImplVulkanH_Frame* fd{
      &window_data_.Frames[window_data_.FrameIndex]};
  err = vkWaitForFences(device_, 1, &fd->Fence, VK_TRUE, UINT64_MAX);
  err = vkResetFences(device_, 1, &fd->Fence);
  err = vkResetCommandPool(device_, fd->CommandPool, 0);
  err = BeginCommandBuffer(fd);
  CmdBeginRenderPass(fd);
  ImGui_ImplVulkan_RenderDrawData(draw_data, fd->CommandBuffer);
  vkCmdEndRenderPass(fd->CommandBuffer);
  err = vkEndCommandBuffer(fd->CommandBuffer);
  err = QueueSubmit(fd, &image_acquired_semaphore, &render_complete_semaphore);
}

VkSemaphore ImGuiGlfwVulkan::GetImageAcquiredSemaphore() {
  return window_data_.FrameSemaphores[window_data_.SemaphoreIndex]
      .ImageAcquiredSemaphore;
}

VkSemaphore ImGuiGlfwVulkan::GetRenderCompleteSemaphore() {
  return window_data_.FrameSemaphores[window_data_.SemaphoreIndex]
      .RenderCompleteSemaphore;
}

VkResult ImGuiGlfwVulkan::BeginCommandBuffer(
    const ImGui_ImplVulkanH_Frame* fd) {
  VkCommandBufferBeginInfo info{};
  info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  return vkBeginCommandBuffer(fd->CommandBuffer, &info);
}

void ImGuiGlfwVulkan::CmdBeginRenderPass(const ImGui_ImplVulkanH_Frame* fd) {
  VkRenderPassBeginInfo info{};
  info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  info.renderPass = window_data_.RenderPass;
  info.framebuffer = fd->Framebuffer;
  info.renderArea.extent.width = static_cast<std::uint32_t>(window_data_.Width);
  info.renderArea.extent.height =
      static_cast<std::uint32_t>(window_data_.Height);
  info.clearValueCount = 1;
  info.pClearValues = &window_data_.ClearValue;
  vkCmdBeginRenderPass(fd->CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
}

VkResult ImGuiGlfwVulkan::QueueSubmit(const ImGui_ImplVulkanH_Frame* fd,
                                      const VkSemaphore* image_semaphore,
                                      const VkSemaphore* render_semaphore) {
  VkPipelineStageFlags wait_stage{
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  VkSubmitInfo info{};
  info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  info.waitSemaphoreCount = 1;
  info.pWaitSemaphores = image_semaphore;
  info.pWaitDstStageMask = &wait_stage;
  info.commandBufferCount = 1;
  info.pCommandBuffers = &fd->CommandBuffer;
  info.signalSemaphoreCount = 1;
  info.pSignalSemaphores = render_semaphore;
  return vkQueueSubmit(graphics_queue_, 1, &info, fd->Fence);
}

void ImGuiGlfwVulkan::FramePresent() {
  if (swap_chain_rebuild_) return;
  VkSemaphore render_complete_semaphore{
      window_data_.FrameSemaphores[window_data_.SemaphoreIndex]
          .RenderCompleteSemaphore};
  VkPresentInfoKHR info{};
  info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  info.waitSemaphoreCount = 1;
  info.pWaitSemaphores = &render_complete_semaphore;
  info.swapchainCount = 1;
  info.pSwapchains = &window_data_.Swapchain;
  info.pImageIndices = &window_data_.FrameIndex;
  VkResult err = vkQueuePresentKHR(graphics_queue_, &info);
  if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR) {
    swap_chain_rebuild_ = true;
    return;
  }
  window_data_.SemaphoreIndex =
      (window_data_.SemaphoreIndex + 1) % window_data_.SemaphoreCount;
}
