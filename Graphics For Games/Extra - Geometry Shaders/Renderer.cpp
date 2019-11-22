#include "Renderer.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent)	{
	//Finally, a use for the third parameter of the Shader class
	//I made you all type out! We're going to use it to add in an
	//extra stage between some simple shaders,
	//which turns GL_POINTS into GL_TRIANGLE_STRIPS
	currentShader = new Shader(	"vertex.glsl",
								"fragment.glsl", 
								"geometry.glsl");

	if(!currentShader->LinkProgram()) {
		return;
	}
	//A new particle emitter!
	emitter = new ParticleEmitter();
	camera  = new Camera(0,0,Vector3(0,0,250.0f));

	projMatrix = Matrix4::Perspective(1.0f,10000.0f,(float)width / (float)height, 45.0f);

	init = true;
}

Renderer::~Renderer(void)	{
	delete emitter;
	delete camera;
}

void Renderer::UpdateScene(float msec) {
	emitter->Update(msec);
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
}

void Renderer::RenderScene()	{
	//glClearColor(0,0,0,1);
	//glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(currentShader->GetProgram());
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);

	SetShaderParticleSize(emitter->GetParticleSize());
	emitter->SetParticleSize(8.0f);
	emitter->SetParticleVariance(1.0f);
	emitter->SetLaunchParticles(16.0f);
	emitter->SetParticleLifetime(2000.0f);
	emitter->SetParticleSpeed(0.1f);
	UpdateShaderMatrices();

	emitter->Draw();

	glUseProgram(0);

	SwapBuffers();
}

void	Renderer::SetShaderParticleSize(float f) {
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "particleSize"), f);
}