/******************************************************************************
Class:MD5Mesh
Implements:Mesh, MD5MeshInstance
Author:Rich Davison	<richard.davison4@newcastle.ac.uk>
Description: Implementation of id Software's MD5 skeletal animation format. 

MD5Meshes are built up of a list of 'sub meshes', built up out of a centralised
list of vertices and triangles. These lists of vertices and triangles are stored
in the MD5Mesh as MD5SubMesh structures, which feed vertex info into Meshes. 
As far as this class is concerned, the first sub mesh in the list is 'this', and 
the rest are 'children' of 'this', with the child functionality provided by the 
ChildMeshInterface class. Calling the update or skinning functions on 'this'
will update and skin all of its children, too - the entire submesh 
functionality is a complete 'black box' as far as the rest of the classes are
concerned.

MD5Mesh supports multiple animations being attached to a MD5Mesh, but only
one animation can be ran at a time (it shouldn't be /that/ hard to extend it!)

If you're going to have lots of instances of a MD5Mesh, you'd be better off
creating an MD5MeshInstance SceneNode subclass, which has a pointer to a
MD5Mesh as a member variable. This is because every MD5Mesh instantiation
will need its own VBOs and animations, so it's going to rather quickly result
in a lot of memory being used.

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*//////////////////////////////////////////////////////////////////////////////
#include "common.h"
#ifdef USE_MD5MESH
#ifdef WEEK_2_CODE
#pragma once


/*Included in the MD5 classes this year is a way of performing the skinning of vertices
on the GPU using a special vertex shader. This takes in the mesh data using what is called
a texture buffer object - a way of treating arbitrary data as a texture, so it can be 
accessed via the sampling functionality in a shader. This tends to be significantly faster
than performing everything on the GPU. 
*/

#define MD5_USE_HARDWARE_SKINNING

#include <fstream>
#include <string>
#include <map>

#include "ChildMeshInterface.h"
#include "Quaternion.h"
#include "Vector3.h"
#include "Vector2.h"

#include "Mesh.h"
#include "MD5Anim.h"
#include "MD5FileData.h"


//Let the compiler know we should compile MD5Anim along with this class
class MD5Anim;


/*
Now for the actual class definition itself. We inherit the ability to store
Mesh instances as children from the ChildMeshInterface - this is so we don't
have to replicate code in the OBJMesh class, which can also have child meshes.

MD5Mesh is also a subclass of Mesh, meaning we get access to all of the usual
Mesh stuff you've been adding in as the tutorial series goes on.
*/

class MD5Mesh : public Mesh, public ChildMeshInterface	{
public:
	//The MD5Anim class works on the data of this class. We don't want any
	//other class messing around with its internal data though, so instead
	//of making accessor functions, we declare MD5Anim a friend class, meaning
	//we give it permission to fiddle with our internals. LOL etc.
	friend class MD5Anim;
	friend class MD5FileData;
	friend class MD5Node;

	MD5Mesh(const MD5FileData&type);
	~MD5Mesh(void);

	///*
	//Draws the entire MD5Mesh, including its submeshes. Inherited from the 
	//Mesh class, overloaded to support drawing of child meshes.
	//*/
	virtual void Draw();

	/*
	To draw an MD5Mesh in a pose, it must go through the process of vertex
	skinning. This function will skin the vertices according to the passed
	in MD5Skeleton, including skinning all of its submeshes.
	*/
	void	SkinVertices(const MD5Skeleton &skel);
				
protected:	
	/////*
	////Once a skeleton has been moved to a new pose, the vertices must be
	////skinned and transformed. This means we must rebuffer the VBOs to
	////graphics memory.
	////*/
	void	RebufferData();

#ifdef MD5_USE_HARDWARE_SKINNING
	void	BufferExtraData();

	//Every vertex needs two bits of data, how many weights it has, and
	//how much influence each of those weights has. We're going to store
	//those inside a new vec2 attribute
	Vector2*			weights; 

	GLuint				weightObject;
#endif

	const MD5FileData &	type;
};
#endif
#endif