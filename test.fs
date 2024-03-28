#version 330 core
out vec4 FragColor;
in vec4 vertexColor;
in vec4 gl_FragCoord;
uniform vec2 windowDimensions;
uniform vec3 cameraLocation = vec3(0.0f,0.0f,-10.0f);
uniform vec3 cameraDirection = vec3(0.0f,0.0f,1.0f);
uniform vec3 lightLocation = vec3(0, 5.0, 2.0);
uniform sampler2D SphereTexture;
uniform sampler2D NormalTexture;
uniform sampler2D HeightTexture;
void main()
{
  FragColor = vec4(1.0,0.0,1.0,1.0);
}
