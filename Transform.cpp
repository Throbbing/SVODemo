#include"Transform.h"
#include"Geometry.h"

using namespace jmxRCore;



Ray Transform::operator()(const Ray& r) const
{
	Ray ray;
	ray.ori = (*this)(r.ori);
	ray.dir = (*this)(r.dir);
	ray.depth = r.depth; 
	ray.time = r.time;
	
	return ray;
}


DifferentialRay Transform::operator()(const DifferentialRay& r) const
{
	DifferentialRay ray;
	ray.ori = (*this)(r.ori);
	ray.dir = (*this)(r.dir);
	if (r.hasDifferentials)
	{
		ray.rxOri = (*this)(r.rxOri);
		ray.rxDir = (*this)(r.rxDir);
		ray.ryOri = (*this)(r.ryOri);
		ray.ryDir = (*this)(r.ryDir);
	}
	ray.depth = r.depth;
	ray.time = r.time;
	ray.hasDifferentials = r.hasDifferentials;

	return ray;
}

BBox Transform::operator()(const BBox& box) const
{
	Point minPP = { INFINITY, INFINITY, INFINITY };
	Point maxPP = -minPP;

	f32 x0 = box.minP.x, x1 = box.maxP.x;
	f32 y0 = box.minP.y, y1 = box.maxP.y;
	f32 z0 = box.minP.z, z1 = box.maxP.z;

	Point p[8] = { { x0, y0, z0},
	{ x1, y0, z0 },
	{ x0, y1, z0 },
	{ x0, y0, z1 },
	{ x1, y1, z0 },
	{ x0, y1, z1 },
	{ x1, y0, z1 },
	{ x1, y1, z1 }
	};
	Point pt;
	for (u32 i = 0; i < 8; ++i)
	{
		pt =(*this)(p[i]);
		if (pt.x < minPP.x) minPP.x = pt.x;
		if (pt.y < minPP.y) minPP.y = pt.y;
		if (pt.z < minPP.z) minPP.z = pt.z;

		if (pt.x > maxPP.x) maxPP.x = pt.x;
		if (pt.y > maxPP.y) maxPP.y = pt.y;
		if (pt.z > maxPP.z) maxPP.z = pt.z;
	}

	return BBox(minPP, maxPP);
}

Mat4x4 Transform::Mat4x4Scale(f32 x, f32 y, f32 z)
{
	return Mat4x4(x, 0.f, 0.f, 0.f,
		0.f, y, 0.f, 0.f,
		0.f, 0.f, z, 0.f,
		0.f, 0.f, 0.f, 1.f);
}
Mat4x4 Transform::Mat4x4Scale(const Vec3& v)
{
	return Mat4x4Scale(v.x, v.y, v.z); 
}

Mat4x4 Transform::Mat4x4Translate(f32 x, f32 y, f32 z)
{
	return Mat4x4(1.f, 0.f, 0.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		x, y, z, 1.f);
}
Mat4x4 Transform::Mat4x4Translate(const Vec3& v)
{
	return Mat4x4Translate(v.x, v.y, v.z);
}

Mat4x4 Transform::Mat4x4Rotate(f32 radian, f32 x,f32 y,f32 z)
{
	f32 cosa = std::cosf(radian);
	f32 onesubcosa = 1.f - cosa;
	f32 sina = std::sinf(radian);

	return Mat4x4(cosa + onesubcosa*x*x, onesubcosa*x*y + sina*z, onesubcosa*y*z + sina*x, 0.f,
		onesubcosa*x*y - sina*z, cosa + onesubcosa*y*y, onesubcosa*y*z + sina*x, 0.f,
		onesubcosa*x*z + sina*y, onesubcosa*y*z - sina*x, cosa + onesubcosa*z*z, 0.f,
		0.f, 0.f, 0.f, 1.f);
}

Mat4x4 Transform::Mat4x4Rotate(f32 radian, const Vec3& v)
{
	return Mat4x4Rotate(radian, v.x, v.y, v.z);
}

Mat4x4 Transform::Mat4x4RotateX(f32 radian)
{
	f32 cosa = std::cosf(radian);
	f32 sina = std::sinf(radian);
	
	return Mat4x4(1.f, 0.f, 0.f, 0.f,
		0.f, cosa, sina, 0.f,
		0.f, -sina, cosa, 0.f,
		0.f, 0.f, 0.f, 1.f);
}

Mat4x4 Transform::Mat4x4RotateY(f32 radian)
{
	f32 cosa = std::cosf(radian);
	f32 sina = std::sinf(radian);

	return Mat4x4(cosa, 0.f, -sina, 0.f,
		0.f, 1.f, 0.f, 0.f,
		sina, 0.f, cosa, 0.f,
		0.f, 0.f, 0.f, 1.f);
}

Mat4x4 Transform::Mat4x4RotateZ(f32 radian)
{
	f32 cosa = std::cosf(radian);
	f32 sina = std::sinf(radian);

	return Mat4x4(cosa, sina, 0.f, 0.f,
		-sina, cosa, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f);
}

Mat4x4 Transform::Mat4x4Camera(const Vec3& look, const Vec3& up, const Point3& p)
{
	Vec3 right = cross(up, look);

	return Mat4x4(right.x, up.x, look.x, 0.f,
		right.y, up.y, look.y, 0.f,
		right.z, up.z, look.z, 0.f,
		-dot(p, right), -dot(p, up), -dot(p, look), 1.f);
}

