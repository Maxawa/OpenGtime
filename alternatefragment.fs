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
float lightStrength = 5.0;
uniform sampler2D SphereTexture;
uniform sampler2D NormalTexture;
uniform sampler2D HeightTexture;
mat2x3 sdSphere(vec3 point, vec3 center, float radius) //first element is distance, next two are uv, last one is normal
{
    vec3 hitVector = point - center;
    vec3 normHit = normalize(hitVector);
    vec2 uv = vec2(atan(-normHit.x, normHit.z)/6.28318530 + 0.501,normHit.y);
    return mat2x3(
                vec3(length(hitVector)-radius,uv),
                normHit
                );
}
float maxVector3(vec3 inputVec){
return max(inputVec.x,max(inputVec.y,inputVec.z));
}
mat2x3 sdBox( vec3 point, vec3 b, vec3 center )
{
    vec3 localPoint = point - center;
    vec3 q = abs(localPoint) - b;
    float maxthree = max(q.x,max(q.y,q.z));
    float distance = length(max(q,0.0)) + min(maxthree,0.0);
    vec3 nq = localPoint/b;
    float normalX = float(abs(nq.x)>0.999)*(1-2*float(nq.x<=nq.y));;
    float normalY = float(abs(nq.y)>0.999)*(1-2*float(nq.y<=nq.z));
    float normalZ = float(abs(nq.z)>0.999)*(1-2*float(nq.z<=nq.x));
  //  normalX = 1-nq.x;
   // normalY = 0;
   // normalZ = 0;
    return mat2x3(
        vec3(distance,0.5,0.5),
        vec3(normalX, normalY, normalZ)
        //localPoint
    );
}
float DiffuseLight(vec3 normal,float strength, vec3 currentPoint){
    vec3 light = normalize(lightLocation - currentPoint);
    return strength*clamp(dot(light, normal),0,1)/pow(length(lightLocation-currentPoint),2);
}
//main cast ray function takes in a starting point, direction (normalized), fragcolor and iteration max
//it will return if it hit something, the iteration count, the object it hit, hit location, the color of that object, the normal of that object.
mat4x3 CastRay(vec3 rayPoint, vec3 direction, int maxIterations){
    vec3 hitColor = vec3(256.0, 256.0, 256.0);
    vec3 hitNormal = vec3(256.0, 256.0, 256.0);
    float distanceTraveled = 0.0;
    int didHit = 0;
    int stepCount = 0;
    int hitID = 0; //1 is sphere, 2 is wall, 3 is light
    
    for (int iterations = 0; iterations <= maxIterations; iterations++){
        stepCount = iterations;
       mat2x3 hitInfo = sdSphere(rayPoint, sphereLocation, sphereRadius);
         // float distSphere = hitInfo[0].x + texture(HeightTexture, hitInfo[0].yz).x/10;
         float distSphere = hitInfo[0].x;
        if (distSphere <= tolerance){
            //hitNormal = hitInfo[1]*texture(NormalTexture, hitInfo[0].yz).yzx;
            hitNormal = hitInfo[1];
           // vec4 color = texture(SphereTexture, uvSphere);
            hitColor = vec3(1.0,1.0,1.0);
            //diffuse
          //  float colorMult = DiffuseLight(normalVector, lightStrength, rayPoint);
            //final color
          //  hitColor = (color * colorMult).xyz;
            didHit = 1;
            hitID = 1;
            break;
        }


        mat2x3 hitInfoBox1 = sdBox(rayPoint, vec3(0.25,4,4) ,sphereLocation + vec3(3.75,0.0,0));
        float distBox1 = hitInfoBox1[0].x;
        if (distBox1 <= tolerance){
            hitNormal = hitInfoBox1[1];
            // vec4 color = texture(SphereTexture, uvSphere);
            hitColor = vec3(1.0,0.0,0.0);
            //diffuse
           // float colorMult = DiffuseLight(normalVector, lightStrength, rayPoint);
            //final color
           // hitColor = (color * colorMult).xyz;
            didHit = 1;
            hitID = 2;
           break;
        }


        mat2x3 hitInfoBox2 = sdBox(rayPoint, vec3(0.25,4,4) ,sphereLocation + vec3(-4.25,0.5,0.0));
        float distBox2 = hitInfoBox2[0].x;
        if (distBox2 <= tolerance){
            hitNormal = hitInfoBox2[1];
            // vec4 color = texture(SphereTexture, uvSphere);
            hitColor = vec3(0.0,1.0,0.0);
            //diffuse
           // float colorMult = DiffuseLight(normalVector, lightStrength, rayPoint);
            //final color
            //hitColor = (color * colorMult).xyz;
            didHit = 1;
            hitID = 2;
            break;
        }


        mat2x3 hitInfoBox3 = sdBox(rayPoint, vec3(4,0.25,4) ,sphereLocation + vec3(0.0,4.25,0));
        float distBox3 = hitInfoBox3[0].x;
        if (distBox3 <= tolerance){
            hitNormal = hitInfoBox3[1];
            // vec4 color = texture(SphereTexture, uvSphere);
            hitColor = vec3(0.0,0.0,1.0);
            //diffuse
          //  float colorMult = DiffuseLight(normalVector, lightStrength, rayPoint);
            //final color
          //  hitColor = (color * colorMult).xyz;
            didHit = 1;
            hitID = 2;
            break;
        }


        mat2x3 hitInfoBox4 = sdBox(rayPoint, vec3(4,0.25,4) ,sphereLocation + vec3(-0.5,-3.75,0.0));
        float distBox4 = hitInfoBox4[0].x;
        if (distBox4 <= tolerance){
            hitNormal = hitInfoBox4[1];
            // vec4 color = texture(SphereTexture, uvSphere);
            hitColor = vec3(1.0,1.0,1.0);
            //diffuse
            //float colorMult = DiffuseLight(normalVector, lightStrength, rayPoint);
            //final color
          //  hitColor = (color * colorMult).xyz;
            didHit = 1;
            hitID = 2;
            break;
        }


        mat2x3 hitInfoBox5 = sdBox(rayPoint, vec3(4.25,4.25,0.25) ,sphereLocation + vec3(-0.25,0.25,4.25));
        float distBox5 = hitInfoBox5[0].x;
        if (distBox5 <= tolerance){
            hitNormal = hitInfoBox5[1];
            // vec4 color = texture(SphereTexture, uvSphere);
            hitColor = vec3(1.0,1.0,0.0);
            //diffuse
 //           float colorMult = DiffuseLight(normalVector, lightStrength, rayPoint);
            //final color
  //          hitColor = (color * colorMult).xyz;
            didHit = 1;
            hitID = 2;
            break;
        }
        mat2x3 lightSphere = sdSphere(rayPoint,lightLocation, 0.2);
        float distLight = lightSphere[0].x;
        if (distLight <= tolerance){
            hitColor = vec3(1.0,1.0,0.0);
            didHit = 1;
            hitID = 3;
            break;
        }
        float stepSize = min(distSphere, min(min(distBox1,min(distBox2,min(distBox3,min(distBox4,distBox5)))), distLight));
        distanceTraveled += stepSize;
        if (distanceTraveled >= 1000.0){
          break;
        }
        
        rayPoint = rayPoint + direction*stepSize;
    }
    

    vec3 randomData = vec3(didHit, stepCount, hitID);

    return mat4x3(randomData, rayPoint, hitColor, hitNormal);
}
void main()
{
    vec3 vectorRight = cross(vec3(0.0,1.0,0.0), cameraDirection);
    vectorRight.y = 0.0;
    vectorRight = normalize(vectorRight);
    vec3 vectorUp = cross(cameraDirection, vectorRight);
    vec3 screenVector = vectorRight * (gl_FragCoord.x-0.5*windowDimensions.x)/1000  + vectorUp * (gl_FragCoord.y-0.5*windowDimensions.y)/1000;
    vec3 currentPoint = cameraLocation + cameraDirection/5 + screenVector/5;
    vec3 pixelVector = normalize(currentPoint - cameraLocation);
    FragColor = vec4(0.0f,0.0f,0.0f,1.0f);
    mat4x3 TheRay = CastRay(currentPoint, pixelVector, 100);
    if(bool(TheRay[0].x)){ //if the ray has hit something
       FragColor = vec4(TheRay[2],1.0); //take over the color from the ray
       vec3 rayHit = TheRay[1];
       if (TheRay[0].z != 3){ //if the ray hits anything but the light
            vec3 lightVec = normalize(lightLocation - rayHit);
            mat4x3 ShadowRay = CastRay(rayHit+lightVec*0.1, lightVec, 50);
            if (ShadowRay[0].z != 3){ //if the point is in shadow (light is obstructed)
              float shadowStrength = 1-clamp(lightStrength*(ShadowRay[0].y)/200,0,1);
              FragColor.xyz = mix(TheRay[2],vec3(0.0,0.0,0.0),shadowStrength);
            } 
       //     float colorMult = DiffuseLight(TheRay[3],lightStrength, rayHit);
        //   FragColor.xyz *= colorMult;
            //FragColor.xyz = TheRay[0].yyy/100; //cool effect
       }
       
    }
    
}

