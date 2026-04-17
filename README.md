# YAGE
YAGE is (Y)et (A)nother (G)raphics (E)ngine written in C++ using OpenGL.

Now supporting day/night cycles for PBR/Blinn-Phong/Phong/Gaussian/Lambertian Lighting models, toggle demonstration at the end:

https://github.com/user-attachments/assets/a8ecc997-92cb-44cb-b4f9-3bcdcc7b7359


Old demonstration, before tone mapping and gamma correction was applied to the classic style lighting shaders:

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



