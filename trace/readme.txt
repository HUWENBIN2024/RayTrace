This file will illustrate how to use this demo program

First things first,this program is only for demo purpose,note that the solution in this program may not be the standard one, many bonuses should have multiple solutions.

If you think the scene is too dark,please adjust the attenuation slider and the ambient light slider

1.Adaptive termination
load some complex scene(e.g. 'recurse_depth.ray' in basic file) ,adjust the Treshold slider,render the scene again and see what happened.

2.Antialiasing by supersampling and averaging down and jittering
Load an arbitrary .ray file and enable 'Super Samp' button before rendering, you can also enable jittering by light up 'Jitter' Button.The 'Number of sup-pixel' slider is used to indicate number of sub-pixel samples being used (see bonus description in course website)

3.Spot lights
Simply load spotlight.ray file

4.Background image
Load testbackground.ray file first,click the 'file' drop down list and load background image, light up the 'Background' button and render, make sure the value of 'Ddepth' slider is more than 1 and 'Threshold' slider is zero, the 'ambient light' slider is not too high(medium should be ok)

5.Accelerate shadow attenuation
This program use octree, you will see it soon

6.Overlapping objects
Load refra_overlapping.ray

7.Texture mapping
Load earth.ray, then load texture image(should be 'worldmap.bmp'),enable the 'texture' button and render

8.Antialiasing by adaptive supersampling
Simply enable the 'Adapt Super' button before rendering,if you want to compare the performance with super sampling by averaging down, you should first increase the value of 'Number of sup-pixel' slider

9.Warn model
Load shapeLight.ray

10.Bump mapping
Load map.ray or wall.ray(simply a white board which has the same aspect ratio with corresponding texture image),load texture image(should be worldmap.bmp or wall.bmp),then load  worldmap_normal.bmp or wall_normal.bmp.First, you can only enable 'Bump mapping' button without enabling 'Texture' button and see the effect,in this mode only the normal is perturbed. Then you can enable both two buttons to see the effect

11.New types of geometry
Load paraboloid.ray or hyperbolic.ray

12.Height-fields
First,load blank.ray(a scene with only some light),then load HField map(should be hf_512_.bmp or hf_854_.bmp) and the corresponding grey map will be automatically loaded. You need to enable 'Octree' before rendering,otherwise it will take so long time to render the scene.

13.Depth of field, soft shadows, motion blur, or glossy reflection
Corresponding .ray file and button to enable the effect:
a)Depth of field:depthOfField.ray, 'DOF' button, 'Focal Length' and 'Aperture Size' slider
b)Soft shadows:softShadow.ray,'soft shadow' button
c)Motion blur:threeBalls.ray,'Motion' button
d)Glossy reflection: glossyReflection.ray,'Glossy' button

14.Higher-level geometry
Here I implement transparent metaballs.Load metaball.ray(you can modify the .ray file to add or decrease the number of balls) and make sure you enable the octree button before rendering

15.Optimization
Here I use Octree.You make already see the effect.

16.CSG
Load csg.ray,again you can modify the .ray file to construct other types of geometry

17.Particle systems
Simply load meteorite.ray,it will take a while to render because my octree cannot support this.

18.Caustics
Load ring.ray,click 'Init Caustics' button and enable 'Enable Caustics' button.It`s a pity that I fail to make it support refraction because time is limited. Hope you can do better than me :)