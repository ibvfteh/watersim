#version 450

layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;

layout(location = 0) in vec4 inPosition[];
layout(location = 1) in vec3 inToCamera[];
layout(location = 2) in vec3 inNormals[];

layout(location = 0) out vec4 outPosition;
layout(location = 1) out vec3 outtoCamera;
layout(location = 2) out vec3 outNormals;

void main() {
    vec3 first = gl_in[0].gl_Position.xyz;
    vec3 secnd = gl_in[1].gl_Position.xyz;
    vec3 third = gl_in[2].gl_Position.xyz;

    vec3 normal = -normalize(cross(secnd - first, third - first));

    for(int i=0; i<3; i++) {
        gl_Position = gl_in[i].gl_Position;
        outPosition = inPosition[i];
        outtoCamera = inToCamera[i];

        outNormals = normal;
        EmitVertex();
    }
    EndPrimitive();
}