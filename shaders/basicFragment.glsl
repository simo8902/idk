#version 460 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 viewPos; // Camera position
uniform vec3 objectColorUniform; // Object color

struct DirectionalLight {
      vec3 direction;
      vec3 ambient;
      vec3 diffuse;
      vec3 specular;
};

uniform DirectionalLight dirLight;

void main() {
      // Normalize the normal
      vec3 norm = normalize(Normal);

      // Calculate the light direction
      vec3 lightDir = normalize(-dirLight.direction);

      // Ambient
      vec3 ambient = dirLight.ambient * objectColorUniform; // Scale by object color

      // Diffuse
      float diff = max(dot(norm, lightDir), 0.0);
      vec3 diffuse = dirLight.diffuse * diff * objectColorUniform; // Scale by object color

      // Specular
      vec3 viewDir = normalize(viewPos - FragPos);
      vec3 reflectDir = reflect(-lightDir, norm);
      float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
      vec3 specular = dirLight.specular * spec;

      // Combine results
      vec3 result = ambient + diffuse + specular;
      FragColor = vec4(result, 1.0);
}
