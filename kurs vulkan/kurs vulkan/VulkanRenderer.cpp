#include "VulkanRenderer.h"

VulkanRenderer::VulkanRenderer()
{
}

int VulkanRenderer::init(GLFWwindow* newWindow)
{
	window = newWindow;
	try {
		createInstance();
		getPhysicalDevice();
		createLogicalDevice();//in this order
	}
	catch (const std::runtime_error& e) {
		printf("ERROR %s/n", e.what());
		return EXIT_FAILURE;
	}

	return 0;
}

void VulkanRenderer::cleanup()
{
	vkDestroyDevice(mainDevice.logicalDevice, nullptr);
	vkDestroyInstance(instance, nullptr);//our custom pointer for nullptr
}

VulkanRenderer::~VulkanRenderer()
{
}

void VulkanRenderer::createInstance()
{
	//info about application itself, for developer convenience
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Vulkan App"; //custom name for app
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0); //custom version
	appInfo.pEngineName = "no engine";
	appInfo.engineVersion= VK_MAKE_VERSION(4, 2, 0);

	appInfo.apiVersion = VK_API_VERSION_1_0; //vulkan version important


	//create info for vkinstance 
	VkInstanceCreateInfo createInfo = {};

	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	//add extra data
	//createInfo.pNext=
	
	//createInfo.flags=

	createInfo.pApplicationInfo = &appInfo;

	//create list of instance extensions
	std::vector<const char*>instanceExtensions = std::vector<const char*>();

	//extension instance for use
	uint32_t glfwExtensionCount = 0; //glfw may require multiple extensions
	const char** glfwExtensions; //array of pointers to cstring

	//get glfw extensions
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	//add glfw extensions to list of extensoins

	for (size_t i = 0;i < glfwExtensionCount;++i) {

		instanceExtensions.push_back(glfwExtensions[i]);
	}

	//check instance extensions are supported
	if (!checkInstanceExtensionSupport(&instanceExtensions)) {
		
		throw std::runtime_error(" Vulkan instance doesnt support required extensions");
	}

	createInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());//glfwExtensionCount
	createInfo.ppEnabledExtensionNames = instanceExtensions.data();//glfwExtensions

	createInfo.enabledLayerCount = 0;//no validation layetrs for now
	createInfo.ppEnabledLayerNames = nullptr;

	//create instance
	VkResult result=vkCreateInstance(&createInfo, nullptr, &instance);//nullptr if u want to create ur own memory

	if (result != VK_SUCCESS) {

		throw std::runtime_error("failed to create a Vulkan instance");
	}
}

void VulkanRenderer::createLogicalDevice()
{
	//get the queue family indices for the chosen physical device (gpu)
	QueueFamilyIndices indices = getQueueFamilies(mainDevice.physicalDevice);
	

	//queue the logical device needs to create and info to do so
	VkDeviceQueueCreateInfo queueCreateInfo={};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = indices.graphicsFamily;//index of family to crete queue from
	queueCreateInfo.queueCount = 1; //number of quues to create
	float priority = 1.0f;
	queueCreateInfo.pQueuePriorities = &priority;//priority for vulcan (how to handle multiples queue 1=highiest)

	//information to create logical device (or just device)
	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;//
	deviceCreateInfo.queueCreateInfoCount = 1; //number of queue create info(ile tworzymy)
	deviceCreateInfo.pQueueCreateInfos=&queueCreateInfo;//list of queue create infos for device to create required queues(struct with info declared above)
	deviceCreateInfo.enabledExtensionCount = 0; //no extensions for logical device (num of enabled logical device extensions)
	deviceCreateInfo.ppEnabledExtensionNames = nullptr;//list of enabled extensions
	
	//physical device features the logical device will be using
	VkPhysicalDeviceFeatures deviceFeatures = {};
	deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

	//create logical device for the physical device
	VkResult result = vkCreateDevice(mainDevice.physicalDevice, &deviceCreateInfo, nullptr, &mainDevice.logicalDevice);
	
	if (result != VK_SUCCESS) {
		throw std::runtime_error("failde to create logical device");
	}

	//queues are created at the same time as the device
	//set handle to queues
	//from given logical device,, of given queue family, of given queue index,place reference in given VkQueue (&graphicsQueue)
	vkGetDeviceQueue(mainDevice.logicalDevice, indices.graphicsFamily,0,&graphicsQueue);//0 is index of queue but we created only 1,
}

void VulkanRenderer::getPhysicalDevice()
{
	//enumerate physical devices the vkiinstance can use
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount,nullptr);

	if (deviceCount == 0) {
		throw std::runtime_error("cant find gpus that support vulkan instance");
	}

	std::vector<VkPhysicalDevice>deviceList(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, deviceList.data());

	//temp mainDevice.physicalDevice = deviceList[0];
	for (const auto& device : deviceList) {

		if (checkDeviceSuitable(device)) {
			mainDevice.physicalDevice = device;
			break;
		}
	}

}

bool VulkanRenderer::checkInstanceExtensionSupport(std::vector<const char*>* checkExtensions)
{
	//rozmiar
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	//vector d³ugoœci rozmiar
	std::vector<VkExtensionProperties>extensions(extensionCount);
	//dane do vectora
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

	//spr czy extensions sa kompatybilne
	for (const auto &checkExtension  : *checkExtensions) {

		bool hasExtension = false;
		for (const auto& extension : extensions) {

			if (strcmp(checkExtension, extension.extensionName)) {///string compare
				hasExtension=true;
				break; 
			}
		}
		if (!hasExtension) {
			return false;
		}
	}
	return true;
}

bool VulkanRenderer::checkDeviceSuitable(VkPhysicalDevice device)
{
	/*
	//info about the ph device(id,name,type)
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(device,&deviceProperties);

	//info about what the device can do
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
	*/

	QueueFamilyIndices indices = getQueueFamilies(device);


	return indices.isValid();

	 
}

QueueFamilyIndices VulkanRenderer::getQueueFamilies(VkPhysicalDevice device)
{
	QueueFamilyIndices indices;
	//get family queue property info
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
	
	std::vector<VkQueueFamilyProperties> queueFamilyList(queueFamilyCount);

	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyList.data());
	
	//go through queues and check if it has at least 1 queue AND IS OF required type (VK_QUEUE_GRAPHICS_BIT)
	int i = 0;
	for (const auto& queueFamily : queueFamilyList) {

		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) { //& bo tak siê sprawdza vk_queue_*_bit, defined by bitfield 

			indices.graphicsFamily = i;
			break;
		}
		//if (indices.isValid())break;
		i++;
	}
	return indices;
}
