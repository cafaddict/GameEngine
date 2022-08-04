namespace Engine {
class VertexBuffer {
 public:
  virtual ~VertexBuffer() {}
  virtual void Bind() const = 0;
  // virtual void
};

}  // namespace Engine