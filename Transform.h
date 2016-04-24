#pragma once


#include"Matrix.h"

namespace jmxRCore
{
	class Ray;
	class DifferentialRay;
	class BBox;
	const Mat4x4 Identity = { 1.f, 0.f, 0.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f };
	class Transform
	{
	public:
		Transform() { matrix = inv = Identity; }
		Transform(const Mat4x4& mat)
		{
			matrix = mat;
			inv = mat.inverse();
		}
		explicit Transform(f32 m[4][4])
		{
			matrix = Mat4x4(m);
			inv = matrix.inverse();
		}
		Transform(std::initializer_list<f32> il)
		{
			matrix = Mat4x4(il);
			inv = matrix.inverse();
		}

		Transform inverse() const
		{
			return Transform(inv);
		}

		Vec3 operator()(const Vec3& v) const
		{
			Vec3 r;
			r.x = v.x*matrix(0, 0) + v.y*matrix(1, 0) + v.z*matrix(2, 0);
			r.y = v.x*matrix(0, 1) + v.y*matrix(1, 1) + v.z*matrix(2, 1);
			r.z = v.x*matrix(0, 2) + v.y*matrix(1, 2) + v.z*matrix(2, 2);

			return r;
		}
		Point3 operator()(const Point3& p) const
		{
			Point3 r;
			r.x = p.x*matrix(0, 0) + p.y*matrix(1, 0) + p.z*matrix(2, 0) + matrix(3, 0);
			r.y = p.x*matrix(0, 1) + p.y*matrix(1, 1) + p.z*matrix(2, 1) + matrix(3, 1);
			r.z = p.x*matrix(0, 2) + p.y*matrix(1, 2) + p.z*matrix(2, 2) + matrix(3, 2);
			f32 w = p.x*matrix(0, 3) + p.y*matrix(1, 3) + p.z*matrix(2, 3) + matrix(3, 3);
			if (w == 1.f) return r;
			else return r / w;
		}
		Normal operator()(const Normal& n) const 
		{
			Normal r; 
			r.x = inv(0, 0)*n.x + inv(0, 1)*n.y + inv(0, 2)*n.z;
			r.y = inv(1, 0)*n.x + inv(1, 1)*n.y + inv(1, 2)*n.z;
			r.z = inv(2, 0)*n.x + inv(2, 1)*n.y + inv(2, 2)*n.z;
			return r;
		}
		Ray operator()(const Ray& r) const;
		DifferentialRay operator()(const DifferentialRay& r) const;
		BBox operator()(const BBox& bbox) const;


		//static method
		//Ëõ·Å¾ØÕó
		static Mat4x4  Mat4x4Scale(f32 x, f32 y, f32 z);
		static Mat4x4  Mat4x4Scale(const Vec3& v);
		//Æ½ÒÆ¾ØÕó
		static Mat4x4  Mat4x4Translate(f32 x, f32 y, f32 z);
		static Mat4x4  Mat4x4Translate(const Vec3& v);
		//Ðý×ª¾ØÕó£¨»¡¶ÈÖÆ£¬ÈÆÖáÐý×ª£©
		static Mat4x4  Mat4x4Rotate(f32 radian, const Vec3& v);
		static Mat4x4  Mat4x4Rotate(f32 radian, f32 x, f32 y, f32 z);
		static Mat4x4  Mat4x4RotateX(f32 radian);
		static Mat4x4  Mat4x4RotateY(f32 radian);
		static Mat4x4  Mat4x4RotateZ(f32 radian);

		//Ïà»ú¾ØÕó
		static Mat4x4   Mat4x4Camera(const Vec3& look, const Vec3& up,
			const Point3& p);
		
		
	private:
		Mat4x4 matrix;
		Mat4x4 inv;
	};
}