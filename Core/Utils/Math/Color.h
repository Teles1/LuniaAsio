#ifndef Color_H
#define Color_H

namespace Lunia {
	namespace Math {
		namespace Color {
			//uint32 ClampedFraction8(float value);
			uint32 ToUInt32f4(const float4& color);
			uint32 ToUInt32f3(const float3& color);
			float4 ToFloat4(unsigned int color);
			float4 Saturate(float4 value);
		}
	}
}
#endif // !Color_H