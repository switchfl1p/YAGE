#!/usr/bin/env python3
# scene_gen.py
import os
import sys
import argparse
from pathlib import Path
from datetime import datetime

def load_template(template_path):
    """Load a template file and return its contents."""
    try:
        with open(template_path, 'r') as f:
            return f.read()
    except FileNotFoundError:
        print(f"Error: Template file not found: {template_path}")
        sys.exit(1)
    except Exception as e:
        print(f"Error reading template {template_path}: {e}")
        sys.exit(1)

def substitute_variables(template, variables):
    """Replace {variable_name} placeholders in template with actual values."""
    result = template
    for key, value in variables.items():
        result = result.replace(f"{{{key}}}", value)
    return result

def create_scene(scene_name, executable_name=None, templates_dir="templates"):
    if executable_name is None:
        executable_name = scene_name.lower().replace(' ', '_')
    
    # Sanitize the scene name for filenames
    scene_name_safe = scene_name.lower().replace(' ', '_')
    
    base_path = Path("src") / scene_name
    shaders_path = base_path / "shaders"
    templates_path = Path(templates_dir)
    
    # Create directories
    base_path.mkdir(parents=True, exist_ok=True)
    shaders_path.mkdir(parents=True, exist_ok=True)
    
    # Template variables
    variables = {
        'scene_name': scene_name,
        'scene_name_safe': scene_name_safe,
        'executable_name': executable_name,
        'date': datetime.now().strftime('%Y-%m-%d'),
        'year': datetime.now().strftime('%Y')
    }
    
    # Load and process templates
    cpp_template = load_template(templates_path / "scene.cpp.template")
    vertex_shader_template = load_template(templates_path / "shader.vert.template")
    fragment_shader_template = load_template(templates_path / "shader.frag.template")
    
    cpp_content = substitute_variables(cpp_template, variables)
    vertex_shader_content = substitute_variables(vertex_shader_template, variables)
    fragment_shader_content = substitute_variables(fragment_shader_template, variables)
    
    # Write the files
    cpp_file = base_path / f"{scene_name_safe}.cpp"
    cpp_file.write_text(cpp_content)
    
    (shaders_path / f"{scene_name_safe}.vert").write_text(vertex_shader_content)
    (shaders_path / f"{scene_name_safe}.frag").write_text(fragment_shader_content)
    
    # Update CMakeLists.txt
    cmake_path = Path("src") / "CMakeLists.txt"
    if cmake_path.exists():
        with open(cmake_path, 'a') as f:
            f.write(f'\nadd_scene("{scene_name}" "{executable_name}" "{scene_name_safe}.cpp")\n')
        print(f"✓ Added to {cmake_path}")
    
    print(f"✓ Scene '{scene_name}' created successfully!")
    print(f"  - Source: {cpp_file}")
    print(f"  - Shaders: {shaders_path}/")
    print(f"    * {scene_name_safe}.vert (vertex shader)")
    print(f"    * {scene_name_safe}.frag (fragment shader)")
    print(f"  - Executable: {executable_name}")

def main():
    parser = argparse.ArgumentParser(description='Create a new scene for the game engine')
    parser.add_argument('name', help='Name of the scene')
    parser.add_argument('-e', '--executable', help='Executable name (defaults to scene name)')
    parser.add_argument('-t', '--templates', default='templates', 
                        help='Path to templates directory (default: templates)')
    
    args = parser.parse_args()
    create_scene(args.name, args.executable, args.templates)

if __name__ == "__main__":
    main()