#pragma once
#pragma once

#include "BRDFLut.h"
#include <fstream>

class IrradianceCube {
private:
	Skybox* pSkybox_;

	struct {
		VkImage image;
		VkImageView view;
		VkDeviceMemory memory;
		VkFramebuffer framebuffer;
	} offscreen;

	struct PushBlock {
		glm::mat4 mvp;
		// Sampling deltas
		float deltaPhi = (2.0f * float(PI)) / 180.0f;
		float deltaTheta = (0.5f * float(PI)) / 64.0f;
	} pushBlock;

	VkDevice device_;
	DeviceHelper* pDevHelper_;
	uint32_t mipLevels_;
	VkFormat imageFormat_;

	uint32_t width_, height_;

	VkBuffer stagingBuffer_;
	VkDeviceMemory stagingBufferMemory_;

	VkImage iRCubeImage_;

	VkFramebuffer iRCubeFrameBuffer_;
	VkRenderPass iRCubeRenderpass_;

	VkDescriptorSetLayout iRCubeDescriptorSetLayout_;
	VkDescriptorPool iRCubeDescriptorPool_;
	VkDescriptorSet iRCubeDescriptorSet_;

	VkDeviceMemory iRCubeImageMemory_;

	VkPipelineLayout iRCubePipelineLayout_;
	VkPipeline iRCubePipeline_;

	VkAttachmentDescription iRCubeattachment;
	VkDescriptorImageInfo irImageInfo;

	VkQueue* pGraphicsQueue_;
	VkCommandPool* pCommandPool_;

	void createiRCubeImage();
	void createiRCubeImageView();
	void createiRCubeImageSampler();

	void createiRCubeDescriptors();

	void createRenderPass();
	void createFrameBuffer();

	void createPipeline();
	void render();

	uint32_t findMemoryType(VkPhysicalDevice gpu_, uint32_t typeFilter, VkMemoryPropertyFlags properties);
	std::vector<char> readFile(const std::string& filename);
	VkShaderModule createShaderModule(VkDevice dev, const std::vector<char>& binary);
	void transitionImageLayout(VkCommandBuffer cmdBuff, VkImageSubresourceRange subresourceRange, VkImageLayout oldLayout, VkImageLayout newLayout, VkImage prefImage);

	void endCommandBuffer(VkDevice device_, VkCommandBuffer cmdBuff, VkQueue* pGraphicsQueue_, VkCommandPool* pCommandPool_);

public:
	VkImageView iRCubeImageView_;
	VkSampler iRCubeImageSampler_;

	IrradianceCube(DeviceHelper* devHelper, VkQueue* graphicsQueue, VkCommandPool* cmdPool, Skybox* pSkybox);

	void geniRCube();
	VkDescriptorSet getDescriptorSet() { return this->iRCubeDescriptorSet_; };
};