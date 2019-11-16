#include "Renderer.h"
#include "../../nclgl/BoundingSphere.h"
#include "../../nclgl/AABoundingBox.h"

Renderer::Renderer(Window& parent) : OGLRenderer(parent), window(&parent), showBoundingVolume(true)
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
		
		s->SetBoundingVolume(new AABoundingBox(s->GetWorldTransform(), s->GetModelScale(), quad));
		//s->SetBoundingVolume(new BoundingSphere(s->GetWorldTransform(), s->GetModelScale(), quad));
		root->AddChild(s);
	}

	LoadHeightMap();
	LoadHellKnight();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_CULL_FACE);
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
	currentShader = NULL;

	//delete heightMap;
	//delete ppQuad1;
	//delete ppQuad2;
	//delete mapCamera;

	glDeleteTextures(2, bufferColourTex);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteTextures(2, bufferColourTex0);
	glDeleteTextures(1, &bufferDepthTex0);
	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &bufferFBO0);
	glDeleteFramebuffers(1, &processFBO);
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
		SetCurrentShader(n->GetShader() != NULL ? n->GetShader() : sceneShader);
		UpdateShaderMatrices();
		glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "time"), GetMS());
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bumpTex"), 1);
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "glossTex"), 7);
		Matrix4 transform = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, (float*)& transform);
		glUniform4fv(glGetUniformLocation(currentShader->GetProgram(), "nodeColour"), 1, (float*)& n->GetColour());
		glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)& camera->GetPosition());
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "useTexture"), (int)n->GetMesh()->GetTexture());
		
		n->Draw(*this);
		glUseProgram(0);
	}
	if (n->GetBoundingVolume() && showBoundingVolume)
	{
		glDisable(GL_CULL_FACE);
		SetCurrentShader(boundingVolumeShader);
		BoundingVolume *bv = n->GetBoundingVolume();
		UpdateShaderMatrices();
		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, (float*)& n->GetBoundingVolume()->GetModelMatrix());
		glUniform4fv(glGetUniformLocation(currentShader->GetProgram(), "boundingVolumeColour"), 1, (float*)& Vector4(1,1,1,1));

		n->GetBoundingVolume()->Draw();
		glUseProgram(0);
		glEnable(GL_CULL_FACE);
	}
}

inline void Renderer::LoadHeightMap()
{
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
	hm->SetBoundingVolume(new AABoundingBox(hm->GetWorldTransform(), hm->GetModelScale(), heightMap));
	//hm->SetBoundingVolume(new BoundingSphere(hm->GetWorldTransform(), hm->GetModelScale(), 100.0f));
	root->AddChild(hm);
}

inline void Renderer::LoadHellKnight()
{
#ifdef MD5_USE_HARDWARE_SKINNING
	currentShader = new Shader(SHADERDIR"skeletonVertex.glsl", SHADERDIR"TexturedFragment.glsl");
#else
	//currentShader = new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR"TexturedFragment.glsl");
#endif

	MD5FileData* hellData = new MD5FileData(MESHDIR"hellknight.md5mesh");

	MD5Node* hellNode = new MD5Node(*hellData);
	hellNode->GetBoundingVolume();
	hellNode->SetShader(new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR"TexturedFragment.glsl"));
	if (!currentShader->LinkProgram()) {
		return;
	}

	hellData->AddAnim(MESHDIR"idle2.md5anim");
	hellNode->PlayAnim(MESHDIR"idle2.md5anim");
	root->AddChild(hellNode);
	//delete hellData;
}

inline void Renderer::InitPostProcessing()
{
	mapCamera = new Camera(-90.0f, -180.0f, Vector3(4000, 10000, 4000));
	camera = new Camera(-90.0f, -180.0f, Vector3(4000, 10000, 4000));
	ppQuad1 = Mesh::GenerateQuad();
	Vector3 quad2Positions[4] = { Vector3(0.5f, -1.0f, 1.0f),Vector3(0.5f, -0.5f, 1.0f),Vector3(1.0f, -1.0f, 1.0f),Vector3(1.0f, -0.5f, 1.0f) };
	ppQuad2 = Mesh::GenerateQuad(quad2Positions);
	
	//processShader = new Shader(SHADERDIR "TexturedVertex.glsl", SHADERDIR "processfrag.glsl");
	//processShader = new Shader(SHADERDIR "TexturedVertex.glsl", SHADERDIR "edgeFrag.glsl");
	processShader = new Shader(SHADERDIR "TexturedVertex.glsl", SHADERDIR "doubleVisionFrag.glsl");

	if (!processShader->LinkProgram()
		|| !sceneShader->LinkProgram()
		|| !heightMap->GetTexture())
	{
		return;
	}

	// Generate our scene depth texture ...
	glGenTextures(1, &bufferDepthTex);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

	for (int i = 0; i < 2; ++i) {
		glGenTextures(1, &bufferColourTex[i]);
		glBindTexture(GL_TEXTURE_2D, bufferColourTex[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	}

	glGenTextures(1, &bufferDepthTex0);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex0);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

	for (int i = 0; i < 2; ++i) {
		glGenTextures(1, &bufferColourTex0[i]);
		glBindTexture(GL_TEXTURE_2D, bufferColourTex0[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	}

	glGenFramebuffers(1, &bufferFBO); // We ’ll render the scene into this
	glGenFramebuffers(1, &bufferFBO0);
	glGenFramebuffers(1, &processFBO); // And do post processing in this


	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex0, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex0, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex0[0], 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE
		|| !bufferDepthTex0
		|| !bufferColourTex0[0])
	{
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[0], 0);



	// We can check FBO attachment success using this command !
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE
		|| !bufferDepthTex
		|| !bufferColourTex[0])
	{
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
