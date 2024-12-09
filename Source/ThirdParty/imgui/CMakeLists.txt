project(imgui)

add_library(imgui STATIC 
    " "
)
target_sources(imgui PRIVATE
 "imconfig.h"
  "imgui.cpp"
  "imgui.h"
  "imgui_demo.cpp"
  "imgui_draw.cpp"
  "imgui_internal.h"
  "imgui_tables.cpp"
  "imgui_widgets.cpp"
  "imstb_rectpack.h"
  "imstb_textedit.h"
  "imstb_truetype.h"
  "backends/imgui_impl_opengl3.cpp"
  "backends/imgui_impl_opengl3.h"
  "backends/imgui_impl_opengl3_loader.h"
  "backends/imgui_impl_win32.cpp"
  "backends/imgui_impl_win32.h"
  "backends/imgui_impl_glfw.h"
  "backends/imgui_impl_glfw.cpp"
)
target_include_directories(imgui  PUBLIC
  ""
  "../glfw/include"
)
