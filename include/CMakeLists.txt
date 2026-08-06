# Engine API
list(APPEND ENGINE_API
        graphics/framebuffer.h
        graphics/material.h
        graphics/multisampledframebuffer.h
        graphics/renderable.h
        graphics/shader.h
        graphics/texture.h
        graphics/viewport.h

        graphics/components/billboardtexture.h
        graphics/components/tileset.h

        ecs/componentmanager.h
        ecs/components/transform.h

        ecs/components/rendering/materialrenderer.h
        ecs/components/rendering/modelrenderer.h
        ecs/components/rendering/tiledata.h
        ecs/components/rendering/tilemap.h

        ecs/components/audio/audioplayer.h

        ecs/components/light2d/light2d.h

        ecs/components/animation/animator.h

        ecs/components/physics/rigidbody.h
        ecs/components/physics/2d/rigidbody2d.h
        ecs/components/physics/2d/boxcollider2d.h
        ecs/components/physics/2d/circlecollider2d.h

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
