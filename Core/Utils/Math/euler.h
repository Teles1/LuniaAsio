#ifndef LUNIA_MATH_EULER_H
#define LUNIA_MATH_EULER_H

namespace Lunia {
	namespace Math
	{
		struct EulerOrder {
			enum type
			{
				XYZ,
				XZY,
				YXZ,
				YZX,
				ZXY,
				ZYX
			};
		};

		const float pi = 3.14159265358979323846264338327950288419716939937510f;
		const float Pi = 3.14159265358979323846264338327950288419716939937510f;
		const float HalfPi = Pi / 2.0f;

		template <typename T>
		inline T DegToRad(const T& angle)
		{
			return (T)(angle * (pi / 180.0));
		}

		template <typename T>
		inline T RadToDeg(const T& angle)
		{
			return (T)(angle * (180.0 / pi));
		}

	}
}// namespace Lunia, namespace Math 


#endif