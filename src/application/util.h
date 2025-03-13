/*
 Marmalade - Lightweight Game Engine
 Copyright (C) 2025 Tayler Parsons
 Copyright (C) 2025 Ryan Bester

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef MARMALADE_UTIL_H
#define MARMALADE_UTIL_H

#include <glad/glad.h>

#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <sstream>

namespace Marmalade {
    class Util {
    public:
        static std::string GenerateUUIDv4();

        static std::string StringToLower(std::string str);

        static void DisplayFile(const std::string& path);

        static void OpenLink(const std::string& link);

        static GLuint LoadGuiTexture(std::string path, int* width = nullptr, int* height = nullptr);

        static void ConstrainImageSize(int maxWidth, int maxHeight, int& imgWidth, int& imgHeight);

        template<typename T>
        static inline bool Contains(const std::vector<T>& vec, const T& item) {
            return std::find(vec.begin(), vec.end(), item) != vec.end();
        }

        template<typename K, typename V>
        static inline std::vector<V*> GetValuePointers(std::unordered_map<K, V>& map) {
            std::vector<V*> values;
            values.reserve(map.size());

            for (auto& [key, value]: map) {
                values.push_back(&value);
            }

            return values;
        }

        template<typename T>
        static inline std::string JoinVector(const std::vector<T>& vec, std::string sep = ", ") {
            std::ostringstream oss;
            for (size_t i = 0; i < vec.size(); ++i) {
                oss << vec[i];
                if (i != vec.size() - 1) {
                    oss << sep;
                }
            }
            return oss.str();
        }
    };
}

#endif
