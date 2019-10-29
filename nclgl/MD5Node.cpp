#include "MD5Node.h"
#ifdef USE_MD5MESH
#ifdef WEEK_2_CODE
MD5Node::MD5Node(const MD5FileData &ofType) : sourceData(ofType)	{
	currentAnim		 = NULL;
	frameTime		 = 0.0f;
	currentAnimFrame = 0;

	ofType.CloneSkeleton(currentSkeleton);

	mesh = ofType.GetRootMesh();
}

MD5Node::~MD5Node(void)	{

}

/*
This Overridden Update function will update the current animation frame used
by this particular mesh instance, by subtracting the incoming time value
by a float /frameTime/, which when less than 0, triggers a frame update. This
float is then reset to the framerate defined within the current animation. A
while is used instead of an if for the unusual occurance of a time value being
large enough that we could risk skipping over frames - generally this only 
happens when we hit a debug breakpoint.

*/
void	MD5Node::Update(float msec) {
	if(currentAnim) {
		frameTime -= msec;
		//Time to calculate which frame we're now on!
		while(frameTime < 0) {
			frameTime += 1000.0f / currentAnim->GetFrameRate();
			//all animations are assumed to be 'looping', so we use the modulo
			//operator to 'wrap around' if we go past the end of the anim
			currentAnimFrame = currentAnimFrame++%(currentAnim->GetNumFrames());
		}
		//Transform this particular node's skeleton to the right frame of
		//anim
		currentAnim->TransformSkeleton(currentSkeleton,currentAnimFrame-1);
	}
	//Call our base class update function, too! Doing so will presever the 
	//ability to build up the world matrices for every node. 
	SceneNode::Update(msec);
}



/*
Swaps the currently used animation of this MD5Mesh. 
*/
void	MD5Node::PlayAnim(std::string name)	{
/*
We want to reset all of the animation details
*/
	currentAnimFrame	= 0;
	frameTime			= 0.0f; 
	currentAnim			= sourceData.GetAnim(name);
}

void	MD5Node::Draw(const OGLRenderer &r) {
	MD5Mesh*m = (MD5Mesh*)mesh;

	/*
	I have added experimental support for performing skinning inside the vertex
	shader, in order for everyone to see one way of getting lots of data into
	'shader space'. To do so, I use an OpenGL feature called a Texture Buffer
	Object, which allows a VBO to be 'seen' as texture data inside a shader.

	I store the current skeleton state inside a VBO, and then bind this VBO
	as a TBO, allowing the vertex shader to see the current skeleton state.

	I also store the mesh weights and anchor transforms inside a TBO, too, so
	that the shader can access all of the data necessary to update the vertex
	to the correct local position. This is a slight change in method to the
	'use lots of uniforms' method outlined in the tutorial writeup, but it is
	still worth considering the uniform method, as the current consoles do not
	support the arbitrary data lookup method used by OpenGL TBOs...
	*/

#ifdef MD5_USE_HARDWARE_SKINNING
	sourceData.BindTextureBuffers();
	sourceData.UpdateTransformTBO(currentSkeleton);

	glUniform1i(glGetUniformLocation(r.GetCurrentShader()->GetProgram(), "weightTex"), MD5_WEIGHT_TEXNUM);
	glUniform1i(glGetUniformLocation(r.GetCurrentShader()->GetProgram(), "transformTex"), MD5_TRANSFORM_TEXNUM);
#else 
	/*
	If we're doing 'software' skinning, then we need to make sure the global mesh
	data is in the correct position for this node before we draw it, which we do
	by calling the 'skin vertices' function of the MD5Mesh, passing it our node's
	current skeleton, which will have been updated in the Update function to be in
	the correct pose for the current frame of animation. 
	*/
	m->SkinVertices(currentSkeleton);
#endif
	//Finally, we draw the mesh, just like the base class Draw function...
	m->Draw();
}


bool	MD5Node::GetJointWorldTransform(const string&name, Matrix4 &t) {
	int index = sourceData.GetIndexForJointName(name);
	if (index < 0) {
		return false;
	}

	t = currentSkeleton.joints[index].transform;

	return true;
}

bool	MD5Node::GetJointLocalTransform(const string&name, Matrix4 &t) {
	int index = sourceData.GetIndexForJointName(name);
	if (index < 0) {
		return false;
	}

	t = currentSkeleton.joints[index].localTransform;

	return true;
}


bool	MD5Node::GetParentLocalOrientation(const string&name, Quaternion &t) {
	int index = sourceData.GetIndexForJointName(name);
	if (index < 0) {
		return false;
	}
	int pIndex = currentSkeleton.joints[index].parent;
	if (index < 0) {
		return false;
	}

	t = currentSkeleton.joints[pIndex].orientation;

	return true;
}

bool	MD5Node::GetParentWorldOrientation(const string&name, Quaternion &t) {
	return false;
}


bool	MD5Node::GetParentLocalTransform(const string&name, Matrix4 &t) {
	int index = sourceData.GetIndexForJointName(name);
	if (index < 0) {
		return false;
	}
	int pIndex = currentSkeleton.joints[index].parent;
	if (index < 0) {
		return false;
	}

	t = currentSkeleton.joints[pIndex].localTransform;

	return true;
}
bool	MD5Node::GetParentWorldTransform(const string&name, Matrix4 &t) {
	int index = sourceData.GetIndexForJointName(name);
	if (index < 0) {
		return false;
	}
	int pIndex = currentSkeleton.joints[index].parent;
	if (index < 0) {
		return false;
	}

	t = currentSkeleton.joints[pIndex].transform;

	return true;
}



bool	MD5Node::SetJointLocalTransform(const string &name, Matrix4 &t) {
	int index = sourceData.GetIndexForJointName(name);
	if (index < 0) {
		return false;
	}

	currentSkeleton.joints[index].localTransform = t;

	return true;
}

bool	MD5Node::SetJointWorldTransform(const string &name, Matrix4 &t) {
	int index = sourceData.GetIndexForJointName(name);
	if (index < 0) {
		return false;
	}

	currentSkeleton.joints[index].transform = t;
	currentSkeleton.joints[index].forceWorld = 1;
	return true;
}

void MD5Node::ApplyTransformsToHierarchy(int startingNode) {
	for (int i = startingNode; i < currentSkeleton.numJoints; ++i) {
		MD5Joint &j = currentSkeleton.joints[i];

		MD5Joint*p = NULL;
		
		if (j.parent >= 0) {
			p = &currentSkeleton.joints[j.parent];

			if (!j.forceWorld) {
				j.transform = p->transform * j.localTransform;
			}
			else {
				bool a = true;
			}
		}
		else {

			j.transform = MD5FileData::conversionMatrix * j.localTransform;
		}
	}
}

void	MD5Node::DebugDrawSkeleton() {
		//Temporary VAO and VBO
		unsigned int skeletonArray;
		unsigned int skeletonBuffer;
	
		glGenVertexArrays(1, &skeletonArray);
		glGenBuffers(1, &skeletonBuffer);
	
		//Temporary chunk of memory to keep our joint positions in
		Vector3*	 skeletonVertices = new Vector3[currentSkeleton.numJoints*2];
	
	
		/*
		Now for each joint we're going to have a pair of vertices - one at the joint position,
		and one at the joint's parent's position. This'll let us draw lines to show the skeletal
		shape. There'll be a bit of overdraw, which could be avoided by using indices. but this way
		is 'good enough'
		*/
		for(int i = 0; i < currentSkeleton.numJoints; ++i) {
			skeletonVertices[i*2] = currentSkeleton.joints[i].transform.GetPositionVector();
	
			if(currentSkeleton.joints[i].parent == -1) {	//No parent, but to keep this simple we'll copy the position again...
				skeletonVertices[(i*2)+1] = currentSkeleton.joints[i].transform.GetPositionVector();;
			}
			else{
				skeletonVertices[(i*2)+1] = currentSkeleton.joints[currentSkeleton.joints[i].parent].transform.GetPositionVector();;
			}
		}
	
		//You should know what this all does by now, except we combine it with the draw operations in a single function
		glBindVertexArray(skeletonArray);
		glBindBuffer(GL_ARRAY_BUFFER, skeletonBuffer);
		glBufferData(GL_ARRAY_BUFFER, currentSkeleton.numJoints*sizeof(Vector3) * 2, skeletonVertices, GL_STREAM_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); 
		glEnableVertexAttribArray(0);
	
		glBindVertexArray(skeletonArray);
	
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
	
		//Draws the array twice, once as points, and once as lines. glLineWidth may or may not actually do anything
		//as it is deprecated functionality in OGL 3.2. 
		glPointSize(5.0f);
		glLineWidth(2.0f);
		glDrawArrays(GL_POINTS, 0, currentSkeleton.numJoints * 2);	// draw Joints
		glDrawArrays(GL_LINES, 0, currentSkeleton.numJoints * 2);	// draw Bones
		glPointSize(1.0f);
		glLineWidth(1.0f);
	
		glBindVertexArray(0);
	
		//Delete the VBO and VAO, and the heap memory we allocated earlier
		glDeleteVertexArrays(1, &skeletonArray);
		glDeleteBuffers(1, &skeletonBuffer);
		delete[]skeletonVertices;
}

void	MD5Node::DebugDrawJointTransforms(float size, bool worldSpace) {
	//Temporary VAO and VBO
	unsigned int skeletonArray;
	unsigned int skeletonBuffer;
	unsigned int skeletonColourBuffer;

	glGenVertexArrays(1, &skeletonArray);
	glGenBuffers(1, &skeletonBuffer);
	glGenBuffers(1, &skeletonColourBuffer);
	//Temporary chunk of memory to keep our joint positions in

	int numVerts = currentSkeleton.numJoints * 6;

	Vector3*	 skeletonVertices = new Vector3[numVerts];
	Vector4*	 skeletonColours  = new Vector4[numVerts];


	for (int i = 0; i < currentSkeleton.numJoints; ++i) {
		Matrix4 transform = (worldSpace ? currentSkeleton.joints[i].transform : currentSkeleton.joints[i].localTransform);

		Vector3 start = transform.GetPositionVector();
		transform.SetPositionVector(Vector3(0, 0, 0));

		Vector4 endX = transform * Vector4(1, 0, 0, 1);
		Vector4 endY = transform * Vector4(0, 1, 0, 1);
		Vector4 endZ = transform * Vector4(0, 0, 1, 1);

		skeletonVertices[(i * 6) + 0] = currentSkeleton.joints[i].transform.GetPositionVector();
		skeletonVertices[(i * 6) + 1] = currentSkeleton.joints[i].transform.GetPositionVector() + (endX.ToVector3() * size);

		skeletonVertices[(i * 6) + 2] = currentSkeleton.joints[i].transform.GetPositionVector();
		skeletonVertices[(i * 6) + 3] = currentSkeleton.joints[i].transform.GetPositionVector() + (endY.ToVector3() * size);
					
		skeletonVertices[(i * 6) + 4] = currentSkeleton.joints[i].transform.GetPositionVector();
		skeletonVertices[(i * 6) + 5] = currentSkeleton.joints[i].transform.GetPositionVector() + (endZ.ToVector3() * size);


		skeletonColours[(i * 6) + 0] = Vector4(1, 0, 0, 1);
		skeletonColours[(i * 6) + 1] = Vector4(1, 0, 0, 1);

		skeletonColours[(i * 6) + 2] = Vector4(0, 1, 0, 1);
		skeletonColours[(i * 6) + 3] = Vector4(0, 1, 0, 1);

		skeletonColours[(i * 6) + 4] = Vector4(0, 0, 1, 1);
		skeletonColours[(i * 6) + 5] = Vector4(0, 0, 1, 1);
	}

	//You should know what this all does by now, except we combine it with the draw operations in a single function
	glBindVertexArray(skeletonArray);
	glBindBuffer(GL_ARRAY_BUFFER, skeletonBuffer);
	glBufferData(GL_ARRAY_BUFFER, currentSkeleton.numJoints*sizeof(Vector3) * 6, skeletonVertices, GL_STREAM_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, skeletonColourBuffer);
	glBufferData(GL_ARRAY_BUFFER, currentSkeleton.numJoints*sizeof(Vector4) * 6, skeletonColours, GL_STREAM_DRAW);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glLineWidth(2.0f);
	glDrawArrays(GL_LINES, 0, currentSkeleton.numJoints * 6);	// draw Bones
	glLineWidth(1.0f);

	glBindVertexArray(0);

	//Delete the VBO and VAO, and the heap memory we allocated earlier
	glDeleteVertexArrays(1, &skeletonArray);
	glDeleteBuffers(1, &skeletonBuffer);
	glDeleteBuffers(1, &skeletonColourBuffer);

	delete[]skeletonVertices;
	delete[]skeletonColours;
}
#endif
#endif