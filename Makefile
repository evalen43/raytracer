all:	 
	g++ -o raytracer  -Wall raytracer.cpp sphere.cpp functions.cpp 

clean:	
	rm raytracer