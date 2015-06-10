#ifndef PARTICLES_H
#define PATICLES_H

#include <gl/glew.h>
#include <gl/GL.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include <fstream>
#include <cstdlib>

class Particles
{
public:
	Particles();
	~Particles();
	struct Particle
	{
		glm::vec3 pos;
		float life;
		glm::vec3 velocity;
		float tTrash;
		glm::vec3 trash1;
		float stopRender;
		
		Particle(){};
		~Particle(){};
		Particle(glm::vec3 pPos, glm::vec3 pVelocity, float pLife, float pStopRender)
		{
			pos = pPos;
			velocity = pVelocity;
			life = pLife;
			tTrash = 0.0f;
			stopRender = pStopRender;
		}
	};
	
	int counter;
	int sizeOfPA; //How many particles
	int sizeOfPL; //How many sets of blood
	int* particlesLife;
	Particle* particlesArray;
	GLuint paticleVBO;
	void Bind();
	void createPaticleData(float startPosX, float startPosZ);
	void createNewParticles(float startPosX, float startPosZ, Particle* ptr);
	void removeParticleData();
	void particleArraySet(glm::vec3 velocity, float startPosX, float startPosZ, int pLife);
};

#endif