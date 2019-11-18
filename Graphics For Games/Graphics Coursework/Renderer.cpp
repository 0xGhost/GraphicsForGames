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

	boundingVolumeShader = new Shader(SHADERDIR"BoundingVolumeVertex.glsl", SHADERDIR"BoundingVolumeFragment.glsl");
	if (!boundingVolumeShader->LinkProgram()) return;

	sceneShader = new Shader(SHADERDIR"BumpVertex.glsl", SHADERDIR"BumpFragment.glsl");
	if (!sceneShader->LinkProgram()) return;

	//sceneShader = new Shader(SHADERDIR"shadowscenevert.glsl",
		//SHADERDIR"shadowscenefrag.glsl");
	//shadowShader = new Shader(SHADERDIR"shadowVert.glsl",
		//SHADERDIR"shadowFrag.glsl");

	if (!sceneShader->LinkProgram())// || !shadowShader->LinkProgram())
	{
		return;
	}

	textureShader = new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR"TexturedFragment.glsl");
	if (!textureShader->LinkProgram()) return;

	skyBoxShader = new Shader(SHADERDIR "skyboxVertex.glsl",
		SHADERDIR "skyboxFragment.glsl");
	if (!skyBoxShader->LinkProgram()) return;

	skyBoxQuad = Mesh::GenerateQuad();

	quad = Mesh::GenerateQuad();
	quad->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"brick.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0));
	if (!quad->GetTexture()) return;

	root = new SceneNode();
	//root->SetBoundingVolume(new )
	for (int i = 0; i < 5; ++i) {
		SceneNode* s = new SceneNode();
		s->SetColour(Vector4(1.0f, 1.0f, 1.0f, 0.5f));
		s->SetTransform(Matrix4::Translation(Vector3(0, 100.0f, -300.0f + 100.0f + 100 * i))
			* Matrix4::Rotation(45, Vector3(1.0f, 0.0f, 0.0f)));
		s->SetModelScale(Vector3(100.0f, 100.0f, 100.0f));
		s->SetMesh(quad);
		
		s->SetBoundingVolume(new AABoundingBox(s->GetWorldTransform(), s->GetModelScale(), quad));
		//s->SetBoundingVolume(new BoundingSphere(s->GetWorldTransform(), s->GetModelScale(), quad));
		root->AddChild(s);
	}

	LoadLights();
	LoadSkyBox();

	heightMapNode = LoadHeightMap();
	hellKnightNode = LoadHellKnight();
	waterNode = LoadWater();
	root->AddChild(heightMapNode);

	heightMapNode->AddChild(hellKnightNode);
	heightMapNode->AddChild(waterNode);

	InitPostProcessing();
	

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_FRONT);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
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
	projMatrix = Matrix4::Perspective(1, 10000, (float)width / (float)height, 45);
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
	frameFrustum.FromMatrix(projMatrix * viewMatrix);
	root->Update(msec);
	waterRotate = msec / 1000.0f;
	waterNode->SetTextureMatrix(waterNode->GetTextureMatrix() * Matrix4::Rotation(waterRotate, Vector3(0.0f, 0.0f, 1.0f)));
}

void Renderer::RenderScene()
{

	BuildNodeLists(root);
	SortNodeLists();

	// draw scene to FBO
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	DrawSkybox();
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
	SetCurrentShader(sceneShader);
	UpdateShaderMatrices();
	
	DrawNodes();
	
	// draw map from mapCamera to FBO0
	bool temp = showBoundingVolume;
	showBoundingVolume = false;
	viewMatrix = mapCamera->BuildViewMatrix();
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	SetCurrentShader(sceneShader);
	projMatrix = Matrix4::Perspective(1.0f, 100000.0f, (float)width / (float)height, 45.0f);
	UpdateShaderMatrices();
	DrawNodes();
	showBoundingVolume = temp;

	DrawPostProcess();
	
	PresentScene();
	SwapBuffers();
	ClearNodeLists();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
		Matrix4 temp = modelMatrix;
		modelMatrix = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
		textureMatrix = n->GetTextureMatrix();	
			
		UpdateShaderMatrices();
		glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "time"), GetMS());
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bumpTex"), 1);
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "cubeTex"), 2);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyBox);
		//glActiveTexture(0);
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "glossTex"), 7);
		Matrix4 transform = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, (float*)& transform);
		glUniform4fv(glGetUniformLocation(currentShader->GetProgram(), "nodeColour"), 1, (float*)& n->GetColour());
		glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)& camera->GetPosition());
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "useTexture"), (int)n->GetMesh()->GetTexture());
		
		SetShaderLight(*lights);
		n->Draw(*this);
		modelMatrix = temp;
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

inline SceneNode* Renderer::LoadHeightMap()
{
	heightMapShader = new Shader(SHADERDIR"HeightMapVertex.glsl", SHADERDIR"bumpFragment.glsl");
	if (!heightMapShader->LinkProgram()) return nullptr;

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
	return hm;
}

inline SceneNode* Renderer::LoadWater()
{
	reflectShader = new Shader(SHADERDIR "bumpVertex.glsl",
		SHADERDIR "reflectFragment.glsl");
	if (!reflectShader->LinkProgram())
	{
		return nullptr;
	}
	waterQuad = Mesh::GenerateQuad();
	waterQuad->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR "water.TGA",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	waterQuad->SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR "waterDOT3.TGA",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	SetTextureRepeating(waterQuad->GetTexture(), true);
	SetTextureRepeating(waterQuad->GetBumpMap(), true);
	waterRotate = 0.0f;

	SceneNode* waterNode = new SceneNode();
	waterNode->SetShader(reflectShader);
	waterNode->SetMesh(waterQuad);
	waterNode->SetColour(Vector4(1.0f, 1.0f, 1.0f, 0.5f));

	float heightX = (RAW_WIDTH * HEIGHTMAP_X / 2.0f);
	float heightY = 256 * HEIGHTMAP_Y / 3.0f;
	float heightZ = (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f);
	waterNode->SetTransform(Matrix4::Translation(Vector3(heightX, heightY, heightZ)) *
		Matrix4::Scale(Vector3(heightX, 1.0f, heightZ)) * Matrix4::Rotation(90, Vector3(1.0f, 0.0f, 0.0f)));
	waterNode->SetTextureMatrix(Matrix4::Scale(Vector3(10.0f, 10.0f, 10.0f)));// *
		//Matrix4::Rotation(waterRotate, Vector3(0.0f, 0.0f, 1.0f)));

	waterNode->SetBoundingVolume(
		new AABoundingBox(Matrix4::Translation(Vector3(heightX, heightY, heightZ)), waterNode->GetModelScale(), waterQuad));
	//hm->SetBoundingVolume(new BoundingSphere(hm->GetWorldTransform(), hm->GetModelScale(), 100.0f));
	return waterNode;
}

inline void Renderer::LoadSkyBox()
{
	skyBox = SOIL_load_OGL_cubemap(
		TEXTUREDIR"rusted_west.jpg", TEXTUREDIR"rusted_east.jpg",
		TEXTUREDIR"rusted_up.jpg", TEXTUREDIR"rusted_down.jpg",
		TEXTUREDIR"rusted_south.jpg", TEXTUREDIR"rusted_north.jpg",
		SOIL_LOAD_RGB,
		SOIL_CREATE_NEW_ID, 0
	);
	if (!skyBox) return;

}

inline SceneNode* Renderer::LoadHellKnight()
{
#ifdef MD5_USE_HARDWARE_SKINNING
	skeletonShader = new Shader(SHADERDIR"skeletonVertex.glsl", SHADERDIR"bumpFragment.glsl");
#else
	//currentShader = new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR"TexturedFragment.glsl");
#endif

	MD5FileData* hellData = new MD5FileData(MESHDIR"hellknight.md5mesh");

	MD5Node* hellNode = new MD5Node(*hellData);

	hellNode->SetTransform(Matrix4::Translation(Vector3(2000, 200, 2000)));

	AABoundingBox *aabb = new AABoundingBox(Matrix4(), Vector3());
	aabb->SetCorner(Vector3(-50, 0, -50), Vector3(50, 150, 50));
	aabb->SetCentre(hellNode->GetWorldTransform().GetPositionVector());
	hellNode->SetBoundingVolume(aabb);
	
	hellNode->SetShader(skeletonShader);
	if (!hellNode->GetShader()->LinkProgram())
	{
		return nullptr;
	}

	hellData->AddAnim(MESHDIR"idle2.md5anim");
	hellNode->PlayAnim(MESHDIR"idle2.md5anim");

	//delete hellData;
	return hellNode;
}

inline void Renderer::LoadLights()
{
	numberOfLight = 1;
	lights = new Light * [numberOfLight];
	lights[0] = new Light(Vector3((RAW_HEIGHT * HEIGHTMAP_X / 2.0f),
		500.0f, (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f)),
		Vector4(1.0, 1.0, 1.0, 1.0), Vector4(0, 0, 1.0, 1.0), PointLight, (RAW_WIDTH * HEIGHTMAP_X) / 2.0f, Vector3(0, -1, 0), 60.0f);
}

inline void Renderer::InitPostProcessing()
{
	mapCamera = new Camera(-90.0f, -180.0f, Vector3(4000, 10000, 4000));
	//camera = new Camera(-90.0f, -180.0f, Vector3(4000, 10000, 4000));
	ppQuad1 = Mesh::GenerateQuad();
	Vector3 quad2Positions[4] = { Vector3(0.5f, -1.0f, 1.0f),Vector3(0.5f, -0.5f, 1.0f),Vector3(1.0f, -1.0f, 1.0f),Vector3(1.0f, -0.5f, 1.0f) };
	ppQuad2 = Mesh::GenerateQuad(quad2Positions);
	
	//processShader = new Shader(SHADERDIR "TexturedVertex.glsl", SHADERDIR "processfrag.glsl");
	//processShader = new Shader(SHADERDIR "TexturedVertex.glsl", SHADERDIR "edgeFrag.glsl");
	processShader = new Shader(SHADERDIR "TexturedVertex.glsl", SHADERDIR "doubleVisionFrag.glsl");

	if (!processShader->LinkProgram())
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

void Renderer::DrawPostProcess()
{
	glBindFramebuffer(GL_FRAMEBUFFER, processFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	SetCurrentShader(processShader);
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	viewMatrix.ToIdentity();
	textureMatrix.ToIdentity();
	UpdateShaderMatrices();

	glDisable(GL_DEPTH_TEST);

	glUniform2f(glGetUniformLocation(currentShader->GetProgram(), "pixelSize"), 1.0f / width, 1.0f / height);
	for (int i = 0; i < POST_PASSES; i++)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "isVertical"), 0);

		ppQuad1->SetTexture(bufferColourTex[0]);
		ppQuad1->Draw();
		// Now to swap the colour buffers , and do the second blur pass
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "isVertical"), 1);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[0], 0);

		ppQuad1->SetTexture(bufferColourTex[1]);
		ppQuad1->Draw();

	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);

	glEnable(GL_DEPTH_TEST);
}

void Renderer::PresentScene() 
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	SetCurrentShader(textureShader);
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	viewMatrix.ToIdentity();
	UpdateShaderMatrices();
	ppQuad1->SetTexture(bufferColourTex[0]);
	ppQuad1->Draw();
	ppQuad2->SetTexture(bufferColourTex0[0]);
	ppQuad2->Draw();

	glUseProgram(0);


}

void Renderer::DrawSkybox()
{
	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);
	//glEnable(GL_STENCIL_TEST);
	SetCurrentShader(skyBoxShader);
	//glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)& camera->GetPosition());
	
	//glActiveTexture(GL_TEXTURE2);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, skyBox);
	//glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "cubeTex"), 2);
	UpdateShaderMatrices();
	skyBoxQuad->Draw();

	glUseProgram(0);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
	//glDisable(GL_STENCIL_TEST);
}

