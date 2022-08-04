#include <string>
namespace Engine {
class Shader {
 public:
  virtual ~Shader() = 0;
  virtual void LoadShader(std::string_view vertexSrc,
                          std::string_view fragmentSrc) = 0;
  virtual void Bind() const = 0;
  virtual void UnBind() const = 0;

 private:
};
}  // namespace Engine