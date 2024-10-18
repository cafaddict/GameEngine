#!/bin/bash
parent_path=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )
cd "$parent_path"
# ../../external/glslc/apple/glslc vert.vert -o vert.spv
# ../../external/glslc/apple/glslc frag.frag -o frag.spv

# ../../external/glslc/apple/glslc vert_vulkan_pbr.vert -o vert_vulkan_pbr.spv
# ../../external/glslc/apple/glslc frag_vulkan_pbr.frag -o frag_vulkan_pbr.spv

# ../../external/glslc/apple/glslc vert_vulkan_pbr.vert -o vert_vulkan_pbr.spv
# ../../external/glslc/apple/glslc frag_vulkan_pbr.frag -o frag_vulkan_pbr.spv


# Compile all vertex and fragment shaders in the current directory
for shader in *.vert *.frag *.comp; do
    if [ -f "$shader" ]; then
        output="${shader%.*}.spv"  # Create output filename by replacing extension with .spv
        echo "Compiling $shader to $output"
        ../../external/glslc/apple/glslc "$shader" -o "$output"
    fi
done