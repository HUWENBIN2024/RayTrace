#include <cmath>
#include <assert.h>

#include "Box.h"

bool Box::intersectLocal( const ray& r, isect& i ) const
{
	// YOUR CODE HERE:
    // Add box intersection code here.
	// it currently ignores all boxes and just returns false.

	vec3f ori = r.getPosition();
	vec3f dir = r.getDirection();
	double Tenter = -INFINITY;
	double Texit = INFINITY;
	double T1, T2;
	double numerical_error = 1.0e-9;
	// for x, y, z planes: 
	for (int p = 0; p < 3; ++p)
	{
		// if the ray is parallel to a plane and the origin isn't in the box, 
		// then there won't be an intersection.
		if (dir[p] == 0 && (ori[p] < -0.5 || ori[p] > 0.5))
		{
			return false;
		}
		else
		{
			T1 = (-0.5 - ori[p]) / dir[p];
			T2 = (0.5 - ori[p]) / dir[p];
			vec3f normal_vec(0, 0, 0);
			if (min(T1, T2) > Tenter)
			{
				Tenter = min(T1, T2);
				normal_vec[p] = (T1 < T2) ? -1 : 1;
				i.N = normal_vec;
			}
			if (max(T1, T2) < Texit)
			{
				Texit = max(T1, T2);
			}
		}
	}
	i.t = (Tenter > numerical_error) ? Tenter : Texit;
	i.obj = this;
	return (Tenter < Texit && Texit > numerical_error) ? true : false;
}
