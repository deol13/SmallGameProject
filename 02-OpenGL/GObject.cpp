#include "GObject.h"
#include "stb_image.h"

#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

GObject::GObject()
{
	gTexture = 0;
	gBuffer = 0;
	gAttribute = 0;
	nrOfVertices = 0;
	drawMode = 0;
}
GObject::GObject(std::vector<Vertex> vertices, int drawMode, GLuint gTexture)
{
	this->vertices = vertices;
	this->drawMode = drawMode;
	this->gTexture = gTexture;
	nrOfVertices = 0;
	gBuffer = 0;
}
GObject::~GObject()
{
}

void GObject::scale(float x, float y, float z) //set scale matrix
{
	scaleMatrix[0][0] = x;
	scaleMatrix[1][1] = y;
	scaleMatrix[2][2] = z;
}
void GObject::rotate(float x, float y, float z) //set rotation matrix
{
	glm::mat4 xMatrix = glm::mat4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, cos(x), -sin(x), 0.0f,
		0.0f, cos(x), cos(x), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
	glm::mat4 yMatrix = glm::mat4(
		cos(y), 0.0f, -sin(y), 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		sin(y), 0.0f, cos(y), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
	glm::mat4 zMatrix = glm::mat4(
		cos(z), -sin(z), 0.0f, 0.0f,
		sin(z), cos(z), 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
	rotationMatrix = ( xMatrix * yMatrix * zMatrix) * rotationMatrix;
}
void GObject::translate(float x, float y, float z) //set translation matrix
{
	translationMatrix[3][0] += x;
	translationMatrix[3][1] += y;
	translationMatrix[3][2] += z;
}

void GObject::loadObjectFile(std::string fileName)
{
	std::ifstream inStream( "Resources/" + fileName, std::ios::binary | std::ios::ate );
	int vectorReserveSize = inStream.tellg() * 0.05f;
	inStream = std::ifstream( "Resources/" + fileName );

	if ( !inStream.is_open() ) {
		std::cout << fileName + " not found\n";
		inStream.close();
		return;
	}

	nrOfVertices = 0;
	std::vector<glm::vec3> positions;
	std::vector<glm::vec2> texCoords;
	//std::vector<glm::vec3> normals;
	glm::vec3 pos = glm::vec3();
	glm::vec2 coord = glm::vec2();
	//glm::vec3 normal = glm::vec3();
	std::string indices[ 2 ] = {};
	int i = 0, j = 0;
	Vertex vert = Vertex();

	std::string segment;
	while ( !inStream.eof() ) {
		inStream >> segment;
		if ( segment == "v" ) {
			inStream >> segment;
			pos.x = std::stof( segment );
			inStream >> segment;
			pos.y = std::stof( segment );
			inStream >> segment;
			pos.z = std::stof( segment );
			positions.push_back( pos );
		} else if ( segment == "vt" ) {
			inStream >> segment;
			coord.s = std::stof( segment );
			inStream >> segment;
			coord.t = std::stof( segment );
			texCoords.push_back( coord );
		} else if ( segment == "f" ) {
			j = 0;
			while ( j < 2 ) {
				inStream >> segment;
				i = 0;
				for ( char& c : segment ) {
					if ( c != '/' ) {
						indices[ i ] += c;
					} else {
						i++;
					}
				}
				pos = positions[ std::stoi( indices[ 0 ] ) - 1 ];
				indices[ 0 ].clear();
				coord = texCoords[ std::stoi( indices[ 1 ] ) - 1 ];
				indices[ 1 ].clear();
				//normal = normals[ std::stoi( indices[ 2 ] ) - 1 ];
				//indices[ 2 ].clear();
				vertices.push_back( { pos.x, pos.y, pos.z, coord.s, coord.t } );
				nrOfVertices++;
				j++;
			}
		}
	}
	inStream.close();

	init();
}

void GObject::bindBuffers()
{
	glGenBuffers(1, &gBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, gBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*(nrOfVertices), &vertices[0], GL_STATIC_DRAW);

	glGenVertexArrays(1, &gAttribute);
	glBindVertexArray(gAttribute);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(0));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(sizeof(float)* 3));
}

void GObject::render()
{
	glm::mat4 worldMatrix = translationMatrix * rotationMatrix * scaleMatrix;
	glUniformMatrix4fv(0, 1, false, &worldMatrix[0][0]);
	glBindVertexArray(gAttribute);
	glDrawArrays(drawMode, 0, nrOfVertices);
}

void GObject::init()
{
	bindBuffers();
}

void GObject::setTexture(GLuint gTexture)
{
	this->gTexture = gTexture;
}
GLuint GObject::getTexture()
{
	return gTexture;
}
void GObject::setVertices(std::vector<Vertex> vertices)
{
	this->vertices = vertices;
}
std::vector<Vertex> GObject::getVertices()
{
	return vertices;
}