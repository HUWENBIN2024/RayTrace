#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "scene.h"
using namespace std;

class Light
	: public SceneElement
{
public:
	virtual vec3f shadowAttenuation(const vec3f& P) const = 0;
	virtual double distanceAttenuation( const vec3f& P ) const = 0;
	virtual vec3f getColor( const vec3f& P ) const = 0;
	virtual vec3f getDirection( const vec3f& P ) const = 0;
	string getType() const { return type; }

protected:
	Light( Scene *scene, const vec3f& col )
		: SceneElement( scene ), color( col ) {}

	vec3f 		color;
	string		type;
};

class DirectionalLight
	: public Light
{
public:
	DirectionalLight( Scene *scene, const vec3f& orien, const vec3f& color )
		: Light(scene, color), orientation(orien) {
		type = "Directional";
	}
	virtual vec3f shadowAttenuation(const vec3f& P) const;
	virtual double distanceAttenuation( const vec3f& P ) const;
	virtual vec3f getColor( const vec3f& P ) const;
	virtual vec3f getDirection( const vec3f& P ) const;

protected:
	vec3f 		orientation;
};

class PointLight
	: public Light
{
public:
	PointLight( Scene *scene, const vec3f& pos, const vec3f& color,
				double a, double b, double c)
		: Light(scene, color), position(pos), a(a), b(b), c(c) {
		type = "Point";
	}
	PointLight(Scene* scene, const vec3f& pos, const vec3f& color)
		: Light(scene, color), position(pos) {
		type = "Point";
	}
	virtual vec3f shadowAttenuation(const vec3f& P) const;
	virtual vec3f shadowAttenuation(const vec3f& P, int index) const;
	virtual double distanceAttenuation( const vec3f& P ) const;
	virtual vec3f getColor( const vec3f& P ) const;
	virtual vec3f getDirection( const vec3f& P ) const;
	virtual vec3f getDirection(const vec3f& P, vec3f newposition) const;

protected:
	vec3f position;
	double a = -1, b = 0, c = 0; // -1 is a flag
};

class AmbientLight
	: public Light
{
public:
	AmbientLight(Scene* scene, const vec3f& color)
		: Light(scene, color) {
		type = "Ambient";
	}
	virtual vec3f shadowAttenuation(const vec3f& P) const;
	virtual double distanceAttenuation(const vec3f& P) const;
	virtual vec3f getColor(const vec3f& P) const;
	virtual vec3f getDirection(const vec3f& P) const;
};

#endif // __LIGHT_H__
