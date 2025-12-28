# YAGE
YAGE is yet another graphics engine written in C++ using OpenGL.

glfw dependencies for Fedora:
```
sudo dnf install wayland-devel wayland-protocols-devel
sudo dnf install libxkbcommon-devel libxkbcommon-x11-devel
```

CMAKE with clangd support:
```
mkdir build
cd build
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
cd ..
ln -s build/compile_commands.json .
```

Current workflow for creating new scenes:

1. Create a new folder in src/
2. Create the .cpp file inside this new folder
3. Create a folder called shaders/ inside your new folder, write your shaders in this folder. Currently only supporting vertex and fragment shaders.
4. The engine expects your .cpp file to implement these 4 functions: init(), display(), framebuffer_size_callback() and key_callback()
5. Open CMakeLists.txt in src and add this line to it ``add_scene("folder_name" "executable_name" "source_code.cpp")``
6. In the build folder run make
7. Your executable will be in build/src/ and your shaders will be in build/src/shaders/

