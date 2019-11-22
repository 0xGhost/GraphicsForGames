/******************************************************************************
Class:ParticleEmitter
Implements:Mesh
Author:Rich Davison
Description:A fairly simple particle emitter. This class uses GL_STREAM_DRAW
to update its VBOs every frame, and sends its data off as a set of POINTS. To
turn this points into particles, we have to use a geometry shader!

To make this class a bit more interesting, it implements what is known as a 
free-list. We have a dynamic number of particles from this emitter - sometimes
we make new ones, sometimes we kill old ones. In a naive implementation, we'd
always call new and delete on these. What we do instead, is when a particle
'dies', we keep its pointer in a 'free list', and instead of calling new, pop
off a pointer from the 'free list' - only if the free list is empty do we call
new. This has the benefit of eventually giving us a stable memory size. 

Further improvements would be to reduce the size of the free list over time - 
in cases where a particle emitter has its rate slowed down, or its launch
number reduced, we'll end up with lots of particles in the free list that will
never be popped off.

Although we're saving memory bandwidth by reducing news and deletes, there's
still a problem. Every time we render the particle emitter, we have to
copy all of our particles (who could be anywhere in memory) into one
contiguous lump of memory to send to the graphics card. Maybe just using
a vector of <Particle> rather than <Particle*> would be better, as vectors
are guaranteed to be in contiguous memory, so we could copy the lot in one go,
and use what is known as an interleaved VBO, to have both vertex position
and colour data in a single VBO. The particle class as it is, will work with
1000s of particles, so have fun optimising it to reduce memory and bandwidth!



-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""      

*//////////////////////////////////////////////////////////////////////////////


#pragma once
#include "../../NCLGL/Vector3.h"
#include "../../NCLGL/Vector4.h"
#include "../../NCLGL/OGLRenderer.h"
#include "../../NCLGL/Mesh.h"
#include <vector>

struct Particle {
	Vector3 position;
	Vector4 colour;
	Vector3 direction;
};

class ParticleEmitter : public Mesh	{
public:
	ParticleEmitter(void);
	~ParticleEmitter(void);

	/*
	To update our particle positions, we must have an update
	function - which has a msec float, just like the other 
	updating functions you've seen.
	*/
	void Update(float msec);

	virtual void Draw();

	/*
	How often we spit out some new particles!
	*/
	float	GetParticleRate()				{return particleRate;}
	void	SetParticleRate(float rate)		{particleRate = rate;}

	/*
	How long each particle lives for!
	*/
	float	GetParticleLifetime()			{return particleLifetime;}
	void	SetParticleLifetime(float life) {particleLifetime = life;}

	/*
	How big each particle will be!
	*/
	float	GetParticleSize()				{return particleSize;}
	void	SetParticleSize(float size)		{particleSize = size;}

	/*
	How much variance of the direction axis each particle can have when 
	being launched. Variance of 0 = each particle's direction is = to 
	the emitter direction. Variance of 1 = Each particle can go in
	any direction (with a slight bias towards the emitter direction)
	*/
	float	GetParticleVariance()				{return particleVariance;}
	void	SetParticleVariance(float variance) {particleVariance = variance;}

	/*
	Linear velocity of the particle
	*/
	float	GetParticleSpeed()				{return particleSpeed;}
	void	SetParticleSpeed(float speed)	{particleSpeed = speed;}

	/*
	How many particles does the emitter launch when it hits it's update time
	*/
	int		GetLaunchParticles()			{return numLaunchParticles;}
	void	SetLaunchParticles(int num)		{numLaunchParticles = num;}

	/*
	Launch direction of the particles
	*/
	void	SetDirection(const Vector3 dir) {initialDirection = dir;}
	Vector3 GetDirection()					{return initialDirection;}

protected:
	/*
	This is the magic of our free list. If there's a particle 'spare',
	this function will return that, otherwise it'll return a 'new' one
	*/
	Particle* GetFreeParticle();

	/*
	Resizes our vertex buffers
	*/
	void	ResizeArrays();

	float particleRate;
	float particleLifetime;
	float particleSize;
	float particleVariance;
	float particleSpeed;
	int	  numLaunchParticles;

	Vector3 initialDirection;

	float nextParticleTime;		//How long until we next spit out some particles?

	unsigned int largestSize;	//How large has our particle array become?

	std::vector<Particle*>	particles;	//Active particles stay in here :)
	std::vector<Particle*>	freeList;	//'Spare' particles stay in here...
};

