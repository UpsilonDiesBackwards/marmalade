add_subdirectory(gui)

# Application
list(APPEND ENGINE_SRC
        application/application.cpp
        application/application.h
        application/logger.h
        application/profiler.cpp
        application/profiler.h
        application/guilogsink.cpp
        application/guilogsink.h
        application/config/recents.cpp
        application/config/recents.h
        application/config/plugins.cpp
        application/config/plugins.h
        application/util.cpp
        application/util.h
        application/pluginloader.cpp
        application/pluginloader.h
        application/config/config.h
        application/config/configutil.h
        application/config/configutil.cpp
        application/config/engineconfig.cpp
        application/config/engineconfig.h

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

        packages/package.cpp
        packages/package.h

        project/project.cpp
        project/project.h
        project/projectfile.cpp
        project/projectfile.h
        project/projectsettings.h
        project/projectmanager.h
        project/projectscenes.cpp
        project/projectscenes.h
        project/projectpackages.h
)

list(APPEND ENGINE_SRC # ECS
        ecs/componentmanager.cpp
        ecs/components/transform.cpp
        ecs/components/texturerenderer.cpp

        ecs/components/physics2d/rigidbody.cpp
        ecs/components/physics2d/colliderdata.cpp
        ecs/components/physics2d/boxcollider.cpp
        ecs/components/physics2d/circlecollider.cpp
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
