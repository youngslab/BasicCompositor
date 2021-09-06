
cmake_minimum_required(VERSION 3.16)

project(drm_resources)

find_package(PkgConfig REQUIRED)
pkg_check_modules(DRM REQUIRED libdrm)

find_package(fmt REQUIRED)
find_package(range-v3 REQUIRED)

add_subdirectory(./thirdpary)
add_subdirectory(./thirdparty)

add_executable(${CMAKE_PROJECT_NAME} ./src/glrenderer.cpp ./src/main.cpp ./src/backend_drm.cpp
	./src/backend_gbm.cpp )

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17 )

target_link_libraries(${PROJECT_NAME} object-drm object-gl raii
	${DRM_LIBRARIES} gbm fmt::fmt EGL GLESv2 -O3
	)

target_include_directories(${PROJECT_NAME} PRIVATE ${DRM_INCLUDE_DIRS}
	PUBLIC ./include)


pkg_check_modules(UBM libubm)
if(${UBM_FOUND})
	#target_sources(${PROJECT_NAME})
	target_link_libraries(${PROJECT_NAME} ${UBM_LIBRARIES})
	target_include_directories(${PROJECT_NAME} PUBLIC ${UBM_INCLUDE_DIRS})
endif()


find_package(ECM 5.85 NO_MODULE)

if(${ECM_FOUND})
list(APPEND CMAKE_MODULE_PATH "${ECM_MODULE_PATH}")

find_package(Wayland )
find_package(WaylandScanner )
find_package(WaylandProtocols 1.20)

if(${Wayland_FOUND})
ecm_add_wayland_client_protocol(GLFW_WAYLAND_PROTOCOL_SOURCES
  PROTOCOL
  "${WaylandProtocols_DATADIR}/stable/xdg-shell/xdg-shell.xml"
  BASENAME xdg-shell)

target_sources(${PROJECT_NAME} PRIVATE ./src/backend_wayland.cpp ${GLFW_WAYLAND_PROTOCOL_SOURCES})
target_link_libraries(${PROJECT_NAME} Wayland::Client Wayland::Egl
	Wayland::Cursor)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

endif()
endif()
