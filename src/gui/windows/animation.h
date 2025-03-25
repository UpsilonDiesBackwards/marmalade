
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

#ifndef MARMALADE_ANIMATION_H
#define MARMALADE_ANIMATION_H

#include "../window.h"
#include "animation/animationdriver.h"
#include "animation/sequence.h"

#include <imgui.h>

#include <memory>
#include <set>

namespace Marmalade::GUI {
    struct SequenceNode {
        int id;
        std::string name;
        ImVec2 position;
    };

    struct NodeTransition {
        int startNodeID;
        int targetNodeID;

        float time;
    };

    class Animation : public Window {
    public:
        std::unique_ptr<Marmalade::Animation::AnimationDriver> driver = nullptr;
        std::unique_ptr<Marmalade::Animation::AnimationSequence> animation = nullptr;

        std::vector<SequenceNode> nodes;
        std::vector<NodeTransition> transitions;

        void Draw() override;

        void DrawSequenceSelector();
        void DrawTopSection(float topHeight);
        void DrawNodeArea();
        void DrawPreviewWindow();
        void DrawInspector();

        void DrawTimeline();
        void DrawRuler(float length, float zoom);
        void DrawFrameMarkers(Marmalade::Animation::AnimationSequence* sequence, float zoom);
        void DrawScrubber(float& playbackTime, float length, float zoom);
        void CreatePlaybackControls(Marmalade::Animation::AnimationSequence& sequence, float deltaTime);

        bool DrawSplitter(const char* id, float* size, float minSize, float maxSize, bool isVertical);

    private:
        int _selectedFrameIndex = -1;
        float _timelineZoom = 1.0f;
        bool _isPlaying = false;
        float playbackTime = 0.0f;
        float scrubberPos = 0.0f;

        const float MIN_ZOOM_THRESHOLD = 2.0f;
        const float SCRUBBER_HEIGHT = 10.0f;

        std::set<const Marmalade::Animation::AnimationSequence*> createdNodes;
        int nextNodeId = 1;

        std::string RemoveFileExtention(std::string& fileName) {
            std::filesystem::path path(fileName);
            return path.stem().string();
        }
    };
}
#endif
