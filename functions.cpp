#include "sphere.h"
#include <vector>
#include <iostream>
#include <memory>
#include "C:\Git\vcpkg\packages\freeimage_x64-windows\include\FreeImage.h"
//This variable controls the maximum recursion depth
#define MAX_RAY_DEPTH 5

float mix(const float &a, const float &b, const float &mix)
{
    return b * mix + a * (1 - mix);
}

/*-----------------------------------------------------------------------------------------------------------------
Main trace function. 
It takes a ray as argument (defined by its origin and direction), and then tests if this ray intersects any of the 
geometry in the scene. If it does, then computes the intersection point, the normal at the intersection point, and 
shade this point using this information.
Shading depends on the surface property (transparent, reflective, diffuse). The function returns a color for
the ray. If the ray intersects an object that is the color of the object at the intersection point, otherwise it
returns the background color.
------------------------------------------------------------------------------------------------------------------*/


Vec3f trace(
    const Vec3f &rayorig,
    const Vec3f &raydir,
    const std::vector<Sphere> &spheres,
    const int &depth)
{
    //if (raydir.length() != 1) std::cerr << "Error raydir not one:" << raydir << std::endl;
    float tnear = INFINITY;
    const Sphere* sphere = NULL;
    // find intersection of this ray with the sphere in the scene
    for (unsigned i = 0; i < spheres.size(); ++i) 
    {
        float t0 = INFINITY, t1 = INFINITY;
        if (spheres[i].intersect(rayorig, raydir, t0, t1)) 
        {
            if (t0 < 0) t0 = t1;
            if (t0 < tnear) 
            {
                tnear = t0;
                sphere = &spheres[i];
            }
        }
    }
    // if there's no intersection return black or background color
    if (!sphere) return Vec3f(2);
    Vec3f surfaceColor = 0; /* color of the ray/surface of the object intersected by the ray*/
    Vec3f phit = rayorig + raydir * tnear; // point of intersection
    Vec3f nhit = phit - sphere->center; // normal at the intersection point
    nhit.normalize(); // normalize normal direction
    
    /* If the normal and the view direction are not opposite to each other reverse the normal direction.
    That also means we are inside the sphere so set the inside bool to true.
    Finally reverse the sign of IdotN which we want positive.*/

    float bias = 1e-4; // add some bias to the point from which we will be tracing
    bool inside = false;
    if (raydir.dot(nhit) > 0) nhit = -nhit, inside = true;
    if ((sphere->transparency > 0 || sphere->reflection > 0) && depth < MAX_RAY_DEPTH) 
    {
        float facingratio = -raydir.dot(nhit);
        // change the mix value to tweak the effect
        float fresneleffect = mix(pow(1 - facingratio, 3), 1, 0.1);
        // compute reflection direction (not need to normalize because all vectors are already normalized)

        Vec3f refldir = raydir - nhit * 2 * raydir.dot(nhit);
        refldir.normalize();
        Vec3f reflection = trace(phit + nhit * bias, refldir, spheres, depth + 1);//--------------------------Recursive
        Vec3f refraction = 0;
        // if the sphere is also transparent compute refraction ray (transmission)
        if (sphere->transparency) 
        {
            float ior = 1.1, eta = (inside) ? ior : 1 / ior; // are we inside or outside the surface?
            float cosi = -nhit.dot(raydir);
            float k = 1 - eta * eta * (1 - cosi * cosi);
            Vec3f refrdir = raydir * eta + nhit * (eta *  cosi - sqrt(k));
            refrdir.normalize();
            refraction = trace(phit - nhit * bias, refrdir, spheres, depth + 1);//----------------------------Recursive
            //std::cout<<refraction<<std::endl;
        }
        // the result is a mix of reflection and refraction (if the sphere is transparent)
        surfaceColor = (
            reflection * fresneleffect +
            refraction * (1 - fresneleffect) * sphere->transparency) * sphere->surfaceColor;
    }
    else {
        // it's a diffuse object, no need to raytrace any further
        for (unsigned i = 0; i < spheres.size(); ++i) {
            if (spheres[i].emissionColor.x > 0) 
            {
                // this is a light
                Vec3f transmission = 1;
                Vec3f lightDirection = spheres[i].center - phit;
                lightDirection.normalize();
                for (unsigned j = 0; j < spheres.size(); ++j) {
                    if (i != j) {
                        float t0, t1;
                        if (spheres[j].intersect(phit + nhit * bias, lightDirection, t0, t1)) {
                            transmission = 0;
                            break;
                        }
                    }
                }
                surfaceColor += sphere->surfaceColor * transmission *
                std::max(float(0), nhit.dot(lightDirection)) * spheres[i].emissionColor;
            }
        }
    }

    return surfaceColor + sphere->emissionColor;
}

/*-------------------------------------------------------------------------------------------------
save_image: Generats jpg file using FreeImage
parameters: (framebuffer, width, height, samples per pixel)
--------------------------------------------------------------------------------------------------*/

float clamp(const float& lo, const float& hi, const float& v) 
{
    return std::max(lo, std::min(hi, v));
}

//void save_image(std::shared_ptr<Vec3f[]> framebuffer, int width, int height, int samples_per_pixel)
void save_image(Vec3f* framebuffer, int width, int height, int samples_per_pixel)  
{
	std::vector<unsigned char> pixels;
    // Save result to a PPM image (keep these flags if you compile under Windows)
    std::ofstream ofs("./untitled.ppm", std::ios::out | std::ios::binary);
    ofs << "P6\n" << width << " " << height << "\n255\n";    
	// std::ofstream ofs;
	// ofs.open("out.ppm");
	// ofs << "P6\n" << width << " " << height << "\n255\n";
	int currentpix = 0;
	for (int j = 0; j < height; ++j) {
		for (int i = 0; i < width; i++)
		{
			// write_color(ofs, framebuffer[currentpix]);
			int b = (static_cast<int>(255.99 * clamp(0, 1, framebuffer[currentpix].x)));
			int g = (static_cast<int>(255.99 * clamp(0, 1, framebuffer[currentpix].y)));
			int r = (static_cast<int>(255.99 * clamp(0, 1, framebuffer[currentpix].z)));
			if (samples_per_pixel > 1) {
				auto scale = 1.0 / samples_per_pixel;
				b *= scale; g *= scale; r *= scale;
			}
			ofs << r << g << b;
			pixels.push_back(r);
			pixels.push_back(g);
			pixels.push_back(b);

			currentpix++;
		}
	}
	ofs.close();
	//writeImage(width, height, pixels);
	// FreeImage_Initialise();
	// FIBITMAP* img = FreeImage_ConvertFromRawBits(pixels.data(), width, height, width * 3, 24, 0xFF0000, 0x00FF00, 0x0000FF, true);
	// FreeImage_FlipVertical(img);
	// char fname[] = "output.png";
	// FreeImage_Save(FIF_PNG, img, fname, 0);

	// FreeImage_DeInitialise();
	// printf("outputting image...\n");
	// std::cout << fname << std::endl;
	std::cout << "Press any key to exit" << std::endl;
	std::cin.get();
	std::cerr << "\nDone.\n";
}

/*--------------------------------------------------------------------------------------------------------
render: Main rendering function. 
Computes a camera ray for each pixel of the image, trace it, and return a color.
Ray hits a sphere: Returns the color of the sphere at the intersection point, else returns the background color.
Parameter(s): vector with list of objects (spheres)
---------------------------------------------------------------------------------------------------------*/

void render(const std::vector<Sphere> &spheres)
{
    unsigned width = 640, height = 480;
    //std::shared_ptr<Vec3f[]> image(new Vec3f(width*height));
    //Vec3f *pixel=image;
    Vec3f *image = new Vec3f[width * height], *pixel = image;
    float invWidth = 1 / float(width), invHeight = 1 / float(height);
    float fov = 30, aspectratio = width / float(height);
    float angle = tan(M_PI * 0.5 * fov / 180.);
    // Trace rays
    int npix=0;
    for (unsigned y = 0; y < height; ++y) {
        for (unsigned x = 0; x < width; ++x, ++pixel) {
            float xx = (2 * ((x + 0.5) * invWidth) - 1) * angle * aspectratio;
            float yy = (1 - 2 * ((y + 0.5) * invHeight)) * angle;
            Vec3f raydir(xx, yy, -1);
            raydir.normalize();
            *pixel = trace(Vec3f(0), raydir, spheres, 0);
            //image[npix] = trace(Vec3f(0), raydir, spheres, 0);
            npix++;
        }
    }
    // Save result to a PPM image (keep these flags if you compile under Windows)
    save_image(image, width, height, 1);    
    // std::ofstream ofs("./untitled.ppm", std::ios::out | std::ios::binary);
    // ofs << "P6\n" << width << " " << height << "\n255\n";
    // for (unsigned i = 0; i < width * height; ++i) {
    //     ofs << (unsigned char)(std::min(float(1), image[i].x) * 255) <<
    //            (unsigned char)(std::min(float(1), image[i].y) * 255) <<
    //            (unsigned char)(std::min(float(1), image[i].z) * 255);
    // }
    //ofs.close();
    delete [] image;
}


