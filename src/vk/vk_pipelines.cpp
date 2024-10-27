#include "vk/vk_pipelines.h"

#include <fstream>
#include <vector>
#include "vk/vk_initializers.h"

bool vkutil::load_shader_module(const char *filePath, 
        VkDevice device, VkShaderModule *outShaderModule) {
    std::ifstream file(filePath, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        return false;
    }

    // find what the size of the file is by looking up the location of the cursor
    // because the cursor is at the end, it gives the size directly in bytes
    size_t fileSize = (size_t) file.tellg();

    // spirv expects the buffer to be on uint32, so make sure to reserve a int
    // vector big enough for the entire file
    std::vector<uint32_t> buffer(fileSize / sizeof(uint32_t));

    // Set cursor to the front, read contents of file into buffer, and close
    file.seekg(0);
    file.read((char*) buffer.data(), fileSize); 
    file.close();

    // Create new shader module using the buffer
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.pNext = nullptr;

    // codeSize has to be in bytes
    createInfo.codeSize = buffer.size() * sizeof(uint32_t);
    createInfo.pNext = nullptr;
    createInfo.pCode = buffer.data();

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device, &createInfo, 
            nullptr, &shaderModule) != VK_SUCCESS) {
        return false;
    }

    *outShaderModule = shaderModule;
    return true;
}
