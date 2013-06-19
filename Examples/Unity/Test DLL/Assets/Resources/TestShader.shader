Shader "TestShader"
{
    SubShader
    {
      Tags { "RenderType" = "Opaque" }
      
      CGPROGRAM
      #pragma surface surf Lambert
      
      struct Input
      {
          float4 color : COLOR;
      };
      
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