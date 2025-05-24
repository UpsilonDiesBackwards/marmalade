# Engine API
list(APPEND ENGINE_API
        graphics/framebuffer.h
        graphics/material.h
        graphics/multisampledframebuffer.h
        graphics/renderable.h
        graphics/shader.h
        graphics/texture.h
        graphics/viewport.h

        io/audiomanager.h
        io/input.h
        io/inputmanager.h

        scene/entity.h
        scene/scene.h
        scene/scenemanager.h

        ecs/componentmanager.h
        ecs/components/transform.h

        ecs/components/rendering/materialrenderer.h
        ecs/components/rendering/tilemap.h

        ecs/components/audio/audioplayer.h

        ecs/components/light2d/light2d.h

        ecs/components/physics2d/rigidbody.h
        ecs/components/physics2d/colliderbase.h
        ecs/components/physics2d/colliderdata.h
        ecs/components/physics2d/boxcollider.h
        ecs/components/physics2d/circlecollider.h

        ecs/components/animation/animator.h

        animation/sequence.h
        animation/animationdriver.h
        animation/animationframe.h

        engineapi.h
        plugin.h
        projectbrowser.h
        versioncontrol.h

        imgui.inc
        interface.h
        wasm.h
)

target_sources(marmalade PRIVATE ${ENGINE_API})
