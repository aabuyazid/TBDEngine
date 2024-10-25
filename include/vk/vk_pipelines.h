#pragma once

#include "vk/vk_types.h"

namespace vkutil {
    bool load_shader_module(const char* filePath, VkDevice device, 
            VkShaderModule* outShaderModule);
}
