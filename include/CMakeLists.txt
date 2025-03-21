# Engine API
list(APPEND ENGINE_API
        graphics/viewport.h
        graphics/renderable.h
        graphics/shader.h
        graphics/texture.h
        graphics/framebuffer.h
        graphics/multisampledframebuffer.h
        graphics/lighting/light.h

        io/input.h
        io/inputmanager.h

        scene/entity.h
        scene/scene.h
        scene/scenemanager.h

        ecs/componentmanager.h
        ecs/components/transform.h
        ecs/components/texturerenderer.h

        ecs/components/physics2d/rigidbody.h
        ecs/components/physics2d/colliderbase.h
        ecs/components/physics2d/colliderdata.h
        ecs/components/physics2d/boxcollider.h
        ecs/components/physics2d/circlecollider.h

        ecs/components/animationplayer.h

        animation/animation.h

        engineapi.h
        plugin.h
)

target_sources(marmalade PRIVATE ${ENGINE_API})
