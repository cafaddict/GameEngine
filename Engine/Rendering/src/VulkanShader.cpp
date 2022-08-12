#include <VulkanShader.hpp>
// #include <VulkanRendering.hpp>

namespace Engine {
VulkanShader::~VulkanShader() {}

void VulkanShader::LoadShader(std::string_view vertexSrc,
                              std::string_view fragmentSrc) {
  auto vertShaderCode = readFile(vertexSrc);
  auto fragShaderCode = readFile(fragmentSrc);
}
void VulkanShader::Bind() const {}
void VulkanShader::UnBind() const {}

std::vector<char> VulkanShader::readFile(const std::string_view filename) {
  std::ifstream file(filename.data(), std::ios::ate | std::ios::binary);
  if (!file.is_open()) {
    throw std::runtime_error("failed to open file!");
  }
  size_t fileSize = (size_t)file.tellg();
  std::vector<char> buffer(fileSize);
  file.seekg(0);
  file.read(buffer.data(), fileSize);
  file.close();

  return buffer;
}
}  // namespace Engine