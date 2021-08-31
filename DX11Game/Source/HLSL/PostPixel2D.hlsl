// 2D�p�s�N�Z���V�F�[�_

// �O���[�o��
cbuffer global : register(b1) {
    matrix g_mTexture;
    float4 g_color;
    int SampleCount;
    float4 Offset[16];
};

// �p�����[�^
struct VS_OUTPUT {
	float4	Position	: SV_Position;
	float2	TexCoord	: TEXCOORD0;
	float4	Diffuse		: COLOR0;
};

Texture2D    g_texture : register(t0);	// �e�N�X�`��
SamplerState g_sampler : register(s0);	// �T���v��

Texture2D<float> g_depthTexture : register(t1);			// �[�x�X�e���V���e�N�X�`��
SamplerComparisonState g_depthSampler : register(s1);	// �[�x�X�e���V���T���v��

// �\�[�x���t�B���^�[
float4 Sobel(float2 tex, Texture2D<float> depthBuffer)
{
	const float threshold = 2.0; //0.05;

	const int2 texAddrOffsets[8] =
	{
		int2(-1, -1),
			int2(0, -1),
			int2(1, -1),
			int2(-1, 0),
			int2(1, 0),
			int2(-1, 1),
			int2(0, 1),
			int2(1, 1),
	};

	float lum[8];
	int i;

	float3 LuminanceConv = { 0.2125f, 0.7154f, 0.0721f };

	uint width, height, levels;

	depthBuffer.GetDimensions(0, width, height, levels);

	for (i = 0; i < 8; i++)
	{
		float3 colour = depthBuffer.Load(int3(int2(tex * float2(width, height)) + texAddrOffsets[i] * threshold, 0));
		lum[i] = dot(colour * threshold, LuminanceConv);
	}

	float x = lum[0] + 2 * lum[3] + lum[5] - lum[2] - 2 * lum[4] - lum[7];
	float y = lum[0] + 2 * lum[1] + lum[2] - lum[5] - 2 * lum[6] - lum[7];
	float edge = sqrt(x * x + y * y);

	return float4(edge, edge, edge, 1);
}

float4 main(VS_OUTPUT input) : SV_Target0
{
	float4 Color = input.Diffuse;
	if (g_mTexture._44 > 0.0f) {
		Color *= g_texture.Sample(g_sampler, input.TexCoord);
        float w = Color.w;
		
		//--- �|�X�g�G�t�F�N�g
		
		//// �K�E�X�u���[
  //      for (int i = 0; i < SampleCount; ++i)
  //      {
  //          Color += Offset[i].z * g_texture.Sample(g_sampler, input.TexCoord + Offset[i].xy);
  //      }
  //      Color.w = w;
		
		// �F����
		float size = 0.006f;
		float2 uvBase = input.TexCoord - 0.5f;
		    // R�l
		float2 uvR = uvBase * (1.0f - size * 2.0f) + 0.5f;
		Color.r = g_texture.Sample(g_sampler, uvR).r;
		    // G�l
		float2 uvG = uvBase * (1.0f - size) + 0.5f;
		Color.g = g_texture.Sample(g_sampler, uvG).g;
		
		
		// �\�[�x���t�B���^�[
		float2 uv = float2(input.TexCoord.x * 4, input.TexCoord.y * 2);
		//coord += 0.5f;
		//float2 uv = float2(coord.x * 4, coord.y * 2);
		float4 outline = Sobel(uv, g_depthTexture);
		
		float o = clamp(outline.x, 0, 1);
		o = step(o, 0.1f);
		//o = 1.0f - o;
		float3 c = Color * o;
		if(o < 0.5f)
		{
			Color = float4(0.2f, 0.2f, 0.2f, 1);
		}
		else
		{
		
			Color = float4(c, w);
		}
	}
	
	return Color * g_color;
}
