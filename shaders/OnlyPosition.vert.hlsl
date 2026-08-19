struct Input
{
    float3 position : TEXCOORD0;
    float3 color : TEXCOORD1;
};

struct Output
{
    float4 Color : TEXCOORD0;
    float4 Position : SV_Position;
};

Output main (Input input)
{
    Output output;
    output.Position = float4(input.position.x, input.position.y, input.position.z, 1.0f);
    output.Color = float4(input.color.r, input.color.g, input.color.b, 1.0f);

    return output;
}