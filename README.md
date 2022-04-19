# RayTrace

## 项目描述：
**Trace是一个在简单的场景种构造递归光线跟踪图像的程序。它在功能上类似于POV-Ray射线追踪器**

## 项目来源：(CSE 457 in University of Washington)
```
https://courses.cs.washington.edu/courses/cse457/21au/src/trace/trace.php
```

## Basic Requirement:  
0. Ray Tracing 框架; Recursion Depth Control 完成
1. Ray Box Intersection 完成
2. Phong Shading Model 完成
3. Distance Attenuation 完成
4. Shadow Attenuation 完成
5. Reflection 完成
6. Refraction 完成

----------------------------------------------------------------------------

## Bonus

----------------------------------------------------------------------------

### UST COMP4411
 
1. [W] Adaptive Termination Condition Based on Ray Contribution H 完成
 
2. [W+W] Antialiasing by Supersampling and Averaging Down; +Jittering 完成

3. [B] Spot Light 聚光灯 H

4. [B] Background Image 背景图片 {[2B] 457 升级版: Background Image Box}  H

5. [B] Accelarate Shadow Attenuation

6. [B] Overlapping Objects (in Refraction) H

7. [BW+?] Texture Mapping; +Additional Mapping; +Mapping on Trimeshes {457没有} H

8. [BW] Antialiasing by Adaptive Supersampling 完成

9. [BW] More Lighting Control e.g. Warn Model {457没有}

10. [2B] Bump Mapping 凹凸贴图 H

11. [2B] Solid Textures 三维纹理 H

12. [2B] New Types of Geometry e.g. Torus, Quadric 二次方表面

13. [2B] Height Field {457没有}

14. [2B+3BW] +depth of field, +soft shadows, +motion blur, +glossy reflection {[2B+4xB+?] 457: Monte Carlo Path Tracing; +Stratified Sampling}

15. [3B] Higher Level Geometry (不能用多边形近似) {[3B+B] 457 +TextureMap on Metaball}

16. [3B] Ray Intersection Optimization. E.g. BSP Tree, Hierachical Bounding Volumes

17. [4B+/-] More Realistic Shading Model {[2B] 457}

18. [4B] Constructive Solid Geometry

19. [4B] Particle Systems Simulation & Renderer {457没有(这不应该是Animator里面的吗)}

20. [4B] Caustics

----------------------------------------------------------------------------

### UW CSE457

'R' means it is a basic requirement there

'M' means monster bell

[R] Barycentric (质心) Interpolation of Normals and Texture Coordinates in Trimesh

[R] Modified Phong Model in Specular Light

[W] Antialiasing by Sample at Random Positions within a Pixel

[W] Beer's Law (Considering Thickness in Shadow Attenuation)

[W] Fresnel Term in Reflection & Refraction (Light Intensity Depend on Angle of Intersection)

[B] In Refraction, deal with objects contained in another object

[2B] Single Image Random Dot Stereograms (可获得3D视效)

[3B] 3D Fractals (Modeler里的L-system扩展至3D)

[4B] 4D Fractals 'Sliced' by 3D

[M] Sub-surface Scattering 半透明半浑浊物体渲染，如牛奶

[M] Metropolis Light Transport 寻找contribution高的ray

[M] Photon Mapping 光子映射

----------------------------------------------------------------------------
