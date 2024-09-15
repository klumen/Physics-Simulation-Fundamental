#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 frgPos;
out vec3 frgNormal;
out vec2 frgTexCoord;

void main()
{
    frgPos = vec3(model * vec4(aPos, 1.0));
    frgNormal = mat3(transpose(inverse(model))) * aNormal;
    frgTexCoord = aTexCoord;
    gl_Position = projection * view * vec4(frgPos, 1.0);
}