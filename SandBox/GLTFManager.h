#pragma once

#include <vector>
#include <glm/gtc/type_ptr.hpp>
#include <filesystem>
#include "MeshHelper.h"

class GLTFObj {
public:
	struct SceneNode {
		SceneNode* parent;
		std::vector<SceneNode*> children;
		MeshHelper::MeshObj mesh;
		glm::mat4 transform;
	};
	bool isSkyBox = false;

	GLTFObj(std::string gltfPath, DeviceHelper* deviceHelper);
	void loadGLTF();

	void createDescriptors();

	void render(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout);
	void renderSkyBox(VkCommandBuffer commandBuffer, VkPipeline pipeline, VkDescriptorSet descSet, VkPipelineLayout pipelineLayout);

	uint32_t getTotalVertices() { return totalVertices_; };
	uint32_t getTotalIndices() { return totalIndices_; };
	MeshHelper* getMeshHelper() { return pSceneMesh; };
private:
	std::string gltfPath_;
	DeviceHelper* pDevHelper;
	uint32_t totalIndices_;
	uint32_t totalVertices_;
	MeshHelper* pSceneMesh;
	tinygltf::Model* pInputModel;
	std::vector<SceneNode*> pNodes;

	void loadImages(tinygltf::Model& in, std::vector<TextureHelper*>& images);
	void loadTextures(tinygltf::Model& in, std::vector<TextureHelper::TextureIndexHolder>& textures);
	void loadMaterials(tinygltf::Model& in, std::vector<MeshHelper::Material>& mats);
	void loadNode(tinygltf::Model& in, const tinygltf::Node& nodeIn, SceneNode* parent, std::vector<SceneNode*>& nodes);
	void drawIndexed(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, SceneNode* pNode);
	void drawSkyBoxIndexed(VkCommandBuffer commandBuffer, VkPipeline pipeline, VkDescriptorSet descSet, VkPipelineLayout pipelineLayout, SceneNode* node);
};