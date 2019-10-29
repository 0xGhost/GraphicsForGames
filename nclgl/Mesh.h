#pragma once
#include "OGLRenderer.h"

enum MeshBuffer
{
	VERTEX_BUFFER, COLOUR_BUFFER, MAX_BUFFER
};

class Mesh
{
public:
	Mesh();
	~Mesh();
	virtual void Draw();
	static Mesh* GenerateTriangle();
	void *colorPtr;
protected:
	void BufferData();

	GLuint arrayObject;
	GLuint bufferObject[MAX_BUFFER];
	GLuint numVertices;
	GLuint type;

	Vector3* vertices;
	Vector4* colours;

};

