#pragma once

#include "graphics/shaders/compiled_shader.h"


namespace kl {
    class ShaderCompiler
    {
    public:
        // Compilation
        CompiledShader compile(const std::string_view& name, const std::string_view& version, const std::string_view& source) const;

        // Predefined compilation
        CompiledShader compile_vertex_shader(const std::string_view& source) const;
        CompiledShader compile_geometry_shader(const std::string_view& source) const;
        CompiledShader compile_pixel_shader(const std::string_view& source) const;
        CompiledShader compile_compute_shader(const std::string_view& source) const;
    };
}
