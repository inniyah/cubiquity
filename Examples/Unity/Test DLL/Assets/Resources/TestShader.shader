Shader "TestShader"
{
    SubShader
    {
      Tags { "RenderType" = "Opaque" }
      
      CGPROGRAM
      #pragma surface surf Lambert vertex:vert
      
      struct Input
      {
          float4 color : COLOR;
      };
      
      void vert (inout appdata_full v, out Input o)
      {
      	v.normal = float3 (0.0f, 0.0f, 1.0f);

    v.tangent = float4 (1.0f, 0.0f, 0.0f, 1.0f);
    
          UNITY_INITIALIZE_OUTPUT(Input,o);
      }
      
      void surf (Input IN, inout SurfaceOutput o)
      {
          o.Albedo = half3(1.0f, 0.0f, 0.0f);
          o.Normal = half3(0.0f, 1.0f, 0.0f);
          //IN.Normal;
      }
      ENDCG
    }
    Fallback "Diffuse"
  }