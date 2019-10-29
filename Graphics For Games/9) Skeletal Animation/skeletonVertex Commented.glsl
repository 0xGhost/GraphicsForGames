#version 330 core	//Note the version change! This is to allow TBOs and layout specifiers.

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

//These are the two VBOs, attached to TBOs. Note that we use a special
//sampler for them, allowing use to use the special texelFetch function
uniform samplerBuffer weightTex;
uniform samplerBuffer transformTex;

in  vec3 position;
in 	vec3 normal;
in 	vec3 tangent;
in  vec2 texCoord;

//This is how to specify which VBO corresponds to which vertex attribute,
//entirely within the GLSL shader. We're making location 10 
//equate to the vec2s we buffered earlier, which store the two weighting components 
//for each vertex - x is its element count, and y is the first element. 
layout(location = 10) in vec2 weighting;

out Vertex	{
	vec4 	colour;
	vec2 	texCoord;
	vec3 	tangent;
	vec3 	worldPos;
	vec3 	normal;
} OUT;

//Here's a recap of how we buffered weights onto the GPU. We used two vec3's, 
//the first storing the weight index, joint index, and weight value, and the second
//storing its position. If we were to store the joint space normal and tangent, we'd
//extend this TBO to be twice as big again, for the 2 extra vec3s...
//struct MD5Weight {
//	int		weightIndex;	//Which weight of the MD5SubMesh this is
//	int		jointIndex;		//Which joint of the MD5Skeleton this weight is relative to
//	float	weightValue;	//How much influence this MD5Weight has on the MD5Vert
//	vec3 	position;		//Anchor position of this MD5Weight
//};


void main(void)	{
	//Give our new vertex attribute data a more useful name... 
	int weightElements 	= int(weighting.x);	
	int weightIndex 	= int(weighting.y);
	
	//We start off with a vertex position at the origin, just like when performing
	//software skinning...
	vec4 vertPos 	= vec4(0,0,0,1);
	
	//We'll also be building up the correct normals and tangents - they are currently in the 'bind pose' orientation in memory...
	vec3 oNormal  	= vec3(0,0,0);
	vec3 oTangent 	= vec3(0,0,0);

	//Then, we go through each of the weights that influence this vertex, defined via its
	//new vertex attribute, and calculate the final influence on the vertices position
	//this weight will have. This is just like CPU skinning, but note that we have to 
	//reconstruct the transformation matrix! 
	for(int i = 0; i < weightElements; ++i) {	
		vec3 firstHalf  = texelFetch(weightTex,((weightIndex+i)*2)).rgb;
		vec3 weightPos  = texelFetch(weightTex,((weightIndex+i)*2)+1).rgb;
		
		mat4 jointTransform;
		jointTransform[0] = texelFetch(transformTex,(int(firstHalf.y)*8)+0);	//firstHalf.y = jointIndex
		jointTransform[1] = texelFetch(transformTex,(int(firstHalf.y)*8)+1);	//firstHalf.y = jointIndex
		jointTransform[2] = texelFetch(transformTex,(int(firstHalf.y)*8)+2);	//firstHalf.y = jointIndex
		jointTransform[3] = texelFetch(transformTex,(int(firstHalf.y)*8)+3);	//firstHalf.y = jointIndex
		
		mat4 invBindPose;
		invBindPose[0] = texelFetch(transformTex,(int(firstHalf.y)*8)+4);	//firstHalf.y = jointIndex
		invBindPose[1] = texelFetch(transformTex,(int(firstHalf.y)*8)+5);	//firstHalf.y = jointIndex
		invBindPose[2] = texelFetch(transformTex,(int(firstHalf.y)*8)+6);	//firstHalf.y = jointIndex
		invBindPose[3] = texelFetch(transformTex,(int(firstHalf.y)*8)+7);	//firstHalf.y = jointIndex	
			
		vertPos += (jointTransform * vec4(weightPos,1.0)) * firstHalf.z; 		// firstHalf.z = weightValue;
		
		//We 'undo' the world orientation of the bind pose on our normal and tangent by transforming them
		//by the inverse bind pose matrix we make in the MD5MeshData class, and then bring them into the
		//local space of our current joint, and add the influence to our output normals - remember, our
		//normals will be 'weighted' by multiple joints, just like positions...
				
		oNormal  += (mat3(invBindPose) * mat3(jointTransform) * normal)  * firstHalf.z;
		oTangent += (mat3(invBindPose) * mat3(jointTransform) * tangent) * firstHalf.z;	
	}
	mat3 normalMatrix = transpose(mat3(modelMatrix));
	
	vertPos.w = 1.0f;

	OUT.texCoord 	= texCoord;
	OUT.normal 		= normalMatrix * normalize(oNormal);
	OUT.tangent 	= normalMatrix * normalize(oTangent);

	//Then, we just transform our vertex into clip space, just like usual!
	gl_Position		= (projMatrix * viewMatrix * modelMatrix) * vertPos;
}