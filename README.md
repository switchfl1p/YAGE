# YAGE
YAGE is (Y)et (A)nother (G)raphics (E)ngine written in C++ using OpenGL.



https://github.com/user-attachments/assets/039cf1e9-18d2-428f-83dd-266c9354c7b2



glfw dependencies for Fedora:
```bash
sudo dnf install wayland-devel wayland-protocols-devel
sudo dnf install libxkbcommon-devel libxkbcommon-x11-devel
```

CMAKE with clangd support:
```bash
mkdir build
cd build
cmake ..
cd ..
ln -s build/compile_commands.json .
```
## Architecture Diagram, Feb 11 2026
![Architecture Diagram, Feb 11 2026](diagram/engine.png)



