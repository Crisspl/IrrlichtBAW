#version 430 core
layout(location = 0) in vec3 vPos;
layout(location = 2) in vec2 vTexCoord;

layout(push_constant, row_major) uniform Block {
	mat4 modelViewProj;
} PushConstants;

layout(location = 0) out vec2 uv;

#include <irr/builtin/glsl/broken_driver_workarounds/amd.glsl>

void main()
{
    gl_Position = irr_builtin_glsl_workaround_AMD_broken_row_major_qualifier_mat4x4(PushConstants.modelViewProj)*vec4(vPos,1.0);
	uv = vTexCoord;
}
		