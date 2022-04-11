#ifndef __RAYTRACER_H__
#define __RAYTRACER_H__

// The main ray tracer.

#include "scene/scene.h"
#include "scene/ray.h"

enum AntialiasingMode { NONE, SUPER, SUPER_JITTER, ADAPTIVE, MODE_COUNT }; // Remember to update num of items in menu correctly
// class TraceUI;

class RayTracer
{
public:
    RayTracer();
    ~RayTracer();

    vec3f trace( Scene *scene, double x, double y );
	vec3f traceRay( Scene *scene, const ray& r, const vec3f& thresh, int depth, bool isInObj = false );

	void setAalmode(int mode) { aalmode = static_cast<AntialiasingMode>(mode); }

	void getBuffer( unsigned char *&buf, int &w, int &h );
	double aspectRatio();
	void traceSetup( int w, int h );
	void traceLines( int start = 0, int stop = 10000000 );
	void tracePixel( int i, int j );

	bool loadScene( char* fn );

	bool sceneLoaded();

	// TraceUI* m_pUI;

private:
	unsigned char *buffer;
	int buffer_width, buffer_height;
	int bufferSize;
	Scene *scene;
	AntialiasingMode aalmode;
	vec3f traceSubPixel(double* coords, int numSubPixels, int &depth); // For Adaptive Supersampling

	bool m_bSceneLoaded;
};

#endif // __RAYTRACER_H__
