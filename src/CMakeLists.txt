add_subdirectory(gui)

# Application
list(APPEND ENGINE_SRC
        application/application.cpp
        application/application.h
        application/profiler.cpp
        application/profiler.h
        application/guilogsink.cpp
        application/guilogsink.h
        application/config.cpp
        application/config.h
        application/util.cpp
        application/util.h

        graphics/viewport.cpp
        graphics/renderable.cpp
        graphics/shader.cpp
        graphics/texture.cpp
        graphics/framebuffer.cpp
        graphics/lighting/light.cpp
        graphics/multisampledframebuffer.cpp

        io/input.cpp
        io/inputmanager.cpp

        scene/entity.cpp
        scene/scene.cpp
        scene/scenemanager.cpp

        project/project.cpp
        project/project.h
        project/projectsettings.h
        project/projectmanager.h
        project/projectscenes.cpp
        project/projectscenes.h
)

list(APPEND ENGINE_SRC # ECS
        ecs/componentmanager.cpp
        ecs/components/transform.cpp
        ecs/components/spriterender.cpp
        ecs/components/physics/boxcollider.cpp
        ecs/components/physics/rigidbody.cpp
)

list(APPEND ENGINE_SRC # Mathematics
        ../include/mathematics/mathematics.h

        mathematics/types/vector2.cpp
        ../include/mathematics/types/vector2.h
        mathematics/types/vector3.cpp
        ../include/mathematics/types/vector3.h
        mathematics/types/vector4.cpp
        ../include/mathematics/types/vector4.h
        mathematics/types/matrix2x2.cpp
        ../include/mathematics/types/matrix2x2.h

        mathematics/functions/sqrt.cpp
        ../include/mathematics/functions/sqrt.h
)

target_sources(marmalade PRIVATE ${ENGINE_SRC} main.cpp)
