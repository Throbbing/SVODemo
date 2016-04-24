#include"Vector.h"

using namespace jmxRCore;


Vec3::Vec3(const Point3& p) :x(p.x), y(p.y), z(p.z){}
Vec3::Vec3(const Normal& n) : x(n.x), y(n.y), z(n.z){}