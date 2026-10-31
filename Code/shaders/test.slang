[shader("vertex")]
float4 vertMain(uint id : SV_VertexID) : SV_Position
{
	float2 pos[3] = {
		float2(0, -0.5),
		float2(0.5, 0.5),
		float2(-0.5, 0.5)
	};
	return float4(pos[id], 0, 1);
}

[shader("fragment")]
float4 fragMain() : SV_Target
{
	return float4(1, 0, 0, 1);
}