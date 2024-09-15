#version 430 core

layout(local_size_x = 256);

layout(std430, binding = 0) buffer rBuffer
{
    vec3 r[];
};

layout(std430, binding = 1) buffer vBuffer
{
    vec3 v[];
};

layout(std430, binding = 2) buffer xBuffer
{
    vec3 x[];
};

uniform vec3 c;
uniform mat3 R;
uniform float dt;
uniform uint size;

void main()
{
    uint i = gl_GlobalInvocationID.x;
    if (i < size)
    {
        v[i] = (c + R * r[i] - x[i]) / dt;
        x[i] = c + R * r[i];
    }
}