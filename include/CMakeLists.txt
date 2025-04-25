# Engine API
list(APPEND ENGINE_API
        graphics/viewport.h
        graphics/renderable.h
        graphics/shader.h
        graphics/texture.h
        graphics/framebuffer.h
        graphics/multisampledframebuffer.h
        graphics/lighting/light.h

        io/audiomanager.h
        io/input.h
        io/inputmanager.h

        scene/entity.h
        scene/scene.h
        scene/scenemanager.h

        ecs/componentmanager.h
        ecs/components/transform.h
        ecs/components/texturerenderer.h

        ecs/components/audio/audioplayer.h

        ecs/components/lighting2d/light2d.h

        ecs/components/physics2d/rigidbody.h
        ecs/components/physics2d/colliderbase.h
        ecs/components/physics2d/colliderdata.h
        ecs/components/physics2d/boxcollider.h
        ecs/components/physics2d/circlecollider.h

        ecs/components/animator.h

        animation/sequence.h
        animation/animationdriver.h
        animation/animationframe.h

        engineapi.h
        plugin.h

        imgui.inc
        gui.h
)

target_sources(marmalade PRIVATE ${ENGINE_API})
