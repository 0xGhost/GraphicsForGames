#include "Renderer.h"
#include "../../nclgl/BoundingSphere.h"
#include "../../nclgl/AABoundingBox.h"

Renderer::Renderer(Window& parent) : OGLRenderer(parent), window(&parent)
{
	BoundingSphere::CreateSphereMesh();
	BoundingBox::CreateBoxMesh();

	camera = new Camera();
	camera->SetPosition(Vector3(0, 100, 750));
	projMatrix = Matrix4::Perspective(1, 10000, (float)width / (float)height, 45);

	heightMapShader = new Shader(SHADERDIR"HeightMapVertex.glsl", SHADERDIR"TexturedFragment.glsl");
	if (!heightMapShader->LinkProgram()) return;

	boundingVolumeShader = new Shader(SHADERDIR"BoundingVolumeVertex.glsl", SHADERDIR"BoundingVolumeFragment.glsl");
	if (!boundingVolumeShader->LinkProgram()) return;

	sceneShader = new Shader(SHADERDIR"SceneVertex.glsl", SHADERDIR"SceneFragment.glsl");
	if (!sceneShader->LinkProgram()) return;

	quad = Mesh::GenerateQuad();
	quad->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"brick.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0));
	if (!quad->GetTexture()) return;

	root = new SceneNode();
	//root->SetBoundingVolume(new )
	for (int i = 0; i < 5; ++i) {
		SceneNode* s = new SceneNode();
		s->SetColour(Vector4(1.0f, 1.0f, 1.0f, 0.5f));
		s->SetTransform(Matrix4::Translation(Vector3(0, 100.0f, -300.0f + 100.0f + 100 * i)));
		s->SetModelScale(Vector3(100.0f, 100.0f, 100.0f));
		s->SetMesh(quad);
		//s->SetBoundingRadius(100.0f);
		s->SetBoundingVolume(new AABoundingBox(s->GetWorldTransform(), s->GetModelScale(), quad));
		//s->SetBoundingVolume(new BoundingSphere(s->GetWorldTransform(), s->GetModelScale(), quad));
		root->AddChild(s);
	}

	heightMap = new HeightMap(TEXTUREDIR"terrain.raw");
	heightMap->SetTexture(SOIL_load_OGL_texture(
		TEXTUREDIR "Barren Reds.JPG", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	heightMap->SetBumpMap(SOIL_load_OGL_texture(
		TEXTUREDIR "Barren RedsDOT3.JPG", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	heightMap->SetGlossMap(SOIL_load_OGL_texture(TEXTUREDIR "glossMap.JPG",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	SetTextureRepeating(heightMap->GetTexture(), true);
	SetTextureRepeating(heightMap->GetBumpMap(), true);
	SetTextureRepeating(heightMap->GetGlossMap(), true);

	SceneNode* hm = new SceneNode();
	hm->SetShader(heightMapShader);
	hm->SetMesh(heightMap);
	hm->SetColour(Vector4(1.0f, 1.0f, 1.0f, 0.5f));
	hm->SetTransform(Matrix4::Translation(Vector3(0, 0, 0)));
	hm->SetModelScale(Vector3(1.0f, 1.0f, 1.0f));
	//s->SetBoundingRadius(100.0f);
	hm->SetBoundingVolume(new AABoundingBox(hm->GetWorldTransform(), hm->GetModelScale(), heightMap));
	//hm->SetBoundingVolume(new BoundingSphere(hm->GetWorldTransform(), hm->GetModelScale(), 100.0f));
	root->AddChild(hm);
	

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_FRONT);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	init = true;
}

Renderer::~Renderer()
{
	delete root;
	delete quad;
	delete camera;
	BoundingSphere::DeleteSphereMesh();
	BoundingBox::DeleteBoxMesh();
}

void Renderer::UpdateScene(float msec)
{
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
	frameFrustum.FromMatrix(projMatrix * viewMatrix);
	root->Update(msec);
}

void Renderer::RenderScene()
{
	BuildNodeLists(root);
	SortNodeLists();
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	//glUseProgram(currentShader->GetProgram());
	//UpdateShaderMatrices();
	//glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	DrawNodes();
	//glUseProgram(0);
	SwapBuffers();
	ClearNodeLists();
}

void Renderer::BuildNodeLists(SceneNode* from)
{
	if (frameFrustum.InsideFrustum(*from)) {
		Vector3 dir = from->GetWorldTransform().GetPositionVector() - camera->GetPosition();
		from->SetCameraDistance(Vector3::Dot(dir, dir));
		float textureA = 1.1f;
		if (from->GetTexture()) glGetTexLevelParameterfv(GL_TEXTURE_2D, 0, GL_TEXTURE_ALPHA_SIZE, &textureA);
		if (from->GetColour().w < 1.0f || textureA < 1.0f)
		{
			transparentNodeList.push_back(from);
		}
		else
		{
			nodeList.push_back(from);
		}
		for (vector<SceneNode*>::const_iterator i = from->GetChildIteratorStart(); i != from->GetChildIteratorEnd(); i++)
		{
			BuildNodeLists(*i);
		}
	}

	
}

void Renderer::SortNodeLists()
{
	std::sort(transparentNodeList.begin(), transparentNodeList.end(), SceneNode::CompareByCameraDistance);
	std::sort(nodeList.begin(), nodeList.end(), SceneNode::CompareByCameraDistance);
}

void Renderer::ClearNodeLists()
{
	transparentNodeList.clear();
	nodeList.clear();
}

void Renderer::DrawNodes()
{
	for (vector<SceneNode*>::const_iterator i = nodeList.begin(); i != nodeList.end(); i++)
	{
		DrawNode(*i);
	}
	for (vector<SceneNode*>::const_reverse_iterator i = transparentNodeList.rbegin(); i != transparentNodeList.rend(); i++)
	{
		DrawNode(*i);
	}
}

void Renderer::DrawNode(SceneNode* n)
{
	if (n->GetMesh())
	{
		/*
		Shader* nodeShader = n->GetShader() != NULL ? n->GetShader() : currentShader;
		if (nodeShader != currentShader)
		{
			int a = 0;
		}*/
		SetCurrentShader(n->GetShader() != NULL ? n->GetShader() : sceneShader);
		UpdateShaderMatrices();
		glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "time"), GetMS());
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
		//glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bumpTex"), 1);
		//glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "glossTex"), 7);
		Matrix4 transform = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, (float*)& transform);
		glUniform4fv(glGetUniformLocation(currentShader->GetProgram(), "nodeColour"), 1, (float*)& n->GetColour());
		glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)& camera->GetPosition());
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "useTexture"), (int)n->GetMesh()->GetTexture());
		
		n->Draw(*this);
		glUseProgram(0);
	}
	if (n->GetBoundingVolume())
	{
		// TODO: replace "currentShader"
		SetCurrentShader(boundingVolumeShader);

		UpdateShaderMatrices();
		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, (float*)& n->GetBoundingVolume()->GetModelMatrix());
		glUniform4fv(glGetUniformLocation(currentShader->GetProgram(), "boundingVolumeColour"), 1, (float*)& Vector4(1,1,1,1));

		n->GetBoundingVolume()->Draw();
		glUseProgram(0);
	}
}
