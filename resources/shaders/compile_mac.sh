#!/bin/bash
parent_path=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )
cd "$parent_path"
# ../../external/glslc/apple/glslc vert.vert -o vert.spv
# ../../external/glslc/apple/glslc frag.frag -o frag.spv

../../external/glslc/apple/glslc vert_vulkan_pbr.vert -o vert_vulkan_pbr.spv
../../external/glslc/apple/glslc frag_vulkan_pbr.frag -o frag_vulkan_pbr.spv