/*
To show of a little of what geometry shaders can do, 
we're going to add one in between our texturedVertex
and texturedFragment shaders. In our program, we're
simply passing a list of points to the vertex shader,
which we translate into screen space, and pass to the
geometry shader (the shader order always goes:
	vertex->geometry->fragment)

In our geometry shader, we're going to take in those 
points, and transform them into quads, using 
triangle strips. As we're in clip space by this 
point in the pipeline (having transformed the positions
in the vertex shader), we can calculate a screen-oriented
quad easily, simply by manipulating the x and y values
we are receiving from the vertex shader. As long 
as we don't touch the z coordinate, the perspective
divide stage will make our quads the correct size
on screen. Neat!

*/
#version 150
 
//This is set by the Renderer SetShaderParticleSize
//function!
uniform float particleSize;

//This is how we tell the geometry shader what type
//of primitive we're taking in, and spitting out.
//points, triangle_strip, and max_Vertices are 
//all GLSL keywords. Points and triangle strips 
//you should know about. Max_Vertices tells GLSL
//how many vertices to expect. As we're turning
//points into quads, this value should be 4.
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

//our vertex shader actually outputs texcoords in 
//its output struct, but we're going to ignore them
//as we're going to generate them ourselves 
in Vertex {
	vec4 colour;
} IN[];

//See how we're outputting texCoords! So as far as the
//fragment shader is concerned, the geometry stage is
//completely hidden
out Vertex {
	vec4 colour;
	vec2 texCoord;
} OUT;
 
void main() {	
//gl_in is another GLSL keyword. It'll give us
//how many primitives we're accepting from
//the vertex shader. as we're taking in points
//our number of primitives is equal to our
//number of vertices in the bound VBO
	for(int i = 0; i < gl_in.length(); ++i) {
		//So, for every iteration of this loop (ie for every
		//point in our VBO, we are going to output 4 vertices


	//We get colour info from our vertex shader for each point,
	//which we can access using the IN struct as usual. BUT!
	//this time, we have an array of 'IN', which we can access
	//using square brackets, just like C++ arrays
		OUT.colour = IN[i].colour;

		//But hang on! We don't put positions in our OUT struct!
		//That's ok, we can get positions using another array -
		//gl_in also works as an array, containing gl_Positions.
		//these are the outputted vertex positions created by
		//our vertex shader.

		//Now we have a position for the input vertex, we can 
		//set the output vertex position and texture coordinate.

		//We have some slightly odd syntax, in that we always
		//put the position data in (gl_Position), but data we
		//want sent to the fragment shader gets put in the OUT
		//struct...

		//top right
		gl_Position = gl_in[ i ].gl_Position;
		gl_Position.x += particleSize;
		gl_Position.y += particleSize;
		OUT.texCoord = vec2(1,0);

		//When we've created a vertex, we call the glsl in-built
		//function EmitVertex, which sends our vertex off for
		//rasterisation.
		EmitVertex();

		//Then we do the other vertices of the quad...
		//Top Left
		gl_Position = gl_in[ i ].gl_Position;
		gl_Position.x -= particleSize;
		gl_Position.y += particleSize;
		OUT.texCoord = vec2(0,0);
		EmitVertex();

		//bottom right
		gl_Position = gl_in[ i ].gl_Position;
		gl_Position.x += particleSize;
		gl_Position.y -= particleSize;
		OUT.texCoord = vec2(1,1);
		EmitVertex();

		//bottom Left
		gl_Position = gl_in[ i ].gl_Position;
		gl_Position.x -= particleSize;
		gl_Position.y -= particleSize;
		OUT.texCoord = vec2(0,1);
		EmitVertex();

		//Once we've emitted 4 vertices, we tell glsl that we've 
		//finished a complete primitive, so it can begin the 
		//rasterisation process on it.
		EndPrimitive();
	}
}

//That's pretty much everything for geometry shaders! If you've taken a look
//at the 'Drawing Text' tutorial, you might want to start thinking about how
//you could start using a geometry shader to render the quads used for every
//character of text. The Texture Atlas tutorial should also help in working
//out which texture coordinates to give to a vertex...