#include "Mesh.h"

Mesh::Mesh()
{
	memset(bufferObject, 0, MAX_BUFFER * sizeof(GLuint));
	glGenVertexArrays(1, &arrayObject);
	indices = NULL;
	numIndices = 0;
	texture = 0;
	textureCoords = nullptr;
	numVertices = 0;
	vertices = nullptr;
	colours = nullptr;
	type = GL_TRIANGLES;
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &arrayObject);
	glDeleteBuffers(MAX_BUFFER, bufferObject);
	glDeleteTextures(1, &texture);
	delete[] textureCoords;
	delete[] vertices;
	delete[] colours;
	delete[] indices;
}

void Mesh::Draw()
{
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject[COLOUR_BUFFER]);
	glBindTexture(GL_TEXTURE_2D, texture);
#if 0
	void* colorPtr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	for (int i = 0; i < numVertices; i++)
	{
		((Vector4*)colorPtr)[i] = Vector4((float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX, 1.0f);
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);
#endif
	glBindVertexArray(arrayObject);
	if (bufferObject[INDEX_BUFFER]) 
	{
		glDrawElements(type, numIndices, GL_UNSIGNED_INT, 0);
	}
	else
	{
		glDrawArrays(type, 0, numVertices);
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

Mesh* Mesh::GenerateTriangle()
{
	Mesh* m = new Mesh();
	m->numVertices = 3;
	m->vertices = new Vector3[m->numVertices];
	m->vertices[0] = Vector3(0.0f, 0.5f, 0.0f);
	m->vertices[1] = Vector3(0.5f, -0.5f, 0.0f);
	m->vertices[2] = Vector3(-0.5f, -0.5f, 0.0f);
	/*m->vertices[3] = Vector3(-0.2f, 0.0f, 0.0f);
	m->vertices[4] = Vector3(-0.1f, 0.2f, 0.0f);
	m->vertices[5] = Vector3(0.1f, 0.2f, 0.0f);
	m->vertices[6] = Vector3(0.2f, 0.0f, 0.0f);
	m->vertices[7] = Vector3(0.1f, -0.2f, 0.0f);*/

	m->textureCoords = new Vector2[m->numVertices];
	m->textureCoords[0] = Vector2(0.5f, 0.0f);
	m->textureCoords[1] = Vector2(1.0f, 1.0f);
	m->textureCoords[2] = Vector2(0.0f, 1.0f);

	m->colours = new Vector4[m->numVertices];
	m->colours[0] = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
	m->colours[1] = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
	m->colours[2] = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
	/*m->colours[3] = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
	m->colours[4] = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
	m->colours[5] = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
	m->colours[6] = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
	m->colours[7] = Vector4(0.0f, 1.0f, 0.0f, 1.0f);*/

	m->BufferData();
	return m;
}

Mesh* Mesh::GenerateQuad(Vector3 positions[4])
{
	Mesh* m = new Mesh();
	m->numVertices = 4;
	m->type = GL_TRIANGLE_STRIP;

	m->vertices = new Vector3[m->numVertices];
	m->textureCoords = new Vector2[m->numVertices];
	m->colours = new Vector4[m->numVertices];
	if (positions == nullptr)
	{
		m->vertices[0] = Vector3(-1.0f, -1.0f, 0.0f);
		m->vertices[1] = Vector3(-1.0f, 1.0f, 0.0f);
		m->vertices[2] = Vector3(1.0f, -1.0f, 0.0f);
		m->vertices[3] = Vector3(1.0f, 1.0f, 0.0f);
	}
	else
	{
		memcpy(m->vertices, positions, 4 * sizeof(Vector3));
	}


	m->textureCoords[0] = Vector2(0.0f, 1.0f);
	m->textureCoords[1] = Vector2(0.0f, 0.0f);
	m->textureCoords[2] = Vector2(1.0f, 1.0f);
	m->textureCoords[3] = Vector2(1.0f, 0.0f);

	for (int i = 0; i < 4; i++)
	{
		m->colours[i] = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	m->BufferData();
	return m;

}



void Mesh::BufferData()
{
	glBindVertexArray(arrayObject);
	glGenBuffers(1, &bufferObject[VERTEX_BUFFER]);
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject[VERTEX_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vector3), vertices, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(VERTEX_BUFFER, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(VERTEX_BUFFER);
	if (textureCoords)
	{
		glGenBuffers(1, &bufferObject[TEXTURE_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[TEXTURE_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vector2), textureCoords, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(TEXTURE_BUFFER, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(TEXTURE_BUFFER);
	}

	if (colours)
	{
		glGenBuffers(1, &bufferObject[COLOUR_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[COLOUR_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vector4), colours, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(COLOUR_BUFFER, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(COLOUR_BUFFER);
	}

	if (indices) 
	{
		glGenBuffers(1, &bufferObject[INDEX_BUFFER]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferObject[INDEX_BUFFER]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(GLuint), indices, GL_STATIC_DRAW);
	}

	glBindVertexArray(0);
}


