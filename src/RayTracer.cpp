// The main ray tracer.

#include <Fl/fl_ask.h>

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
		//thresh; // 干什么用的？
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
	vec3f col;

	if( !scene )
		return;

	double x = double(i)/double(buffer_width);
	double y = double(j)/double(buffer_height);

	col = trace( scene,x,y );

	unsigned char *pixel = buffer + ( i + j * buffer_width ) * 3;

	pixel[0] = (int)( 255.0 * col[0]);
	pixel[1] = (int)( 255.0 * col[1]);
	pixel[2] = (int)( 255.0 * col[2]);
}