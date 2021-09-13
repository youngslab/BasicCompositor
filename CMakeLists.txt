
cmake_minimum_required(VERSION 3.16)

project(drm_resources)

find_package(PkgConfig REQUIRED)
pkg_check_modules(DRM REQUIRED libdrm)

find_package(fmt REQUIRED)
find_package(range-v3 REQUIRED)

add_subdirectory(./thirdparty)

add_executable(${CMAKE_PROJECT_NAME}
	./src/main.cpp
	./backend/backend.cpp
	./backend/offscreen.cpp
	./engine/mesh.cpp
	./engine/entity.cpp
	./engine/material.cpp
	./engine/attribute.cpp
	./engine/operations.cpp
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17 )

target_link_libraries(${PROJECT_NAME} object-drm object-gl raii
	${DRM_LIBRARIES} fmt::fmt EGL GLESv2 -O3
	)

target_include_directories(${PROJECT_NAME} PRIVATE ${DRM_INCLUDE_DIRS}
	PUBLIC ./include ./backend)

pkg_check_modules(DRM libdrm)
if(${DRM_FOUND})
	target_sources(${PROJECT_NAME} PUBLIC ./backend/drm.cpp)
	target_link_libraries(${PROJECT_NAME} ${DRM_LIBRARIES})
	target_include_directories(${PROJECT_NAME} PUBLIC ${DRM_INCLUDE_DIRS})

pkg_check_modules(GBM gbm)
if(${GBM_FOUND})
	target_sources(${PROJECT_NAME} PUBLIC ./backend/gbm.cpp)
	target_link_libraries(${PROJECT_NAME} ${GBM_LIBRARIES})
	target_include_directories(${PROJECT_NAME} PUBLIC ${GBM_INCLUDE_DIRS})
	target_compile_definitions(${PROJECT_NAME} PRIVATE BACKEND_GBM=1)
endif()
endif()


# UBM setup
pkg_check_modules(UBM libubm)
if(${UBM_FOUND})
	#target_sources(${PROJECT_NAME})
	target_link_libraries(${PROJECT_NAME} ${UBM_LIBRARIES})
	target_include_directories(${PROJECT_NAME} PUBLIC ${UBM_INCLUDE_DIRS})
endif()

# x11
find_package(X11)
if(${X11_FOUND})
target_link_libraries(${PROJECT_NAME} ${X11_LIBRARIES} )
target_include_directories(${PROJECT_NAME} PRIVATE ${X11_INCLUDE_DIR})
target_compile_definitions(${PROJECT_NAME} PRIVATE BACKEND_X11=1)
target_sources(${PROJECT_NAME} PRIVATE ./backend/x11.cpp)

endif()

# Waland steup
find_package(ECM 5.68 NO_MODULE)
if(${ECM_FOUND})
list(APPEND CMAKE_MODULE_PATH "${ECM_MODULE_PATH}")

find_package(Wayland 1.18)
find_package(WaylandScanner 1.18)
find_package(WaylandProtocols 1.20)

if(${Wayland_FOUND})
ecm_add_wayland_client_protocol(GLFW_WAYLAND_PROTOCOL_SOURCES
  PROTOCOL
  "${WaylandProtocols_DATADIR}/stable/xdg-shell/xdg-shell.xml"
  BASENAME xdg-shell)

target_sources(${PROJECT_NAME} PRIVATE ./backend/wayland.cpp ${GLFW_WAYLAND_PROTOCOL_SOURCES})
target_compile_definitions(${PROJECT_NAME} PRIVATE BACKEND_WAYLAND=1)
target_link_libraries(${PROJECT_NAME} Wayland::Client Wayland::Egl
	Wayland::Cursor)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

endif()
endif()
