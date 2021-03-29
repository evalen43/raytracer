#include <cstdlib>
#include <cstdio>
#include <cmath>
//#include <fstream>
#include <vector>
#include <iostream>
#include <cassert>
#include "sphere.h"
#include <memory>

#if defined __linux__ || defined __APPLE__
// "Compiled for Linux
#else
// Windows doesn't define these values by default, Linux does

//#define M_PI 3.141592653589793
//#define INFINITY 1e8
#endif

Vec3f trace(
    const Vec3f &rayorig,
    const Vec3f &raydir,
    const std::vector<Sphere> &spheres,
    const int &depth);
void render(const std::vector<Sphere> &spheres);

void save_image(std::shared_ptr<Vec3f[]> framebuffer, int width, int height, int samples_per_pixel); 


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