all:	 
	c++ -o raytracer  -Wall raytracer.cpp sphere.cpp functions.cpp FreeImage.lib

clean:	
	rm raytracer