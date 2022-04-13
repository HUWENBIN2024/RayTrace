#include "ray.h"
#include "material.h"
#include "light.h"
#define PI 3.14159265358979323846

// Apply the phong model to this point on the surface of the object, returning
// the color of that point.
vec3f Material::shade( Scene *scene, const ray& r, const isect& i ) const
{
	// YOUR CODE HERE
	vec3f I_result(0, 0, 0);

	// k_e
	I_result += ke;

	// Others
	for (Scene::cliter itr = scene->beginLights(); itr != scene->endLights(); ++itr) {
		vec3f isectPos = r.at(i.t);
		// k_a; Normally, there is only one ambient light.
		if ((*itr)->getType() == "Ambient") {
			I_result += prod((vec3f(1,1,1)-kt),prod(ka, (*itr)->getColor(isectPos)));
			continue;
		}
		// distance attenuation
		double d_atten = (*itr)->distanceAttenuation(isectPos);
		// shadow attenuation
		vec3f s_atten = (*itr)->shadowAttenuation(isectPos);
		// k_d
		vec3f L_i = (*itr)->getDirection(isectPos);
		vec3f diffuse = max(0.0, (i.N * L_i)) * kd;
		// k_s
		mat4f Rfl = mat4f::rotate(i.N, PI);
		vec3f V = Rfl * L_i; V = V.normalize();
		vec3f specular = pow(max(0.0, (-r.getDirection())*V),128*shininess) * ks;
		// final
		I_result += prod((vec3f(1, 1, 1) - kt), d_atten * prod(prod(s_atten, (*itr)->getColor(isectPos)),(diffuse+specular)));
	}

	// For now, this method just returns the diffuse color of the object.
	// This gives a single matte color for every distinct surface in the
	// scene, and that's it.  Simple, but enough to get you started.
	// (It's also inconsistent with the phong model...)

	// Your mission is to fill in this method with the rest of the phong
	// shading model, including the contributions of all the light sources.
    // You will need to call both distanceAttenuation() and shadowAttenuation()
    // somewhere in your code in order to compute shadows and light falloff.

	return I_result;
}
