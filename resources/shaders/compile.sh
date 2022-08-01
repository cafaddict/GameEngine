#!/bin/bash
parent_path=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )
cd "$parent_path"
../../external/glslc/linux/glslc vert.vert -o vert.spv
../../external/glslc/linux/glslc frag.frag -o frag.spv