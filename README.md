# RayTrace

## Basic Requirement:

0. Ray Tracing 框架; Recursion Depth Control
1. Ray Box Intersection
2. Phong Shading Model 完成
3. Distance Attenuation 完成
4. Shadow Attenuation 完成
5. Reflection
6. Refraction

----------------------------------------------------------------------------

## Bonus

----------------------------------------------------------------------------

### UST COMP4411

[W] Adaptive Termination Condition Based on Ray Contribution

[W+W] Antialiasing by Supersampling and Averaging Down; +Jittering

[B] Spot Light 聚光灯

[B] Background Image 背景图片 {[2B] 457 升级版: Background Image Box}

[B] Accelarate Shadow Attenuation

[B] Overlapping Objects (in Refraction)

[BW+?] Texture Mapping; +Additional Mapping; +Mapping on Trimeshes {457没有}

[BW] Antialiasing by Adaptive Supersampling

[BW] More Lighting Control e.g. Warn Model {457没有}

[2B] Bump Mapping 凹凸贴图

[2B] Solid Textures 三维纹理

[2B] New Types of Geometry e.g. Torus, Quadric 二次方表面

[2B] Height Field {457没有}

[2B+3BW] +depth of field, +soft shadows, +motion blur, +glossy reflection {[2B+4xB+?] 457 升级版: Monte Carlo Path Tracing; +Stratified Sampling(分层抽样)}

[3B] Higher Level Geometry (不能用多边形近似) {[3B+B] 457 +TextureMap on Metaball}

[3B] Ray Intersection Optimization. E.g. BSP Tree, Hierachical Bounding Volumes

[4B+/-] More Realistic Shading Model {[2B] 457}

[4B] Constructive Solid Geometry

[4B] Particle Systems Simulation & Renderer {457没有(这不应该是Animator里面的吗)}

[4B] Caustics

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
