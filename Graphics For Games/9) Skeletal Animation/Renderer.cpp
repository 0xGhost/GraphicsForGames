
#include "Renderer.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent)	{	
	camera			= new Camera(0,-90.0f,Vector3(-180,60,0));

#ifdef MD5_USE_HARDWARE_SKINNING
	currentShader   = new Shader("skeletonVertex.glsl", SHADERDIR"TexturedFragment.glsl");
#else
	currentShader   = new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR"TexturedFragment.glsl");
#endif

	hellData		= new MD5FileData(MESHDIR"hellknight.md5mesh");

	hellNode		= new MD5Node(*hellData);

	if(!currentShader->LinkProgram()) {
		return;
	}

	hellData->AddAnim(MESHDIR"idle2.md5anim");
	hellNode->PlayAnim(MESHDIR"idle2.md5anim");

	projMatrix = Matrix4::Perspective(1.0f,10000.0f,(float)width / (float)height, 45.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	init = true;
}

Renderer::~Renderer(void)	{
	delete camera;

	delete hellData;
	delete hellNode;
}

 void Renderer::UpdateScene(float msec)	{
	camera->UpdateCamera(msec); 
	viewMatrix		= camera->BuildViewMatrix();

	hellNode->Update(msec);
}

void Renderer::RenderScene()	{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glUseProgram(currentShader->GetProgram());
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);

	UpdateShaderMatrices();
#ifdef MD5_USE_HARDWARE_SKINNING
	for(int y = 0; y < 10; ++y) {
		for(int x = 0; x < 10; ++x) {
			modelMatrix = Matrix4::Translation(Vector3(x * 100, 0, y * 100));
			UpdateShaderMatrices();	
			hellNode->Draw(*this);
		}
	}
#else
	hellNode->Draw(*this);
#endif

	glUseProgram(0);
	SwapBuffers();
}
