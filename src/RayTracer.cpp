// The main ray tracer.

#include <Fl/fl_ask.h>
#include <time.h>

#include "RayTracer.h"
#include "scene/light.h"
#include "scene/material.h"
#include "scene/ray.h"
#include "fileio/read.h"
#include "fileio/parse.h"
#include <math.h>
#include "ui/TraceUI.h"
#define  NUMERICAL_ERROR 1.0e-9
extern TraceUI* traceUI;

// Trace a top-level ray through normalized window coordinates (x,y)
// through the projection plane, and out into the scene.  All we do is
// enter the main ray-tracing method, getting things started by plugging
// in an initial ray weight of (0.0,0.0,0.0) and an initial recursion depth of 0.
vec3f RayTracer::trace( Scene *scene, double x, double y )
{
    ray r( vec3f(0,0,0), vec3f(0,0,0) );
    scene->getCamera()->rayThrough( x,y,r );
	return traceRay( scene, r, vec3f(1.0,1.0,1.0), 0 ).clamp();
}

// Do recursive ray tracing!  You'll want to insert a lot of code here
// (or places called from here) to handle reflection, refraction, etc etc.
vec3f RayTracer::traceRay( Scene *scene, const ray& r, 
	const vec3f& thresh, int depth, bool isInObj )
{
	isect i;

	if( scene->intersect( r, i ) ) {
		// An intersection occured!  We've got work to do.  For now,
		// this code gets the material for the surface that was intersected,
		// and asks that material to provide a color for the ray.  
		//
		// This is a great place to insert code for recursive ray tracing.
		// Instead of just returning the result of shade(), add some
		// more steps: add in the contributions from reflected and refracted
		// rays.
		// 
		// YOUR CODE HERE
		vec3f I_result(0, 0, 0); //(0,0,0) += shade, then += kr*reflection, then += kt*refraction, then = result

		// Shading
		const Material& m = i.getMaterial();
		I_result += m.shade(scene, r, i);

		vec3f ones(1.0, 1.0, 1.0);
		I_result = prod(ones - m.kt, I_result);

		// recursion for reflection and refraction
		if (depth < traceUI->getDepth())
		{
			// reflection
			vec3f next_ray_position = r.getPosition() + r.getDirection() * i.t;
			vec3f I = r.getDirection().normalize();
			vec3f N = i.N.normalize();
			if (m.kr[0] > 0 || m.kr[1] > 0 || m.kr[2] > 0)
			{
				vec3f reflect_dir = (I - 2 * (I * N) * N).normalize();
				ray reflect_ray(next_ray_position, reflect_dir);
				I_result += prod(m.kr, traceRay(scene, reflect_ray, thresh, depth + 1, isInObj));
			}

			// refraction
			if (m.kt[0] > 0 || m.kt[1] > 0 || m.kt[2] > 0)
			{
				double n_i = isInObj ? m.index : 1;
				double n_t = isInObj ? 1 : m.index;
				vec3f L = -r.getDirection().normalize();

				double index_ratio = n_i / n_t;
				double discriminant = 1 - pow(index_ratio, 2) * (1 - pow(N * L, 2));

				if (discriminant > 0)
				{
					vec3f transmission_ray_dir = (index_ratio * N * L - sqrt(discriminant)) * N - index_ratio * L;
					ray transmission_ray(next_ray_position, transmission_ray_dir);
					I_result += prod(m.kt, traceRay(scene, transmission_ray, thresh, depth + 1, !isInObj));
				}
			}

		}
		//thresh; // ¸ÉÊ²Ã´ÓÃµÄ£¿
		return I_result;
	
	} else {
		// No intersection.  This ray travels to infinity, so we color
		// it according to the background color, which in this (simple) case
		// is just black.

		return vec3f( 0.0, 0.0, 0.0 );
	}
}


RayTracer::RayTracer()
{
	buffer = NULL;
	buffer_width = buffer_height = 256;
	scene = NULL;
	aalmode = NONE;

	m_bSceneLoaded = false;
}


RayTracer::~RayTracer()
{
	delete [] buffer;
	delete scene;
}

void RayTracer::getBuffer( unsigned char *&buf, int &w, int &h )
{
	buf = buffer;
	w = buffer_width;
	h = buffer_height;
}

double RayTracer::aspectRatio()
{
	return scene ? scene->getCamera()->getAspectRatio() : 1;
}

bool RayTracer::sceneLoaded()
{
	return m_bSceneLoaded;
}

bool RayTracer::loadScene( char* fn )
{
	try
	{
		scene = readScene( fn );
	}
	catch( ParseError pe )
	{
		fl_alert( "ParseError: %s\n", pe );
		return false;
	}

	if( !scene )
		return false;
	
	buffer_width = 256;
	buffer_height = (int)(buffer_width / scene->getCamera()->getAspectRatio() + 0.5);

	bufferSize = buffer_width * buffer_height * 3;
	buffer = new unsigned char[ bufferSize ];
	
	// separate objects into bounded and unbounded
	scene->initScene();
	
	// Add any specialized scene loading code here
	
	m_bSceneLoaded = true;

	return true;
}

void RayTracer::traceSetup( int w, int h )
{
	if( buffer_width != w || buffer_height != h )
	{
		buffer_width = w;
		buffer_height = h;

		bufferSize = buffer_width * buffer_height * 3;
		delete [] buffer;
		buffer = new unsigned char[ bufferSize ];
	}
	memset( buffer, 0, w*h*3 );
}

void RayTracer::traceLines( int start, int stop )
{
	vec3f col;
	if( !scene )
		return;

	if( stop > buffer_height )
		stop = buffer_height;

	for( int j = start; j < stop; ++j )
		for( int i = 0; i < buffer_width; ++i )
			tracePixel(i,j);
}

void RayTracer::tracePixel( int i, int j )
{
	vec3f col(0,0,0);

	if( !scene )
		return;

	srand((unsigned)time(0));

	switch (aalmode) {
	case NONE: {
		double x = double(i) / double(buffer_width);
		double y = double(j) / double(buffer_height);
		col = trace(scene, x, y);
		break;
	}
	case SUPER: { // ²»È¡ÖÐÐÄµã£¬È¡×óÏÂµÄµã£¬±Ï¾¹case NONEÒ²ÊÇÕâÃ´¸ÉµÄ
		double x0 = double(i) / double(buffer_width);
		double y0 = double(j) / double(buffer_height);
		int numSubPixels = traceUI->getSubPixels();
		double xdelta = 1 / double(buffer_width) / numSubPixels;
		double ydelta = 1 / double(buffer_height) / numSubPixels;
		double x[5] = { 0 }; double y[5] = { 0 };
		for (int i = 0; i < numSubPixels; ++i) {
			x[i] = x0 + i * xdelta;
			y[i] = y0 + i * ydelta;
		}
		for (int j = 0; j < numSubPixels; ++j) {
			for (int i = 0; i < numSubPixels; ++i) {
				col += trace(scene, x[i], y[j]) / pow(numSubPixels, 2);
			}
		}
		break;
	}
	case SUPER_JITTER: {
		double x0 = double(i) / double(buffer_width);
		double y0 = double(j) / double(buffer_height);
		int numSubPixels = traceUI->getSubPixels();
		double xdelta = 1 / double(buffer_width) / numSubPixels;
		double ydelta = 1 / double(buffer_height) / numSubPixels;
		for (int j = 0; j < numSubPixels; ++j) {
			for (int i = 0; i < numSubPixels; ++i) {
				double xi = x0 + (i + (rand() % 500) / 500) * xdelta;
				double yj = y0 + (j + (rand() % 500) / 500) * ydelta;
				col += trace(scene, xi, yj);
			}
		}
		col /= pow(numSubPixels, 2);
		break;
	}
	case ADAPTIVE: {
		double x0 = double(i) / double(buffer_width);
		double y0 = double(j) / double(buffer_height);
		int numSubPixels = traceUI->getSubPixels() + 1; // If numSubPixels = 1, then we cannot do subdivide, so we do +1.
		double xdelta = 1 / double(buffer_width);
		double ydelta = 1 / double(buffer_height);
		double coords[4] = { x0, y0, x0 + xdelta, y0 + ydelta };
		int recursion_depth = 0;
		col = traceSubPixel(coords, numSubPixels, recursion_depth);
	}
	}

	unsigned char *pixel = buffer + ( i + j * buffer_width ) * 3;

	pixel[0] = (int)( 255.0 * col[0]);
	pixel[1] = (int)( 255.0 * col[1]);
	pixel[2] = (int)( 255.0 * col[2]);
}

vec3f RayTracer::traceSubPixel(double* coords, int numSubPixels, int &depth) { // coords: {x0, y0, x0+dx, y0+dx}
	vec3f c[4]; // color of leftlower, rightlower, leftupper, rightupper
	c[0] = trace(scene, coords[0], coords[1]);
	c[1] = trace(scene, coords[2], coords[1]);
	c[2] = trace(scene, coords[0], coords[3]);
	c[3] = trace(scene, coords[2], coords[3]);
	// Compute max difference of 3 channels
	vec3f Max(0, 0, 0), Min(999, 999, 999);
	for (int i = 0; i < 4; ++i) {
		for (int a = 0; a < 3; ++a) {
			Max[a] = max(Max[a], c[i][a]);
			Min[a] = min(Min[a], c[i][a]);
		}
	}
	vec3f diff = Max - Min;
	double diffrgb = diff[0] + diff[1] + diff[2]; // sum of 3 channels
	if (diffrgb <= 3.0/256 || depth >= 5) { // ËÄ½ÇÍ¬É« (diffrgb < some threshold epsilon), »òµÝ¹éÌ«Éî
		return (c[0] + c[1] + c[2] + c[3]) / 4;
	}
	else {
		double dx = (coords[2] - coords[0]) / numSubPixels;
		double dy = (coords[3] - coords[1]) / numSubPixels;
		vec3f result(0, 0, 0);
		for (int j = 0; j < numSubPixels; ++j) {
			for (int i = 0; i < numSubPixels; ++i) {
				double subCoords[4] = { coords[0] + i * dx, coords[1] + j * dy, coords[0] + (i + 1) * dx, coords[1] + (j + 1) * dy };
				++depth;
				result += traceSubPixel(subCoords, numSubPixels, depth);
				--depth;
			}
		}
		result /= pow(numSubPixels, 2);
		return result;
	}
}