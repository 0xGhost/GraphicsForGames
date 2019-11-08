#pragma once
/*
Class:OGLRenderer
Author:Rich Davison	<richard.davison4@newcastle.ac.uk>
Description:Abstract base class for the graphics tutorials. Creates an OpenGL 
3.2 CORE PROFILE rendering context. Each lesson will create a renderer that 
inherits from this class - so all context creation is handled automatically,
but students still get to see HOW such a context is created.

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*/
#pragma once
#include "Common.h"

#include <string>
#include <fstream>
#include <vector>

#include "GL/glew.h"
#include "GL/wglew.h"

#include "SOIL.h"

#include "Vector4.h"
#include "Vector3.h"
#include "Vector2.h"
#include "Quaternion.h"
#include "Matrix4.h"
#include "Window.h"
#include "light.h"

#include "Shader.h"		//Students make this file...
#include "Mesh.h"		//And this one...

using std::vector;

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "SOIL.lib")

#define OPENGL_DEBUGGING

static const float biasValues[16] = {
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 0.5, 0.0,
	0.5, 0.5, 0.5, 1.0
};
static const Matrix4 biasMatrix(const_cast<float*>(biasValues));

enum DebugDrawMode {
	DEBUGDRAW_ORTHO,
	DEBUGDRAW_PERSPECTIVE
};

struct DebugDrawData {
	vector<Vector3> lines;
	vector<Vector3> colours;

	GLuint array;
	GLuint buffers[2];

	DebugDrawData();
	void Draw();

	~DebugDrawData() {
		glDeleteVertexArrays(1, &array);
		glDeleteBuffers(2,buffers);
	}

	inline void Clear() {
		lines.clear();
		colours.clear();
	}

	inline void AddLine(const Vector3 &from,const Vector3 &to,const Vector3 &fromColour,const Vector3 &toColour) {
		lines.push_back(from);
		lines.push_back(to);

		colours.push_back(fromColour);
		colours.push_back(toColour);
	} 
};


class Shader;

class OGLRenderer	{
public:
	friend class Window;
	OGLRenderer(Window &parent);
	virtual ~OGLRenderer(void);

	virtual void	RenderScene()		= 0;
	virtual void	UpdateScene(float msec);
	void			SwapBuffers();

	bool			HasInitialised() const;	
	
	static void		DrawDebugLine  (DebugDrawMode mode, const Vector3 &from,const Vector3 &to,const Vector3 &fromColour = Vector3(1,1,1),const Vector3 &toColour = Vector3(1,1,1));
	static void		DrawDebugBox   (DebugDrawMode mode, const Vector3 &at,const Vector3 &scale,const Vector3 &colour = Vector3(1,1,1));
	static void		DrawDebugCross (DebugDrawMode mode, const Vector3 &at,const Vector3 &scale,const Vector3 &colour = Vector3(1,1,1));
	static void		DrawDebugCircle(DebugDrawMode mode, const Vector3 &at,const float radius,const Vector3 &colour = Vector3(1,1,1));	
	
	void			SetAsDebugDrawingRenderer() {
		debugDrawingRenderer = this;
	}

	Shader*			GetCurrentShader() const {
		return currentShader;
	}

protected:
	void SetShaderLight(const Light *l);
	virtual void	Resize(int x, int y);	
	void			UpdateShaderMatrices();
	void			SetCurrentShader(Shader*s);

	void			SetTextureRepeating(GLuint target, bool state);

	//void			SetShaderLight(const Light &l);

	void			DrawDebugPerspective(Matrix4*matrix = 0);
	void			DrawDebugOrtho(Matrix4*matrix = 0);

	Shader* currentShader;
	

	Matrix4 projMatrix;		//Projection matrix
	Matrix4 modelMatrix;	//Model matrix. NOT MODELVIEW
	Matrix4 viewMatrix;		//View matrix
	Matrix4 textureMatrix;	//Texture matrix

	int		width;			//Render area width (not quite the same as window width)
	int		height;			//Render area height (not quite the same as window height)
	bool	init;			//Did the renderer initialise properly?

	HDC		deviceContext;	//...Device context?
	HGLRC	renderContext;	//Permanent Rendering Context

	static DebugDrawData* orthoDebugData;
	static DebugDrawData* perspectiveDebugData;

	static OGLRenderer*	  debugDrawingRenderer;
	static Shader*		  debugDrawShader;

#ifdef _DEBUG
	static void CALLBACK DebugCallback(GLuint source, GLuint type,GLuint id, GLuint severity,
									   int length, const char* message, void* userParam);
#endif

	static bool	drawnDebugOrtho;
	static bool	drawnDebugPerspective;

};