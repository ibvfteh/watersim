#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 underView;
    mat4 proj;
    vec4 cameraPos;
} ubo;

layout(binding = 3) uniform sampler2D heightmap;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in int inMaterial;

layout(location = 0) out vec4 outPosition;
layout(location = 1) out vec3 toCamera;
layout(location = 2) out vec3 outNormal;

void main() {
    vec3 position = inPosition;

    vec4 worldPosition = ubo.model * vec4(position, 1.0);
    outPosition = ubo.proj * ubo.view * worldPosition;

    position.y += 0.1f * texture(heightmap, inTexCoord).r;
    toCamera = ubo.cameraPos.xyz - worldPosition.xyz;
    worldPosition = ubo.model * vec4(position, 1.0);
    gl_Position = ubo.proj * ubo.view * worldPosition;

    outNormal = inNormal;
}