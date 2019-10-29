/******************************************************************************
Class:MD5Anim
Implements:
Author:Rich Davison	<richard.davison4@newcastle.ac.uk>
Description: Implementation of id Software's MD5 skeletal animation format.

MD5Anims are slightly less complicated than MD5Meshes. They consist of a 
'base frame', which is like a bind pose for an animation, and a number of
animation frames, each of which has a number of values in it, equating to the
differences between this frame of animation and the base frame. Depending on
the software used to export the MD5Anim, the baseframe might be 'empty',
meaning each frame consist of every transform for every joint.

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*//////////////////////////////////////////////////////////////////////////////
#include "common.h"
#ifdef USE_MD5MESH
#ifdef WEEK_2_CODE
#pragma once

#include <fstream>
#include <string>

#include "quaternion.h"
#include "Vector3.h"

#include "MD5Mesh.h"
#include "MD5FileData.h"


/*
MD5 Files are plain text, with each section in the file marked with tags.
It's good practice to use defines to keep these tag strings, even though
most of them will only actually be used once.
*/
#define MD5_VERSION_TAG			"MD5Version"
#define MD5_COMMANDLINE_TAG		"commandline"
#define MD5_NUMJOINTS_TAG		"numJoints"
#define MD5_ANIM_NUMFRAMES		"numFrames"
#define MD5_ANIM_FRAMERATE		"frameRate"
#define MD5_ANIM_COMPONENTS		"numAnimatedComponents"
#define MD5_ANIM_HIERARCHY		"hierarchy"
#define MD5_ANIM_BOUNDS			"bounds"
#define MD5_ANIM_BASEFRAME		"baseframe"
#define MD5_ANIM_FRAME			"frame"


/*
Each MD5 anim frame has the differences between the baseframe and itself,
stored as an array of floating points. So if the x axis position of a joint
changes in a frame, the joints position.x is replaced with a float from the
array. Whether a frame changes a joint's position is determined by a mask,
which is ANDed against using the following defines, where a value of > 1
means the component changes.
*/
#define MD5_ANIM_XPOS			1
#define MD5_ANIM_YPOS			2
#define MD5_ANIM_ZPOS			4

#define MD5_ANIM_XQUAT			8
#define MD5_ANIM_YQUAT			16
#define MD5_ANIM_ZQUAT			32

/*
Every MD5Anim has a number of MD5AnimJoints. These are essentially the
same as MD5Mesh joints, with an added bitmask, which determines which
components of the joint, if any, are updated in this animation.
*/
struct MD5AnimJoint{
	std::string  name;	//Name of this joint
	int parent;			//Index of the parent of this joint
	int flags;			//bitmask used to determine which components update
	int frameIndex;		//First float in the changes array that effects joint
};

/*
Every MD5Anim frame has an axis aligned bounding box, stretching from min
to max. Useful for collision detection etc.
TODO: These haven't actually been transformed into the correct axis yet!
*/
struct MD5Bounds {
	Vector3 min;
	Vector3 max;
};

/*
Every MD5Anim has an MD5BaseFrame, consisting of the orientations and positions
of every MD5AnimJoint
*/
struct MD5BaseFrame {
	Quaternion* orientations;	//Orientations for every base frame joint
	Vector3*	positions;		//Positions for every base frame joint

	MD5BaseFrame::MD5BaseFrame() {
		orientations = NULL;
		positions    = NULL;
	}

	//Delete our heap memory
	~MD5BaseFrame() {
		delete[] orientations;
		delete[] positions;
	};
};

/*
Every MD5Anim is made up of a number of MD5Frame structs - one for each
frame of the animation. It consists of an array of floats, which equate
to the orientation and position changes from the baseframe for the current
animation frame. Which component equates to each frame 'delta' is determined
by the flags variable of the MD5AnimJoint
*/
struct MD5Frame {
	float* components;

	MD5Frame::MD5Frame() {
		components = NULL;
	}

	~MD5Frame() {
		delete[] components;
	};
};

//Tell the compiler that we need the MD5Skeleton structure compiled
struct MD5Skeleton;

/*
Now for the class definition. Every MD5Anim has a number of joints
(which should equal the number of joints of the MD5Mesh it is to be applied to)
a number of frames of animation, an axis-aligned bounding box for every frame, 
and a baseFrame.
*/
class MD5Anim	{
public:
	//Constructor takes in a filename to load the MD5Anim data from
	MD5Anim(std::string filename);
	~MD5Anim(void);

	//Transforms the passed in skeleton to the correct positions and
	//orientations for the desired frame
	void	TransformSkeleton(MD5Skeleton &skel,  unsigned int frame);

	//Returns the framerate
	unsigned int	GetFrameRate() {return frameRate;}
	//Returns the number of frames of animation
	unsigned int	GetNumFrames() {return numFrames;}

protected:
	//Helper function used by the constructor to load in an MD5Anim from the 
	//relevent file
	void	LoadMD5Anim(std::string filename);

	//Helper function for LoadMD5Anim to load in the joints
	void	LoadMD5AnimHierarchy(std::ifstream &from, unsigned int &count);

	//Helper function for LoadMD5Anim to load in the bounding boxes
	void	LoadMD5AnimBounds(std::ifstream &from,unsigned int &count );

	//Helper function for LoadMD5Anim to load in the base frame
	void	LoadMD5AnimBaseFrame(std::ifstream &from);

	//Helper function for LoadMD5Anim to load in animation frames
	void	LoadMD5AnimFrame(std::ifstream &from, unsigned int &count);

	unsigned int	frameRate;		//Required framerate of this animation
	unsigned int	numJoints;		//Number of joints in this animation
	unsigned int	numFrames;		//Number of frames in this animation
	unsigned int	numAnimatedComponents; //Number of transform differences per frame

	MD5AnimJoint*	joints;			//Array of joints for this animation
	MD5Bounds*		bounds;			//Array of bounding boxes for this animation
	MD5Frame*		frames;			//Array of individual frames for this animation
	MD5BaseFrame	baseFrame;		//BaseFrame for this animation
};
#endif
#endif