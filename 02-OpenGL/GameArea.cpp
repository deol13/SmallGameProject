#include "GameArea.h"
#include "stb_image.h"
#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

GameArea::GameArea()
{

}
GameArea::~GameArea()
{
	delete[] GAMap;
}

//GA
void GameArea::createGA(int GASIZE)
{
	IBOCounter = 0;
	GAwidth = GASIZE;
	GAheight = GASIZE;
	GASize = GAwidth*GAheight;

	GAMap = new Vertex[GASize];
	//for (int column = 0; column < GAheight; column++)
	//{
	//	for (int row = 0; row < GAwidth; row++)
	//	{
	//		GAMap[column*GAwidth + row] = Vertex((row), 1.0f, (column));//Vertex(-(float)row, 1.0f, -(float)column); , (float)row / (float)width, (float)column / (float)height);
	//	}
	//}

	GAMap[0] = Vertex((float)GASIZE, 1.0f, (float)GASIZE, 0.0f, 1.0f );
	GAMap[1] = Vertex((float)GASIZE, 1.0f, 0.0f, 0.0f, 0.0f);
	GAMap[2] = Vertex(0.0f, 1.0f, (float)GASIZE, 1.0f, 1.0f);
	GAMap[3] = Vertex(0.0f, 1.0f, 0.0f, 1.0f, 0.0f);


	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
		printf("Error");
}
void GameArea::createIBO()
{
	std::vector<unsigned int> IBO;
	IBO.push_back(0);
	IBO.push_back(1);
	IBO.push_back(2);
	IBO.push_back(3);
	IBOCounter = 4;

	/*for (int i = 0; i < GAheight - 1; i++)
	{
		if ((i % 2) == 0)
		{
			for (int j = 0; j < GAwidth; j++)
			{
				IBO.push_back(i*GAwidth + j);
				IBO.push_back((i + 1)*GAwidth + j);
			}
		}
		else
		{
			for (int j = GAwidth - 1; j >= 0; j--)
			{
				IBO.push_back(i*GAwidth + j);
				IBO.push_back((i + 1)*GAwidth + j);
			}
		}
		IBO.push_back(IBO.back());
	}
	IBOCounter = IBO.size();*/

	glGenBuffers(1, &gIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, IBOCounter * sizeof(unsigned int), &IBO[0], GL_STATIC_DRAW);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
		printf("Error");
}

void GameArea::loadImage()
{
	int comp;
	const char* filename = new char();
	filename = "GridImage.png";
	GLubyte* gImage = stbi_load(filename, &imageWidth, &imageHeight, &comp, 4);

	if (gImage == nullptr)
		throw(std::string("Failed to load texture"));

	createTexture(gImage);
}

void GameArea::createTexture(GLubyte* gImage)
{
	//create grass texture
	glGenTextures(1, &imageTex);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, imageTex);

	glTexStorage2D(GL_TEXTURE_2D, 8, GL_RGBA8, imageWidth, imageHeight);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, imageWidth, imageHeight, GL_BGRA, GL_UNSIGNED_BYTE, (const void*)gImage);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void GameArea::GABuffers()
{
	glGenBuffers(1, &gGABuffer);
	glBindBuffer(GL_ARRAY_BUFFER, gGABuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*(GASize), &GAMap[0], GL_STATIC_DRAW);

	glGenVertexArrays(1, &gGAAttribute);
	glBindVertexArray(gGAAttribute);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(0));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(sizeof(float) * 3));

#ifdef _DEBUG
	{GLenum err = glGetError(); if (err)
		int x = 0; }
#endif
}


//Get
int GameArea::getIBOCount()
{
	return IBOCounter;
}
int GameArea::getWidth()
{
	return GAwidth;
}
int GameArea::getHeight()
{
	return GAheight;
}