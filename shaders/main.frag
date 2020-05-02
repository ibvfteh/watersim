#version 450
#extension GL_ARB_separate_shader_objects : enable

struct Material
{
	vec4 diffuse;
	int diffuseTextureId;
	float fuzziness;
	float refractionIndex;
	uint materialModel;
};

layout(binding = 1) readonly buffer materialArray { Material[] materials; };

layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in flat int fragMaterial;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = materials[fragMaterial].diffuse;//texture(texSampler, fragTexCoord);
}