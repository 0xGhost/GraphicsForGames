#include "Renderer.h"
#include "../../nclgl/BoundingSphere.h"
#include "../../nclgl/AABoundingBox.h"

Renderer::Renderer(Window& parent) : OGLRenderer(parent), window(&parent), showBoundingVolume(true)
{
	BoundingSphere::CreateSphereMesh();
	BoundingBox::CreateBoxMesh();
	camera = new Camera();
	skyBoxQuad = Mesh::GenerateQuad();
	
	

	quad = Mesh::GenerateQuad();
	sphere = new OBJMesh();
	if (!sphere->LoadOBJMesh(MESHDIR "sphere.obj"))
	{
		return;
	}

	
	boundingVolumeShader = new Shader(SHADERDIR"BoundingVolumeVertex.glsl", SHADERDIR"BoundingVolumeFragment.glsl");
	if (!boundingVolumeShader->LinkProgram()) return;

	sceneShader = new Shader(SHADERDIR"BumpVertex.glsl", SHADERDIR"BumpFragment.glsl");
	if (!sceneShader->LinkProgram()) return;
	
	skyBoxShader = new Shader(SHADERDIR "skyboxVertex.glsl", SHADERDIR "skyboxFragment.glsl");
	if (!skyBoxShader->LinkProgram()) return;
	LoadSkyBox();
	LoadLights();
	InitScene0();
	

	InitPostProcessing();
	showPostProcessing = false;

	InitScene1();
	InitShadow();

	sceneNum = 1;
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_CULL_FACE);
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
	delete oceanMesh;
	delete skyBoxQuad;
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
	glDeleteTextures(1, &shadowTex);
	glDeleteFramebuffers(1, &shadowFBO);
}

void Renderer::UpdateScene(float msec)
{
	projMatrix = perspectiveMatrix;
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
	frameFrustum.FromMatrix(projMatrix * viewMatrix);
	root[sceneNum]->Update(msec);
	waterRotate = msec / 1000.0f;
	waterNode->SetTextureMatrix(waterNode->GetTextureMatrix() * Matrix4::Rotation(waterRotate, Vector3(0.0f, 0.0f, 1.0f)));

}

void Renderer::RenderScene()
{

	BuildNodeLists(root[sceneNum]);
	SortNodeLists();

	if (sceneNum == 0)
	{
		// draw scene to FBO
		glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		projMatrix = perspectiveMatrix;
		SetCurrentShader(sceneShader);
		UpdateShaderMatrices();
		DrawNodes();
		DrawSkybox(sceneNum);

		// draw map from mapCamera to FBO0
		bool temp = showBoundingVolume;
		showBoundingVolume = false;
		viewMatrix = mapCamera->BuildViewMatrix();
		glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO0);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		SetCurrentShader(sceneShader);
		projMatrix = perspectiveMatrix;
		UpdateShaderMatrices();
		DrawNodes();
		showBoundingVolume = temp;

		DrawPostProcess();
		PresentScene();
	}
	if (sceneNum == 1)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		projMatrix = perspectiveMatrix;
		UpdateShaderMatrices();
		DrawSkybox(sceneNum);
		//DrawOcean();
		
		DrawShadowScene(); // First render pass ...
		DrawCombinedScene(); // Second render pass ...
		
	}

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
	glEnable(GL_STENCIL_TEST);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glStencilFunc(GL_ALWAYS, 2, ~0);
	glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
	if (n->GetMesh())
	{
		if (n->GetShader()) SetCurrentShader(n->GetShader());
		//SetCurrentShader(n->GetShader() != NULL ? n->GetShader() : sceneShader);
		Matrix4 temp = modelMatrix;
		modelMatrix = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
		textureMatrix = n->GetTextureMatrix();	
			
		UpdateShaderMatrices();
		
		glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "time"), GetMS());
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bumpTex"), 1);
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "cubeTex"), 2);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyBox[0]);
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "cubeTex2"), 3);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyBox[1]);
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
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glStencilFunc(GL_NOTEQUAL, 2, ~0);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glDisable(GL_STENCIL_TEST);
}

inline void Renderer::InitScene0()
{
	camera->SetPosition(Vector3(0, 100, 750));
	projMatrix = perspectiveMatrix;

	textureShader = new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR"TexturedFragment.glsl");
	if (!textureShader->LinkProgram()) return;
	
	quad->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"brick.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0));
	if (!quad->GetTexture()) return;
	quad->SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR"brickDOT3.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0));

	root[0] = new SceneNode();
	//root->SetBoundingVolume(new )
	for (int i = 0; i < 5; ++i) {
		SceneNode* s = new SceneNode();
		s->SetColour(Vector4(1.0f, 1.0f, 1.0f, 0.5f));
		s->SetTransform(Matrix4::Translation(Vector3(1000, 1500.0f, 2000.0f + 100.0f + 500 * i))
			* Matrix4::Rotation(45, Vector3(1.0f, 0.0f, 0.0f)));
		s->SetModelScale(Vector3(100.0f, 100.0f, 100.0f));
		s->SetMesh(quad);

		s->SetBoundingVolume(new AABoundingBox(s->GetWorldTransform(), s->GetModelScale(), quad));
		//s->SetBoundingVolume(new BoundingSphere(s->GetWorldTransform(), s->GetModelScale(), quad));
		root[0]->AddChild(s);
	}

	// portal
	SceneNode* p = new SceneNode();
	p->SetColour(Vector4(1.0f, 1.0f, 1.0f, 0.5f));
	p->SetTransform(Matrix4::Translation(Vector3(4000, 1000.0f, 4000.0f)));
	p->SetModelScale(Vector3(200.0f, 200.0f, 200.0f));
	p->SetMesh(sphere);
	Shader* portalShader = new Shader(SHADERDIR"portalVertex.glsl", SHADERDIR"portalFragment.glsl");
	if (!portalShader->LinkProgram()) return;
	p->SetShader(portalShader);
	p->SetBoundingVolume(new AABoundingBox(p->GetWorldTransform(), p->GetModelScale(), sphere));
	//s->SetBoundingVolume(new BoundingSphere(s->GetWorldTransform(), s->GetModelScale(), quad));
	root[0]->AddChild(p);


	

	heightMapNode = LoadHeightMap();
	hellKnightNode = LoadHellKnight();
	waterNode = LoadWater();
	root[0]->AddChild(heightMapNode);

	heightMapNode->AddChild(hellKnightNode);
	heightMapNode->AddChild(waterNode);

	
}

inline void Renderer::InitScene1()
{
	root[1] = new SceneNode();
	LoadOcean();
	LoadPyramid();
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

inline void Renderer::LoadOcean()
{
	oceanMesh = new HeightMap();
	oceanMesh->SetTexture(SOIL_load_OGL_texture(
		TEXTUREDIR "water.tga", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	oceanMesh->SetBumpMap(SOIL_load_OGL_texture(
		TEXTUREDIR "waterDOT3.tga", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	SetTextureRepeating(oceanMesh->GetTexture(), true);
	SetTextureRepeating(oceanMesh->GetBumpMap(), true);
	shadowOceanShader = new Shader(SHADERDIR"OceanShadowVertex.glsl", SHADERDIR"bumpFragment.glsl");
	if (!shadowOceanShader->LinkProgram()) 
		return;
	/*
	oceanNode = new SceneNode();
	oceanNode->SetShader(shadowSceneShader);
	oceanNode->SetMesh(oceanMesh);
	oceanNode->SetColour(Vector4(1.0f, 1.0f, 1.0f, 0.5f));
	oceanNode->SetTransform(Matrix4::Translation(Vector3(0, 0, 0)));
	oceanNode->SetModelScale(Vector3(1.0f, 1.0f, 1.0f));
	oceanNode->SetBoundingVolume(new AABoundingBox(oceanNode->GetWorldTransform(), oceanNode->GetModelScale(), oceanMesh));
	return oceanNode;
	*/
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
	
	skyBox[0] = SOIL_load_OGL_cubemap(
		TEXTUREDIR"rusted_west.jpg", TEXTUREDIR"rusted_east.jpg",
		TEXTUREDIR"rusted_up.jpg", TEXTUREDIR"rusted_down.jpg",
		TEXTUREDIR"rusted_south.jpg", TEXTUREDIR"rusted_north.jpg",
		SOIL_LOAD_RGB,
		SOIL_CREATE_NEW_ID, 0
	); 

	skyBox[1] = SOIL_load_OGL_cubemap(
		TEXTUREDIR"lagoon_ft.tga", TEXTUREDIR"lagoon_bk.tga",
		TEXTUREDIR"lagoon_up.tga", TEXTUREDIR"lagoon_dn.tga",
		TEXTUREDIR"lagoon_rt.tga", TEXTUREDIR"lagoon_lf.tga",
		SOIL_LOAD_RGB,
		SOIL_CREATE_NEW_ID, 0
	);
	if (!skyBox[0] || !skyBox[1]) return;
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

	hellNode->SetTransform(Matrix4::Translation(Vector3(2000, 1000.0f, 2000)));

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
	//lights[0] = new Light(Vector3((RAW_HEIGHT * HEIGHTMAP_X / 2.0f),
		//500.0f, (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f)),
		//Vector4(1.0, 1.0, 0.5, 1.0), Vector4(1.0, 0.5, 0.5, 1.0), DirectionLight, (RAW_WIDTH * HEIGHTMAP_X) / 2.0f, Vector3(0, -1, 0), 60.0f);
	Vector3 d = Vector3(0, -1,0);
	//Vector3 d = Vector3(-0.15, -1, 0.15);
	d.Normalise();
	lights[0] = new Light(Vector3(0.0f, 500.0f, 0.0f),
		Vector4(1, 1, 1, 1), Vector4(1, 1, 1, 1), SpotLight, 5500.0f, d, 80.0f);
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
	int pass;
	if (showPostProcessing)
	{
		pass = POST_PASSES;
		SetCurrentShader(processShader); 
	}
	else
	{
		pass = 1;
		SetCurrentShader(textureShader);
	}
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

void Renderer::DrawSkybox(int i)
{
	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);
	if(sceneNum == 0)
	glEnable(GL_STENCIL_TEST);
	SetCurrentShader(skyBoxShader);
	//glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)& camera->GetPosition());
	
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyBox[i]);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "cubeTex"), 2);
	UpdateShaderMatrices();
	skyBoxQuad->Draw();

	glUseProgram(0);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
	glDisable(GL_STENCIL_TEST);
}

inline void Renderer::InitShadow()
{
	shadowSceneShader = new Shader(SHADERDIR"shadowscenevert.glsl",
		SHADERDIR"shadowscenefrag.glsl");
	shadowShader = new Shader(SHADERDIR"shadowVert.glsl",
		SHADERDIR"shadowFrag.glsl");

	if (!shadowSceneShader->LinkProgram() || !shadowShader->LinkProgram())
	{
		return;
	}

	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE,
		GL_COMPARE_R_TO_TEXTURE);

	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &shadowFBO);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D, shadowTex, 0);
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawShadowScene()
{
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);

	glClear(GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);

	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	SetCurrentShader(shadowShader);

	if (lights[0]->GetType() == DirectionLight)
	{
		projMatrix = Matrix4::Orthographic(-1, 20000, 1000, -1000, 1000, -1000);
	}
	else
	{
		projMatrix = Matrix4::Perspective(1.0f, 15000.0f,
			(float)width / (float)height, 105.0f);
	}
	viewMatrix = Matrix4::BuildViewMatrix(
		lights[0]->GetPosition(), /*lights[0]->GetPosition() +*/ Vector3(1,0,1));
	textureMatrix = biasMatrix * (projMatrix * viewMatrix);

	UpdateShaderMatrices();

	
	
	DrawOcean();
	DrawPyramid();
	glUseProgram(0);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, width, height);
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawCombinedScene() {
	SetCurrentShader(shadowSceneShader);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"bumpTex"), 1);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"shadowTex"), 8);

	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(),
		"cameraPos"), 1, (float*)& camera->GetPosition());

	SetShaderLight(*lights);

	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glActiveTexture(GL_TEXTURE0);

	viewMatrix = camera->BuildViewMatrix();
	UpdateShaderMatrices();

	//DrawPyramid();
	DrawOcean();
	//DrawFloor();
	//DrawMesh();
	DrawPyramid();
	glUseProgram(0);
}

inline void Renderer::DrawOcean()
{
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "time"), GetMS());
	oceanMesh->Draw();
}

inline void Renderer::LoadPyramid()
{
	pyramidMesh = new OBJMesh();
	if (!pyramidMesh->LoadOBJMesh(MESHDIR"pyramid.obj"))
	{
		return;
	}
	
	pyramidMesh->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"brick.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0));
	if (!pyramidMesh->GetTexture()) return;
	pyramidMesh->SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR"brickDOT3.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0));
	if (!pyramidMesh->GetBumpMap()) return;
	SetTextureRepeating(waterQuad->GetTexture(), true);
	SetTextureRepeating(waterQuad->GetBumpMap(), true);
}

inline void Renderer::DrawPyramid()
{


	Matrix4 modelMatrixP = Matrix4::Translation(Vector3(500, 800, 300))
		* Matrix4::Rotation(-90.0f, Vector3(1, 0, 0))
		* Matrix4::Scale(Vector3(800, 800, 800));

	Matrix4 tempMatrix = textureMatrix * modelMatrixP;

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram()
		, "textureMatrix"), 1, false, *&tempMatrix.values);

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram()
		, "modelMatrix"), 1, false, *&modelMatrixP.values);
	
	pyramidMesh->Draw();

}
