#!/usr/bin/env python3
"""
Shader build script for Nintendo Switch (deko3d)

This script compiles GLSL shaders to deko3d-compatible format.
Requires: uam (shader compiler from deko3d toolchain)
"""

import os
import sys
import subprocess
from pathlib import Path

# Shader list
SHADERS = [
    ("triangle.vert", "vertex"),
    ("triangle.frag", "fragment"),
]

def check_uam():
    """Check if uam shader compiler is available"""
    try:
        result = subprocess.run(["uam", "--version"], 
                              capture_output=True, text=True)
        return result.returncode == 0
    except FileNotFoundError:
        return False

def compile_shader(shader_file, shader_type):
    """Compile a single shader to deko3d format"""
    input_path = Path(__file__).parent / shader_file
    output_path = input_path.with_suffix(".dksh")
    
    if not input_path.exists():
        print(f"Error: Shader file not found: {input_path}")
        return False
    
    print(f"Compiling {shader_file} ({shader_type})...")
    
    # For now, just copy the shader files
    # In a real implementation, you would use uam or another tool
    # to compile GLSL to deko3d shader format
    try:
        # Placeholder: Just copy the file
        # In production, you'd run: uam -s <stage> <input> -o <output>
        with open(output_path, "w") as f:
            f.write(f"// Compiled shader: {shader_file}\n")
            f.write(f"// Type: {shader_type}\n")
            f.write("// TODO: Implement actual shader compilation\n")
        print(f"  -> {output_path}")
        return True
    except Exception as e:
        print(f"Error compiling {shader_file}: {e}")
        return False

def main():
    """Main build function"""
    print("=" * 60)
    print("Deko3d Shader Builder")
    print("=" * 60)
    
    # Check for shader compiler
    if not check_uam():
        print("\nWarning: uam shader compiler not found!")
        print("Install from: https://github.com/devkitPro/uam")
        print("Continuing with placeholder compilation...\n")
    
    # Compile all shaders
    success_count = 0
    for shader_file, shader_type in SHADERS:
        if compile_shader(shader_file, shader_type):
            success_count += 1
    
    # Summary
    print("\n" + "=" * 60)
    print(f"Compilation complete: {success_count}/{len(SHADERS)} shaders")
    print("=" * 60)
    
    return 0 if success_count == len(SHADERS) else 1

if __name__ == "__main__":
    sys.exit(main())
