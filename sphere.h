#include "Vector3.h"

#define _USE_MATH_DEFINES

//#include <vector>

//This variable controls the maximum recursion depth
//#define MAX_RAY_DEPTH 5

class Sphere 
{ 
public: 
    Vec3f center;                           /// position of the sphere 
    float radius, radius2;                  /// sphere radius and radius^2 
    Vec3f surfaceColor, emissionColor;      /// surface color and emission (light) 
    float transparency, reflection;         /// surface transparency and reflectivity 
    Sphere( 
        const Vec3f &c, 
        const float &r, 
        const Vec3f &sc, 
        const float &refl= 0, 
        const float &transp = 0, 
        const Vec3f &ec = Vec3f(0)); 

//Compute a ray-sphere intersection using the geometric solution
	bool intersect(const Vec3f &rayorig, const Vec3f &raydir, float &t0, float &t1) const ;

}; 

