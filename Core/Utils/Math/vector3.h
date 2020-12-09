#ifndef Lunia_MATH_VECTOR3_H
#define Lunia_MATH_VECTOR3_H

#include <cassert>
#include <cmath>
#include "basevector.h"
namespace Lunia {
	namespace Math
	{

		template <typename T>
		struct Vector3 : BaseVector<T, 3>
		{

			// members

			T x;;
			T y;
			T z;;

			// constructors

			Vector3() : x(0), y(0), z(0)
			{
			}

			Vector3(const T& vx, const T& vy, const T& vz)
				: x(vx), y(vy), z(vz)
			{
			}

			Vector3(const T v[])
				: x(v[0]), y(v[1]), z(v[2])
			{
			}

			Vector3(const Vector3& v)
				: x(v.x), y(v.y), z(v.z)
			{
			}

			/*Vector3(const T& v)
			: x(v),y(v),z(v)
			{
			}*/

			// operators

			Vector3 operator + () const
			{
				return *this;
			}

			Vector3 operator - () const
			{
				return Vector3(-x, -y, -z);
			}

			Vector3 operator + (const Vector3& v) const
			{
				return Vector3(x + v.x, y + v.y, z + v.z);
			}

			Vector3 operator - (const Vector3& v) const
			{
				return Vector3(x - v.x, y - v.y, z - v.z);
			}

			Vector3 operator * (const Vector3& v) const
			{
				return Vector3(x * v.x, y * v.y, z * v.z);
			}

			Vector3 operator * (const T& s) const
			{
				return Vector3(x * s, y * s, z * s);
			}

			Vector3 operator / (const T& s) const
			{
				assert(s != 0);
				T is = 1 / s;
				return Vector3(x * is, y * is, z * is);
			}

			Vector3& operator += (const Vector3& v)
			{
				x += v.x;
				y += v.y;
				z += v.z;
				return *this;
			}

			Vector3& operator -= (const Vector3& v)
			{
				x -= v.x;
				y -= v.y;
				z -= v.z;
				return *this;
			}

			Vector3& operator *= (const Vector3& v)
			{
				x *= v.x;
				y *= v.y;
				z *= v.z;
				return *this;
			}

			Vector3& operator *= (const T& s)
			{
				x *= s;
				y *= s;
				z *= s;
				return *this;
			}

			Vector3& operator /= (const T& s)
			{
				assert(s != 0);
				T is = 1 / s;
				x *= is;
				y *= is;
				z *= is;
				return *this;
			}

			Vector3& operator = (const Vector3& v)
			{
				x = v.x;
				y = v.y;
				z = v.z;
				return *this;
			}

			T Normalize()
			{
				T s = x * x + y * y + z * z;
				T squareroot = (T)sqrt(s);
				if (s != 0)
				{
					s = 1 / squareroot;
					x *= s;
					y *= s;
					z *= s;
				}
				return squareroot;
			}

			void SetVector(const T& vx, const T& vy, const T& vz)
			{
				x = vx;
				y = vy;
				z = vz;
			}

			void SetVector(const T& v)
			{
				x = v;
				y = v;
				z = v;
			}
		};

		// inline functions

		template <typename T>
		inline T LengthSquared(const Vector3<T>& v)
		{
			return v.x * v.x + v.y * v.y + v.z * v.z;
		}

		template <typename T>
		inline T Length(const Vector3<T>& v)
		{
			return (T)sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
		}

		template <typename T>
		inline T DotProduct(const Vector3<T>& a, const Vector3<T>& b)
		{
			return a.x * b.x + a.y * b.y + a.z * b.z;
		}

		template <typename T>
		inline Vector3<T> CrossProduct(const Vector3<T>& a, const Vector3<T>& b)
		{
			return Vector3<T>(
				(a.y * b.z) - (a.z * b.y),
				(a.z * b.x) - (a.x * b.z),
				(a.x * b.y) - (a.y * b.x));
		}

		template <typename T>
		inline Vector3<T> Normalize(const Vector3<T>& v)
		{
			T s = v.x * v.x + v.y * v.y + v.z * v.z;
			if (s == 0)
				return v;

			s = 1 / (T)sqrt(s);
			return Vector3<T>(v.x * s, v.y * s, v.z * s);
		}

		template <typename T>
		inline Vector3<T> Reflect(const Vector3<T>& v, const Vector3<T>& normal)
		{
			T s = DotProduct(v, normal) * 2;
			return Vector3<T>(
				v.x + normal.x * s,
				v.y + normal.y * s,
				v.z + normal.z * s);
		}

		template <typename T>
		inline Vector3<T> Lerp(const Vector3<T>& a, const Vector3<T>& b, const T& time)
		{
			return Vector3<T>(
				a.x + (b.x - a.x) * time,
				a.y + (b.y - a.y) * time,
				a.z + (b.z - a.z) * time);
		}

		template <typename T>
		inline Vector3<T> operator * (const T& s, const Vector3<T>& v)
		{
			return Vector3<T>(s * v.x, s * v.y, s * v.z);
		}

	}

}

//#include "vector3float.h"

#ifndef Lunia_MATH_NOTYPENAME

	typedef Lunia::Math::Vector3<float>	vec3f;
	typedef Lunia::Math::Vector3<float>	point3f;
	typedef Lunia::Math::Vector3<float>	color3f;
	typedef Lunia::Math::Vector3<double> vec3d;
	typedef Lunia::Math::Vector3<double> point3d;
	typedef Lunia::Math::Vector3<double> color3d;

	typedef Lunia::Math::Vector3<float>	float3;
	typedef Lunia::Math::Vector3<double> double3;

#endif // ALLM_MATH_NOTYPENAME


#endif