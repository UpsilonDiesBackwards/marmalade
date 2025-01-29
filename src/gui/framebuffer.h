
#ifndef ENGINE_FRAMEBUFFER_H
#define ENGINE_FRAMEBUFFER_H


class Framebuffer {
public:
    Framebuffer();
    ~Framebuffer();

    void Bind();
    void Unbind();
    void Resize(int width, int height);

    void Resolve();

    unsigned int GetTexture() const;
    unsigned int GetResolvedTexture() const;
    unsigned int GetFBO() const;
    unsigned int GetResolvedFBO() const;

    int width = 1920;
    int height = 1080;

    int sampleCount = 16; // MSAA sample count. not a const cause we want this to be editable by user
private:
    unsigned int fbo, resolvedFBO;
    unsigned int texture, resolvedTexture;
    unsigned int rbo;

    void Initialize();
};


#endif //ENGINE_FRAMEBUFFER_H
