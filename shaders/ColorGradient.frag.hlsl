struct Input
{
    float4 position : SV_Position;
    float4 color : TEXCOORD0;
};

float4 main (Input input) : SV_Target0
{

    return float4(1.0f, 1.0f, 1.0f, 1.0f);
}