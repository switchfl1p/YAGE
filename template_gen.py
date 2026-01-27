#!/usr/bin/env python3
"""
Script to process a C++ scene file and its associated resources into templates.

Usage: python template_gen.py <filename.cpp>
Example: python template_gen.py banana.cpp
"""

import sys
import os
import re
import shutil
from pathlib import Path


def parse_cpp_and_replace_shaders(cpp_content):
    """
    Parse C++ content and replace shader initializations with template variables.
    
    Returns:
        tuple: (modified_content, shader_mapping)
        shader_mapping is a dict like {0: 'on_frag_light.vert', 1: 'on_frag_light.frag', ...}
    """
    shader_mapping = {}
    shader_counter = 0
    
    # Pattern to match shader initialization lines
    # Matches: Shader variable_name("filename.ext");
    pattern = r'(Shader\s+\w+\s*\(\s*")([^"]+)("\s*\)\s*;)'
    
    def replace_shader(match):
        nonlocal shader_counter
        prefix = match.group(1)
        shader_filename = match.group(2)
        suffix = match.group(3)
        
        # Store the original shader filename
        shader_mapping[shader_counter] = shader_filename
        
        # Create template variable
        template_var = f"{{shader_{shader_counter}}}"
        shader_counter += 1
        
        return f"{prefix}{template_var}{suffix}"
    
    modified_content = re.sub(pattern, replace_shader, cpp_content)
    
    return modified_content, shader_mapping


def process_cpp_file(src_folder, scene_name, templates_folder):
    """
    Process the C++ file and create a template version.
    
    Args:
        src_folder: Path to the source folder (e.g., /src/banana)
        scene_name: Name without extension (e.g., 'banana')
        templates_folder: Path to templates output folder
    """
    cpp_file = src_folder / f"{scene_name}.cpp"
    
    if not cpp_file.exists():
        print(f"Error: {cpp_file} not found!")
        return None
    
    # Read the C++ file
    with open(cpp_file, 'r', encoding='utf-8') as f:
        cpp_content = f.read()
    
    # Process shader replacements
    modified_content, shader_mapping = parse_cpp_and_replace_shaders(cpp_content)
    
    # Write the template file
    template_file = templates_folder / f"{scene_name}.cpp.template"
    with open(template_file, 'w', encoding='utf-8') as f:
        f.write(modified_content)
    
    print(f"✓ Created {template_file}")
    print(f"  Found {len(shader_mapping)} shader(s):")
    for idx, filename in shader_mapping.items():
        print(f"    shader_{idx} -> {filename}")
    
    return shader_mapping


def process_shaders(src_folder, templates_folder):
    """
    Process shader files from the shaders folder.
    
    Args:
        src_folder: Path to the source folder (e.g., /src/banana)
        templates_folder: Path to templates output folder
    """
    shaders_folder = src_folder / "shaders"
    
    if not shaders_folder.exists():
        print(f"Warning: {shaders_folder} not found, skipping shaders")
        return
    
    # Get all files in the shaders folder
    shader_files = [f for f in shaders_folder.iterdir() if f.is_file()]
    
    if not shader_files:
        print(f"Warning: No shader files found in {shaders_folder}")
        return
    
    print(f"\n✓ Processing {len(shader_files)} shader file(s):")
    
    for shader_file in shader_files:
        # Copy to templates folder with .template extension
        template_name = f"{shader_file.name}.template"
        template_path = templates_folder / template_name
        
        shutil.copy2(shader_file, template_path)
        print(f"  {shader_file.name} -> {template_name}")


def process_meshes(src_folder, templates_folder):
    """
    Process mesh files from the meshes folder.
    
    Args:
        src_folder: Path to the source folder (e.g., /src/banana)
        templates_folder: Path to templates output folder
    """
    meshes_folder = src_folder / "meshes"
    
    if not meshes_folder.exists():
        print(f"Warning: {meshes_folder} not found, skipping meshes")
        return
    
    # Get all files in the meshes folder
    mesh_files = [f for f in meshes_folder.iterdir() if f.is_file()]
    
    if not mesh_files:
        print(f"Warning: No mesh files found in {meshes_folder}")
        return
    
    print(f"\n✓ Processing {len(mesh_files)} mesh file(s):")
    
    for mesh_file in mesh_files:
        # Copy to templates folder with .template extension
        template_name = f"{mesh_file.name}.template"
        template_path = templates_folder / template_name
        
        shutil.copy2(mesh_file, template_path)
        print(f"  {mesh_file.name} -> {template_name}")


def main():
    if len(sys.argv) != 2:
        print("Usage: python template_gen.py <filename.cpp>")
        print("Example: python template_gen.py banana.cpp")
        sys.exit(1)
    
    # Get the filename argument
    cpp_filename = sys.argv[1]
    
    # Validate it ends with .cpp
    if not cpp_filename.endswith('.cpp'):
        print(f"Error: Filename must end with .cpp, got: {cpp_filename}")
        sys.exit(1)
    
    # Extract scene name (without .cpp extension)
    scene_name = cpp_filename[:-4]
    
    # Define paths (relative to current directory)
    src_base = Path("src")
    src_folder = src_base / scene_name
    templates_folder = Path("templates")
    
    # Check if source folder exists
    if not src_folder.exists():
        print(f"Error: Source folder {src_folder} not found!")
        sys.exit(1)
    
    # Create templates folder if it doesn't exist
    templates_folder.mkdir(exist_ok=True)
    print(f"Using templates folder: {templates_folder.absolute()}\n")
    
    # Process the C++ file
    print(f"Processing {cpp_filename}...")
    shader_mapping = process_cpp_file(src_folder, scene_name, templates_folder)
    
    if shader_mapping is None:
        sys.exit(1)
    
    # Process shader files
    process_shaders(src_folder, templates_folder)
    
    # Process mesh files
    process_meshes(src_folder, templates_folder)
    
    print(f"\n✓ All done! Templates created in {templates_folder.absolute()}")


if __name__ == "__main__":
    main()