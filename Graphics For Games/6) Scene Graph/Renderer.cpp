#include "Renderer.h"

Renderer::Renderer(Window& parent) : OGLRenderer(parent)
{
	CubeRobot::CreateCube();
	camera = new Camera();
	currentShader = new Shader(SHADERDIR"SceneVertex.glsl", SHADERDIR"SceneFragment.glsl");
	if (!currentShader->LinkProgram()) return;

	projMatrix = Matrix4::Perspective(1, 10000, (float)width / (float)height, 45);

	camera->SetPosition(Vector3(0, 30, 175));

	root = new SceneNode();
	root->AddChild(new CubeRobot());

	glEnable(GL_DEPTH_TEST);
	init = true;
}

Renderer::~Renderer()
{
	delete root;
	CubeRobot::DeleteCube();
}

void Renderer::UpdateScene(float msec)
{
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
	root->Update(msec);
}

void Renderer::RenderScene()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	UpdateShaderMatrices();
	

	DrawNode(root);
	glUseProgram(0);
	SwapBuffers();
}

void Renderer::DrawNode(SceneNode* n)
{	
	if (n->GetMesh())
	{
		Shader* nodeShader = n->GetShader() != NULL ? n->GetShader() : currentShader;
		glUseProgram(nodeShader->GetProgram());
		glUniform1i(glGetUniformLocation(nodeShader->GetProgram(), "diffuseTex"), 1);
		Matrix4 transform = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
		glUniformMatrix4fv(glGetUniformLocation(nodeShader->GetProgram(), "modelMatrix"), 1, false, (float*)& transform);
		glUniform4fv(glGetUniformLocation(nodeShader->GetProgram(), "nodeColour"), 1, (float*)&n->GetColour());
		glUniform1i(glGetUniformLocation(nodeShader->GetProgram(), "useTexture"), (int)n->GetMesh()->GetTexture());
		n->Draw(*this);
	}
	for (vector<SceneNode*>::const_iterator i = n->GetChildIteratorStart(); i != n->GetChildIteratorEnd(); ++i) 
	{
		DrawNode(*i);
	}
}
