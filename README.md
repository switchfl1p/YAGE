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

There is a helpful python script for generating the folder structure and boilerplate code required for a new scene. Run in project root:

```
# Basic usage, executable has same name as source file
./scene_gen.py scene_name

# With custom executable name
./scene_gen.py scene_name -e executable_name
```



