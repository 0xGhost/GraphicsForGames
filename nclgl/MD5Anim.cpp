#include "MD5Anim.h"
#ifdef USE_MD5MESH
#ifdef WEEK_2_CODE
MD5Anim::MD5Anim(std::string filename)	{
	numAnimatedComponents = 0;
	frameRate	= 0;
	numJoints	= 0;
	numFrames	= 0;
	joints		= NULL;
	bounds		= NULL;
	frames		= NULL;

	LoadMD5Anim(filename);
}

/*
Destructor simply deletes our heap memory
*/
MD5Anim::~MD5Anim(void)	{
	delete[] joints;
	delete[] bounds;
	delete[] frames;
}
/************************************************************************/
/*                                                                      */
/************************************************************************/
void MD5Anim::LoadMD5Anim( std::string filename )	{
	//The MD5Anim is human readable, and stores its data in an easily
	//traversable way, so we can simply stream data from the file
	std::ifstream f(filename,std::ios::in);	

	if(!f) {	//Opening the file has failed :(
		return;
	}

	//We have our MD5 file handle!
	unsigned int md5Version		 = 0;
	unsigned int numLoadedFrames = 0;
	unsigned int numLoadedJoints = 0;
	unsigned int numLoadedBounds = 0;

	/*
	Now we simply load in the data, while there's still data to load
	*/
	while(!f.eof()) {
		std::string currentLine;	//A temporary string to keep the current line in
		f >> currentLine;			//stream the next line of the file in

		if(currentLine.empty()) {	//Actually, there's nothing in this line...usually
			continue;				//because we've hit the end of the file.
		}
		/*
		String::Find returns a value, equating to the position in the string the search 
		string is - or the special value 'npos' if the searchstring is not found at all
		*/
		else if(currentLine.find(MD5_VERSION_TAG) != std::string::npos) {
			//We've found the MD5 version string!

			//ifstream allows us to stream ints,floats etc into variables
			f >> md5Version;
			std::cout << "MD5 File version is: " << md5Version << std::endl;
		}
		else if(currentLine.find(MD5_COMMANDLINE_TAG) != std::string::npos) {
			/*
			MD5Anim files sometimes have a 'command line' value, used by the game
			toolchain to generate some data. We don't care about it!
			*/
			std::cout << "Ignoring commandline value" << std::endl;
		}
		else if(currentLine.find(MD5_ANIM_NUMFRAMES) != std::string::npos) {
			f >> numFrames;	//Loading in the number of frames held in this MD5Anim file
			std::cout << "Expecting file to have " << numFrames << " frames" << std::endl;

			//If we have an incorrectly generated MD5Anim file, this might go wrong, as
			//there might be more frames than we've generated space for...
			bounds = new MD5Bounds[numFrames];
			frames = new MD5Frame [numFrames];
		}
		else if(currentLine.find(MD5_NUMJOINTS_TAG) != std::string::npos) {
			f >> numJoints;	//Loading in the number of joints in this MD5Anim file
			std::cout << "Expecting file to have " << numJoints << " joints" << std::endl;

			joints = new MD5AnimJoint[numJoints];
		}
		else if(currentLine.find(MD5_ANIM_FRAMERATE) != std::string::npos) {
			f >> frameRate; //Loading in the framerate of this anim
		}
		else if(currentLine.find(MD5_ANIM_COMPONENTS) != std::string::npos) {
			f >> numAnimatedComponents;	//Loading in the number of delta values in this anim
		}
		else if(currentLine.find(MD5_ANIM_HIERARCHY) != std::string::npos) {
			LoadMD5AnimHierarchy(f, numLoadedJoints);	//Let's load in the joint hierarchy!
		}
		else if(currentLine.find(MD5_ANIM_BOUNDS) != std::string::npos) {
			LoadMD5AnimBounds(f,numLoadedBounds);		//Let's load in the AABBs!
		}
		else if(currentLine.find(MD5_ANIM_BASEFRAME) != std::string::npos) {
			LoadMD5AnimBaseFrame(f);					//Let's load in the base frame!
		}
		else if(currentLine.find(MD5_ANIM_FRAME) != std::string::npos) {
			LoadMD5AnimFrame(f,numLoadedFrames);		//Let's load in an animation frame!
		}
	}

	//If we get to here, we've loaded in everything from the file, so we can close it
	f.close();

	//If what we've loaded in does not equal what we /should/ have loaded in, we'll output an error
	//
	if(numLoadedFrames != numFrames || numLoadedJoints != numJoints || numLoadedBounds != numFrames) {
		std::cout << "MD5Anim file has incorrect data..." << std::endl;
	}
}

/*
Loads in the MD5Anim joint hierarchy. Uses the same ifstream as LoadMD5Anim, passed
by reference. We also pass a count variable by reference, which this function will
increment for every joint we load in. 
*/
void MD5Anim::LoadMD5AnimHierarchy( std::ifstream &from,unsigned int &count )	{
	/*
	The hierarchy section of the file should look something like this...

	hierarchy {
		"name"	parent flags frameIndex	//
		...more things
	}

	"hierarchy" is loaded in by LoadMD5Anim, so the first thing this function should see
	is a brace
	*/
	std::string tempLine; //Another temporary line to stream things into...

	do {
		from >> tempLine;	//Stream a line in

		if(tempLine == "{") {			//In a well-behaved MD5 file, the first line will be '{'
		}
		else if(tempLine[0] == '"'){	//It's a joint!
			//substr cuts out a section of a string, exclusive of the first and second parameter
			//positions (which should both be '"'
			joints[count].name = tempLine.substr(1,tempLine.find('"',1)-1);

			from >> joints[count].parent;
			from >> joints[count].flags;
			from >> joints[count].frameIndex;

			++count;
		}	
	}while(tempLine != "}");	//Hit an end bracket...
}

/*
Loads in the MD5Anim anim frames AABBs. Uses the same ifstream as LoadMD5Anim, passed
by reference. We also pass a count variable by reference, which this function will
increment for every joint we load in. 
*/
void MD5Anim::LoadMD5AnimBounds( std::ifstream &from,unsigned int &count  )	{
	/*
	The bounds section of the file should look like this:
	bounds {
		( min.x min.y min.z ) ( max.x max.y max.z )
		...more things
	}

	"bounds" is loaded in by LoadMD5Anim, so the first thing this function should see
	is a brace
	*/

	char skipChar;			//We skip the brackets by streaming them into this
	std::string tempLine;	//Another temporary line to stream things into...

	do {
		from >> tempLine;	

		if(tempLine == "{") {				//In a well-behaved MD5 file, the first line will be '{'
		}
		else if(tempLine[0] == '('){		//It's a bounding box!
			from >> bounds[count].min.x;
			from >> bounds[count].min.y;
			from >> bounds[count].min.z;

			from >> skipChar;				//Skip the ')'
			from >> skipChar;				//skip the '('

			from >> bounds[count].max.x;
			from >> bounds[count].max.y;
			from >> bounds[count].max.z;

			from >> skipChar;				//Skip the ')'

			++count;
		}
	}while(tempLine != "}");		//Hit an end bracket...
}

/*
Loads in the MD5Anim base frame, which consists of the default
positions and orientations of every joint in the mesh. 
Uses the same ifstream as LoadMD5Anim, passed by reference.
*/
void MD5Anim::LoadMD5AnimBaseFrame( std::ifstream &from )	{
	/*
	The baseframe section of the file should look like this:
	baseframe {
		( pos.x pos.y pos.z ) ( quat.x quat.y quat.z )
		...more things
	}

	"baseframe" is loaded in by LoadMD5Anim, so the first thing this function 
	should see is a brace
	*/

	char skipChar;			//We skip the brackets by streaming them into this
	std::string tempLine;	//Another temporary line to stream things into...

	/*
	We need to initialise enough space on the heap for every joint transform
	*/
	baseFrame.orientations	= new Quaternion[numJoints];
	baseFrame.positions		= new Vector3[numJoints];

	int current = 0;

	do {
		from >> tempLine;	

		if(tempLine == "{") {			//In a well-behaved MD5 file, the first line will be '{'
		}
		else if(tempLine[0] == '('){	//It's a base frame (probably)!
			from >> baseFrame.positions[current].x;
			from >> baseFrame.positions[current].y;
			from >> baseFrame.positions[current].z;

			from >> skipChar;	//End Bracket
			from >> skipChar;	//Begin Bracket

			from >> baseFrame.orientations[current].x;
			from >> baseFrame.orientations[current].y;
			from >> baseFrame.orientations[current].z;

			/*
			To save a tiny bit of space, the 4th component of the orientation
			quaternion is left out of the files. As we're dealing with unit length 
			quaternions (i.e they have a length of 1), the 4th component will be 
					sqrt of (1.0 - length of the other 3 components)
			*/

			baseFrame.orientations[current].GenerateW();
			baseFrame.orientations[current].Normalise();

			from >> skipChar;	//End Bracket
			++current;
		}
	}while(tempLine != "}");
}

/*
Loads in an MD5Frane, which consists of the differences from the baseframe
of the positions and orientations of every joint in the mesh. 
Uses the same ifstream as LoadMD5Anim, passed by reference.
We also pass a count variable by reference, which this function will
increment for every joint we load in. 
*/
void MD5Anim::LoadMD5AnimFrame( std::ifstream &from, unsigned int &count)	{
	/*
	Each animframe section of the file should look like this:
	frame framenum {
		linear array of floating point values
		...
	}

	"frame" is loaded in by LoadMD5Anim, so the first thing this function 
	should see is the framenum
	*/

	std::string tempLine;	//Another temporary line to stream things into...

	int frameNum;
	from >> frameNum;	//Stream in the current frame number

	/*
	Every frame has the same number of 'delta' floats - so even if a joint
	is only modified in a single frame, it will have a delta value in every
	frame.
	*/
	frames[frameNum].components = new float[numAnimatedComponents];

	from >> tempLine;	//Load in the next line, which /should/ be "{"

	if(tempLine == "{") {//In a well-behaved MD5 file, the first line will be '{'
		for(unsigned int i = 0; i < numAnimatedComponents; ++i) {
			//stream in the delta values of the current animation frame
			from >> frames[frameNum].components[i];
		}
		from >> tempLine;	// Should be '}'
		++count;
	}
}


//Transforms the passed in skeleton to the correct positions and
//orientations for the desired frame
void	MD5Anim::TransformSkeleton(MD5Skeleton &skel, unsigned int frameNum) {
	/*
	Here's the most important function of the MD5Anim class. This transforms an input
	skeleton's joints (generally this will be the 'working' skeleton from an MD5Mesh instance)
	to the required transforms to represent the desired frame of animation
	*/

	if(frameNum > numFrames) {	//This probably shouldn't ever happen!
		return;
	}

	//Grab a reference to the frame data for the relevant frame
	MD5Frame&frame = frames[frameNum];

	//For each joint in the animation
	for(unsigned int i = 0; i < numJoints; ++i) {
		//Grab COPIES of the position and orientation of the baseframe joint
		Vector3		animPos	 = baseFrame.positions[i];
		Quaternion  animQuat = baseFrame.orientations[i];

		/*
		Each frame has a number of 'delta' components, and each joint
		uses a number of these components to update its position and
		orientation. Whether or not each value is updated or not is
		determined by the joints flags variable. The starting
		component for each joint is determined by the frameIndex value
		of the joint.

		For each value of the joint (ie its 3 position values and its 3
		orientation components (we don't bother with the 4th as we can 
		reconstruct it) we check the flags to see if it should be updated,
		update it if necessary, and increment a counter so we access the
		next component.

		*/

		int j = 0;

		if (joints[i].flags & MD5_ANIM_XPOS) {//X component of Position
			animPos.x = frame.components[joints[i].frameIndex+j];
			++j;
		}
	
		if (joints[i].flags & MD5_ANIM_YPOS) {//Y component of Position
			animPos.y = frame.components[joints[i].frameIndex+j];
			++j;
		}
	
		if (joints[i].flags & MD5_ANIM_ZPOS) {//Z component of Position
			animPos.z = frame.components[joints[i].frameIndex+j];
			++j;
		}
		
		if (joints[i].flags & MD5_ANIM_XQUAT) {//X component of Orientation
			animQuat.x = frame.components[joints[i].frameIndex+j];
			++j;
		}
		
		if (joints[i].flags & MD5_ANIM_YQUAT) {//Y component of Orientation
			animQuat.y = frame.components[joints[i].frameIndex+j];
			++j;
		}
		
		if (joints[i].flags & MD5_ANIM_ZQUAT) {//Z component of Orientation
			animQuat.z = frame.components[joints[i].frameIndex+j];
			++j;
		}

		animQuat.GenerateW(); //We only get updated x,y,z so must generate W again...
		animQuat.Normalise(); //And we should probably normalise it, too, to keep to unit length

		//now we have a copy of the baseframe joint transformed to the animation pose, we can start
		//applying it to the input skeleton.

		//First, let's get a reference to the skeleton joint equating to the current baseframe joint
		MD5Joint &skelJoint = skel.joints[i];

		//I'm fairly sure this doesn't ever actually change...
		skelJoint.parent	= joints[i].parent;
		skelJoint.forceWorld = false;

		//We'll set its position and orientation to the transformed baseframe variables

		skelJoint.position		= animPos;
		skelJoint.orientation	= animQuat;	

		//Now to set the local transform of the current joint. We start by turning the orientation
		//quaternion into a Matrix4, then we set the resulting matrix translation to the
		//transformed baseframe position

		skelJoint.localTransform		= animQuat.ToMatrix();
		skelJoint.localTransform.SetPositionVector(animPos);

		//If the joint has no parent (determined by a negative parent variable) we need to 
		//transform the joint's transform to the correct rotation, using the conversion matrix
		if(skelJoint.parent < 0) {	//Base Joint, so we're done
			skelJoint.transform = MD5FileData::conversionMatrix * skelJoint.localTransform;
		}
		else{	
			//If this joint /does/ have a parent, we transform the joint's transform by its
			//parent transform. Note that we don't have to transform it by the conversion matrix
			//again, as the parent node will already contain it, due to being propagated from 
			//the root node. Matrices are fun!
			MD5Joint &parent = skel.joints[skelJoint.parent];
			skelJoint.transform = parent.transform * skelJoint.localTransform;
		}
	}
}
#endif
#endif