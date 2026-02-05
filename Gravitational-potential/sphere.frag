uniform vec3 lightDir;   // Direction towards the light
uniform vec3 baseColor;  // Sphere color

uniform float radius;
uniform vec2 pos;

void main()
{
    //pixels mapped [-1,1]
    vec2 p = (gl_FragCoord.xy - pos) / radius;

    float r2 = dot(p, p);
    float z = sqrt(1.0 - r2);
    vec3 normal = normalize(vec3(p.x, p.y, z));
    
    //lambert diffuse
    vec3 l = normalize(lightDir);
    float diff = max(dot(normal, l), 0.05);
    
    vec3 color = baseColor * diff;
    gl_FragColor = vec4(color, 1);
}
