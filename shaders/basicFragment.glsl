#version 460 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 viewPos;
uniform vec3 objectColorUniform;

uniform vec3 globalAmbientColor;

struct DirectionalLight {
      vec3 direction;
      vec3 ambient;
      vec3 diffuse;
      vec3 specular;
};

uniform DirectionalLight dirLight;

void main() {
      vec3 norm = normalize(Normal);

      vec3 lightDir = normalize(-dirLight.direction);

      // ************ Global Ambient Lighting ************ //
      vec3 globalAmbient = globalAmbientColor * objectColorUniform;

      // ************ Directional Light Calculations ************ //

      vec3 ambient = dirLight.ambient * objectColorUniform;

      float diff = max(dot(norm, lightDir), 0.0);
      vec3 diffuse = dirLight.diffuse * diff * objectColorUniform;

      vec3 viewDir = normalize(viewPos - FragPos);
      vec3 reflectDir = reflect(-lightDir, norm);
      float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
      vec3 specular = dirLight.specular * spec;

      // ************ Combine Global and User-Defined Lighting ************ //
      vec3 result = globalAmbient + ambient + diffuse + specular;

      FragColor = vec4(result, 1.0);
}
