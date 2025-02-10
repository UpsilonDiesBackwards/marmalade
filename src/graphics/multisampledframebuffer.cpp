/*
 * Marmalade - Lightweight Game Engine
 * Copyright (C) 2025 Tayler Parsons
 * Copyright (C) 2025 Ryan Bester
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <graphics/multisampledframebuffer.h>

#include <glad/glad.h>

#include <iostream>

/*
 * This handles both regular and Multisampled Framebuffers.
 *
 * Some of this is a bit messy cause in order to get the multisampled framebuffers rendered in an ImGui window we need
 * to resolve a multisampled framebuffer back into a regular framebuffer, and then blit it so it can be rendered.
 */

MultiSampledFramebuffer::MultiSampledFramebuffer() {
    Initialize();
}

MultiSampledFramebuffer::~MultiSampledFramebuffer() {
    glDeleteFramebuffers(1, &fbo);
    glDeleteFramebuffers(1, &resolvedFBO);
    glDeleteTextures(1, &texture);
    glDeleteTextures(1, &resolvedTexture);
    glDeleteRenderbuffers(1, &rbo);
}


void MultiSampledFramebuffer::Initialize() {
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
        std::cout << "ERROR: MultiSampledFramebuffer is not complete!" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void MultiSampledFramebuffer::Bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

void MultiSampledFramebuffer::Unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void MultiSampledFramebuffer::Resolve() {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, resolvedFBO);

    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void MultiSampledFramebuffer::Resize(int newW, int newH) {
    width = newW;
    height = newH;

    glBindTexture(GL_TEXTURE_2D, resolvedTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, sampleCount, GL_DEPTH24_STENCIL8, width, height);
}

unsigned int MultiSampledFramebuffer::GetTexture() const {
    return resolvedTexture;
}

unsigned int MultiSampledFramebuffer::GetFBO() const {
    return resolvedFBO;
}
