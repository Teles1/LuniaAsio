namespace Lunia {
	namespace Math {
		namespace Color {
			uint32 ClampedFraction8(float value);
			uint32 ToUInt32(const float4& color);
			uint32 ToUInt32(const float3& color);
			float4 ToFloat4(unsigned int color);
			float4 Saturate(float4 value);
		}
	}
}