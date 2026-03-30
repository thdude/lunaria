#version 460
#extension GL_EXT_buffer_reference : require

const vec4 verts[3] = {vec4(-1, 1, 0, 1), vec4(-1, -1, 0, 1), vec4(1, 0, 0, 1)};

layout (buffer_reference, std430) readonly buffer DataBuffer {
    mat4 projection;
    mat4 view;
    mat4 model;
};

layout (push_constant, std430) uniform PushConstant {
    DataBuffer databuffer;
} pcs;

void main()
{
    gl_Position = pcs.databuffer.projection * pcs.databuffer.view * pcs.databuffer.model * verts[gl_VertexIndex];
};