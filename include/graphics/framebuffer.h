
#ifndef MARMALADE_FRAMEBUFFER_H
#define MARMALADE_FRAMEBUFFER_H


class Framebuffer {
public:
    Framebuffer();
    ~Framebuffer();

    void Bind();
    void Unbind();
    void Resize(int width, int height);

    unsigned int GetTexture() const;
    unsigned int GetFBO() const;

    int width = 1920;
    int height = 1080;
private:
    unsigned int fbo;
    unsigned int texture;
    unsigned int rbo;

    void Initialise();
};


#endif //MARMALADE_FRAMEBUFFER_H
