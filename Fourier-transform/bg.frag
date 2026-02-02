#version 300 es
precision highp float;

uniform vec2 resolution;

out vec4 fragColor;

void main()
{
    vec2 uv = gl_FragCoord.xy;

    float dBg = distance(uv, resolution / 2.0);
    float noise = fract(sin(dot(uv, vec2(12.9898, 78.233))) * 43758.5453);
    float bgScale = dBg / (length(resolution) / 2.0) + (noise - 0.5) / 8.0;
    vec3 bgColor = mix(vec3(1.0, 0.7, 0.2), vec3(1.0, 0.2, 0.7), bgScale);

    fragColor = vec4(bgColor * 0.2, 1.0);
}
