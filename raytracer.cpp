#include <cstdlib>
#include <cstdio>
#include <cmath>
//#include <fstream>
#include <vector>
#include <iostream>
#include <cassert>
#include "sphere.h"

#if defined __linux__ || defined __APPLE__
// "Compiled for Linux
#else
// Windows doesn't define these values by default, Linux does


//#define M_PI 3.141592653589793
//#define INFINITY 1e8
#endif

// //This variable controls the maximum recursion depth
// #define MAX_RAY_DEPTH 5

// float mix(const float &a, const float &b, const float &mix)
// {
//     return b * mix + a * (1 - mix);
// }

/*This is the main trace function. It takes a ray as argument (defined by its origin and direction).
We test if this ray intersects any of the geometry in the scene. If the ray intersects an object, we compute the
intersection point, the normal at the intersection point, and shade this point using this information.
Shading depends on the surface property (is it transparent, reflective, diffuse). The function returns a color for
the ray. If the ray intersects an object that is the color of the object at the intersection point, otherwise it
returns the background color.*/

Vec3f trace(
    const Vec3f &rayorig,
    const Vec3f &raydir,
    const std::vector<Sphere> &spheres,
    const int &depth);


/*Main rendering function. We compute a camera ray for each pixel of the image trace it and return a color.
If the ray hits a sphere, we return the color of the sphere at the intersection point, else we return the
background color.*/

void render(const std::vector<Sphere> &spheres)
{
    unsigned width = 640, height = 480;
    Vec3f *image = new Vec3f[width * height], *pixel = image;
    float invWidth = 1 / float(width), invHeight = 1 / float(height);
    float fov = 30, aspectratio = width / float(height);
    float angle = tan(M_PI * 0.5 * fov / 180.);
    // Trace rays
    for (unsigned y = 0; y < height; ++y) {
        for (unsigned x = 0; x < width; ++x, ++pixel) {
            float xx = (2 * ((x + 0.5) * invWidth) - 1) * angle * aspectratio;
            float yy = (1 - 2 * ((y + 0.5) * invHeight)) * angle;
            Vec3f raydir(xx, yy, -1);
            raydir.normalize();
            *pixel = trace(Vec3f(0), raydir, spheres, 0);
        }
    }
    // Save result to a PPM image (keep these flags if you compile under Windows)
    std::ofstream ofs("./untitled.ppm", std::ios::out | std::ios::binary);
    ofs << "P6\n" << width << " " << height << "\n255\n";
    for (unsigned i = 0; i < width * height; ++i) {
        ofs << (unsigned char)(std::min(float(1), image[i].x) * 255) <<
               (unsigned char)(std::min(float(1), image[i].y) * 255) <<
               (unsigned char)(std::min(float(1), image[i].z) * 255);
    }
    ofs.close();
    delete [] image;
}

/*In the main function, we will create the scene which is composed of 5 spheres and 1 light (which is also a sphere).
Then, once the scene description is complete we render that scene, by calling the render() function.*/

int main(int argc, char **argv)
{
    srand(13);
    std::vector<Sphere> spheres;
    // position, radius, surface color, reflectivity, transparency, emission color
    spheres.push_back(Sphere(Vec3f( 0.0, -10004, -20), 10000, Vec3f(0.20, 0.20, 0.20), 0, 0.0));
    spheres.push_back(Sphere(Vec3f( 0.0,      0, -20),     4, Vec3f(1.00, 0.32, 0.36), 1, 0.5));
    spheres.push_back(Sphere(Vec3f( 5.0,     -1, -15),     2, Vec3f(0.90, 0.76, 0.46), 1, 0.0));
    spheres.push_back(Sphere(Vec3f( 5.0,      0, -25),     3, Vec3f(0.65, 0.77, 0.97), 1, 0.0));
    spheres.push_back(Sphere(Vec3f(-5.5,      0, -15),     3, Vec3f(0.90, 0.90, 0.90), 1, 0.0));
    // light
    spheres.push_back(Sphere(Vec3f( 0.0,     20, -30),     3, Vec3f(0.00, 0.00, 0.00), 0, 0.0, Vec3f(3)));
    render(spheres);

    return 0;
}