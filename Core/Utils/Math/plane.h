#ifndef LUNIA_MATH_PLANE_H
#define LUNIA_MATH_PLANE_H

#include "vector3.h"

namespace Lunia {
	namespace Math
	{

		template <typename T>
		struct Plane
		{

			// members

			Vector3<T>	normal;
			T			dist;

			// constructors

			Plane()
			{
			}

			Plane(const Vector3<T>& n, const T& d)
				: normal(n), dist(d)
			{
			}

			Plane(const Vector3<T>& n, const Vector3<T>& point)
				: normal(n), dist(DotProduct(n, point))
			{
			}

			Plane(const Vector3<T>& v1, const Vector3<T>& v2, const Vector3<T>& v3)
				: normal(Normalize(CrossProduct(v2 - v1, v3 - v1))), dist(DotProduct(v1, normal))
			{
			}

			Plane(const Plane& plane)
				: normal(plane.normal), dist(plane.dist)
			{
			}

			// methods		

			T GetOffset(const Vector3<T>& point) const
			{
				return DotProduct(normal, point) - dist;
			}

			bool IsFront(const Vector3<T>& point) const
			{
				return GetOffset(point) > 0;
			}

			bool IsBack(const Vector3<T>& point) const
			{
				return GetOffset(point) < 0;
			}
		};

	}
}// namespace AllM, namespace Math 


#ifndef LUNIA_MATH_NOTYPENAME

typedef Lunia::Math::Plane<float>	plane3f;
typedef Lunia::Math::Plane<double>	plane3d;

#endif // LUNIA_MATH_NOTYPENAME


#endif