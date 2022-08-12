#include <Shader.hpp>
#include <vector>
#include <fstream>
#include <vulkan/vulkan.h>
namespace Engine {
class VulkanShader : public Shader {
 public:
  virtual ~VulkanShader();
  virtual void LoadShader(std::string_view vertexSrc,
                          std::string_view fragmentSrc) override;
  virtual void Bind() const override;
  virtual void UnBind() const override;

  std::vector<char> readFile(const std::string_view filename);
};
}  // namespace Engine