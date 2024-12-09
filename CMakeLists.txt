cmake_minimum_required(VERSION 3.15)

project(DataDrivenRendering)

set(Application_Name "DataDrivenRendering")

set(DCMAKE_SYSTEM_VERSION 10.0)
option(USE_LIBRARY "Use Library" ON)

include(CMakeDependentOption)

cmake_dependent_option(
  USE_STATIC_LIBRARY "Use Static Library" ON
  USE_LIBRARY ON
)
cmake_dependent_option(
  USE_SHARED_LIBRARY "Use Shared Library" OFF
  USE_LIBRARY ON
)

set(EnginePath ${PROJECT_SOURCE_DIR}/Source/Engine)
set(EditorPath ${PROJECT_SOURCE_DIR}/Source/Editor)
set(LibraryPath ${PROJECT_SOURCE_DIR}/Source/ThirdParty)
set(ResourcePath ${PROJECT_SOURCE_DIR}/Resource)
if(USE_LIBRARY)
add_subdirectory(${LibraryPath}/Message)
add_subdirectory(${LibraryPath}/glfw)
add_subdirectory(${LibraryPath}/glad)
add_subdirectory(${LibraryPath}/imgui)
add_subdirectory(${LibraryPath}/assimp)

file(GLOB_RECURSE _sources 
  ${EnginePath}/*.cpp 
  ${EnginePath}/*.h 
  ${EditorPath}/*.cpp 
  ${EditorPath}/*.h 
  ${ResourcePath}/*.glsl
  ${LibraryPath}/glm-0.9.9.8/glm/*.hpp)

  add_library(${Application_Name} STATIC "")

target_sources(${Application_Name} PRIVATE
  ${_sources}
)
target_link_libraries(${Application_Name} PRIVATE
  Message
)
target_link_libraries(${Application_Name} PRIVATE
  glfw
)
target_link_libraries(${Application_Name} PRIVATE
  glad
)
target_link_libraries(${Application_Name} PUBLIC
  imgui
)
target_link_libraries(${Application_Name} PUBLIC
  assimp
)
  target_link_libraries(${Application_Name} PUBLIC
    ${PROJECT_SOURCE_DIR}/Libs/freetype.lib
  )
target_include_directories(${Application_Name}  PUBLIC
  ${LibraryPath}/glad/include
  ${LibraryPath}/glfw/include
  ${LibraryPath}/Freetype/include
  ${LibraryPath}/Message
  ${LibraryPath}/glm-0.9.9.8/glm
  ${LibraryPath}/imgui
  ${LibraryPath}/assimp
  ${EditorPath}
  ${EnginePath}
  ${EnginePath}/Core
  ${EnginePath}/Platform
  ${EnginePath}/ThirdParty
  ${EnginePath}/Resource
)

target_compile_definitions(${Application_Name} PRIVATE
  GLFW_INCLUDE_NONE 
  RESOURCE_PATH ${ResourcePath}
)
else()

endif()


add_executable(Example1 "Example1.cpp")
target_link_libraries(Example1 PRIVATE
  ${Application_Name}
)