struct Input
{
    float3 position : TEXCOORD0;
    float2 texCoord : TEXCOORD1;
};

struct Output
{
    float2 TexCoord : TEXCOORD0;
    float4 Position : SV_Position;
};

Output main (Input input)
{
    Output output;
    output.Position = float4(input.position, 1.0f);
    output.TexCoord = input.texCoord;

    return output;
}