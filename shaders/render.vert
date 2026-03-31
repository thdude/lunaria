#version 460
#extension GL_EXT_buffer_reference : require

struct Vertex
{
    vec3 position;
    float u;
    vec3 normal;
    float v;
    vec4 color;
};

layout (buffer_reference, std430) readonly buffer VertexBuffer {
    Vertex vertices[];
};

layout (buffer_reference, std430) readonly buffer DataBuffer {
    mat4 projection;
    mat4 view;
    mat4 model;
};

layout (push_constant, std430) uniform PushConstant {
    DataBuffer databuffer;
    VertexBuffer vertexbuffer;
} pcs;

void main()
{
    Vertex vertex = pcs.vertexbuffer.vertices[gl_VertexIndex];
    gl_Position = pcs.databuffer.projection * pcs.databuffer.view * pcs.databuffer.model * vec4(vertex.position, 1);
};