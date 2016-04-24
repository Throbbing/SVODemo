/**
*　　　　　　　　┏┓　　　┏┓+ +
*　　　　　　　┏┛┻━━━┛┻┓ + +
*　　　　　　　┃　　　　　　　┃ 　
*　　　　　　　┃　　　━　　　┃ ++ + + +
*　　　　　　 ████━████ ┃+
*　　　　　　　┃　　　　　　　┃ +
*　　　　　　　┃　　　┻　　　┃
*　　　　　　　┃　　　　　　　┃ + +
*　　　　　　　┗━┓　　　┏━┛
*　　　　　　　　　┃　　　┃　　　　　　　　　　　
*　　　　　　　　　┃　　　┃ + + + +
*　　　　　　　　　┃　　　┃　　　　Code is far away from bug with the animal protecting　　　　　　　
*　　　　　　　　　┃　　　┃ + 　　　　神兽保佑,代码无bug　　
*　　　　　　　　　┃　　　┃
*　　　　　　　　　┃　　　┃　　+　　　　　　　　　
*　　　　　　　　　┃　 　　┗━━━┓ + +
*　　　　　　　　　┃ 　　　　　　　┣┓
*　　　　　　　　　┃ 　　　　　　　┏┛
*　　　　　　　　　┗┓┓┏━┳┓┏┛ + + + +
*　　　　　　　　　　┃┫┫　┃┫┫
*　　　　　　　　　　┗┻┛　
*/
#pragma once
#include"Math.h"

#include"Matrix.h"
#include"Transform.h"

namespace jmxRCore
{



	//forward declaration
	class Shape;

	class Ray
	{
	public:
		Point3 ori;			//12字节
		Vec3 dir;		//24字节
		u32 depth;			//28字节
		f32 time;
	

	public:
		Ray() :time(0) { depth = 0; }
		Ray(const Point& o, const Vec3& d, u32 dd = 0) :ori(o), dir(d), depth(dd){ }
		Ray(const Ray& r) :ori(r.ori), dir(r.dir), depth(r.depth){ }

		

		Point operator()(f32 t) const { return ori + t*dir; }


	};

	class DifferentialRay :public Ray	//保存额外的偏移量，为了纹理反走样
	{
	public:
		Point rxOri, ryOri;
		Vec3 rxDir, ryDir;
		bool hasDifferentials;

		DifferentialRay() { hasDifferentials = false; }

		explicit DifferentialRay(const Ray& ray) :Ray(ray), hasDifferentials(false) { }

		DifferentialRay(const Ray& ray, const Point& rxo, const Point& ryo,
			const Vec3& rxd, const Vec3& ryd) :Ray(ray), rxOri(rxo), ryOri(ryo),
			rxDir(rxd), ryDir(ryd), hasDifferentials(true) { }
	};


	class BBox
	{
	public:
		Point minP, maxP;

		BBox()
		{
			minP = { INFINITY, INFINITY, INFINITY };
			maxP = { -INFINITY, -INFINITY, -INFINITY };
		}
		BBox(const Point& minP, const Point& maxP) :minP(minP), maxP(maxP) {}
		BBox(std::initializer_list<f32> il)
		{
			auto p = il.begin();
			minP.x = p[0]; minP.y = p[1]; minP.z = p[2];
			maxP.x = p[3]; maxP.y = p[4]; maxP.z = p[5];
		}



		bool		intersect(const Ray& r, f32 tmin, f32 tmax,
			_Out f32* thit0 = nullptr, _Out f32* thit1 = nullptr);

		BBox		transform(const Mat4x4& mat);


		f32			extent(u32 i)
		{
			return maxP[i] - minP[i];
		}

		f32			area()
		{
			f32 x = extent(0);
			f32 y = extent(1);
			f32 z = extent(2);
			return 2.f*(x*y + x*z + y*z);
		}

		bool		inside(const Point& p)
		{
			if (minP.x < p.x&&p.x < maxP.x&&
				minP.y < p.y&&p.y < maxP.y&&
				minP.z < p.z&&p.z < maxP.z)
				return true;
			return false;
		}
		BBox	unionBox(const BBox& bbox)
		{
			BBox b;
			b.maxP.x = max(maxP.x, bbox.maxP.x);
			b.maxP.y = max(maxP.y, bbox.maxP.y);
			b.maxP.z = max(maxP.z, bbox.maxP.z);
			b.minP.x = min(minP.x, bbox.minP.x);
			b.minP.y = min(minP.y, bbox.minP.y);
			b.minP.z = min(minP.z, bbox.minP.z);

			return b;
		}
		bool	overlaps(const BBox& b)
		{
			bool x = (maxP.x >= b.minP.x) && (minP.x <= b.maxP.x);
			bool y = (maxP.y >= b.minP.y) && (minP.y <= b.maxP.y);
			bool z = (maxP.z >= b.minP.z) && (minP.z <= b.maxP.z);
			return (x && y && z);
		}




	};

	class DifferentialGeometry
	{
	public:
		Point  hitPoint;
		Normal hitNormal;
		f32	   u, v;


		Vec3   dpdu, dpdv;
		Vec3   dndu, dndv;

		mutable Vec3 dpdx, dpdy;
		mutable f32 dudx, dudy;
		mutable f32 dvdx, dvdy;

		const Shape* shape;

	private:
		mutable bool		 dif;

	public:
		DifferentialGeometry(){}
		DifferentialGeometry(const Point& pp, const Normal& nn,
			f32 uu, f32 vv,
			const Vec3& pu, const Vec3& pv,
			const Vec3& nu, const Vec3& nv,
			const Shape* ss) :hitPoint(pp), hitNormal(nn),
			u(uu), v(vv),
			dpdu(pu), dpdv(pv), dndu(nu), dndv(nv),
			shape(ss)
		{
			dif = false;
		}

		void		computeDifferential(const DifferentialRay& ray) const;
	};



	FORCEINLINE bool hitPlanar(const Ray& r, const Vec3& p, const Vec3& n,
		_Out f32* thit)
	{
		f32 dn = dot(r.dir, n);
		if (fabs(dn) < EPSILON)
			return false;

		if (thit)
			*thit = (dot(p, n) - dot(Vec3(r.ori.x,r.ori.y,r.ori.z), n)) / dn;

		return true;
	}


	struct Rect
	{
		Point2 left;
		Point2 right;
		
		Rect() {}
		Rect(const Point2& l, const Point2& r) :
			left(l), right(r){}

		Rect(f32 lx, f32 ly, f32 rx, f32 ry) :
			left(Point2(lx, ly)), right(Point2(rx, ry)){}
	};

}