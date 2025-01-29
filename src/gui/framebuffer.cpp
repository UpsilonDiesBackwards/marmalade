
#include <iostream>
#include "../gui/framebuffer.h"
#include "glad/glad.h"

/*
 * This handles both regular and Multisampled Framebuffers.
 *
 * Some of this is a bit messy cause in order to get the multisampled framebuffers rendered in an ImGui window we need
 * to resolve a multisampled framebuffer back into a regular framebuffer, and then blit it so it can be rendered.
 */

Framebuffer::Framebuffer() {
    Initialize();
}

Framebuffer::~Framebuffer() {
    glDeleteFramebuffers(1, &fbo);
    glDeleteFramebuffers(1, &resolvedFBO);
    glDeleteTextures(1, &texture);
    glDeleteTextures(1, &resolvedTexture);
    glDeleteRenderbuffers(1, &rbo);
}


void Framebuffer::Initialize() {
    glGenFramebuffers(1, &fbo); // Create frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenTextures(1, &texture); // Create texture buffer
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture);

    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, sampleCount, GL_RGB, width, height, GL_TRUE);
    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

    // Attach it to currently bound fbo
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, texture, 0);

    glGenRenderbuffers(1, &rbo); // Create render buffer
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);

    glRenderbufferStorageMultisample(GL_RENDERBUFFER, sampleCount, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER, rbo);

    glGenFramebuffers(1, &resolvedFBO); // Create resolved framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, resolvedFBO);

    glGenTextures(1, &resolvedTexture); // Create resolved texture
    glBindTexture(GL_TEXTURE_2D, resolvedTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, resolvedTexture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR: Framebuffer is not complete!" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::Bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

void Framebuffer::Unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::Resolve() {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, resolvedFBO);

    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::Resize(int newW, int newH) {
    width = newW;
    height = newH;

    glBindTexture(GL_TEXTURE_2D, resolvedTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);


    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, sampleCount, GL_DEPTH24_STENCIL8, width, height);
}

unsigned int Framebuffer::GetTexture() const {
    return texture;
}

unsigned int Framebuffer::GetFBO() const {
    return fbo;
}

unsigned int Framebuffer::GetResolvedTexture() const {
    return resolvedTexture;
}

unsigned int Framebuffer::GetResolvedFBO() const {
    return resolvedFBO;
}
