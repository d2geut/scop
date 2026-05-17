#version 330 core
in vec3 normal;
in vec3 position;
in vec3 color;
out vec4 fragColor;

uniform vec3 viewPos;

struct Light {
    vec3 position;
    vec3 direction;
    float range;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;

struct Material {
    // vec3 ambient;
    // vec3 diffuse;
    vec3 specular;
    float shininess;
};
uniform Material material;

vec3 calcDirectionalLight() {
    vec3 ambient = color * light.ambient;

    vec3 lightDir = normalize(-light.direction);
    vec3 pixelNorm = normalize(normal);
    float diff = max(dot(pixelNorm, lightDir), 0.0);
    vec3 diffuse =  diff * color * light.diffuse;

    vec3 viewDir = normalize(viewPos - position);
    vec3 reflectDir = reflect(-lightDir, pixelNorm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * material.specular * light.specular;

    return (ambient + diffuse + specular);
}

vec3 calcPointLight() {
    vec3 ambient = color * light.ambient;

    float dist = length(light.position - position);
    float atten = 1.0 / (dist * dist + 1.0);
    float window = pow(max(1.0 - pow(dist / light.range, 4.0), 0.0), 2.0);
    float attenuation = atten * window;
    vec3 lightDir = (light.position - position) / dist;

    vec3 pixelNorm = normalize(normal);
    float diff = max(dot(pixelNorm, lightDir), 0.0);
    vec3 diffuse =  diff * color * light.diffuse;

    vec3 viewDir = normalize(viewPos - position);
    vec3 reflectDir = reflect(-lightDir, pixelNorm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * material.specular * light.specular;

    vec3 result = ambient + (diffuse + specular) * attenuation;
    return result;
}

float random(vec2 uv) {
    return fract(sin(dot(uv, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
    // init color
    vec3 result = vec3(0.0);

    // directional light result
    result += calcDirectionalLight() * 0.02;
    // all point light result
    result += calcPointLight() * 5.0;

    // tone mapping
    vec3 mapped = result / (result + vec3(1.0));

    // 감마 보정
    vec3 color = pow(mapped, vec3(1.0 / 2.2));

    // dithering
    float noise = (random(gl_FragCoord.xy) - 0.5) / 255.0;
    color += noise;
    fragColor = vec4(color, 1.0);
}