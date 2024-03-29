#pragma once

#ifndef VULKANRENDERER_H
#define VULKANRENDERER_H

#include <Vulkan/vulkan.h>
#include <cstdio>
#include <array>
#include "SDL2/SDL.h"
#include "SDL2/SDL_vulkan.h"
#include <vector>
#include <cstring>
#include <optional>
#include <set>
#include <string>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <unordered_map>
#include <chrono>


#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

// Forward declaration to model helper
class ModelHelper;
class TextureHelper;

#ifdef NDEBUG
const bool enableValLayers = false;
#else
const bool enableValLayers = true;
#endif

struct UniformBufferObject {
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};

// Queue family struct
struct QueueFamilyIndices {
	// Graphics families initialization
	std::optional<uint32_t> graphicsFamily;

	// Present families initialization as well
	std::optional<uint32_t> presentFamily;

	// General check to make things a bit more conveneient
	bool isComplete() { return (graphicsFamily.has_value() && presentFamily.has_value()); }
};

// Swap chain support details struct - holds information to create the swapchain
struct SWChainSuppDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;

	VkSurfaceFormatKHR chooseSwSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwPresMode(const std::vector<VkPresentModeKHR>& availablePresModes);
	VkExtent2D chooseSwExtent(const VkSurfaceCapabilitiesKHR& capabilities, SDL_Window* window);
};

class VulkanRenderer {

private:
	uint32_t imageIndex;

	// SDL Surface handle
	VkSurfaceKHR surface;

	// Find the queue families given a physical device, called in isSuitable to find if the queue families support VK_QUEUE_GRAPHICS_BIT
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physicalDevice);

	// Swap chain handles
	VkSwapchainKHR swapChain;
	std::vector<VkImage> SWChainImages;
	VkFormat SWChainImageFormat;
	VkExtent2D SWChainExtent;
	std::vector<VkImageView> SWChainImageViews;

	// Command Buffers - Command pool and command buffer handles
	VkCommandPool commandPool;

	// Color image and mem handles
	VkImage colorImage;
	VkDeviceMemory colorImageMemory;
	VkImageView colorImageView;

	// Depth image and mem handles
	VkImage depthImage;
	VkDeviceMemory depthImageMemory;
	VkImageView depthImageView;

	// Handle to hold the frame buffers
	std::vector<VkFramebuffer> SWChainFrameBuffers;

	// Uniform buffers handle
	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBuffersMemory;

	// Descriptor set handles
	std::vector<VkDescriptorSet> descriptorSets;


	// Handles for the two semaphores - one for if the image is available and the other to present the image
	std::vector<VkSemaphore> imageAcquiredSema;
	std::vector<VkSemaphore> renderedSema;

	// Handle for the in-flight-fence
	std::vector<VkFence> inFlightFences;
	std::vector<VkFence> imagesInFlight;

	// Private helper methods ///////////////////////////////////////////////////////////////////////////////////////////////////////////
	void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
	bool checkExtSupport(VkPhysicalDevice physicalDevice);

	SWChainSuppDetails getDetails(VkPhysicalDevice physicalDevice);
	bool isSuitable(VkPhysicalDevice physicalDevice);

	VkFormat findDepthFormat();
	VkFormat findSupportedFormat(const std::vector<VkFormat>& potentialFormats, VkImageTiling tiling, VkFormatFeatureFlags features);

	void recordCommandBuffer(std::vector<ModelHelper*> models, std::vector<TextureHelper*> textures, VkCommandBuffer commandBuffer, uint32_t imageIndex);

	// Helper methods for the graphics pipeline
	static std::vector<char> readFile(const std::string& fileName);

	VkShaderModule createShaderModule(const std::vector<char>& binary);

	// Additional swap chain methods
	void cleanupSWChain();


public:
	VkClearValue clearValue;
	// Extension and validation arrays
	const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};
	const std::vector<const char*> deviceExts = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	};

	float camX = 5.0f;
	float camY = 0.0f;
	float camZ = 0.0f;

	bool rotate = false;

	bool frBuffResized = false;

	VkPhysicalDevice GPU = VK_NULL_HANDLE;
	VkDevice device;

	int numModels;
	int numTextures;
	size_t currentFrame = 0;

	// Handles for all variables, made public so they can be accessed by main and destroys
	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	// Pipeline Layout for "gloabls" to change shaders
	VkPipelineLayout pipeLineLayout;
	// The graphics pipeline handle
	VkPipeline graphicsPipeline;
	// Render pass handles
	VkRenderPass renderPass;
	// Handle for the list of command buffers
	std::vector<VkCommandBuffer> commandBuffers;
	// Descriptor pool handles
	VkDescriptorPool descriptorPool;
	// Device and queue handles
	VkQueue graphicsQueue;
	VkQueue presentQueue;

	QueueFamilyIndices QFIndices;

	VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;

	// Descriptor Set Layout Handle
	VkDescriptorSetLayout uniformDescriptorSetLayout;
	VkDescriptorSetLayout textureDescriptorSetLayout;

	// Create the vulkan instance
	VkInstance createVulkanInstance(SDL_Window* window, const char* appName);
	// Check if the validation layers requested are supported
	bool checkValLayerSupport();
	// Debug messenger methods - create, populate, and destroy the debug messenger
	void setupDebugMessenger(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger);
	// Create the SDL surface using Vulkan
	void createSurface(SDL_Window* window);
	// Poll through the available physical devices and choose one using isSuitable()
	void pickPhysicalDevice();
	// Using the physical device, create the logical device
	void createLogicalDevice();
	// Initialize the swap chain
	void createSWChain(SDL_Window* window);
	// With the swap chain, create the image views
	void createImageViews();
	// Create the render pass
	void createRenderPass();
	// Create the descriptor set layout
	void createDescriptorSetLayout();
	// Create the graphics pipeline
	void createGraphicsPipeline();
	// You have to first record all the operations to perform, so we need a command pool
	void createCommandPool();
	// Color image function
	void createColorResources();
	// Create depth image function
	void createDepthResources();
	// Creating the all-important frame buffer
	void createFrameBuffer();
	// Texture image creation
	void createTextureImage();
	// Texture image's view
	void createTextureImageView();
	// Texture image's sampler
	void createTextureImageSampler();
	// Load all models
	
	// Creation of uniform buffers
	void createUniformBuffers();
	// Descriptor Pool creation
	void createDescriptorPool();
	// Descriptor Set Creations
	void createDescriptorSets();
	// Create a list of command buffer objects
	void createCommandBuffers(int numFramesInFlight);
	// Create the semaphores, signaling objects to allow asynchronous tasks to happen at the same time
	void createSemaphores(const int maxFramesInFlight);


	void recreateSwapChain(SDL_Window* window);

	// Display methods - uniform buffer and fetching new frames
	void updateUniformBuffer(uint32_t currentImage);
	void drawNewFrame(SDL_Window* window, std::vector<ModelHelper*> models, std::vector<TextureHelper*> textures, int maxFramesInFlight);
	void postDrawEndCommandBuffer(VkCommandBuffer commandBuffer, SDL_Window* window, std::vector<ModelHelper*> models, int maxFramesInFlight);

	// For use from helper classes
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

	void freeEverything(int framesInFlight, std::vector<ModelHelper*> models);

	// HELPER METHODS

	VkCommandBuffer beginSingleTimeCommands();
	void endSingleTimeCommands(VkCommandBuffer commandBuffer);

	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	void createImage(uint32_t width, uint32_t height, uint32_t mipLevel, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
};

#endif