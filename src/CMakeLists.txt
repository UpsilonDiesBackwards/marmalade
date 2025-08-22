add_subdirectory(gui)

# Application
list(APPEND ENGINE_SRC
        application/application.cpp
        application/application.h
        application/autosave.cpp
        application/autosave.h
        application/logger.h
        application/time.cpp
        application/time.h
        application/guilogsink.cpp
        application/guilogsink.h
        application/config/recents.cpp
        application/config/recents.h
        application/config/plugins.cpp
        application/config/plugins.h
        application/util.cpp
        application/util.h

        application/config/config.h
        application/config/configutil.h
        application/config/configutil.cpp
        application/config/engineconfig.cpp
        application/config/engineconfig.h

        application/i18n.h

        application/integration.cpp
        application/integration.h

        application/plugins/pluginloader.cpp
        application/plugins/pluginloader.h
        application/plugins/interfaceimpl.cpp
        application/plugins/interfaceimpl.h
        application/plugins/projectbrowserimpl.cpp
        application/plugins/projectbrowserimpl.h
        application/plugins/versioncontrolimpl.cpp
        application/plugins/versioncontrolimpl.h

        graphics/viewport.cpp
        graphics/renderable.cpp
        graphics/shader.cpp
        graphics/texture.cpp
        graphics/framebuffer.cpp
        graphics/multisampledframebuffer.cpp
        graphics/material.cpp

        graphics/components/billboardtexture.cpp
        graphics/components/tileset.cpp

        io/audiomanager.cpp
        io/input.cpp
        io/inputmanager.cpp

        scene/entity.cpp
        scene/scene.cpp
        scene/scenemanager.cpp

        animation/sequence.cpp
        animation/animationdriver.cpp

        packages/package.cpp
        packages/package.h

        project/assetmetadata.h
        project/assetregistry.cpp
        project/assetregistry.h
        project/assetregistryconfig.h
        project/project.cpp
        project/project.h
        project/projectfile.cpp
        project/projectfile.h
        project/projectsettings.h
        project/projectmanager.h
        project/projectscenes.cpp
        project/projectscenes.h
        project/projectpackages.h

        physics/2d/physicsengine2d.cpp
        physics/2d/collisionevent2d.cpp
)

list(APPEND ENGINE_SRC # ECS
        ecs/componentmanager.cpp
        ecs/components/transform.cpp

        ecs/components/rendering/materialrenderer.cpp
        ecs/components/rendering/modelrenderer.cpp
        ecs/components/rendering/tilemap.cpp

        ecs/components/audio/audioplayer.cpp

        ecs/components/light2d/light2d.cpp

        ecs/components/animation/animator.cpp

        ecs/components/physics/2d/rigidbody2d.cpp
        ecs/components/physics/2d/boxcollider2d.cpp
        ecs/components/physics/2d/circlecollider2d.cpp
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
