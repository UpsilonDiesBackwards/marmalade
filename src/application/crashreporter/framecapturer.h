// Marmalade - Lightweight Game Engine
// Copyright (C) 2026 Tayler Parsons
// Copyright (C) 2026 Ryan Bester
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef MARMALADE_FRAMECAPTURER_H
#define MARMALADE_FRAMECAPTURER_H

#include <glad/glad.h>

#include <stb_image_write.h>

#include <vector>

namespace Marmalade {
    class FrameCapturer {
    public:
        FrameCapturer() = default;

        void Initialize() {
            glGenBuffers(2, pbos);
        }

        void Resize(int width, int height) {
            fbWidth = width;
            fbHeight = height;

            buf.resize(width * height * 4);

            for (unsigned int pbo: pbos) {
                glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo);
                glBufferData(GL_PIXEL_PACK_BUFFER, width * height * 4, nullptr, GL_STREAM_READ);
            }
            glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
        }

        void CaptureFrame() {
            glBindBuffer(GL_PIXEL_PACK_BUFFER, pbos[write_idx]);

            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glReadPixels(0, 0, fbWidth, fbHeight, GL_RGBA, GL_UNSIGNED_BYTE, 0);

            glBindBuffer(GL_PIXEL_PACK_BUFFER, pbos[read_idx]);

            glGetBufferSubData(GL_PIXEL_PACK_BUFFER, 0, buf.size(), buf.data());

            std::swap(write_idx, read_idx);

            glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
        }

        void SaveScreenshot() const {
            const auto now = std::chrono::system_clock::now();
            const auto t = std::chrono::system_clock::to_time_t(now);
            const auto tm = *std::localtime(&t);

            std::ostringstream oss;
            oss << "screenshot_" << std::put_time(&tm, "%Y-%m-%d_%H-%M-%S") << ".png";

            stbi_flip_vertically_on_write(true);
            stbi_write_png(oss.str().c_str(), fbWidth, fbHeight, 4, buf.data(), fbWidth * 4);
        }

        [[nodiscard]] const uint8_t* GetBuffer() const {
            return buf.data();
        }

    private:
        GLuint pbos[2];
        std::vector<uint8_t> buf;

        int fbWidth = 0, fbHeight = 0;

        int write_idx = 0;
        int read_idx = 1;
    };
}

#endif
