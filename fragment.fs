#version 330 core
out vec4 FragColor;
in vec4 vertexColor;
in vec4 gl_FragCoord;
uniform vec2 windowDimensions;
uniform vec3 cameraLocation = vec3(0.0f,0.0f,-10.0f);
uniform vec3 cameraDirection = vec3(0.0f,0.0f,1.0f);
vec3 sphereLocation = vec3(0.0f,0.0f,10.0f);
float sphereRadius = 1.0f;
float tolerance = 0.001f;
uniform vec3 lightLocation = vec3(0, 5.0, 2.0);
vec3 lightVector = normalize(lightLocation - sphereLocation);
float halfsqrt2 = sqrt(2)*0.5;
vec4 sdSphere( vec3 point, vec3 center, float radius)
{
    vec3 hitVector = point - center;
    return vec4(normalize(hitVector),length(hitVector)-radius);
}
vec4 sdBox( vec3 point, vec3 b, vec3 center )
{
    point = point - center;
    vec3 q = abs(point) - b;
    float maxthree = max(q.x,max(q.y,q.z));
    float distance = length(max(q,0.0)) + min(maxthree,0.0);
    
    return vec4(normalize(vec3(1,1,1)), distance);
}
float ease(float x){    
    return 1 - pow(1 - x, 3);
}
void main()
{
    vec3 vectorRight = cross(vec3(0.0,1.0,0.0), cameraDirection);
    vectorRight.y = 0.0;
    vectorRight = normalize(vectorRight);
    vec3 vectorUp = cross(cameraDirection, vectorRight);
    vec3 screenVector = vectorRight * (gl_FragCoord.x-0.5*windowDimensions.x)/1000  + vectorUp * (gl_FragCoord.y-0.5*windowDimensions.y)/1000;
    vec3 currentPoint = cameraLocation + cameraDirection + screenVector;
    vec3 pixelVector = normalize(currentPoint - cameraLocation);
   // vec3 pixelVector = normalize(vec3((gl_FragCoord.x-0.5*windowDimensions.x)/100,(gl_FragCoord.y-0.5*windowDimensions.y)/100,10.0f));
  //  vec3 currentPoint = vec3((gl_FragCoord.x-0.5*windowDimensions.x)/100,(gl_FragCoord.y-0.5*windowDimensions.y)/100,0.0f);
    FragColor = vec4(0.0f,0.0f,0.2f,1.0f);
    for (int iterations = 0; iterations < 100; iterations++){
       // vec4 hitInfo = sdBox(currentPoint, vec3(1.0,1.0,1.0),sphereLocation);
       vec4 hitInfo = sdSphere(currentPoint, sphereLocation, 1.0);
        if (hitInfo.w <= tolerance){
            vec3 color = vec3(1.0,1.0,1.0);
            //color = hitInfo.xyz;
            //specular
            vec3 viewDir = cameraLocation - currentPoint;
            vec3 halfwayDir = normalize(hitInfo.xyz + viewDir);
            float spec = pow(max(dot(hitInfo.xyz, halfwayDir), 0.0), 20);
            spec = 0;
            //diffuse
            float colorMult = clamp(dot(lightVector, hitInfo.xyz),0,1);
            //final color
            FragColor = vec4(color*colorMult+color*spec+0.1f,1.0f);
            //sFragColor = vec4(color,1.0);
        }
        currentPoint = currentPoint + pixelVector*hitInfo.w;
    }
    
}
