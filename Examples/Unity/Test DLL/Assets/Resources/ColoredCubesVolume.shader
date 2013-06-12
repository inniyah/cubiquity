// Upgrade NOTE: commented out 'float3 _WorldSpaceCameraPos', a built-in variable
// Upgrade NOTE: commented out 'float4x4 _Object2World', a built-in variable
// Upgrade NOTE: commented out 'float4x4 _World2Object', a built-in variable

Shader "ColoredCubesVolume"
{
   Properties {
      _SpecColor ("Specular Material Color", Color) = (1,1,1,1) 
      _Shininess ("Shininess", Float) = 10
   }
   SubShader {
      Pass {    
         Tags { "LightMode" = "ForwardBase" } 
            // pass for ambient light and first light source
 
         CGPROGRAM
 
         #pragma vertex vert  
         #pragma fragment frag 
         
         #pragma target 3.0
         #pragma only_renderers d3d9
 
         // User-specified properties
         uniform float4 _SpecColor; 
         uniform float _Shininess;
 
         // The following built-in uniforms (apart from _LightColor0) 
         // are defined in "UnityCG.cginc", which could be #included 
         //uniform float4 unity_Scale; // w = 1/scale; see _World2Object
         // uniform float3 _WorldSpaceCameraPos;
         // uniform float4x4 _Object2World; // model matrix
         // uniform float4x4 _World2Object; // inverse model matrix 
            // (all but the bottom-right element have to be scaled 
            // with unity_Scale.w if scaling is important) 
         //uniform float4 _WorldSpaceLightPos0; 
            // position or direction of light source
         uniform float4 _LightColor0; 
            // color of light source (from "Lighting.cginc")
 
         struct vertexInput {
            float4 vertex : POSITION;
            //float3 normal : NORMAL;
         };
         struct vertexOutput {
            float4 pos : SV_POSITION;
            float4 posWorld : TEXCOORD0;
            float4 posModel : TEXCOORD1;
            float4 colour : COLOR;
            //float3 normalDir : TEXCOORD1;
         };
         
		float3 unpackPosition(float packedPosition)
		{	
			// Store the input in each component
			float3 packedVec = float3(packedPosition, packedPosition, packedPosition);
		
			// Convert each component to a value in the range 0-255      	
			float3 result = floor(packedVec / float3(65536.0, 256.0, 1.0));	      	
			float3 shiftedResult = float3(0.0, result.rg) * 256.0;	
			result -= shiftedResult;
			
			result -= float3(0.5, 0.5, 0.5);
		
			// Return the result	
			return result;
		}
		
		float3 floatToRGB(float inputVal)
		{	
			// Store the input in each component
			float3 inputVec = float3(inputVal, inputVal, inputVal);
		
			// Convert each component to a value in the range 0-255      	
			float3 result = floor(inputVec / float3(256.0, 16.0, 1.0));	      	
			float3 shiftedResult = float3(0.0, result.rg) * 16.0;	
			result -= shiftedResult;
		
			// Convert to range 0-1
			result /= 15.0;
		
			// Return the result	
			return result;
		}
		
		float positionBasedNoise(float4 positionAndStrength)
		{
			//'floor' is more widely supported than 'round'. Offset consists of:
			//  - An integer to push us away from the origin (divide by zero causes a ringing artifact
			//    at one point in the world, and we want to pushthis somewhere it won't be seen.)
			//  - 0.5 to perform the rounding
			//  - A tiny offset to prevent sparkes as faces are exactly on rounding boundary.
			float3 roundedPos = floor(positionAndStrength.xyz + vec3(1000.501));
		
			//Our noise function generate banding for high inputs, so wrap them
			roundedPos = fmod(roundedPos, float3(17.0, 19.0, 23.0));
		
			//Large number is arbitrary, but smaller number lead to banding. '+ 1.0' prevents divide-by-zero
			float noise = 100000000.0 / (dot(roundedPos, roundedPos) + 1.0);
			noise = fract(noise);
		
			//Scale the noise
			float halfNoiseStrength = positionAndStrength.w * 0.5;
			noise = -halfNoiseStrength + positionAndStrength.w * noise; //http://www.opengl.org/wiki/GLSL_Optimizations#Get_MAD
		
			return noise;
		}
 
         vertexOutput vert(vertexInput input) 
         {
            vertexOutput output;
 
            float4x4 modelMatrix = _Object2World;
            float4x4 modelMatrixInverse = _World2Object; 
               // multiplication with unity_Scale.w is unnecessary 
               // because we normalize transformed vectors
               
            float4 unpackedPos = float4(unpackPosition(input.vertex.x), 1.0f);
            
            output.posModel = unpackedPos;
            
            output.colour = float4(floatToRGB(input.vertex.y), 1.0f);
 
            output.posWorld = mul(modelMatrix, unpackedPos);
            //output.normalDir = normalize(float3(mul(float4(input.normal, 0.0), modelMatrixInverse)));
            output.pos = mul(UNITY_MATRIX_MVP, unpackedPos);
            return output;
         }
 
         float4 frag(vertexOutput input) : COLOR
         {
            //float3 normalDirection = normalize(input.normalDir);
            // Compute the surface normal in the fragment shader.
      		float3 normalDirection = normalize(cross(ddx(input.posWorld.xyz), ddy(input.posWorld.xyz)));
      		
      		//Add noise
	    	float noise = positionBasedNoise(float4(input.posModel.xyz, 0.1));
	    	
	    	float3 colourWithNoise = input.colour.rgb + float3(noise, noise, noise);
 
            float3 viewDirection = normalize(
               _WorldSpaceCameraPos - float3(input.posWorld));
            float3 lightDirection;
            float attenuation;
 
            if (0.0 == _WorldSpaceLightPos0.w) // directional light?
            {
               attenuation = 1.0; // no attenuation
               lightDirection = 
                  normalize(float3(_WorldSpaceLightPos0));
            } 
            else // point or spot light
            {
               float3 vertexToLightSource = 
                  float3(_WorldSpaceLightPos0 - input.posWorld);
               float distance = length(vertexToLightSource);
               attenuation = 1.0 / distance; // linear attenuation 
               lightDirection = normalize(vertexToLightSource);
            }
 
            float3 ambientLighting = 
               float3(UNITY_LIGHTMODEL_AMBIENT) * float3(colourWithNoise);
 
            float3 diffuseReflection = 
               attenuation * float3(_LightColor0) * float3(colourWithNoise)
               * max(0.0, dot(normalDirection, lightDirection));
 
            float3 specularReflection;
            if (dot(normalDirection, lightDirection) < 0.0) 
               // light source on the wrong side?
            {
               specularReflection = float3(0.0, 0.0, 0.0); 
                  // no specular reflection
            }
            else // light source on the right side
            {
               specularReflection = attenuation * float3(_LightColor0) 
                  * float3(_SpecColor) * pow(max(0.0, dot(
                  reflect(-lightDirection, normalDirection), 
                  viewDirection)), _Shininess);
            }
            
            // This seems a bit crazy, but apparently Unity doubles dynamic lights.
            // http://forum.unity3d.com/threads/33955-Why-is-ambient-light-at-half-strength?p=220635&viewfull=1#post220635
            diffuseReflection *= 2.0f;
            specularReflection *= 2.0f;
 
            return float4(ambientLighting + diffuseReflection 
               + specularReflection, 1.0);
         }
 
         ENDCG
      }
   }
   // The definition of a fallback shader should be commented out 
   // during development:
   // Fallback "Specular"
}
