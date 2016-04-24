#pragma once 

#include<assert.h>

#include"JmxRConfig.h"
#include<cmath>
#ifndef XAxis
#define XAxis 0
#endif // !XAxis

#ifndef YAxis
#define YAxis 1
#endif // !YAxis

#ifndef ZAxis
#define ZAxis 2
#endif 

//向量库
namespace jmxRCore
{

	struct Vec2
	{
		//public data
		f32 x, y;

		//public method

		//Construction
		Vec2(){ x = 0; y = 0; }
		Vec2(f32 xx, f32 yy) :x(xx), y(yy){ Assert(valid()); }
		Vec2(const Vec2& vec2) :x(vec2.x), y(vec2.y){ Assert(valid()); }
		Vec2(std::initializer_list<f32> il)
		{
			Assert(il.size() == 2);
			auto p = il.begin();
			x = p[0];
			y = p[1];
		}
		Vec2& operator=(const Vec2& vec2)
		{
			x = vec2.x;
			y = vec2.y;
			Assert(valid());
			return *this;
		}
		FORCEINLINE bool valid()
		{
			return !Isnan(x) && !Isnan(y);
		}
		f32 operator[](int i)
		{
			Assert(i >= 0 && i < 2);

			if (i == 0)
				return x;
			else
				return y;
		}
		FORCEINLINE f32 length2() const
		{
			return x*x + y*y;
		}
		FORCEINLINE f32 length()  const
		{
			return std::sqrtf(x*x + y*y);
		}
		FORCEINLINE void normalize()
		{
			f32 len = length();
			x /= len;
			y /= len;
		}

#if defined(_DEBUG)||defined(DEBUG)
		void printVec(std::ostream& out)
		{
			out << "{ " << x << "  " << y << " }" << std::endl;
		}

#endif // 0

		

	};

#pragma region	 Vec2Operator
	inline Vec2 operator+(const Vec2& v1, const Vec2& v2)
	{
		return Vec2(v1.x + v2.x, v1.y + v2.y);
	}
	inline void operator+=(Vec2& v1, const Vec2& v2)
	{
		v1.x += v2.x;
		v1.y += v2.y;
	}

	inline Vec2 operator-(const Vec2& v1, const Vec2& v2)
	{
		return Vec2(v1.x - v2.x, v1.y - v2.y);
	}

	inline void operator-=(Vec2& v1, const Vec2& v2)
	{
		v1.x -= v2.x;
		v1.y -= v2.y;
	}

	inline Vec2 operator-(const Vec2& v1)
	{
		return Vec2(-v1.x, -v1.y);
	}



	inline f32 dot(const Vec2& v1, const Vec2& v2)
	{
		return v1.x*v2.x + v1.y*v2.y;
	}

	inline Vec2 operator* (const Vec2& v, f32 value)
	{
		return  Vec2(v.x*value, v.y*value);
	}

	inline Vec2 operator*(f32 value, const Vec2& v)
	{
		return v*value;
	}

	inline void operator*=(Vec2& v1, f32 value)
	{
		v1.x *= value;
		v1.y *= value;
	}

	inline Vec2 operator/(const Vec2& v, f32 value)
	{
		return Vec2(v.x / value, v.y / value);
	}

	inline void operator/=(Vec2& v, f32 value)
	{
		v.x /= value;
		v.y /= value;
	}

#pragma endregion

	struct Point3;
	struct Normal;

	struct Vec3
	{
		//public data
		f32 x, y, z;

		//public method
		Vec3() { x = 0; y = 0; }
		Vec3(f32 xx, f32 yy, f32 zz) :x(xx), y(yy), z(zz){ Assert(valid()); }
		Vec3(const Vec3& v) :x(v.x), y(v.y), z(v.z) { Assert(valid()); }
		
		Vec3(std::initializer_list<f32> il)
		{
			Assert(il.size() == 3);
			auto p = il.begin();
			x = p[0];
			y = p[1];
			z = p[2];
		}
		explicit Vec3(const Normal& n);
		explicit Vec3(const Point3& p);
		
		Vec3& operator=(const Vec3& v)
		{
			x = v.x;
			y = v.y;
			z = v.z;
			Assert(valid());
			return *this;
		}
		FORCEINLINE bool valid()
		{
			return !Isnan(x) && !Isnan(y) && !Isnan(z);
		}

		f32 operator[](int i) const
		{
			Assert(i >= 0 && i < 3);

			if (i == 0)
				return x;
			else if (i == 1)
				return y;
			else
				return z;
		}

		FORCEINLINE f32 length2() const
		{
			return x*x + y*y + z*z;
		}
		FORCEINLINE f32 length() const
		{
			return sqrtf(x*x + y*y + z*z);
		}

		FORCEINLINE void normalize()
		{
			f32 len = length();
			if (len < 0.00000001f)
				len = 1.f;
			x /= len;
			y /= len;
			z /= len;
		}

#if defined(_DEBUG)||defined(DEBUG)
		void printVec(std::ostream& out)
		{
			out << "{ " << x << "  " << y << "  " << z << " }" << std::endl;
		}

#endif // 0

	};

	

#pragma region Vec3Operator
	inline Vec3 operator+(const Vec3& v1, const Vec3& v2)
	{
		return Vec3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
	}

	inline void operator+=(Vec3& v1, const Vec3& v2)
	{
		v1.x += v2.x;
		v1.y += v2.y;
		v1.z += v2.z;
	}

	inline Vec3 operator-(const Vec3& v1, const Vec3& v2)
	{
		return Vec3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
	}

	inline Vec3 operator-(const Vec3& v1)
	{
		return Vec3(-v1.x, -v1.y, -v1.z);
	}

	inline void operator-=(Vec3& v1, const Vec3& v2)
	{
		v1.x -= v2.x;
		v1.y -= v2.y;
		v1.z -= v2.z;
	}

	inline Vec3 operator*(const Vec3& v1, f32 value)
	{
		return Vec3(v1.x*value, v1.y*value, v1.z*value);
	}

	inline Vec3 operator*(f32 value, const Vec3& v1)
	{
		return v1*value;
	}

	inline void operator*=(Vec3& v1, f32 value)
	{
		v1.x *= value;
		v1.y *= value;
		v1.z *= value;
	}



	inline Vec3 operator/(const Vec3& v1, f32 value)
	{
		return Vec3(v1.x / value, v1.y / value, v1.z / value);
	}

	inline void operator/=(Vec3& v1, f32 value)
	{
		v1.x /= value;
		v1.y /= value;
		v1.z /= value;
	}

	inline f32 dot(const Vec3& v1, const Vec3& v2)
	{
		return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
	}

	inline Vec3 cross(const Vec3& v1, const Vec3& v2)
	{
		return Vec3(v1.y*v2.z - v1.z*v2.y,
			v1.z*v2.x - v1.x*v2.z,
			v1.x*v2.y - v1.y*v2.x);
	}

#pragma endregion


	struct Vec4
	{
		//public data
		f32 x, y, z, w;

		Vec4(f32 v = 0) { x = y = z = w = v; }
		Vec4(f32 xx, f32 yy, f32 zz, f32 ww) :x(xx), y(yy), z(zz), w(ww){ Assert(valid()); }
		Vec4(const Vec4& vec4) :x(vec4.x), y(vec4.y), z(vec4.z), w(vec4.w){ Assert(valid()); }
		Vec4(const Vec3& vec3, f32 ww) :x(vec3.x), y(vec3.y), z(vec3.z), w(ww){ Assert(valid()); }
		Vec4(std::initializer_list<f32> il)
		{
			Assert(il.size() == 4);
			auto p = il.begin();
			x = p[0];
			y = p[1];
			z = p[2];
			w = p[3];
		}
		Vec4&  operator=(const Vec4& vec4)
		{
			x = vec4.x;
			y = vec4.y;
			z = vec4.z;
			w = vec4.w;
			return *this;
		}
		FORCEINLINE bool valid()
		{
			return !Isnan(x) && !Isnan(y) && !Isnan(z) && !Isnan(w);
		}

		f32 operator[](int i) const
		{
			Assert(i >= 0 && i < 4);

			switch (i)
			{
			case 0:return x; break;
			case 1:return y; break;
			case 2:return z; break;
			case 3:return w; break;
			default:return 0;
				break;
			}
		}

		FORCEINLINE f32 length2() const
		{
			return x*x + y*y + z*z + w*w;
		}
		FORCEINLINE f32 length() const
		{
			return sqrtf(x*x + y*y + z*z + w*w);
		}

		FORCEINLINE void normalize()
		{
			f32 len = length();
			x /= len;
			y /= len;
			z /= len;
			w /= len;
		}


#if defined(_DEBUG)||defined(DEBUG)
		void printVec(std::ostream& out)
		{
			out << "{ " << x << "  " << y << "  " << z << "  " << w << " }" << std::endl;
		}

#endif // 0

		Vec3 vec3() { return Vec3(x, y, z); }
	};

	using Vector = Vec3;

#pragma region Vec4Operator
	inline Vec4 operator+(const Vec4& v1, const Vec4& v2)
	{
		return Vec4(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w);
	}

	inline void operator+=(Vec4& v1, const Vec4& v2)
	{
		v1.x += v2.x;
		v1.y += v2.y;
		v1.z += v2.z;
		v1.w += v2.w;
	}

	inline Vec4 operator-(const Vec4& v1, const Vec4& v2)
	{
		return Vec4(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w);
	}

	inline Vec4 operator-(const Vec4& v1)
	{
		return Vec4(-v1.x, -v1.y, -v1.z, -v1.w);
	}

	inline void operator-=(Vec4& v1, const Vec4& v2)
	{
		v1.x -= v2.x;
		v1.y -= v2.y;
		v1.z -= v2.z;
		v1.w -= v2.w;
	}

	inline Vec4 operator*(const Vec4& v1, f32 value)
	{
		return Vec4(v1.x*value, v1.y*value, v1.z*value, v1.w*value);
	}

	inline Vec4 operator*(f32 value, const Vec4& v1)
	{
		return v1*value;
	}

	inline void operator*=(Vec4& v1, f32 value)
	{
		v1.x *= value;
		v1.y *= value;
		v1.z *= value;
		v1.w *= value;
	}



	inline Vec4 operator/(const Vec4& v1, f32 value)
	{
		return Vec4(v1.x / value, v1.y / value, v1.z / value, v1.w / value);
	}

	inline void operator/=(Vec4& v1, f32 value)
	{
		v1.x /= value;
		v1.y /= value;
		v1.z /= value;
		v1.w /= value;
	}

	inline f32 dot(const Vec4& v1, const Vec4& v2)
	{
		return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z + v1.w * v2.w;
	}

#pragma endregion



	struct Point2
	{
		Point2() { x = y = 0; }
		Point2(f32 val) :x(val), y(val){}
		Point2(f32 xx, f32 yy) :x(xx), y(yy) {}
		Point2(std::initializer_list<f32> il)
		{
			auto p = il.begin();
			x = p[0];
			y = p[1];
		}

		f32 operator[](u32 i)
		{
			Assert(i == 0 || i == 1);

			if (i == 0)
				return x;
			else
				return y;
		}
		
		Point2 operator+ (const Point2& p) const
		{
			return Point2(x + p.x, y + p.y);
		}
		void operator+=(const Point2& p)
		{
			x += p.x;
			y += p.y;
			
		}
		Point2 operator-(const Point2& p) const
		{
			return Point2(x - p.x, y - p.y);
		}
		void operator-=(const Point2& p) 
		{
			x -= p.x;
			y -= p.y;
		}
		Point2 operator*(const Point2& p) const
		{
			return Point2(x*p.x, y*p.y); 
		}
		void operator*=(const Point2& p)
		{
			x *= p.x;
			y *= p.y;
		}
		Point2 operator/(const Point2& p)const
		{
			return Point2(x / p.x, y / p.y);
		}
		void operator/=(const Point2& p)
		{
			x /= p.x;
			y /= p.y;
		}
		Point2 operator+(const Vec2& v) const
		{
			return Point2(x + v.x, y + v.y);
		}
		void operator+=(const Vec2& v)
		{
			x += v.x;
			y += v.y;
		}
		Point2 operator-(const Vec2& v) const
		{
			return Point2(x - v.x, y - v.y);
		}
		void operator-=(const Vec3& v)
		{
			x -= v.x;
			y -= v.y;
		}

		std::string toString()
		{
			std::ostringstream ss;
			ss << "Point2: " << "( " << x << " , " << y << " )" << std::endl;
			return ss.str();
		}
		
		//公有成员
		f32 x, y;
	};

	struct Point3
	{
		Point3() { x = y = 0; }
		Point3(f32 val) :x(val), y(val){}
		Point3(f32 xx, f32 yy,f32 zz) :x(xx), y(yy),z(zz) {}
		Point3(std::initializer_list<f32> il)
		{
			auto p = il.begin();
			x = p[0];
			y = p[1];
			z = p[2];
		}
		explicit Point3(const Vec3& v)
		{
			x = v.x;
			y = v.y;
			z = v.z;
		}
		f32 operator[](u32 i) const
		{
			Assert(i >=0&&i<3);

			if (i == 0)
				return x;
			else if (i == 1)
				return y;
			else
				return z;
		}
		
		f32 length2() const
		{
			return x*x + y*y + z*z;
		}

		Point3 operator+ (const Point3& p) const
		{
			return Point3(x + p.x, y + p.y, z + p.z);
		}
		void operator+=(const Point3& p)
		{
			x += p.x;
			y += p.y;
			z += p.z;

		}
		Point3 operator-(const Point3& p) const
		{
			return Point3(x - p.x, y - p.y,z-p.z);
		}
		
		void operator-=(const Point3& p)
		{
			x -= p.x;
			y -= p.y;
			z -= p.z;
		}
		Point3 operator*(const Point3& p) const
		{
			return Point3(x*p.x, y*p.y,z*p.z);
		}
		void operator*=(const Point3& p)
		{
			x *= p.x;
			y *= p.y;
			z *= p.z;
		}
		Point3 operator/(const Point3& p)const
		{
			return Point3(x / p.x, y / p.y,z/p.z);
		}
		void operator/=(const Point3& p)
		{
			x /= p.x;
			y /= p.y;
			z /= p.z;
		}
		Point3 operator+(const Vec3& v) const
		{
			return Point3(x + v.x, y + v.y,z+v.z);
		}
		void operator+=(const Vec3& v)
		{
			x += v.x;
			y += v.y;
			z += v.z;
		}
		Point3 operator-(const Vec3& v) const
		{
			return Point3(x - v.x, y - v.y, z - v.z);
		}
		void operator-=(const Vec3& v)
		{
			x -= v.x;
			y -= v.y;
			z -= v.z;
		}

		std::string toString() const
		{
			std::ostringstream ss;
			ss << "Point3: " << "( " << x << " , " << y << " , " << z << " )" << std::endl;
			return ss.str();
		}


		//公有成员
		f32 x, y, z;
	};
	inline Point3 operator-(const Point3& p) 
	{
		return Point3(-p.x, -p.y, -p.z);
	}

	using Point = Point3;
	
	

	struct Normal
	{
		Normal() { x = y = z = 0; }
		Normal(f32 val) :x(val), y(val), z(val) { }
		Normal(f32 xx, f32 yy, f32 zz) :x(xx), y(yy), z(zz){}
		explicit Normal(const Vector& v)
		{
			x = v.x;
			y = v.y;
			z = v.z;
		}

		void normalize()
		{
			f32 v = 1.f / (x*x + y*y + z*z);
			x *= v;
			y *= v;
			z *= v;
		}
		


		f32 operator[](u32 i) const
		{
			Assert(i >= 0 && i<3);

			if (i == 0)
				return x;
			else if (i == 1)
				return y;
			else
				return z;
		}

		Normal operator+ (const Normal& p) const
		{
			return Normal(x + p.x, y + p.y, z + p.z);
		}
		void operator+=(const Normal& p)
		{
			x += p.x;
			y += p.y;
			z += p.z;

		}
		Normal operator-(const Normal& p) const
		{
			return Normal(x - p.x, y - p.y, z - p.z);
		}
		void operator-=(const Normal& p)
		{
			x -= p.x;
			y -= p.y;
			z -= p.z;
		}
		Normal operator*(const Normal& p) const
		{
			return Normal(x*p.x, y*p.y, z*p.z);
		}
		void operator*=(const Normal& p)
		{
			x *= p.x;
			y *= p.y;
			z *= p.z;
		}
		Normal operator/(const Normal& p)const
		{
			return Normal(x / p.x, y / p.y, z / p.z);
		}
		void operator/=(const Normal& p)
		{
			x /= p.x;
			y /= p.y;
			z /= p.z;
		}
		Normal operator+(const Vec3& v) const
		{
			return Normal(x + v.x, y + v.y, z + v.z);
		}
		void operator+=(const Vec3& v)
		{
			x += v.x;
			y += v.y;
			z += v.z;
		}
		Normal operator-(const Vec3& v) const
		{
			return Normal(x - v.x, y - v.y, z - v.z);
		}
		void operator-=(const Vec3& v)
		{
			x -= v.x;
			y -= v.y;
			z -= v.z;
		}

		std::string toString() const
		{
			std::ostringstream ss;
			ss << "Normal: " << "( " << x << " , " << y << " , " << z << " )" << std::endl;
			return ss.str();
		}
		
		f32 x, y, z;
	};





	FORCEINLINE f32 dot(const Point3& p, const Vec3& v)
	{
		return p.x*v.x + p.y*v.y + p.z*v.z;
	}
	FORCEINLINE f32 dot(const Vec3& v, const Point3& p)
	{
		return v.x*p.x + v.y*p.y + v.z*p.z;
	}

	FORCEINLINE f32 dot(const Normal& n, const Vec3& v)
	{
		return n.x*v.x + n.y*v.y + n.z*v.z;
	}
	FORCEINLINE f32 dot(const Vec3& v, const Normal& n)
	{
		return v.x*n.x + v.y*n.y + v.z*n.z;
	}

	FORCEINLINE f32 dot(const Normal& n, const Point3& v)
	{
		return n.x*v.x + n.y*v.y + n.z*v.z;
	}
	FORCEINLINE f32 dot(const Point3& v, const Normal& n)
	{
		return v.x*n.x + v.y*n.y + v.z*n.z;
	}

	FORCEINLINE Vec2 normalize(const Vec2& v)  
	{
		f32 l = v.length2();
		return Vec2(v.x / l, v.y / l);
	}
	FORCEINLINE Vec3 normalize(const Vec3& v)
	{
		f32 l = 1.f / v.length2();
		return Vec3(v.x*l, v.y*l, v.z*l);
	}
	FORCEINLINE Vec4 normalize(const Vec4& v)
	{
		f32 l = 1.f / v.length2();
		return Vec4(v.x*l, v.y*l, v.z*l, v.w*l);
	}
	
}
