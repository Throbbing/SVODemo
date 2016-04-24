#pragma once


#include"Vector.h"

namespace jmxRCore
{






	class Mat4x4
	{


		



	public:

		//构造函数
		Mat4x4(){ memset(&m[0][0], 0, sizeof(f32) * 16); }
		Mat4x4(f32 m00, f32 m01, f32 m02, f32 m03,
			f32 m10, f32 m11, f32 m12, f32 m13,
			f32 m20, f32 m21, f32 m22, f32 m23,
			f32 m30, f32 m31, f32 m32, f32 m33)
		{
			m[0][0] = m00, m[0][1] = m01, m[0][2] = m02, m[0][3] = m03;
			m[1][0] = m10, m[1][1] = m11, m[1][2] = m12, m[1][3] = m13;
			m[2][0] = m20, m[2][1] = m21, m[2][2] = m22, m[2][3] = m23;
			m[3][0] = m30, m[3][1] = m31, m[3][2] = m32, m[3][3] = m33;

		}
		Mat4x4(std::initializer_list<f32> il)
		{

			Assert(il.size() == 16);
			auto p = il.begin();
			m[0][0] = *(p++), m[0][1] = *(p++), m[0][2] = *(p++), m[0][3] = *(p++);
			m[1][0] = *(p++), m[1][1] = *(p++), m[1][2] = *(p++), m[1][3] = *(p++);
			m[2][0] = *(p++), m[2][1] = *(p++), m[2][2] = *(p++), m[2][3] = *(p++);
			m[3][0] = *(p++), m[3][1] = *(p++), m[3][2] = *(p++), m[3][3] = *(p++);

		}
		Mat4x4(const Vec4& row0,
			const Vec4& row1,
			const Vec4& row2,
			const Vec4& row3)
		{
			memcpy(&m[0][0], &row0, sizeof(f32) * 4);
			memcpy(&m[1][0], &row1, sizeof(f32) * 4);
			memcpy(&m[2][0], &row2, sizeof(f32) * 4);
			memcpy(&m[3][0], &row3, sizeof(f32) * 4);

		}
		explicit Mat4x4(f32 mat[4][4])
		{
			memcpy(m, mat, sizeof(f32) * 16);
		}
		Mat4x4(const Mat4x4& mat)
		{
			memcpy(&m[0][0], &mat.m[0][0], sizeof(f32) * 16);
		}

		Mat4x4& operator=(const Mat4x4& mat)
		{
			memcpy(&m[0][0], &mat.m[0][0], sizeof(f32) * 16);
			return *this;
		}

		FORCEINLINE f32& operator()(u32 x, u32 y)
		{
			Assert(x >= 0 && x < 4);
			Assert(y >= 0 && y < 4);

			return m[x][y];
		}
		FORCEINLINE f32 operator()(u32 x, u32 y) const
		{
			Assert(x >= 0 && x < 4);
			Assert(y >= 0 && y < 4);

			return m[x][y];
		}

		FORCEINLINE f32& operator()(u32 i)
		{
			Assert(i >= 0 && i < 16);

			return m[i / 4][i % 4];
		}
		FORCEINLINE f32 operator()(u32 i) const
		{
			Assert(i >= 0 && i < 16);

			return m[i / 4][i % 4];
		}

		Mat4x4 operator+(const Mat4x4& mat) const
		{
			Mat4x4 r;
			for (u32 i = 0; i < 16; ++i)
				r(i) = m[i / 4][i % 4] + mat(i);

			return r;
		}
		Mat4x4 operator-(const Mat4x4& mat) const
		{
			Mat4x4 r;
			for (u32 i = 0; i < 16; ++i)
				r(i) = m[i / 4][i % 4] - mat(i);

			return r;
		}
		Mat4x4 operator*(const Mat4x4& mat) const
		{
			Mat4x4 r;
			for (u32 i = 0; i < 4; ++i)
			{
				for (u32 j = 0; j < 4; ++j)
				{
					r(i, j) = m[i][0] * mat(0, j) +
						m[i][1] * mat(1, j) +
						m[i][2] * mat(2, j) +
						m[i][3] * mat(3, j);
				}
			}
			return r;
		}
		Mat4x4 transpose()					const
		{
			Mat4x4 r;
			for (u32 i = 0; i < 4; ++i)
			{
				for (u32 j = 0; j < 4; ++j)
				{
					r(j, i) = m[i][j];
				}
			}
			return r;
		}
		Mat4x4 inverse()					const
		{
			int indxc[4], indxr[4];
			int ipiv[4] = { 0, 0, 0, 0 };
			float minv[4][4];
			memcpy(minv, m, 4 * 4 * sizeof(f32));
			for (int i = 0; i < 4; i++) {
				int irow = -1, icol = -1;
				float big = 0.;
				// Choose pivot
				for (int j = 0; j < 4; j++) {
					if (ipiv[j] != 1) {
						for (int k = 0; k < 4; k++) {
							if (ipiv[k] == 0) {
								if (fabsf(minv[j][k]) >= big) {
									big = float(fabsf(minv[j][k]));
									irow = j;
									icol = k;
								}
							}
							else if (ipiv[k] > 1)
								Assert(false);
						}
					}
				}
				++ipiv[icol];
				// Swap rows _irow_ and _icol_ for pivot
				if (irow != icol) {
					for (int k = 0; k < 4; ++k)
						std::swap(minv[irow][k], minv[icol][k]);
				}
				indxr[i] = irow;
				indxc[i] = icol;
				if (minv[icol][icol] == 0.)
					Assert(false);

				// Set $m[icol][icol]$ to one by scaling row _icol_ appropriately
				float pivinv = 1.f / minv[icol][icol];
				minv[icol][icol] = 1.f;
				for (int j = 0; j < 4; j++)
					minv[icol][j] *= pivinv;

				// Subtract this row from others to zero out their columns
				for (int j = 0; j < 4; j++) {
					if (j != icol) {
						float save = minv[j][icol];
						minv[j][icol] = 0;
						for (int k = 0; k < 4; k++)
							minv[j][k] -= minv[icol][k] * save;
					}
				}
			}
			// Swap columns to reflect permutation
			for (int j = 3; j >= 0; j--) {
				if (indxr[j] != indxc[j]) {
					for (int k = 0; k < 4; k++)
						std::swap(minv[k][indxr[j]], minv[k][indxc[j]]);
				}
			}
			return Mat4x4(minv);
		}


		friend Vec4	operator*(const Vec4& v, const Mat4x4& m)
		{
			Vec4 r;
			r.x = v.x*m(0, 0) + v.y*m(1, 0) + v.z*m(2, 0) + v.w*m(3, 0);
			r.y = v.x*m(0, 1) + v.y*m(1, 1) + v.z*m(2, 1) + v.w*m(3, 1);
			r.z = v.x*m(0, 2) + v.y*m(1, 2) + v.z*m(2, 2) + v.w*m(3, 2);
			r.w = v.x*m(0, 3) + v.y*m(1, 3) + v.z*m(2, 3) + v.w*m(3, 3);

			return r;
		}

		

		std::string toString()
		{
			std::ostringstream ss;
			ss << "Matrix: " << std::endl;
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					ss << m[i][j] << " ";
				}
				ss << std::endl;
			}
		}

		//	static Mat4x4						Identity;




		//缩放矩阵
		static Mat4x4  Mat4x4Scale(f32 x, f32 y, f32 z);
		static Mat4x4  Mat4x4Scale(const Vec3& v);
		//平移矩阵
		static Mat4x4  Mat4x4Translate(f32 x, f32 y, f32 z);
		static Mat4x4  Mat4x4Translate(const Vec3& v);
		//旋转矩阵（弧度制，绕轴旋转）
		static Mat4x4  Mat4x4Rotate(f32 radian, const Vec3& v);
		static Mat4x4  Mat4x4Rotate(f32 radian, f32 x, f32 y, f32 z);
		static Mat4x4  Mat4x4RotateX(f32 radian);
		static Mat4x4  Mat4x4RotateY(f32 radian);
		static Mat4x4  Mat4x4RotateZ(f32 radian);

		private:
			//16位字节对齐
			//m[4][4]按照行存储方式存放数据
			f32 m[4][4];
	};



}





