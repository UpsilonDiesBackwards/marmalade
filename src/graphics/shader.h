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

#ifndef ENGINE_SHADER_H
#define ENGINE_SHADER_H

#include <glm/glm.hpp>

#include <string>
/**
 * \brief Creates and manages an OpenGL shader program
 */

class Shader {
public:
    unsigned int ID;

    Shader(const char* vPath, const char* fPath);

    void Use();

    /**
     * \brief Sets a boolean parameter within a shader program
     * \param name Name of the parameter to update
     * \param value Value to change the parameter to
     */
    void SetBool(const std::string &name, bool value) const;

    /**
     * \brief Set an integer parameter within a shader program
     * \param name Name of the parameter to update
     * \param value Value to change the parameter to
     */
    void SetInt(const std::string &name, int value) const;

    /**
     * \brief Set an float parameter within a shader program
     * \param name Name of the parameter to update
     * \param value Value to change the parameter to
     */
    void SetFloat(const std::string &name, float value) const;

    /**
     * \brief Set an Vector2 parameter within a shader program
     * \param name Name of the parameter to update
     * \param value Value to change the parameter to
     */
    void SetVec2(const std::string& name, const glm::vec2& value) const;

    /**
     * \brief Set an Vector3 parameter within a shader program
     * \param name Name of the parameter to update
     * \param value Value to change the parameter to
     */
    void SetVec3(const std::string& name, glm::vec3 value) const;

    /**
     * \brief Set an Matrix4 parameter within a shader program
     * \param name Name of the parameter to update
     * \param value Value to change the parameter to
     */
    void SetMat4(const std::string &name, const glm::mat4 &mat) const;

private:
    /**
     * \brief Check for any OpenGL errors and return any
     * \param shader Shader program ID
     * \param type Type of shader object to check errors for
     */
    void GetError(unsigned int shader, std::string type);
};

#endif
