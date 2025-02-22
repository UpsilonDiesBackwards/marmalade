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
        ecs/components/spriterender.h
        ecs/components/physics/rigidbody.h
        ecs/components/physics/boxcollider.h
        ecs/components/physics/colliderbase.h
)

target_sources(marmalade PRIVATE ${ENGINE_API})
