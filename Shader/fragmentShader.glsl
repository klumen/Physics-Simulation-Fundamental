#version 430 core

struct Material
{
    sampler2D diffuse0;
    sampler2D specular0;
    float Ns;
};

struct DirLight
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
vec3 calculate_direction_light(DirLight light, vec3 normal, vec3 viewDir);

struct PointLight
{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};
vec3 calculate_point_light(PointLight light, vec3 normal, vec3 viewDir, vec3 frgPos);

struct SpotLight
{
    vec3 position;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;

    float cutoff;
    float outerCutoff;
};
vec3 calculate_spot_light(SpotLight light, vec3 normal, vec3 viewDir, vec3 frgPos);

in vec3 frgPos;
in vec3 frgNormal;
in vec2 frgTexCoord;

out vec4 frgColor;

uniform vec3 viewPos;
uniform Material material;
uniform DirLight dirLight;
uniform SpotLight spotLight;

uniform bool hasTexture;

void main()
{
    vec3 normal = normalize(frgNormal);
    vec3 viewDir = normalize(viewPos - frgPos);
    vec3 result = vec3(0.0, 0.0, 0.0);
    result += calculate_direction_light(dirLight, normal, viewDir);
    // result += calculate_spot_light(spotLight, normal, viewDir, frgPos);

    frgColor = vec4(result, 1.0) + vec4(0.0, 0.0, 0.1, 1.0);
}

vec3 calculate_direction_light(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    vec3 halfDir = normalize((lightDir + viewDir) / 2.0);

    vec3 ambient = light.ambient;// * vec3(texture(material.diffuse0, frgTexCoord));
    vec3 diffuse = light.diffuse * max(dot(lightDir, normal), 0);// * vec3(texture(material.diffuse0, frgTexCoord));
    vec3 specular = light.specular * pow(max(dot(halfDir, normal), 0), material.Ns);// * vec3(texture(material.specular0, frgTexCoord));

    return (ambient + diffuse + specular);
}

vec3 calculate_point_light(PointLight light, vec3 normal, vec3 viewDir, vec3 frgPos)
{
    vec3 lightDir = normalize(light.position - frgPos);
    vec3 halfDir = normalize((lightDir + viewDir) / 2.0);

    float dis = length(light.position - frgPos);
    float attenuation = 1.0 / (light.constant + light.linear * dis + light.quadratic * (dis * dis));

    vec3 ambient = light.ambient * vec3(texture(material.diffuse0, frgTexCoord));
    vec3 diffuse = light.diffuse * max(dot(lightDir, normal), 0) * vec3(texture(material.diffuse0, frgTexCoord));
    vec3 specular = light.specular * pow(max(dot(halfDir, normal), 0), material.Ns) * vec3(texture(material.specular0, frgTexCoord));

    return (ambient + diffuse + specular) * attenuation;
}

vec3 calculate_spot_light(SpotLight light, vec3 normal, vec3 viewDir, vec3 frgPos)
{
    vec3 lightDir = normalize(light.position - frgPos);
    vec3 halfDir = normalize((lightDir + viewDir) / 2.0);

    float dis = length(light.position - frgPos);
    float attenuation = 1.0 / (light.constant + light.linear * dis + light.quadratic * (dis * dis));

    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutoff - light.outerCutoff;
    float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);

    vec3 ambient = light.ambient * vec3(texture(material.diffuse0, frgTexCoord));
    vec3 diffuse = light.diffuse * max(dot(lightDir, normal), 0) * vec3(texture(material.diffuse0, frgTexCoord));
    vec3 specular = light.specular * pow(max(dot(halfDir, normal), 0), material.Ns) * vec3(texture(material.specular0, frgTexCoord));

    return (ambient + diffuse + specular) * attenuation * intensity;
}