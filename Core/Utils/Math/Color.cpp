#pragma once

#include "Color.h"

namespace Lunia {
	namespace Math {
		namespace Color {

			uint32 ToUInt32f4(const float4& color) {
				uint32 r = uint32(color[0] * 255.0f);
				uint32 g = uint32(color[1] * 255.0f);
				uint32 b = uint32(color[2] * 255.0f);
				uint32 a = uint32(color[3] * 255.0f);
				return (a << 24) | (r << 16) | (g << 8) | (b);

				/*const uint32 r = ClampedFraction8(color[0]) << 1;
				const uint32 g = ClampedFraction8(color[1]) >> 7;
				const uint32 b = ClampedFraction8(color[2]) >> 15;
				const uint32 a = ClampedFraction8(color[3]) << 9;

				return a | r | g | b ;*/

			}

			uint32 ToUInt32f3(const float3& color) {
				/*const uint32 r = ClampedFraction8(color[0]) << 1;
				const uint32 g = ClampedFraction8(color[1]) >> 7;
				const uint32 b = ClampedFraction8(color[2]) >> 15;

				return  r | g | b ;*/

				uint32 r = uint32(color[0] * 255.0f);
				uint32 g = uint32(color[1] * 255.0f);
				uint32 b = uint32(color[2] * 255.0f);

				return (r << 16) | (g << 8) | (b);
			}

			float4 ToFloat4(unsigned int color) {

				float a = float((color & 0xff000000) >> 24) / 255.0f;
				float r = float((color & 0x00ff0000) >> 16) / 255.0f;
				float g = float((color & 0x0000ff00) >> 8) / 255.0f;
				float b = float((color & 0x000000ff)) / 255.0f;

				return float4(r, g, b, a);
			}

			float4 Saturate(float4 value) {
				if (value[0] < 0) value[0] = 0;
				if (value[1] < 0) value[1] = 0;
				if (value[2] < 0) value[2] = 0;
				if (value[3] < 0) value[3] = 0;

				if (value[0] > 1.0f) value[0] = 1.0f;
				if (value[1] > 1.0f) value[1] = 1.0f;
				if (value[2] > 1.0f) value[2] = 1.0f;
				if (value[3] > 1.0f) value[3] = 1.0f;

				return value;
			}

		}
	}
}

