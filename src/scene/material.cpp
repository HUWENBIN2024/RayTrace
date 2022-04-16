#include "ray.h"
#include "material.h"
#include "light.h"
#include "../ui/TraceUI.h"
#define PI 3.14159265358979323846

extern TraceUI* traceUI;

// Apply the phong model to this point on the surface of the object, returning
// the color of that point.
vec3f Material::shade(Scene* scene, const ray& r, const isect& i) const
{
	// YOUR CODE HERE
	vec3f I_result(0, 0, 0);

	// k_e
	I_result += ke;

	// k_a
	I_result += prod(ka, traceUI->getAmbientLight());

	// Others
	for (Scene::cliter itr = scene->beginLights(); itr != scene->endLights(); ++itr) {
		vec3f isectPos = r.at(i.t);
		// extra ambient light; Normally, there is only one ambient light.
		if ((*itr)->getType() == "Ambient") {
			I_result += prod((vec3f(1, 1, 1) - kt), prod(ka, (*itr)->getColor(isectPos)));
			continue;
		}
		// distance attenuation
		double d_atten = (*itr)->distanceAttenuation(isectPos);
		// shadow attenuation
		vec3f s_atten(0,0,0);
		if (traceUI->SoftShadowIsOn() && (*itr)->getType() == "Point") {
			int t = pow(traceUI->getNumSampleRays(), 2);
			for (int i = 0; i < t; ++i) {
				s_atten += dynamic_cast<PointLight*>(*itr)->shadowAttenuation(isectPos, i) / t;
			}
		}
		else {
			s_atten = (*itr)->shadowAttenuation(isectPos);
		}
		// k_d
		vec3f L_i = (*itr)->getDirection(isectPos);
		vec3f diffuse = max(0.0, (i.N * L_i)) * kd;
		diffuse = prod(diffuse, (vec3f(1, 1, 1) - kt));
		// k_s
		vec3f V = L_i - 2 * (L_i * i.N) * i.N;
		vec3f specular = pow(max(0.0, (-r.getDirection()) * V), 128 * shininess) * ks;
		// final
		I_result += d_atten * prod(prod(s_atten, (*itr)->getColor(isectPos)), (diffuse + specular));
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