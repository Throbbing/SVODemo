#include"Geometry.h"
#include"Math.h"
#include"Matrix.h"
#include"JmxR.h"

using namespace jmxRCore;

bool BBox::intersect(const Ray& r, f32 tmin, f32 tmax,
	_Out f32* thit0 , _Out f32* thit1 )
{
	f32 t0 = tmin, t1 = tmax;
	for (u32 i = 0; i < 3; ++i)
	{
		f32 inv = 1.f / r.dir[i];
		f32 ttmin = (minP[i] - r.ori[i])*inv;
		f32 ttmax = (maxP[i] - r.ori[i])*inv;

		if (ttmin>ttmax) std::swap(ttmin, ttmax);

		t0 = max(t0, ttmin);
		t1 = min(t1, ttmax);
		if (t0 > t1) return false;
	}
	if (thit0) *thit0 = t0;
	if (thit1) *thit1 = t1;
	return true;
}

BBox BBox::transform(const Mat4x4& m)
{
	Point minPP = { INFINITY, INFINITY, INFINITY };
	Point maxPP = -minPP;

	f32 x0 = minP.x, x1 = maxP.x;
	f32 y0 = minP.y, y1 = maxP.y;
	f32 z0 = minP.z, z1 = maxP.z;

	Vec4 v[8] = { { x0,y0,z0,1 },
	{ x1,y0,z0,1},
	{x0,y1,z0,1},
	{x0,y0,z1,1},
	{x1,y1,z0,1},
	{x0,y1,z1,1},
	{x1,y0,z1,1},
	{x1,y1,z1,1}
	};
	Vec4 vt;
	for (u32 i = 0; i < 8; ++i)
	{
		vt = v[i] * m;
		if (vt.x < minPP.x) minPP.x = vt.x;
		if (vt.y < minPP.y) minPP.y = vt.y;
		if (vt.z < minPP.z) minPP.z = vt.z;

		if (vt.x > maxPP.x) maxPP.x = vt.x;
		if (vt.y > maxPP.y) maxPP.y = vt.y;
		if (vt.z > maxPP.z) maxPP.z = vt.z;
	}

	return BBox(minPP, maxPP);

}


void DifferentialGeometry::computeDifferential(const DifferentialRay& ray) const 
{
	if (dif)
		return;

	if (!ray.hasDifferentials)
	{
		dpdx = Vec3(); dpdy = Vec3();
		dudx = 0; dudy = 0;
		dvdx = 0; dvdy = 0;
		return;
	}

	//计算dpdx dpdy
	// (o+t*d-p)*n=0; p=hitPoint  n=hitNormal;

	Point p = hitPoint;
	Normal n = hitNormal;

	f32 tx = (dot(p, n) - dot(ray.rxOri, n)) / dot(ray.rxDir, n);
	Point px = ray(tx);

	f32 ty = (dot(p, n) - dot(ray.ryOri, n)) / dot(ray.ryDir, n);
	Point py = ray(ty);

	dpdx = Vec3((px - p) / 1.f);
	dpdy = Vec3((py - p) / 1.f);

	//计算dudx dvdx
	//三个方程两个未知数，选择较大的两个值
	u16 axis[2];
	u16 small;
	Vec4 deltaPx = Vec4(Vec3(px - p),0.f);
	if (deltaPx[0] > deltaPx[1])
	{
		axis[0] = 0;
		small = 1;
	}
	else
	{
		axis[0] = 1;
		small = 0;
	}
	if (deltaPx[small] > deltaPx[2])
		axis[1] = small;
	else
		axis[1] = 2;

	f32 m[4] = { dpdu[axis[0]], dpdv[axis[0]], dpdu[axis[1]], dpdu[axis[1]] };
	f32 im[4];
	Mat2Inv(m, im);
	dudx = im[0] * deltaPx[axis[0]] + im[1] * deltaPx[axis[1]];
	dvdx = im[2] * deltaPx[axis[0]] + im[3] * deltaPx[axis[1]];


	//计算dudy dvdy
	Vec4 deltaPy = Vec4(Vec3(py - p), 0.f);
	if (deltaPy[0] > deltaPy[1])
	{
		axis[0] = 0;
		small = 1;
	}
	else
	{
		axis[0] = 1;
		small = 0;
	}
	if (deltaPy[small] > deltaPy[2])
		axis[1] = small;
	else
		axis[1] = 2;

	
	dudy = im[0] * deltaPy[axis[0]] + im[1] * deltaPy[axis[1]];
	dvdy = im[2] * deltaPy[axis[0]] + im[3] * deltaPy[axis[1]];
	
	
	
	dif = true;



}