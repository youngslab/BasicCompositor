
cmake_minimum_required(VERSION 3.10)

project(drm_resources)

find_package(PkgConfig REQUIRED)
pkg_check_modules(DRM REQUIRED libdrm)

find_package(fmt REQUIRED)
find_package(range-v3 REQUIRED)

add_subdirectory(./thirdpary)

add_executable(${CMAKE_PROJECT_NAME} ./src/main.cpp ./src/backend_drm.cpp
	./src/backend_gbm.cpp)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17 )

target_link_libraries(${PROJECT_NAME} ${DRM_LIBRARIES} fmt::fmt
	range-v3::range-v3 EGL GLESv2 gbm raii object-drm
	)

target_include_directories(${PROJECT_NAME} PRIVATE ${DRM_INCLUDE_DIRS}
	PUBLIC ./include)


pkg_check_modules(UBM libubm)
if(${UBM_FOUND})
	#target_sources(${PROJECT_NAME})
	target_link_libraries(${PROJECT_NAME} ${UBM_LIBRARIES})
	target_include_directories(${PROJECT_NAME} PUBLIC ${UBM_INCLUDE_DIRS})
endif()

