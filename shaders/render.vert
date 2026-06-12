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
};

layout (buffer_reference, std430) readonly buffer ObjectBuffer {
    mat4 objects[];
};

layout (push_constant, std430) uniform PushConstant {
    DataBuffer databuffer;
    VertexBuffer vertexbuffer;
    ObjectBuffer objectbuffer;
} pcs;

void main()
{
    Vertex vertex = pcs.vertexbuffer.vertices[gl_VertexIndex];
    gl_Position = pcs.databuffer.projection * pcs.databuffer.view * pcs.objectbuffer.objects[gl_InstanceIndex] * vec4(vertex.position, 1);
};