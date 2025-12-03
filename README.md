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
