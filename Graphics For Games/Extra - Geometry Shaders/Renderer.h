/*
In this code-tutorial, we're going to do two new things!

Firstly, we're going to implement a particle emitter. Particle
emitters are generally the first 'cool' thing anyone tries to 
create in OpenGL, and work by firing lots of little textured
quads around on screen. 

Secondly, we're going to create these particle quads by using
a geometry shader. Geometry shaders don't get used that much
(they aren't supported on the consoles), but have been part
of graphics hardware for a while now. We can use them to
'amplify' vertices, which in this case will allow us to turn
single vertices into quads.
*/

#pragma once

#include "../../NCLGL/OGLRenderer.h"
#include "../../NCLGL/Camera.h"
#include "ParticleEmitter.h"	//A new class!

class Renderer : public OGLRenderer	{
public:
	Renderer(Window &parent);
	virtual ~Renderer(void);

	virtual void RenderScene();
	virtual void UpdateScene(float msec);

protected:
	void	SetShaderParticleSize(float f);	//And a new setter

	ParticleEmitter*	emitter;	//A single particle emitter
	Camera*				camera;		//And the camera we're used to by now...
};

