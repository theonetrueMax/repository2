#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <vector>

#include "Utilities.h"

class VulkanRenderer
{
public:
	VulkanRenderer();

	int init(GLFWwindow* newWindow);
	void cleanup();

	~VulkanRenderer();

private:
	GLFWwindow* window;

	//vulkan components
	VkInstance instance; //core

	struct {
		VkPhysicalDevice physicalDevice;
		VkDevice logicalDevice;
	}mainDevice;

	VkQueue graphicsQueue;

	//vulkan function
	//-create function
	void createInstance();
	void createLogicalDevice();

	// get functions
	void getPhysicalDevice();

	//support function
	//--checker functions
	bool checkInstanceExtensionSupport(std::vector<const char*>* checkExtensions);
	bool checkDeviceSuitable(VkPhysicalDevice device);
	//--get functions
	 QueueFamilyIndices getQueueFamilies(VkPhysicalDevice device);
};

