
#ifndef ENGINE_MULTISAMPLEDFRAMEBUFFER_H
#define ENGINE_MULTISAMPLEDFRAMEBUFFER_H

class MultiSampledFramebuffer {
public:
    MultiSampledFramebuffer();
    ~MultiSampledFramebuffer();

    void Bind();
    void Unbind();
    void Resize(int width, int height);

    void Resolve();

    unsigned int GetTexture() const;
    unsigned int GetFBO() const;

    int width = 1920;
    int height = 1080;

    int sampleCount = 16; // MSAA sample count. not a const because we want this to be editable by user
private:
    unsigned int fbo, resolvedFBO;
    unsigned int texture, resolvedTexture;
    unsigned int rbo;

    void Initialize();
};

#endif //ENGINE_MULTISAMPLEDFRAMEBUFFER_H
