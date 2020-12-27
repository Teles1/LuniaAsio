#ifndef MinMax_HeaderGuard
#define MinMax_HeaderGuard
#pragma once
namespace Lunia {
	namespace Math {
		template <typename T>
		inline T Minimum(const T& a, const T& b)
		{
			return a < b ? a : b;
		}

		template <typename T>
		inline T Maximum(const T& a, const T& b)
		{
			return a > b ? a : b;
		}

		template<>
		inline float4 Maximum(const float4& a, const float4& b) {
			float4 result;
			result[0] = Maximum(a[0], b[0]);
			result[1] = Maximum(a[1], b[1]);
			result[2] = Maximum(a[2], b[2]);
			result[3] = Maximum(a[3], b[3]);
			return result;
		}

		template<>
		inline float3 Maximum(const float3& a, const float3& b) {
			float3 result;
			result[0] = Maximum(a[0], b[0]);
			result[1] = Maximum(a[1], b[1]);
			result[2] = Maximum(a[2], b[2]);
			return result;
		}

		template<>
		inline float2 Maximum(const float2& a, const float2& b) {
			float2 result;
			result[0] = Maximum(a[0], b[0]);
			result[1] = Maximum(a[1], b[1]);
			return result;
		}

		template<>
		inline float4 Minimum(const float4& a, const float4& b) {
			float4 result;
			result[0] = Minimum(a[0], b[0]);
			result[1] = Minimum(a[1], b[1]);
			result[2] = Minimum(a[2], b[2]);
			result[3] = Minimum(a[3], b[3]);
			return result;
		}

		template<>
		inline float3 Minimum(const float3& a, const float3& b) {
			float3 result;
			result[0] = Minimum(a[0], b[0]);
			result[1] = Minimum(a[1], b[1]);
			result[2] = Minimum(a[2], b[2]);
			return result;
		}

		template<>
		inline float2 Minimum(const float2& a, const float2& b) {
			float2 result;
			result[0] = Minimum(a[0], b[0]);
			result[1] = Minimum(a[1], b[1]);
			return result;
		}
	}
}
#endif