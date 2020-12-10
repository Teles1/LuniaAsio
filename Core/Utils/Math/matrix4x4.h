#ifndef ALLM_MATH_MATRIX4X4_H
#define ALLM_MATH_MATRIX4X4_H

#include "minmax.h"
#include <cmath>
#include <limits>
#include <algorithm>
#include "vector3.h"
#include "vector4.h"
#include "euler.h"
#include "quaternion.h"
#include "plane.h"


namespace Lunia {
	namespace Math
	{
		template <typename T>
		struct Quaternion;


		template <typename T>
		struct Matrix4x4
		{
			inline static Matrix4x4<T> CreateIdentity() {
				Matrix4x4<T> tmp;
				tmp.Identity();
				return tmp;
			}
			/*
			The ALLM_MATH matrix convention:

			; offsets
			[ 0  1  2  3]
			[ 4  5  6  7]
			[ 8  9  0 11]
			[12 13 14 15]

			; scaling
			[sx -- -- --] (sx,sy,sz)
			[-- sy -- --]
			[-- -- sz --]
			[-- -- -- --]

			; translation
			[-- -- -- --]
			[-- -- -- --]
			[-- -- -- --]
			[tx ty tz --] (tx,ty,tz)

			; rotation
			[xx xy xz --] x-axis (xx,xy,xz)
			[yx yy yz --] y-axis (yx,yy,yz)
			[zx zy zz --] z-axis (zx,zy,zz)
			[-- -- -- --]

			*/

			// members

			union
			{
				T m16[16];
				T m44[4][4];
			};

			void Zero() {
				for (int i = 0; i < 16; i++) {
					m16[i] = 0;
				}
			}

			const T& operator()(int row, int col) const { return m44[row][col]; }
			T& operator()(int row, int col) { return m44[row][col]; }

			// constructors

			Matrix4x4()
			{
			}
			Matrix4x4(T a, T b, T c, T d,
				T e, T f, T g, T h,
				T i, T j, T k, T l,
				T m, T n, T o, T p
			) {

				m44[0][0] = a; m44[0][1] = b; m44[0][2] = c; m44[0][3] = d;
				m44[1][0] = e; m44[1][1] = f; m44[1][2] = g; m44[1][3] = h;
				m44[2][0] = i; m44[2][1] = j; m44[2][2] = k; m44[2][3] = l;
				m44[3][0] = m; m44[3][1] = n; m44[3][2] = o; m44[3][3] = p;

			}

			Matrix4x4(const T& s)
			{
				//??JUHNU CORRECTED???
				m44[0][0] = s; m44[0][1] = 0; m44[0][2] = 0; m44[0][3] = 0;
				m44[1][0] = 0; m44[1][1] = s; m44[1][2] = 0; m44[1][3] = 0;
				m44[2][0] = 0; m44[2][1] = 0; m44[2][2] = s; m44[2][3] = 0;
				m44[3][0] = 0; m44[3][1] = 0; m44[3][2] = 0; m44[3][3] = s;
			}

			Matrix4x4(const Matrix4x4& u)
			{
				for (int i = 0; i < 4; ++i)
				{
					m44[i][0] = u.m44[i][0];
					m44[i][1] = u.m44[i][1];
					m44[i][2] = u.m44[i][2];
					m44[i][3] = u.m44[i][3];
				}
			}

			Matrix4x4(const Quaternion<T>& q)
			{
				*this = q;
			}

			// operators

			Matrix4x4 operator + (const Matrix4x4& u) const
			{
				Matrix4x4 r;
				for (int i = 0; i < 4; ++i)
				{
					r.m44[i][0] = m44[i][0] + u.m44[i][0];
					r.m44[i][1] = m44[i][1] + u.m44[i][1];
					r.m44[i][2] = m44[i][2] + u.m44[i][2];
					r.m44[i][3] = m44[i][3] + u.m44[i][3];
				}
				return r;
			}

			Matrix4x4 operator - (const Matrix4x4& u) const
			{
				Matrix4x4 r;
				for (int i = 0; i < 4; ++i)
				{
					r.m44[i][0] = m44[i][0] - u.m44[i][0];
					r.m44[i][1] = m44[i][1] - u.m44[i][1];
					r.m44[i][2] = m44[i][2] - u.m44[i][2];
					r.m44[i][3] = m44[i][3] - u.m44[i][3];
				}
				return r;
			}

			Matrix4x4 operator * (const Matrix4x4& u) const
			{
				Matrix4x4 r;
				for (int i = 0; i < 4; ++i)
				{
					const T* v = m16 + i * 4;
					r.m44[i][0] = v[0] * u.m44[0][0] + v[1] * u.m44[1][0] + v[2] * u.m44[2][0] + v[3] * u.m44[3][0];
					r.m44[i][1] = v[0] * u.m44[0][1] + v[1] * u.m44[1][1] + v[2] * u.m44[2][1] + v[3] * u.m44[3][1];
					r.m44[i][2] = v[0] * u.m44[0][2] + v[1] * u.m44[1][2] + v[2] * u.m44[2][2] + v[3] * u.m44[3][2];
					r.m44[i][3] = v[0] * u.m44[0][3] + v[1] * u.m44[1][3] + v[2] * u.m44[2][3] + v[3] * u.m44[3][3];
				}
				return r;
			}

			Matrix4x4& operator += (const Matrix4x4& u)
			{
				for (int i = 0; i < 4; ++i)
				{
					m44[i][0] += u.m44[i][0];
					m44[i][1] += u.m44[i][1];
					m44[i][2] += u.m44[i][2];
					m44[i][3] += u.m44[i][3];
				}
				return *this;
			}

			Matrix4x4& operator -= (const Matrix4x4& u)
			{
				for (int i = 0; i < 4; ++i)
				{
					m44[i][0] -= u.m44[i][0];
					m44[i][1] -= u.m44[i][1];
					m44[i][2] -= u.m44[i][2];
					m44[i][3] -= u.m44[i][3];
				}
				return *this;
			}

			Matrix4x4& operator *= (const Matrix4x4& u)
			{
				for (int i = 0; i < 4; ++i)
				{
					T v[4];
					v[0] = m44[i][0];
					v[1] = m44[i][1];
					v[2] = m44[i][2];
					v[3] = m44[i][3];
					m44[i][0] = v[0] * u.m44[0][0] + v[1] * u.m44[1][0] + v[2] * u.m44[2][0] + v[3] * u.m44[3][0];
					m44[i][1] = v[0] * u.m44[0][1] + v[1] * u.m44[1][1] + v[2] * u.m44[2][1] + v[3] * u.m44[3][1];
					m44[i][2] = v[0] * u.m44[0][2] + v[1] * u.m44[1][2] + v[2] * u.m44[2][2] + v[3] * u.m44[3][2];
					m44[i][3] = v[0] * u.m44[0][3] + v[1] * u.m44[1][3] + v[2] * u.m44[2][3] + v[3] * u.m44[3][3];
				}
				return *this;
			}

			Matrix4x4& operator *= (const T& s)
			{
				for (int i = 0; i < 16; ++i)
				{
					m16[i] *= s;
				}
				return *this;
			}

			void operator = (const Matrix4x4& u)
			{
				memcpy(&m16[0], u.m16, sizeof(T) * 16);
			}

			void operator = (const Quaternion<T>& q)
			{
				T sx = q.x * 2;
				T sy = q.y * 2;
				T sz = q.z * 2;
				T wx = q.w * sx;
				T wy = q.w * sy;
				T wz = q.w * sz;
				T xx = q.x * sx;
				T xy = q.x * sy;
				T xz = q.x * sz;
				T yy = q.y * sy;
				T yz = q.y * sz;
				T zz = q.z * sz;

				m44[0][0] = 1 - yy - zz;
				m44[0][1] = xy + wz;
				m44[0][2] = xz - wy;
				m44[0][3] = 0;
				m44[1][0] = xy - wz;
				m44[1][1] = 1 - xx - zz;
				m44[1][2] = yz + wx;
				m44[1][3] = 0;
				m44[2][0] = xz + wy;
				m44[2][1] = yz - wx;
				m44[2][2] = 1 - xx - yy;
				m44[2][3] = 0;
				m44[3][0] = 0;
				m44[3][1] = 0;
				m44[3][2] = 0;
				m44[3][3] = 1;
			}

			Vector4<T>& operator [] (int index)
			{
				assert(index >= 0 && index <= 3);
				return reinterpret_cast<Vector4<T>*>(this)[index];
			}

			const Vector4<T>& operator [] (int index) const
			{
				assert(index >= 0 && index <= 3);
				return reinterpret_cast<const Vector4<T>*>(this)[index];
			}

			// methods

			void Identity()
			{
				m44[0][0] = 1; m44[0][1] = 0; m44[0][2] = 0; m44[0][3] = 0;
				m44[1][0] = 0; m44[1][1] = 1; m44[1][2] = 0; m44[1][3] = 0;
				m44[2][0] = 0; m44[2][1] = 0; m44[2][2] = 1; m44[2][3] = 0;
				m44[3][0] = 0; m44[3][1] = 0; m44[3][2] = 0; m44[3][3] = 1;
			}

			void Scale(const T& x, const T& y, const T& z)
			{
				m44[0][0] = x; m44[0][1] = 0; m44[0][2] = 0; m44[0][3] = 0;
				m44[1][0] = 0; m44[1][1] = y; m44[1][2] = 0; m44[1][3] = 0;
				m44[2][0] = 0; m44[2][1] = 0; m44[2][2] = z; m44[2][3] = 0;
				m44[3][0] = 0; m44[3][1] = 0; m44[3][2] = 0; m44[3][3] = 1;
			}

			void Scale(const Vector3<T>& scale)
			{
				Scale(scale.x, scale.y, scale.z);
			}

			void Translate(const T& x, const T& y, const T& z)
			{
				m44[0][0] = 1; m44[0][1] = 0; m44[0][2] = 0; m44[0][3] = 0;
				m44[1][0] = 0; m44[1][1] = 1; m44[1][2] = 0; m44[1][3] = 0;
				m44[2][0] = 0; m44[2][1] = 0; m44[2][2] = 1; m44[2][3] = 0;
				m44[3][0] = x; m44[3][1] = y; m44[3][2] = z; m44[3][3] = 1;
			}

			void Translate(const Vector3<T>& translate)
			{
				Translate(translate.x, translate.y, translate.z);
			}

			void RotateX(const T& angle)
			{
				T s = (T)sin(angle);
				T c = (T)cos(angle);

				m44[0][0] = 1; m44[0][1] = 0; m44[0][2] = 0; m44[0][3] = 0;
				m44[1][0] = 0; m44[1][1] = c; m44[1][2] = s; m44[1][3] = 0;
				m44[2][0] = 0; m44[2][1] = -s; m44[2][2] = c; m44[2][3] = 0;
				m44[3][0] = 0; m44[3][1] = 0; m44[3][2] = 0; m44[3][3] = 1;
			}

			void RotateY(const T& angle)
			{
				T s = (T)sin(angle);
				T c = (T)cos(angle);

				m44[0][0] = c; m44[0][1] = 0; m44[0][2] = -s; m44[0][3] = 0;
				m44[1][0] = 0; m44[1][1] = 1; m44[1][2] = 0; m44[1][3] = 0;
				m44[2][0] = s; m44[2][1] = 0; m44[2][2] = c; m44[2][3] = 0;
				m44[3][0] = 0; m44[3][1] = 0; m44[3][2] = 0; m44[3][3] = 1;
			}

			void RotateZ(const T& angle)
			{
				T s = (T)sin(angle);
				T c = (T)cos(angle);

				m44[0][0] = c; m44[0][1] = s; m44[0][2] = 0; m44[0][3] = 0;
				m44[1][0] = -s; m44[1][1] = c; m44[1][2] = 0; m44[1][3] = 0;
				m44[2][0] = 0; m44[2][1] = 0; m44[2][2] = 1; m44[2][3] = 0;
				m44[3][0] = 0; m44[3][1] = 0; m44[3][2] = 0; m44[3][3] = 1;
			}

			void RotateXYZ(const T& yaw, const T& pitch, const T& roll)
			{
				T sy = (T)sin(yaw);
				T cy = (T)cos(yaw);
				T sp = (T)sin(pitch);
				T cp = (T)cos(pitch);
				T sr = (T)sin(roll);
				T cr = (T)cos(roll);
				T spsy = sp * sy;
				T spcy = sp * cy;

				m44[0][0] = cr * cp;
				m44[0][1] = sr * cp;
				m44[0][2] = -sp;
				m44[0][3] = 0;
				m44[1][0] = cr * spsy - sr * cy;
				m44[1][1] = sr * spsy + cr * cy;
				m44[1][2] = cp * sy;
				m44[1][3] = 0;
				m44[2][0] = cr * spcy + sr * sy;
				m44[2][1] = sr * spcy - cr * sy;
				m44[2][2] = cp * cy;
				m44[2][3] = 0;
				m44[3][0] = 0;
				m44[3][1] = 0;
				m44[3][2] = 0;
				m44[3][3] = 1;
			}

			void RotateEuler(const Vector3<T>& v, EulerOrder::type euler)
			{
				switch (euler)
				{
				case EulerOrder::XYZ: RotateX(v.x); MultRotateY(v.y); MultRotateZ(v.z); break;
				case EulerOrder::XZY: RotateX(v.x); MultRotateZ(v.z); MultRotateY(v.y); break;
				case EulerOrder::YXZ: RotateY(v.y); MultRotateX(v.x); MultRotateZ(v.z); break;
				case EulerOrder::YZX: RotateY(v.y); MultRotateZ(v.z); MultRotateX(v.x); break;
				case EulerOrder::ZXY: RotateZ(v.z); MultRotateX(v.x); MultRotateY(v.y); break;
				case EulerOrder::ZYX: RotateZ(v.z); MultRotateY(v.y); MultRotateX(v.x); break;
				default: break;
				}
			}

			void RotateAngleAxis(const T& angle, const Vector3<T>& axis)
			{
				T length2 = LengthSquared(axis);
				if (length2 == 0)
				{
					Identity();
					return;
				}

				Vector3<T> n = axis / (T)sqrt(length2);
				T s = (T)sin(angle);
				T c = (T)cos(angle);
				T k = 1 - c;

				T xx = n.x * n.x * k + c;
				T yy = n.y * n.y * k + c;
				T zz = n.z * n.z * k + c;
				T xy = n.x * n.y * k;
				T yz = n.y * n.z * k;
				T zx = n.z * n.x * k;
				T xs = n.x * s;
				T ys = n.y * s;
				T zs = n.z * s;

				m44[0][0] = xx;
				m44[0][1] = xy + zs;
				m44[0][2] = zx - ys;
				m44[0][3] = 0;
				m44[1][0] = xy - zs;
				m44[1][1] = yy;
				m44[1][2] = yz + xs;
				m44[1][3] = 0;
				m44[2][0] = zx + ys;
				m44[2][1] = yz - xs;
				m44[2][2] = zz;
				m44[2][3] = 0;
				m44[3][0] = 0;
				m44[3][1] = 0;
				m44[3][2] = 0;
				m44[3][3] = 1;
			}

			void MultScale(const T& sx, const T& sy, const T& sz)
			{
				for (int i = 0; i < 4; ++i)
				{
					m44[i][0] *= sx;
					m44[i][1] *= sy;
					m44[i][2] *= sz;
				}
			}

			void MultScale(const Vector3<T>& scale)
			{
				MultScale(scale.x, scale.y, scale.z);
			}

			void MultTranslate(const T& tx, const T& ty, const T& tz)
			{
				for (int i = 0; i < 4; ++i)
				{
					T e = m44[i][3];
					m44[i][0] += e * tx;
					m44[i][1] += e * ty;
					m44[i][2] += e * tz;
				}
			}

			void MultTranslate(const Vector3<T>& translate)
			{
				MultTranslate(translate.x, translate.y, translate.z);
			}

			void MultRotateX(const T& angle)
			{
				T s = (T)sin(angle);
				T c = (T)cos(angle);

				for (int i = 0; i < 4; ++i)
				{
					T y = m44[i][1];
					T z = m44[i][2];
					m44[i][1] = y * c - z * s;
					m44[i][2] = z * c + y * s;
				}
			}

			void MultRotateY(const T& angle)
			{
				T s = (T)sin(angle);
				T c = (T)cos(angle);

				for (int i = 0; i < 4; ++i)
				{
					T x = m44[i][0];
					T z = m44[i][2];
					m44[i][0] = x * c + z * s;
					m44[i][2] = z * c - x * s;
				}
			}

			void MultRotateZ(const T& angle)
			{
				T s = (T)sin(angle);
				T c = (T)cos(angle);

				for (int i = 0; i < 4; ++i)
				{
					T x = m44[i][0];
					T y = m44[i][1];
					m44[i][0] = x * c - y * s;
					m44[i][1] = x * s + y * c;
				}
			}

			void MultRotateXYZ(const T& yaw, const T& pitch, const T& roll)
			{
				T sy = (T)sin(yaw);
				T cy = (T)cos(yaw);
				T sp = (T)sin(pitch);
				T cp = (T)cos(pitch);
				T sr = (T)sin(roll);
				T cr = (T)cos(roll);
				T spsy = sp * sy;
				T spcy = sp * cy;

				T m00 = cr * cp;
				T m01 = sr * cp;
				T m02 = -sp;
				T m10 = cr * spsy - sr * cy;
				T m11 = sr * spsy + cr * cy;
				T m12 = cp * sy;
				T m20 = cr * spcy + sr * sy;
				T m21 = sr * spcy - cr * sy;
				T m22 = cp * cy;

				for (int i = 0; i < 4; ++i)
				{
					T x = m44[i][0];
					T y = m44[i][1];
					T z = m44[i][2];
					m44[i][0] = x * m00 + y * m10 + z * m20;
					m44[i][1] = x * m01 + y * m11 + z * m21;
					m44[i][2] = x * m02 + y * m12 + z * m22;
				}
			}

			void MultRotateEuler(const Vector3<T>& v, EulerOrder::type euler)
			{
				switch (euler)
				{
				case EulerOrder::XYZ: MultRotateX(v.x); MultRotateY(v.y); MultRotateZ(v.z); break;
				case EulerOrder::XZY: MultRotateX(v.x); MultRotateZ(v.z); MultRotateY(v.y); break;
				case EulerOrder::YXZ: MultRotateY(v.y); MultRotateX(v.x); MultRotateZ(v.z); break;
				case EulerOrder::YZX: MultRotateY(v.y); MultRotateZ(v.z); MultRotateX(v.x); break;
				case EulerOrder::ZXY: MultRotateZ(v.z); MultRotateX(v.x); MultRotateY(v.y); break;
				case EulerOrder::ZYX: MultRotateZ(v.z); MultRotateY(v.y); MultRotateX(v.x); break;
				default: break;
				}
			}

			void MultMatrix4x4(const Matrix4x4& u)
			{
				for (int i = 0; i < 4; ++i)
				{
					T v[4];
					v[0] = m44[i][0];
					v[1] = m44[i][1];
					v[2] = m44[i][2];
					v[3] = m44[i][3];
					m44[i][0] = v[0] * u.m44[0][0] + v[1] * u.m44[1][0] + v[2] * u.m44[2][0] + v[3] * u.m44[3][0];
					m44[i][1] = v[0] * u.m44[0][1] + v[1] * u.m44[1][1] + v[2] * u.m44[2][1] + v[3] * u.m44[3][1];
					m44[i][2] = v[0] * u.m44[0][2] + v[1] * u.m44[1][2] + v[2] * u.m44[2][2] + v[3] * u.m44[3][2];
					m44[i][3] = v[0] * u.m44[0][3] + v[1] * u.m44[1][3] + v[2] * u.m44[2][3] + v[3] * u.m44[3][3];
				}
			}

			void MultMatrix3x4(const Matrix4x4& u)
			{
				for (int i = 0; i < 4; ++i)
				{
					T v[3];
					v[0] = m44[i][0];
					v[1] = m44[i][1];
					v[2] = m44[i][2];
					m44[i][0] = v[0] * u.m44[0][0] + v[1] * u.m44[1][0] + v[2] * u.m44[2][0];
					m44[i][1] = v[0] * u.m44[0][1] + v[1] * u.m44[1][1] + v[2] * u.m44[2][1];
					m44[i][2] = v[0] * u.m44[0][2] + v[1] * u.m44[1][2] + v[2] * u.m44[2][2];
				}
				m44[3][0] += u.m44[3][0];
				m44[3][1] += u.m44[3][1];
				m44[3][2] += u.m44[3][2];
			}

			void MultMatrix3x3(const Matrix4x4& u)
			{
				for (int i = 0; i < 4; ++i)
				{
					T v[3];
					v[0] = m44[i][0];
					v[1] = m44[i][1];
					v[2] = m44[i][2];
					m44[i][0] = v[0] * u.m44[0][0] + v[1] * u.m44[1][0] + v[2] * u.m44[2][0];
					m44[i][1] = v[0] * u.m44[0][1] + v[1] * u.m44[1][1] + v[2] * u.m44[2][1];
					m44[i][2] = v[0] * u.m44[0][2] + v[1] * u.m44[1][2] + v[2] * u.m44[2][2];
				}
			}

			void MultInverseMatrix3x3(const Matrix4x4& u)
			{
				T m00 = m44[0][0];
				T m01 = m44[0][1];
				T m02 = m44[0][2];
				T m10 = m44[1][0];
				T m11 = m44[1][1];
				T m12 = m44[1][2];
				T m20 = m44[2][0];
				T m21 = m44[2][1];
				T m22 = m44[2][2];

				for (int i = 0; i < 4; ++i)
				{
					const T* v = u.m16 + i * 4;
					m44[i][0] = v[0] * m00 + v[1] * m10 + v[2] * m20;
					m44[i][1] = v[0] * m01 + v[1] * m11 + v[2] * m21;
					m44[i][2] = v[0] * m02 + v[1] * m12 + v[2] * m22;
				}
			}

			void OrthogonalProjection(float l, float r, float t, float b, float nearp, float farp) {


				float width = r - l;
				float height = (b - t);
				float q = 1 / (farp - nearp);
				Zero();

				m44[0][0] = 2 / width;
				m44[1][1] = -2 / height;
				m44[2][2] = q;
				m44[3][2] = -q * nearp;
				m44[3][3] = 1;

				m44[3][0] = (l + r) / (l - r);
				m44[3][1] = -(t + b) / (t - b);



			}

			void OrthogonalProjectionLH(float width, float height, float nearp, float farp) {
				float q = 1 / (farp - nearp);
				Zero();

				m44[0][0] = 2 / width;
				m44[1][1] = 2 / height;
				m44[2][2] = q;
				m44[3][2] = -q * nearp;
				m44[3][3] = 1;
			}

			void OrthogonalProjectionRH(float width, float height, float nearp, float farp) {

				Zero();

				m44[0][0] = 2 / width;
				m44[1][1] = 2 / height;
				m44[2][2] = 1 / (nearp - farp);
				m44[3][2] = nearp / (nearp - farp);
				m44[3][3] = 1;
			}

			/*void Projection(float front, float back, float right, float left, float top, float bottom) {
					Zero();


				  m44[0][0] = -2*front/(right-left);
				  m44[1][1] = -2*front/(top-bottom);
				  m44[2][2] = (front+back) / (front-back);


				  m44[0][2] = (right+left) / (right-left);
				  m44[1][2] = (top+bottom) / (top-bottom);

				  m44[3][2] = -1;

				  m44[2][3] = (-2*front*back)/(front-back);

			}*/


			void Projection(float nearp, float farp, float aspect) {
				float l = 0;
				float t = 0;
				float w = (2 * nearp) / aspect;
				float h = (2 * nearp) / 1;
				float Q = farp / (farp - nearp);

				Zero();

				m44[0][0] = w - l * Q * nearp;
				m44[1][1] = h - t * Q * nearp;
				m44[2][2] = Q;
				m44[3][2] = -Q * nearp;
				m44[2][3] = 1;

			}

			void Projection(float nearp, float farp, float aspect, float l, float t) {
				float w = (2 * nearp) / aspect;
				float h = (2 * nearp) / 1;
				float Q = farp / (farp - nearp);

				Zero();

				m44[0][0] = w;//+l/**Q*nearp*/;
				m44[1][1] = h;//+t*Q*nearp;

				m44[2][0] = -l * Q * nearp;
				m44[2][1] = -t * Q * nearp;

				m44[2][2] = Q - 0.46;
				m44[3][2] = -Q * nearp;
				m44[2][3] = 1;

			}

			void ProjectionZ(float zoom, float nearp, float farp, float aspect, float l, float t) {
				float w = (2 * nearp) / aspect / zoom;
				float h = (2 * nearp) / 1 / zoom;
				float Q = farp / (farp - nearp);

				Zero();

				m44[0][0] = w;//+l/**Q*nearp*/;
				m44[1][1] = h;//+t*Q*nearp;

				m44[2][0] = -l * Q * nearp;
				m44[2][1] = -t * Q * nearp;

				m44[2][2] = Q - 0.46f;
				m44[3][2] = -Q * nearp;
				m44[2][3] = 1;

			}

			void AxonometricProjection(float x, float y, float nearp, float farp) {
				float Q = 1 / (farp - nearp);

				m44[0][0] = 1 / x * 2;
				m44[0][1] = 0;
				m44[0][2] = 0;
				m44[0][3] = 0;

				m44[1][0] = 0;
				m44[1][1] = 1 / y * 2;
				m44[1][2] = 0;
				m44[1][3] = 0;

				m44[2][0] = 0;
				m44[2][1] = 0;
				m44[2][2] = Q;
				m44[2][3] = 0;

				m44[3][0] = 0;
				m44[3][1] = 0;
				m44[3][2] = -nearp * Q;
				m44[3][3] = 1;
			}


			void AxonometricProjection(float relativeScaleX, float relativeScaleY, float nearp, float farp, float zerop, float xscale, float yscale) {
				float Q = 1 / (farp - nearp);
				//float N=1/nearp;


				m44[0][0] = Q * relativeScaleX;
				m44[0][1] = 0;
				m44[0][2] = 0;
				m44[0][3] = 0;

				m44[1][0] = 0;
				m44[1][1] = Q * relativeScaleY;
				m44[1][2] = 0;
				m44[1][3] = 0;

				m44[2][0] = -1 * xscale * Q;
				m44[2][1] = -1 * yscale * Q;
				m44[2][2] = Q;
				m44[2][3] = 0;

				m44[3][0] = zerop * xscale * Q;
				m44[3][1] = zerop * yscale * Q;
				m44[3][2] = -nearp * Q;
				m44[3][3] = 1;
			}

			/*
					void ProjectionMAD(float zoom,float z,float nearp, float farp, float aspect, float l, float t) {
						float w = (2 * nearp)/aspect/zoom;
						float h = (2 * nearp)/1/zoom;
						float Q = farp/(farp-nearp);

						Zero();

						m44[0][0] = w;//+l/**Q*nearp*/;
						/*			m44[1][1] = h;//+t*Q*nearp;

									m44[2][0] = -l*Q*nearp;
									m44[2][1] = -t*Q*nearp;

									m44[2][2] = Q+-z*Q*nearp;
									m44[3][2] = -Q*nearp;
									m44[2][3] = 1;

								}

						*/

			void Projection(float nearp, float farp, float fovHoriz, float fovVert) {
				float    h, w, Q;

				w = (float)(1.0 / tan(fovHoriz * 0.5f));  // 1/tan(x) == cot(x)
				h = (float)(1.0 / tan(fovVert * 0.5f));   // 1/tan(x) == cot(x)
				Q = farp / (farp - nearp);

				Zero();

				m44[0][0] = w;
				m44[1][1] = h;
				m44[2][2] = Q;
				m44[3][2] = -Q * nearp;
				m44[2][3] = 1;


			}

			void ProjectionFov(float nearp, float farp, float fovVert, float aspect)
			{
				float h = (float)1.0 / tan(fovVert * 0.5f);
				float w = h / aspect;
				float q = farp / (farp - nearp);

				Zero();

				m44[0][0] = w;
				m44[1][1] = h;
				m44[2][2] = q;
				m44[3][2] = -q * nearp;
				m44[2][3] = 1;
			}



			void Transpose()
			{
				std::swap(m44[0][1], m44[1][0]);
				std::swap(m44[0][2], m44[2][0]);
				std::swap(m44[0][3], m44[3][0]);
				std::swap(m44[1][2], m44[2][1]);
				std::swap(m44[1][3], m44[3][1]);
				std::swap(m44[2][3], m44[3][2]);
			}

			void Transpose(const Matrix4x4& u)
			{
				m44[0][0] = u.m44[0][0];
				m44[0][1] = u.m44[1][0];
				m44[0][2] = u.m44[2][0];
				m44[0][3] = u.m44[3][0];
				m44[1][0] = u.m44[0][1];
				m44[1][1] = u.m44[1][1];
				m44[1][2] = u.m44[2][1];
				m44[1][3] = u.m44[3][1];
				m44[2][0] = u.m44[0][2];
				m44[2][1] = u.m44[1][2];
				m44[2][2] = u.m44[2][2];
				m44[2][3] = u.m44[3][2];
				m44[3][0] = u.m44[0][3];
				m44[3][1] = u.m44[1][3];
				m44[3][2] = u.m44[2][3];
				m44[3][3] = u.m44[3][3];
			}

			void Adjoint()
			{
				T v00 = m44[1][1] * m44[2][2] - m44[1][2] * m44[2][1];
				T v01 = m44[2][1] * m44[0][2] - m44[2][2] * m44[0][1];
				T v02 = m44[0][1] * m44[1][2] - m44[0][2] * m44[1][1];
				T v10 = m44[1][2] * m44[2][0] - m44[1][0] * m44[2][2];
				T v11 = m44[2][2] * m44[0][0] - m44[2][0] * m44[0][2];
				T v12 = m44[0][2] * m44[1][0] - m44[0][0] * m44[1][2];
				T v20 = m44[1][0] * m44[2][1] - m44[1][1] * m44[2][0];
				T v21 = m44[2][0] * m44[0][1] - m44[2][1] * m44[0][0];
				T v22 = m44[0][0] * m44[1][1] - m44[0][1] * m44[1][0];
				T v30 = -(m44[0][0] * m44[3][0] + m44[1][0] * m44[3][1] + m44[2][0] * m44[3][2]);
				T v31 = -(m44[0][1] * m44[3][0] + m44[1][1] * m44[3][1] + m44[2][1] * m44[3][2]);
				T v32 = -(m44[0][2] * m44[3][0] + m44[1][2] * m44[3][1] + m44[2][2] * m44[3][2]);
				m44[0][0] = v00;
				m44[0][1] = v01;
				m44[0][2] = v02;
				m44[1][0] = v10;
				m44[1][1] = v11;
				m44[1][2] = v12;
				m44[2][0] = v20;
				m44[2][1] = v21;
				m44[2][2] = v22;
				m44[3][0] = v30;
				m44[3][1] = v31;
				m44[3][2] = v32;
			}

			void Adjoint(const Matrix4x4& u)
			{
				m44[0][0] = u.m44[1][1] * u.m44[2][2] - u.m44[1][2] * u.m44[2][1];
				m44[0][1] = u.m44[2][1] * u.m44[0][2] - u.m44[2][2] * u.m44[0][1];
				m44[0][2] = u.m44[0][1] * u.m44[1][2] - u.m44[0][2] * u.m44[1][1];
				m44[0][3] = u.m44[0][3];
				m44[1][0] = u.m44[1][2] * u.m44[2][0] - u.m44[1][0] * u.m44[2][2];
				m44[1][1] = u.m44[2][2] * u.m44[0][0] - u.m44[2][0] * u.m44[0][2];
				m44[1][2] = u.m44[0][2] * u.m44[1][0] - u.m44[0][0] * u.m44[1][2];
				m44[1][3] = u.m44[1][3];
				m44[2][0] = u.m44[1][0] * u.m44[2][1] - u.m44[1][1] * u.m44[2][0];
				m44[2][1] = u.m44[2][0] * u.m44[0][1] - u.m44[2][1] * u.m44[0][0];
				m44[2][2] = u.m44[0][0] * u.m44[1][1] - u.m44[0][1] * u.m44[1][0];
				m44[2][3] = u.m44[2][3];
				m44[3][0] = -(u.m44[0][0] * u.m44[3][0] + u.m44[1][0] * u.m44[3][1] + u.m44[2][0] * u.m44[3][2]);
				m44[3][1] = -(u.m44[0][1] * u.m44[3][0] + u.m44[1][1] * u.m44[3][1] + u.m44[2][1] * u.m44[3][2]);
				m44[3][2] = -(u.m44[0][2] * u.m44[3][0] + u.m44[1][2] * u.m44[3][1] + u.m44[2][2] * u.m44[3][2]);
				m44[3][3] = u.m44[3][3];
			}

			void Inverse()
			{
				T s = 1 / GetDeterminant();
				T v00 = (m44[1][1] * m44[2][2] - m44[1][2] * m44[2][1]) * s;
				T v01 = (m44[2][1] * m44[0][2] - m44[2][2] * m44[0][1]) * s;
				T v02 = (m44[0][1] * m44[1][2] - m44[0][2] * m44[1][1]) * s;
				T v10 = (m44[1][2] * m44[2][0] - m44[1][0] * m44[2][2]) * s;
				T v11 = (m44[2][2] * m44[0][0] - m44[2][0] * m44[0][2]) * s;
				T v12 = (m44[0][2] * m44[1][0] - m44[0][0] * m44[1][2]) * s;
				T v20 = (m44[1][0] * m44[2][1] - m44[1][1] * m44[2][0]) * s;
				T v21 = (m44[2][0] * m44[0][1] - m44[2][1] * m44[0][0]) * s;
				T v22 = (m44[0][0] * m44[1][1] - m44[0][1] * m44[1][0]) * s;
				T v30 = -(v00 * m44[3][0] + v10 * m44[3][1] + v20 * m44[3][2]);
				T v31 = -(v01 * m44[3][0] + v11 * m44[3][1] + v21 * m44[3][2]);
				T v32 = -(v02 * m44[3][0] + v12 * m44[3][1] + v22 * m44[3][2]);
				m44[0][0] = v00;
				m44[0][1] = v01;
				m44[0][2] = v02;
				m44[1][0] = v10;
				m44[1][1] = v11;
				m44[1][2] = v12;
				m44[2][0] = v20;
				m44[2][1] = v21;
				m44[2][2] = v22;
				m44[3][0] = v30;
				m44[3][1] = v31;
				m44[3][2] = v32;
			}

			void Inverse(const Matrix4x4& u)
			{
				T s = 1 / u.GetDeterminant();
				m44[0][0] = (u.m44[1][1] * u.m44[2][2] - u.m44[1][2] * u.m44[2][1]) * s;
				m44[0][1] = (u.m44[2][1] * u.m44[0][2] - u.m44[2][2] * u.m44[0][1]) * s;
				m44[0][2] = (u.m44[0][1] * u.m44[1][2] - u.m44[0][2] * u.m44[1][1]) * s;
				m44[0][3] = u.m44[0][3];
				m44[1][0] = (u.m44[1][2] * u.m44[2][0] - u.m44[1][0] * u.m44[2][2]) * s;
				m44[1][1] = (u.m44[2][2] * u.m44[0][0] - u.m44[2][0] * u.m44[0][2]) * s;
				m44[1][2] = (u.m44[0][2] * u.m44[1][0] - u.m44[0][0] * u.m44[1][2]) * s;
				m44[1][3] = u.m44[1][3];
				m44[2][0] = (u.m44[1][0] * u.m44[2][1] - u.m44[1][1] * u.m44[2][0]) * s;
				m44[2][1] = (u.m44[2][0] * u.m44[0][1] - u.m44[2][1] * u.m44[0][0]) * s;
				m44[2][2] = (u.m44[0][0] * u.m44[1][1] - u.m44[0][1] * u.m44[1][0]) * s;
				m44[2][3] = u.m44[2][3];
				m44[3][0] = -(m44[0][0] * u.m44[3][0] + m44[1][0] * u.m44[3][1] + m44[2][0] * u.m44[3][2]);
				m44[3][1] = -(m44[0][1] * u.m44[3][0] + m44[1][1] * u.m44[3][1] + m44[2][1] * u.m44[3][2]);
				m44[3][2] = -(m44[0][2] * u.m44[3][0] + m44[1][2] * u.m44[3][1] + m44[2][2] * u.m44[3][2]);
				m44[3][3] = u.m44[3][3];
			}

			void Inverse(const Quaternion<T>& q)
			{
				// quaternion inverse

				T n = -1 / q.Norm();
				T qx = q.x * n;
				T qy = q.y * n;
				T qz = q.z * n;
				T qw = q.w * -n;

				// quaternion to matrix

				T sx = qx * 2;
				T sy = qy * 2;
				T sz = qz * 2;
				T wx = qw * sx;
				T wy = qw * sy;
				T wz = qw * sz;
				T xx = qx * sx;
				T xy = qx * sy;
				T xz = qx * sz;
				T yy = qy * sy;
				T yz = qy * sz;
				T zz = qz * sz;

				m44[0][0] = 1 - yy - zz;
				m44[0][1] = xy + wz;
				m44[0][2] = xz - wy;
				m44[0][3] = 0;
				m44[1][0] = xy - wz;
				m44[1][1] = 1 - xx - zz;
				m44[1][2] = yz + wx;
				m44[1][3] = 0;
				m44[2][0] = xz + wy;
				m44[2][1] = yz - wx;
				m44[2][2] = 1 - xx - yy;
				m44[2][3] = 0;
				m44[3][0] = 0;
				m44[3][1] = 0;
				m44[3][2] = 0;
				m44[3][3] = 1;
			}

			void OrthoNormalize()
			{
				Vector3<T> x = GetX();
				Vector3<T> y = GetY();
				Vector3<T> z = GetZ();

				x = Normalize(x);
				y -= x * DotProduct(x, y);
				y = Normalize(y);
				z = CrossProduct(x, y);

				SetX(x);
				SetY(y);
				SetZ(z);
			}

			void Mirror(const Matrix4x4& transform, const Plane<T>& plane)
			{
				// components
				Vector3<T> x = transform.GetX();
				Vector3<T> y = transform.GetY();
				Vector3<T> z = transform.GetZ();
				Vector3<T> t = transform.GetT();
				Vector3<T> n2 = plane.normal * -2;

				// mirror translation
				Vector3<T> mt = t + n2 * (DotProduct(t, plane.normal) - plane.dist);

				// mirror x rotation
				x += t;
				x += n2 * (DotProduct(x, plane.normal) - plane.dist);
				x -= mt;

				// mirror y rotation
				y += t;
				y += n2 * (DotProduct(y, plane.normal) - plane.dist);
				y -= mt;

				// mirror z rotation
				z += t;
				z += n2 * (DotProduct(z, plane.normal) - plane.dist);
				z -= mt;

				// write result
				SetX(x);
				SetY(y);
				SetZ(z);
				SetT(mt);
				m44[0][3] = 0;
				m44[1][3] = 0;
				m44[2][3] = 0;
				m44[3][3] = 1;
			}

			void Direction(const Vector3<T>& position, const Vector3<T>& direction, const Vector3<T>& up) {
				Vector3<T> z = Normalize(direction);
				Vector3<T> x = Normalize(CrossProduct(up, z));
				Vector3<T> y = CrossProduct(z, x);

				SetX(x);
				SetY(y);
				SetZ(z);
				SetT(position);
				m44[0][3] = 0;
				m44[1][3] = 0;
				m44[2][3] = 0;
				m44[3][3] = 1;
			}

			void DirectionSafe(const Vector3<T>& position, const Vector3<T>& direction, Vector3<T> up) {
				Vector3<T> z = Normalize(direction);

				if (fabs(DotProduct(z, up)) > T(0.99999))
				{
					up = float3(-up.z, up.x, up.y);
				}

				Vector3<T> x = Normalize(CrossProduct(up, z));
				Vector3<T> y = CrossProduct(z, x);

				SetX(x);
				SetY(y);
				SetZ(z);
				SetT(position);
				m44[0][3] = 0;
				m44[1][3] = 0;
				m44[2][3] = 0;
				m44[3][3] = 1;
			}

			void LookAt(const Vector3<T>& target, const Vector3<T>& view, const Vector3<T>& up)
			{
				Vector3<T> z = Normalize(target - view);
				Vector3<T> x = Normalize(CrossProduct(up, z));
				Vector3<T> y = CrossProduct(z, x);

				m44[0][0] = x.x;
				m44[1][0] = x.y;
				m44[2][0] = x.z;
				m44[3][0] = -DotProduct(x, view);
				m44[0][1] = y.x;
				m44[1][1] = y.y;
				m44[2][1] = y.z;
				m44[3][1] = -DotProduct(y, view);
				m44[0][2] = z.x;
				m44[1][2] = z.y;
				m44[2][2] = z.z;
				m44[3][2] = -DotProduct(z, view);
				m44[0][3] = 0;
				m44[1][3] = 0;
				m44[2][3] = 0;
				m44[3][3] = 1;
			}

			T GetDeterminant() const
			{
				return
					m44[0][0] * m44[1][1] * m44[2][2] +
					m44[0][1] * m44[1][2] * m44[2][0] +
					m44[0][2] * m44[1][0] * m44[2][1] -
					m44[0][2] * m44[1][1] * m44[2][0] -
					m44[0][1] * m44[1][0] * m44[2][2] -
					m44[0][0] * m44[1][2] * m44[2][1];
			}

			Vector3<T> GetEuler(EulerOrder::type euler = EulerOrder::XYZ) const
			{
				// =========================================================================
				// original implementation from the Graphics Gems IV, Academic Press, 1994.
				// =========================================================================
#define EulSafe "\000\001\002\000"
#define EulNext "\001\002\000\001"
#define EulGetOrd(ord,i,j,k,n,s,f) {unsigned o=ord;f=o&1;s=o&1; n=o&1;i=EulSafe[o&3];j=EulNext[i+n];k=EulNext[i+1-n];}
#define EulOrd(i,p,r,f) (((((((i)<<1)+(p))<<1)+(r))<<1)+(f))

#define EulOrdXYZs EulOrd(0,0,0,0)
#define EulOrdXYXs EulOrd(0,0,1,0)
#define EulOrdXZYs EulOrd(0,1,0,0)
#define EulOrdXZXs EulOrd(0,1,1,0)
#define EulOrdYZXs EulOrd(1,0,0,0)
#define EulOrdYZYs EulOrd(1,0,1,0)
#define EulOrdYXZs EulOrd(1,1,0,0)
#define EulOrdYXYs EulOrd(1,1,1,0)
#define EulOrdZXYs EulOrd(2,0,0,0)
#define EulOrdZXZs EulOrd(2,0,1,0)
#define EulOrdZYXs EulOrd(2,1,0,0)
#define EulOrdZYZs EulOrd(2,1,1,0)

#define EulOrdZYXr EulOrd(0,0,0,1)
#define EulOrdXYXr EulOrd(0,0,1,1)
#define EulOrdYZXr EulOrd(0,1,0,1)
#define EulOrdXZXr EulOrd(0,1,1,1)
#define EulOrdXZYr EulOrd(1,0,0,1)
#define EulOrdYZYr EulOrd(1,0,1,1)
#define EulOrdZXYr EulOrd(1,1,0,1)
#define EulOrdYXYr EulOrd(1,1,1,1)
#define EulOrdYXZr EulOrd(2,0,0,1)
#define EulOrdZXZr EulOrd(2,0,1,1)
#define EulOrdXYZr EulOrd(2,1,0,1)
#define EulOrdZYZr EulOrd(2,1,1,1)

				int order = 0;
				switch (euler)
				{
				case EulerOrder::XYZ: order = EulOrdXYZs; break;
				case EulerOrder::XZY: order = EulOrdXZYs; break;
				case EulerOrder::YXZ: order = EulOrdYXZs; break;
				case EulerOrder::YZX: order = EulOrdYZXs; break;
				case EulerOrder::ZXY: order = EulOrdZXYs; break;
				case EulerOrder::ZYX: order = EulOrdZYXs; break;
				}

				int i, j, k, n, s, f;
				EulGetOrd(order, i, j, k, n, s, f);

				T x, y, z;

				if (s == 1)
				{
					T sy = (T)sqrt(m44[j][i] * m44[j][i] + m44[k][i] * m44[k][i]);
					if (sy > std::numeric_limits<T>::epsilon() * 16)
					{
						x = (T)atan2(m44[j][i], m44[k][i]);
						y = (T)atan2(sy, m44[i][i]);
						z = (T)atan2(m44[i][j], -m44[i][k]);
					}
					else
					{
						x = (T)atan2(-m44[k][j], m44[j][j]);
						y = (T)atan2(sy, m44[i][i]);
						z = 0;
					}
				}
				else
				{
					T cy = (T)sqrt(m44[i][i] * m44[i][i] + m44[i][j] * m44[i][j]);
					if (cy > std::numeric_limits<T>::epsilon() * 16)
					{
						x = (T)atan2(m44[j][k], m44[k][k]);
						y = (T)atan2(-m44[i][k], cy);
						z = (T)atan2(m44[i][j], m44[i][i]);
					}
					else
					{
						x = (T)atan2(-m44[k][j], m44[j][j]);
						y = (T)atan2(-m44[i][k], cy);
						z = 0;
					}
				}

				if (n == 1)
				{
					x = -x;
					y = -y;
					z = -z;
				}

				return f == 1 ?
					Vector3<T>(z, y, x) : Vector3<T>(x, y, z);
			}

			void SetX(const T& xx, const T& xy, const T& xz)
			{
				m44[0][0] = xx;
				m44[0][1] = xy;
				m44[0][2] = xz;
			}

			void SetY(const T& yx, const T& yy, const T& yz)
			{
				m44[1][0] = yx;
				m44[1][1] = yy;
				m44[1][2] = yz;
			}

			void SetZ(const T& zx, const T& zy, const T& zz)
			{
				m44[2][0] = zx;
				m44[2][1] = zy;
				m44[2][2] = zz;
			}

			void SetT(const T& tx, const T& ty, const T& tz)
			{
				m44[3][0] = tx;
				m44[3][1] = ty;
				m44[3][2] = tz;
			}

			void SetX(const Vector3<T>& x)
			{
				m44[0][0] = x.x;
				m44[0][1] = x.y;
				m44[0][2] = x.z;
			}

			void SetY(const Vector3<T>& y)
			{
				m44[1][0] = y.x;
				m44[1][1] = y.y;
				m44[1][2] = y.z;
			}

			void SetZ(const Vector3<T>& z)
			{
				m44[2][0] = z.x;
				m44[2][1] = z.y;
				m44[2][2] = z.z;
			}

			void SetT(const Vector3<T>& t)
			{
				m44[3][0] = t.x;
				m44[3][1] = t.y;
				m44[3][2] = t.z;
			}

			template <typename T>
			inline Vector3<T> PreMulVectorNoTranslation(const Vector3<T>& v) const
			{
				return Vector3<T>(
					v.x * m44[0][0] + v.y * m44[1][0] + v.z * m44[2][0],
					v.x * m44[0][1] + v.y * m44[1][1] + v.z * m44[2][1],
					v.x * m44[0][2] + v.y * m44[1][2] + v.z * m44[2][2]);
			}

			Vector3<T>& GetX()
			{
				return *reinterpret_cast<Vector3<T>*>(m16 + 0);
			}

			Vector3<T>& GetY()
			{
				return *reinterpret_cast<Vector3<T>*>(m16 + 4);
			}

			Vector3<T>& GetZ()
			{
				return *reinterpret_cast<Vector3<T>*>(m16 + 8);
			}

			Vector3<T>& GetT()
			{
				return *reinterpret_cast<Vector3<T>*>(m16 + 12);
			}

			const Vector3<T>& GetX() const
			{
				return *reinterpret_cast<const Vector3<T>*>(m16 + 0);
			}

			const Vector3<T>& GetY() const
			{
				return *reinterpret_cast<const Vector3<T>*>(m16 + 4);
			}

			const Vector3<T>& GetZ() const
			{
				return *reinterpret_cast<const Vector3<T>*>(m16 + 8);
			}

			const Vector3<T>& GetT() const
			{
				return *reinterpret_cast<const Vector3<T>*>(m16 + 12);
			}
		};

		// inline operators

		template <typename T>
		inline void operator *= (Vector3<T>& v, const Matrix4x4<T>& m)
		{
			v = Vector3<T>(
				v.x * m.m44[0][0] + v.y * m.m44[1][0] + v.z * m.m44[2][0] + m.m44[3][0],
				v.x * m.m44[0][1] + v.y * m.m44[1][1] + v.z * m.m44[2][1] + m.m44[3][1],
				v.x * m.m44[0][2] + v.y * m.m44[1][2] + v.z * m.m44[2][2] + m.m44[3][2]);
		}

		template <typename T>
		inline void operator *= (Vector4<T>& v, const Matrix4x4<T>& m)
		{
			v = Vector4<T>(
				v.x * m.m44[0][0] + v.y * m.m44[1][0] + v.z * m.m44[2][0] + v.w * m.m44[3][0],
				v.x * m.m44[0][1] + v.y * m.m44[1][1] + v.z * m.m44[2][1] + v.w * m.m44[3][1],
				v.x * m.m44[0][2] + v.y * m.m44[1][2] + v.z * m.m44[2][2] + v.w * m.m44[3][2],
				v.x * m.m44[0][3] + v.y * m.m44[1][3] + v.z * m.m44[2][3] + v.w * m.m44[3][3]);
		}

		template <typename T>
		inline Vector3<T> operator * (const Vector3<T>& v, const Matrix4x4<T>& m)
		{
			return Vector3<T>(
				v.x * m.m44[0][0] + v.y * m.m44[1][0] + v.z * m.m44[2][0] + m.m44[3][0],
				v.x * m.m44[0][1] + v.y * m.m44[1][1] + v.z * m.m44[2][1] + m.m44[3][1],
				v.x * m.m44[0][2] + v.y * m.m44[1][2] + v.z * m.m44[2][2] + m.m44[3][2]);
		}

		template <typename T>
		inline Vector4<T> operator * (const Vector4<T>& v, const Matrix4x4<T>& m)
		{
			return Vector4<T>(
				v.x * m.m44[0][0] + v.y * m.m44[1][0] + v.z * m.m44[2][0] + v.w * m.m44[3][0],
				v.x * m.m44[0][1] + v.y * m.m44[1][1] + v.z * m.m44[2][1] + v.w * m.m44[3][1],
				v.x * m.m44[0][2] + v.y * m.m44[1][2] + v.z * m.m44[2][2] + v.w * m.m44[3][2],
				v.x * m.m44[0][3] + v.y * m.m44[1][3] + v.z * m.m44[2][3] + v.w * m.m44[3][3]);
		}

	}
}// namespace Lunia, namespace Math 


#ifndef LUNIA_MATH_NOTYPENAME

namespace Lunia {
	typedef Lunia::Math::Matrix4x4<float>	float4x4;
	typedef Lunia::Math::Matrix4x4<float>	matrix4x4f;
	typedef Lunia::Math::Matrix4x4<float>	matrix4f;
	typedef Lunia::Math::Matrix4x4<double>	matrix4d;
	typedef Lunia::Math::Matrix4x4<double>	matrix4x4d;
	typedef Lunia::Math::Matrix4x4<double>	double4x4;
};
#endif // ALLM_MATH_NOTYPENAME


#endif