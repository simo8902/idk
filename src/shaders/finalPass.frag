#version 450 core

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D lightingTexture;

out vec4 FragColor;
in vec2 TexCoords;

uniform vec3 viewPos;  // Camera position

void main()
{
    // Retrieve data from the G-buffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;  // World space position
    vec3 Normal = normalize(texture(gNormal, TexCoords).rgb);  // Normal vector
    vec3 Albedo = texture(gAlbedoSpec, TexCoords).rgb;  // Color of the object

    // Retrieve lighting data (from the lighting pass)
    vec3 lighting = texture(lightingTexture, TexCoords).rgb;  // Lighting result (diffuse + specular)

    // Simple diffuse lighting
    float diffuse = max(dot(Normal, lighting), 0.0);

    // Specular lighting (using view position, world position, and normal)
    vec3 viewDir = normalize(viewPos - FragPos);  // Direction from fragment to camera
    vec3 reflectDir = reflect(-lighting, Normal);  // Reflection direction
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);  // Phong specular component

    // Combine the diffuse, specular, and albedo with the lighting
    vec3 finalColor = (diffuse + spec) * Albedo;  // Simple diffuse + specular lighting model

    // Output the final color
    FragColor = vec4(finalColor, 1.0);  // Set the final output color with full opacity
}
