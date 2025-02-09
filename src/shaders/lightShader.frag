#version 450 core

out vec4 FragColor;
in vec2 TexCoords;

// G-buffer samplers (if needed)
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

// Directional light properties
struct DirectionalLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform DirectionalLight dirLight;
uniform vec3 viewPos; // Camera position in world space

void main()
{
    // Retrieve data from G-buffer (if applicable)
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal  = normalize(texture(gNormal, TexCoords).rgb);
    vec3 Albedo  = texture(gAlbedoSpec, TexCoords).rgb;

    // Set directional light properties to white
    vec3 ambient = dirLight.ambient * Albedo;
    vec3 lightDir = normalize(-dirLight.direction);
    float diff = max(dot(Normal, lightDir), 0.0);
    vec3 diffuse = dirLight.diffuse * diff * Albedo;

    float specularStrength = 1.0;
    float shininess        = 32.0;

    vec3 viewDir    = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, Normal);
    float spec      = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular  = dirLight.specular * spec * vec3(1.0); // White specular

    // Combine results
    vec3 lighting = ambient + diffuse + specular;

    // Output final color
    FragColor = vec4(lighting, 1.0);
}
