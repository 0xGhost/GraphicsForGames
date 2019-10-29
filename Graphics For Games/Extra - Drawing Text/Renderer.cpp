/*
In this code tutorial, we're going to do something you've probably been
wondering how to do for a while, render text!

We're going to render text in two slightly different forms - one in 
orthographic mode (really handy for drawing GUIs) and once in 
world space (handy for info text above a character or something?)

The font is white, so can be tinted to any other colour easily enough.

To make the text drawing easy, we're going to use a subclass of Mesh, so
text drawing should fit neatly into any program based on this framework :)

If you run the program, you should see a line of text in the top left of the
screen, drawn in orthographic mode, and a line of text in 'world space',
at a position near the origin.



The font for this tutorial was generated using a nice easy program
obtained from:
//http://www.lmnopc.com/bitmapfontbuilder/bitmap-font-builder-download/

We could take this further and use more advanced font generation programs,
which support kerning (look it up!), but monospaced fonts are good enough
for now!
*/


#include "Renderer.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent)	{
	camera  = new Camera();	//A camera!

	//Again there's no fancy shader stuff, so it's just tutorial 3 again...
	currentShader = new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR"TexturedFragment.glsl");
	
	if(!currentShader->LinkProgram()) {
		return;
	}
	/*
	Just makes a new 'font', a struct containing a texture (of the tahoma font)
	and how many characters across each axis the font contains. (look at the
	font texture in paint.net if you don't quite 'get' this)
	*/
	basicFont = new Font(SOIL_load_OGL_texture(TEXTUREDIR"tahoma.tga",SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_COMPRESS_TO_DXT),16,16);

	//The font is not alpha blended! It has a black background.
	//but that doesn't matter, we can fiddle blend func to do 
	//'additive blending', meaning black won't show up ;)
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);

	init = true;
}

/*
Keep it clean!
*/
Renderer::~Renderer(void)	{
	delete camera;
	delete basicFont;
}

/*
Keep it moving!
*/
void Renderer::UpdateScene(float msec)	{
	camera->UpdateCamera(msec);
}

void Renderer::RenderScene()	{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear Screen And Depth Buffer

	glUseProgram(currentShader->GetProgram());	//Enable the shader...
	//And turn on texture unit 0
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);

//Render function to encapsulate our font rendering!
	DrawText("This is orthographic text!", Vector3(0,0,0), 16.0f);
	DrawText("This is perspective text!!!!", Vector3(0,0,-1000), 64.0f, true);

	glUseProgram(0);	//That's everything!

	SwapBuffers();
}

/*
Draw a line of text on screen. If we were to have a 'static' line of text, we'd
probably want to keep the TextMesh around to save processing it every frame, 
but for a simple demonstration, this is fine...
*/
void Renderer::DrawText(const std::string &text, const Vector3 &position, const float size, const bool perspective)	{
	//Create a new temporary TextMesh, using our line of text and our font
	TextMesh* mesh = new TextMesh(text,*basicFont);

	//This just does simple matrix setup to render in either perspective or
	//orthographic mode, there's nothing here that's particularly tricky.
	if(perspective) {
		modelMatrix = Matrix4::Translation(position) * Matrix4::Scale(Vector3(size,size,1));
		viewMatrix = camera->BuildViewMatrix();
		projMatrix = Matrix4::Perspective(1.0f,10000.0f,(float)width / (float)height, 45.0f);
	}
	else{	
		//In ortho mode, we subtract the y from the height, so that a height of 0
		//is at the top left of the screen, which is more intuitive
		//(for me anyway...)
		modelMatrix = Matrix4::Translation(Vector3(position.x,height-position.y, position.z)) * Matrix4::Scale(Vector3(size,size,1));
		viewMatrix.ToIdentity();
		projMatrix = Matrix4::Orthographic(-1.0f,1.0f,(float)width, 0.0f,(float)height, 0.0f);
	}
	//Either way, we update the matrices, and draw the mesh
	UpdateShaderMatrices();
	mesh->Draw();

	delete mesh; //Once it's drawn, we don't need it anymore!
}