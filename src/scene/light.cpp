#include <cmath>

#include "light.h"
#include "../ui/TraceUI.h"
extern TraceUI* traceUI;

double DirectionalLight::distanceAttenuation( const vec3f& P ) const
{
	// distance to light is infinite, so f(di) goes to 0.  Return 1.
	return 1.0;
}

vec3f DirectionalLight::shadowAttenuation( const vec3f& P ) const
{
    // YOUR CODE HERE:
    // You should implement shadow-handling code here.
	ray r(P, getDirection(P)); isect i;
	vec3f s_atten(1, 1, 1);
	if (false) { //BW5 Accelerate Shadow Attenuation
		if (scene->willIntersectOpaqueObject(r, i)) return vec3f(0, 0, 0);
	}
	if (scene->intersect(r, i)) {
		if (i.getMaterial().kt != vec3f(0, 0, 0)) { // Speed up the algorithm
			s_atten = prod(s_atten, i.getMaterial().kt);
			s_atten = prod(s_atten, shadowAttenuation(r.at(i.t))); // recursive
		}
		else {
			s_atten = vec3f(0, 0, 0);
		}
	}
    return s_atten;
}

vec3f DirectionalLight::getColor( const vec3f& P ) const
{
	// Color doesn't depend on P 
	return color;
}

vec3f DirectionalLight::getDirection( const vec3f& P ) const
{
	return -orientation;
}

double PointLight::distanceAttenuation( const vec3f& P ) const
{
	// YOUR CODE HERE
	double d = pow((position - P) * (position - P), 0.5);
	d *= traceUI->getDistScale();
	double epsilon = 0.01;
	// You'll need to modify this method to attenuate the intensity 
	// of the light based on the distance between the source and the 
	// point P.  For now, I assume no attenuation and just return 1.0
	if (a == -1) { // coeffs not from .ray file
		double* aa = traceUI->getDACoeff();
		return min(1, 1 / aa[0] + aa[1] * d + aa[2] * d * d); // Avoid negative or infinity result
	}
	else {// coeffs from .ray file
		return min(1, 1 / a + b * d + c * d * d); // Avoid negative or infinity result
	}
}

vec3f PointLight::getColor( const vec3f& P ) const
{
	// Color doesn't depend on P 
	return color;
}

vec3f PointLight::getDirection( const vec3f& P ) const
{
	return (position - P).normalize();
}

vec3f PointLight::getDirection(const vec3f& P, vec3f newposition) const
{
	return (newposition - P).normalize();
}

vec3f PointLight::shadowAttenuation(const vec3f& P) const
{
	ray r; isect i;
	r = ray(P, getDirection(P));
	double src_t = (position - P)[0] / getDirection(P)[0];
	if (position == P) src_t = 0;
	vec3f s_atten(1, 1, 1);
	if (false) { //BW5 Accelerate Shadow Attenuation
		if (scene->willIntersectOpaqueObject(r, i)) {
			if (i.t < src_t) {
				return vec3f(0, 0, 0);
			}
		}
	}
	if (scene->intersect(r, i)) {
		if (i.t >= src_t && src_t >= 0) {
			return s_atten;
		}
		else if (i.t >= src_t && src_t < 0) {
			return vec3f(0, 0, 0);
		}
		else {
			s_atten = prod(s_atten, i.getMaterial().kt);
			s_atten = prod(s_atten, shadowAttenuation(r.at(i.t))); // recursive
			return s_atten;
		}
	}
	else {
		if (src_t >= 0) {
			return s_atten;
		}
		else {
			return vec3f(0, 0, 0);
		}
	}
}

vec3f PointLight::shadowAttenuation(const vec3f& P, int index) const
{
    // YOUR CODE HERE:
    // You should implement shadow-handling code here.
	vec3f newposition; ray r; isect i;

		vec3f dir(0, 0, 0);
		vec3f dir_c = (position - P); // c means center
		vec3f perp(0, 0, 0);
		if (dir_c[0] != 0) { perp[0] = (-dir_c[1] - dir_c[2]) / dir_c[0]; perp[1] = 1; perp[2] = 1; }
		else if (dir_c[1] != 0) { perp[1] = (-dir_c[0] - dir_c[2]) / dir_c[1]; perp[0] = 1; perp[2] = 1; }
		else if (dir_c[2] != 0) { perp[2] = (-dir_c[1] - dir_c[0]) / dir_c[2]; perp[1] = 1; perp[0] = 1; }
		else { cerr << "zero vector" << endl; }
		int* randnum = traceUI->getRayTracer()->randnum;
		dir = dir_c + perp.normalize() * getDistributedDistance(randnum[index], 0.1);
		mat4f random_rotate = mat4f::rotate(dir_c.normalize(), getRandomAngle(randnum[index + 1]));
		dir = random_rotate * dir;
		newposition = dir + P;

	r = ray(P, getDirection(P, newposition));
	double src_t = (newposition - P)[0] / getDirection(P, newposition)[0];
	if (newposition == P) src_t = 0;
	vec3f s_atten(1, 1, 1);
	if (false) { //BW5 Accelerate Shadow Attenuation
		if (scene->willIntersectOpaqueObject(r, i)) {
			if (i.t < src_t) {
				return vec3f(0, 0, 0);
			}
		}
	}
	if (scene->intersect(r, i)) {
		if (i.t >= src_t && src_t >= 0) {
			return s_atten;
		}
		else if (i.t >= src_t && src_t < 0) {
			return vec3f(0, 0, 0);
		}
		else {
			s_atten = prod(s_atten, i.getMaterial().kt);
			s_atten = prod(s_atten, shadowAttenuation(r.at(i.t), index)); // recursive
			return s_atten;
		}
	}
	else {
		if (src_t >= 0) {
			return s_atten;
		}
		else {
			return vec3f(0, 0, 0);
		}
	}
}

double AmbientLight::distanceAttenuation(const vec3f& P) const
{
	return 1.0;
}


vec3f AmbientLight::shadowAttenuation(const vec3f& P) const
{
	return vec3f(1, 1, 1);
}

vec3f AmbientLight::getColor(const vec3f& P) const
{
	// Color doesn't depend on P 
	return color;
}

vec3f AmbientLight::getDirection(const vec3f& P) const
{
	return vec3f(0,0,0);
}