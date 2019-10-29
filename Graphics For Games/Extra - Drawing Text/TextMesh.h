/******************************************************************************
Class:TextMesh
Implements:Mesh
Author:Rich Davison
Description:A simple extension to the Mesh class, which will build up a mesh
that represents a string of text, rendered using a texture containing a 
bitmap font. Once the TextMesh has been generated, it can be used to render
a line of text on screen - either for multiple frames by keeping its pointer
like any other mesh, or by simply creating and deleting TextMeshes as text
is required.

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   This is nyantext!!
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*//////////////////////////////////////////////////////////////////////////////
#pragma once
#include "../../NCLGL/mesh.h"
#include <vector>

/*
A TextMesh uses a 'Font', simply a struct which holds a texture, and some info
about that texture. bitmap fonts have a grid like array of cells, usually
arranged so that they are in ascending byte order. So by storing the 
number of cells across and down the font texture has, we can work out which
bit of the texture is which text character (assuming they are in byte order).

The Font class cleans up after itself, so we don't have to keep track of its
font texture anywhere else!
*/
struct Font {
	GLuint	texture;
	int		xCount;
	int		yCount;

	Font(GLuint tex, unsigned int xCount, unsigned int yCount) {
		this->texture = tex;
		this->xCount  = xCount;
		this->yCount  = yCount;
	}
	~Font() {
		glDeleteTextures(1,&texture);
	}
};

class TextMesh : public Mesh	{
public:
	TextMesh(const std::string &text, const Font &font);
	~TextMesh(void);
protected:

	const Font& font;
};

