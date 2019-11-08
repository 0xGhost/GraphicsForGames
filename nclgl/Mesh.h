#pragma once
#include "OGLRenderer.h"

enum MeshBuffer
{
	VERTEX_BUFFER, COLOUR_BUFFER, TEXTURE_BUFFER, NORMAL_BUFFER, INDEX_BUFFER, MAX_BUFFER
};

class Mesh
{
public:
	Mesh();
	~Mesh();
	virtual void Draw();
	static Mesh* GenerateTriangle();
	static Mesh* GenerateQuad(Vector3 position[4] = nullptr);
	static Mesh* GenerateCube();


	void *colorPtr;
	void SetTexture(GLuint tex) { texture = tex; }
	GLuint GetTexture() { return texture; }

protected:
	void BufferData();
	void GenerateNormals();

	GLuint arrayObject;
	GLuint bufferObject[MAX_BUFFER];
	GLuint numVertices;
	GLuint type;

	GLuint numIndices;
	unsigned int* indices;
	GLuint texture;
	Vector2* textureCoords;
	Vector3* vertices;
	Vector4* colours;
	Vector3* normals;
};

