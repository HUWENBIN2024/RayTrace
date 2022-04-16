// The main ray tracer.

#include <Fl/fl_ask.h>
#include <time.h>
#include <math.h>

#include "RayTracer.h"
#include "scene/light.h"
#include "scene/material.h"
#include "scene/ray.h"
#include "fileio/read.h"
#include "fileio/parse.h"
#include "ui/TraceUI.h"
#include "vecmath/vecmath.h"
#include "fileio/bitmap.h"
#define  NUMERICAL_ERROR 1.0e-9
extern TraceUI* traceUI;
extern int**** randnumbers;

// Trace a top-level ray through normalized window coordinates (x,y)
// through the projection plane, and out into the scene.  All we do is
// enter the main ray-tracing method, getting things started by plugging
// in an initial ray weight of (0.0,0.0,0.0) and an initial recursion depth of 0.
vec3f RayTracer::trace( Scene *scene, double x, double y )
{
	if (traceUI->DOFisOn()) {
		vec3f result(0, 0, 0);
		int t = traceUI->getNumSampleRays();
		for (int i = 0; i < t*t; ++i) {
			ray r;
			scene->getCamera()->DOFrayThrough(x, y, r, i, t);
			result += traceRay(scene, r, vec3f(1.0, 1.0, 1.0), 0).clamp() / (t*t);
		}
		return result;
	}
	else {
		ray r(vec3f(0, 0, 0), vec3f(0, 0, 0));
		scene->getCamera()->rayThrough(x, y, r);
		return traceRay(scene, r, vec3f(1.0, 1.0, 1.0), 0).clamp();
	}
}

// Do recursive ray tracing!  You'll want to insert a lot of code here
// (or places called from here) to handle reflection, refraction, etc etc.
vec3f RayTracer::traceRay( Scene *scene, const ray& r, 
	const vec3f& thresh, int depth, bool isInObj, double intensity )
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
    
		// recursion for reflection and refraction
		if (depth < traceUI->getDepth() && (traceUI->m_Termination->value() < intensity || traceUI->m_Termination->value() == 0 ))
		{
			vec3f next_ray_position = r.at(i.t);
			vec3f I = r.getDirection().normalize();
			vec3f N = i.N.normalize();
			N = isInObj ? -N : N; // Beware of direction of N

			// reflection
			if (m.kr[0] > 0 || m.kr[1] > 0 || m.kr[2] > 0)
			{
				vec3f reflect_dir(0, 0, 0);
				if (traceUI->GlossyReflectionIsOn()) { // Glossy Reflection BW14
					// compute reflect_dir according to some distribution. apply monte carlo
					vec3f reflect_dir_c = (I - 2 * (I * N) * N).normalize(); // c means center
					vec3f perp;
					if (reflect_dir_c[0] != 0) { perp[0] = (-reflect_dir_c[1] - reflect_dir_c[2]) / reflect_dir_c[0]; perp[1] = 1; perp[2] = 1; }
					else if (reflect_dir_c[1] != 0) { perp[1] = (-reflect_dir_c[0] - reflect_dir_c[2]) / reflect_dir_c[1]; perp[0] = 1; perp[2] = 1; }
					else if (reflect_dir_c[2] != 0) { perp[2] = (-reflect_dir_c[1] - reflect_dir_c[0]) / reflect_dir_c[2]; perp[1] = 1; perp[0] = 1; }
					else { cerr << "reflect direction vector is zero vector" << endl; }
					int t = pow(traceUI->getNumSampleRays(),2);
					for (int i = 0; i < t; ++i) {
						reflect_dir = reflect_dir_c + perp.normalize() * getDistributedDistance(randnum[i], 0.03);
						mat4f random_rotate = mat4f::rotate(reflect_dir_c, getRandomAngle(randnum[i+1]));
						reflect_dir = reflect_dir.normalize();
						reflect_dir = random_rotate * reflect_dir;
						ray reflect_ray(next_ray_position, reflect_dir);
						I_result += prod(m.kr, traceRay(scene, reflect_ray, thresh, depth + 1, isInObj)) / t;
					}
				}
				else {
					reflect_dir = (I - 2 * (I * N) * N).normalize();
					ray reflect_ray(next_ray_position, reflect_dir);
					I_result += prod(m.kr, traceRay(scene, reflect_ray, thresh, depth + 1, isInObj));
				}
			}

			// refraction
			if (m.kt[0] > 0 || m.kt[1] > 0 || m.kt[2] > 0)
			{
				double n_i = isInObj ? m.index : 1;
				double n_t = isInObj ? 1 : m.index;
				vec3f L = -I;

				double index_ratio = n_i / n_t;
				double discriminant = 1 - pow(index_ratio, 2) * (1 - pow(N * L, 2));

				if (discriminant > 0)
				{
					vec3f transmission_ray_dir = (index_ratio * N * L - sqrt(discriminant)) * N - index_ratio * L;
					ray transmission_ray(next_ray_position, transmission_ray_dir);
					I_result += prod(m.kt, traceRay(scene, transmission_ray, thresh, depth + 1, !isInObj, (m.kt[0] + m.kt[1] + m.kt[2]) / 3 * intensity));
				}
			}

		}
		//thresh; // ¸ÉÊ²Ã´ÓÃµÄ£¿
		return I_result;
	
	} else {
		if (traceUI->BGisOn()) {
			unsigned char* bg;
			double x = r.getDirection()[0], y = r.getDirection()[1], z = r.getDirection()[2];
			double xn, yn;
			if (abs(z) >= abs(x) && abs(z) >= abs(y)) {
				if (z > 0) {
					bg = bgBox[0];
					xn = (-x / abs(z)) / 2 + 0.5;
					yn = (y / abs(z)) / 2 + 0.5;
				}
				else {
					bg = bgBox[1];
					xn = (-x / abs(z)) / 2 + 0.5;
					yn = (-y / abs(z)) / 2 + 0.5;
				}
			}
			if (abs(x) >= abs(y) && abs(x) >= abs(z)) {
				if (x > 0) {
					bg = bgBox[2];
					xn = (-y / abs(x)) / 2 + 0.5;
					yn = (z / abs(x)) / 2 + 0.5;
				}
				else {
					bg = bgBox[3];
					xn = (y / abs(x)) / 2 + 0.5;
					yn = (z / abs(x)) / 2 + 0.5;
				}
			}
			if (abs(y) >= abs(x) && abs(y) >= abs(z)) {
				if (y > 0) {
					bg = bgBox[5];
					xn = (x / abs(y)) / 2 + 0.5;
					yn = (z / abs(y)) / 2 + 0.5;
				}
				else {
					bg = bgBox[4];
					xn = (-x / abs(y)) / 2 + 0.5;
					yn = (z / abs(y)) / 2 + 0.5;
				}
			}
			int xc = int(xn * bgSize), yc = int(yn * bgSize);
			int indx = 3 * (min(yc,bgSize-1) * bgSize + min(xc,bgSize-1));
			return vec3f(double(bg[indx]) / 255, double(bg[indx + 1]) / 255, double(bg[indx + 2]) / 255);
		}
		else {
			return vec3f(0, 0, 0);
		}
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

	vec3f** RRAS = new vec3f*[stop+1]; // RRAS means adaptive result record for adaptive supersampling
	for (int j = start; j < stop+1; ++j) { // Initialization with (-1,-1,-1)
		RRAS[j] = new vec3f[buffer_width+1];
		for (int i = 0; i < buffer_width+1; ++i) {
			RRAS[j][i] = vec3f(-1, -1, -1);
		}
	}

	for( int j = start; j < stop; ++j )
		for( int i = 0; i < buffer_width; ++i )
			tracePixel(i,j,RRAS);

	for (int j = start; j < stop+1; ++j) { // deletion
		delete[] RRAS[j];
	}
	delete[] RRAS;
	RRAS = nullptr;
}

void RayTracer::tracePixel( int i, int j , vec3f** RRAS)
{
	randnum = randnumbers[j][i][0];
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
	case SUPER: {
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
				randnum = randnumbers[j][i][j * numSubPixels + i];
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
				double xi = x0 + (i + double(rand() % 500) / 500) * xdelta;
				double yj = y0 + (j + double(rand() % 500) / 500) * ydelta;
				randnum = randnumbers[j][i][j * numSubPixels + i];
				col += trace(scene, xi, yj);
			}
		}
		col /= pow(numSubPixels, 2);
		break;
	}
	case ADAPTIVE: {
		double x0 = double(i) / double(buffer_width);
		double y0 = double(j) / double(buffer_height);
		double xdelta = 1 / double(buffer_width);
		double ydelta = 1 / double(buffer_height);
		double coords[4] = { x0, y0, x0 + xdelta, y0 + ydelta };
		vec3f mns1(-1, -1, -1);
		if (RRAS[j][i] == mns1) RRAS[j][i] = trace(scene, coords[0], coords[1]);
		if (RRAS[j][i+1]==mns1) RRAS[j][i+1]=trace(scene, coords[2], coords[1]);
		if (RRAS[j+1][i]==mns1) RRAS[j+1][i]=trace(scene, coords[0], coords[3]);
		RRAS[j+1][i+1] = trace(scene, coords[2], coords[3]); // we are sure that right upper has not been computed
		vec3f c[4]; // color of leftlower, rightlower, leftupper, rightupper
		c[0] = RRAS[j][i];
		c[1] = RRAS[j][i + 1];
		c[2] = RRAS[j + 1][i];
		c[3] = RRAS[j + 1][i + 1];
		int recursion_depth = 0;
		// Here we fix no. of subpixels to be 2.
		col = traceSubPixel(c, coords, recursion_depth);
	}
	}

	unsigned char *pixel = buffer + ( i + j * buffer_width ) * 3;

	pixel[0] = (int)( 255.0 * col[0]);
	pixel[1] = (int)( 255.0 * col[1]);
	pixel[2] = (int)( 255.0 * col[2]);
}

vec3f RayTracer::traceSubPixel(vec3f* c, double* coords, int& depth) { // coords: {x0, y0, x0+dx, y0+dx}
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
	if (diffrgb <= 3.0/256 || depth >= 5) { // same colors (diffrgb < some threshold epsilon), or depth too large
		return (c[0] + c[1] + c[2] + c[3]) / 4;
	}
	else {
		double x0 = coords[0];
		double y0 = coords[1];
		double dx = (coords[2] - coords[0]) / 2;
		double dy = (coords[3] - coords[1]) / 2;
		vec3f result(0, 0, 0);
		double new_coords[4][4] = {  {x0,y0,x0 + dx,y0 + dy},
									{x0 + dx,y0,x0 + 2 * dx,y0 + dy},
									{x0,y0 + dy,x0 + dx,y0 + 2 * dy},
									{x0 + dx,y0 + dy,x0 + 2 * dx,y0 + 2 * dy} };
		vec3f c_up = trace(scene, x0 + dx, y0 + 2 * dy);
		vec3f c_down = trace(scene, x0 + dx, y0);
		vec3f c_left = trace(scene, x0, y0 + dy);
		vec3f c_right = trace(scene, x0 + 2 * dx, y0 + dy);
		vec3f c_center = trace(scene, x0 + dx, y0 + dy);
		vec3f new_c[4][4] = { {c[0],c_down,c_left,c_center},{c_down,c[1],c_center,c_right},
								{c_left,c_center,c[2],c_up},{c_center,c_right,c_up,c[3]} };
		for (int i = 0; i < 4; ++i) {
			++depth;
			result += traceSubPixel(new_c[i], new_coords[i], depth);
			--depth;
		}
		result /= 4;
		return result;
	}
}

void RayTracer::setBg(string dir) {
	string files[6] = { dir + "up.bmp",dir + "down.bmp",dir + "left.bmp",dir + "right.bmp",dir + "front.bmp",dir + "back.bmp" };
	for (int i = 0; i < 6; ++i) {
		const char* fname = files[i].data();
		int width; int height;
		bgBox[i] = readBMP(fname, width, height);
		bgSize = width;
	}
}