#include"Matrix.h"
using namespace jmxRCore;

Mat4x4 Mat4x4::Mat4x4Scale(f32 x, f32 y, f32 z)
{
	return Mat4x4(x, 0.f, 0.f, 0.f,
		0.f, y, 0.f, 0.f,
		0.f, 0.f, z, 0.f,
		0.f, 0.f, 0.f, 1.f);
}
Mat4x4 Mat4x4::Mat4x4Scale(const Vec3& v)
{
	return Mat4x4Scale(v.x, v.y, v.z);
}

Mat4x4 Mat4x4::Mat4x4Translate(f32 x, f32 y, f32 z)
{
	return Mat4x4(1.f, 0.f, 0.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		x, y, z, 1.f);
}
Mat4x4 Mat4x4::Mat4x4Translate(const Vec3& v)
{
	return Mat4x4Translate(v.x, v.y, v.z);
}

Mat4x4 Mat4x4::Mat4x4Rotate(f32 radian, f32 x, f32 y, f32 z)
{
	f32 cosa = std::cosf(radian);
	f32 onesubcosa = 1.f - cosa;
	f32 sina = std::sinf(radian);

	return Mat4x4(cosa + onesubcosa*x*x, onesubcosa*x*y + sina*z, onesubcosa*x*z - sina*y, 0.f,
		onesubcosa*x*y - sina*z, cosa + onesubcosa*y*y, onesubcosa*y*z + sina*x, 0.f,
		onesubcosa*x*z + sina*y, onesubcosa*y*z - sina*x, cosa + onesubcosa*z*z, 0.f,
		0.f, 0.f, 0.f, 1.f);
}

Mat4x4 Mat4x4::Mat4x4Rotate(f32 radian, const Vec3& v)
{
	return Mat4x4Rotate(radian, v.x, v.y, v.z);
}

Mat4x4 Mat4x4::Mat4x4RotateX(f32 radian)
{
	f32 cosa = std::cosf(radian);
	f32 sina = std::sinf(radian);

	return Mat4x4(1.f, 0.f, 0.f, 0.f,
		0.f, cosa, sina, 0.f,
		0.f, -sina, cosa, 0.f,
		0.f, 0.f, 0.f, 1.f);
}

Mat4x4 Mat4x4::Mat4x4RotateY(f32 radian)
{
	f32 cosa = std::cosf(radian);
	f32 sina = std::sinf(radian);

	return Mat4x4(cosa, 0.f, -sina, 0.f,
		0.f, 1.f, 0.f, 0.f,
		sina, 0.f, cosa, 0.f,
		0.f, 0.f, 0.f, 1.f);
}

Mat4x4 Mat4x4::Mat4x4RotateZ(f32 radian)
{
	f32 cosa = std::cosf(radian);
	f32 sina = std::sinf(radian);

	return Mat4x4(cosa, sina, 0.f, 0.f,
		-sina, cosa, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f);
}

