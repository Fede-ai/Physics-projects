#version 300 es
precision highp float;

uniform sampler2D previousFrame;
uniform vec2 pos1;
uniform vec2 pos2;
uniform vec2 resolution;
uniform bool isPlaying;

uniform float fade;
uniform float radius;
uniform float strength;

out vec4 fragColor;

void main()
{
    vec2 uv = gl_FragCoord.xy;
    vec4 prev = texture(previousFrame, uv / resolution);

    if (isPlaying) {
        prev.rgb = max((prev.rgb - 0.003) * fade, 0.0);
    }

    vec2 v = pos2 - pos1;
    vec2 w = uv - pos1;
    float t = dot(w, v) / dot(v, v);
    t = clamp(t, 0.0, 1.0);
    vec2 closestPoint = pos1 + t * v;

    float d = length(uv - closestPoint) / radius;
    float glow = strength * exp(-d * d);
    vec3 pointColor = vec3(0.2, 0.7, 1.0) * glow;

    float dBg = distance(uv, resolution / 2.0);
    float noise = fract(sin(dot(uv, vec2(12.9898, 78.233))) * 43758.5453);
    float bgScale = dBg / (length(resolution) / 2.0) + (noise - 0.5) / 8.0;
    vec3 bgColor = mix(vec3(0.2, 0.7, 1.0), vec3(0.7, 0.2, 1.0), bgScale);

    fragColor = vec4(max(prev.rgb, pointColor + bgColor * 0.2), 1.0);
}
