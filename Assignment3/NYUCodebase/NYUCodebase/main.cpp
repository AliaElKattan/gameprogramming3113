#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <string.h>
#include <vector>

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

#include "ShaderProgram.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//using namespace std;
SDL_Window* displayWindow;

GLuint LoadTexture(const char *filePath) {
	int w, h, comp;
	unsigned char* image = stbi_load(filePath, &w, &h, &comp, STBI_rgb_alpha);
	if (image == NULL) {
		std::cout << "Unable to load image. Make sure the path is correct\n";
		assert(false);
	}

	GLuint retTexture;
	glGenTextures(1, &retTexture);
	glBindTexture(GL_TEXTURE_2D, retTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	stbi_image_free(image);
	return retTexture;
}

class Bullet{
public: 
	Bullet();
	void drawBullet(ShaderProgram &program, glm::mat4 matrix, float x, float y, bool alive);
	//bool bulletisAlive = true;
};

class Entity {

private:
	int index;
	int spriteCountX;
	int spriteCountY;
public:
	Entity();
	void DrawSprite(ShaderProgram &program, int index, int spriteCountX, int spriteCount);
	bool isColliding(float x, float y, float bulletx, float bullety);
	bool isAlive = true;
};



int main(int argc, char *argv[])
{
	int wins = 0;
	int freq = 1;
	int freq2 = 0;
	float enemyammo_x;
	float enemyammo_y;
	float ammopos_x[10][5];
	float ammopos_y[10][5];
	bool bulletalive = false;
	bool ammoalive = false;
	bool trigger = false;
	float bulletx = 0;
	float playerx = 0.;
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Alia's Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
	glewInit();
#endif

	ShaderProgram textured_program;
	ShaderProgram untext_program;
	textured_program.Load(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl"); //for textured
	untext_program.Load(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl"); //for untextured
		

	//GLuint fontTexture = LoadTexture(RESOURCE_FOLDER"font2.png");
	//GLuint spritesheet = LoadTexture(RESOURCE_FOLDER"elementStone021.png");
	//GLuint background = LoadTexture(RESOURCE_FOLDER"backgrounds.png");
	//GLuint bee = LoadTexture(RESOURCE_FOLDER"bee_fly.png");

	glm::mat4 projectionMatrix = glm::mat4(1.0f);
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	glm::mat4 viewMatrix = glm::mat4(1.0f);

	projectionMatrix = glm::ortho(-1.777f, 1.777f, -1.0f, 1.0f, -1.0f, 1.0f);
	
	//float lastFrameTicks = 0.0f;
	//float charx = -2.f;
	glEnable(GL_BLEND);

	float bullety = -.8f;
	//int state = 0;

	GLuint spritesheet = LoadTexture(RESOURCE_FOLDER"characters_1.png");
	Entity player;
	Entity enemies[10][5];
	Bullet ammo;
	Bullet enemyammo[10][5];
	SDL_Event event;
	
	int bulletindex = 0;
	float lastFrameTicks = 0.0f;
    bool done = false;
	while (!done) {

	//ammo[bulletindex] =new Bullet();

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}

			else if (event.type == SDL_KEYDOWN) {
				if (event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
					trigger = true;// DO AN ACTION WHEN SPACE IS PRESSED!
					//ammo.bulletisAlive = true;
					bulletalive = true;
				}
			}


		}

		glClearColor(0.f, .0f, 0.f, 1.0f); //BACKGROUND COLOUR
		glClear(GL_COLOR_BUFFER_BIT);

		textured_program.SetModelMatrix(modelMatrix);
		textured_program.SetProjectionMatrix(projectionMatrix);
		textured_program.SetViewMatrix(viewMatrix);

		untext_program.SetModelMatrix(modelMatrix);
		untext_program.SetProjectionMatrix(projectionMatrix);
		untext_program.SetViewMatrix(viewMatrix);

		//glUseProgram(textured_program.programID);


		//	glDisableVertexAttribArray(program.positionAttribute);

		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;

		//glBindTexture(GL_TEXTURE_2D, spritesheet);
		//glUseProgram(untext_program.programID);
		//untext_program.SetColor(.0f, .0f, .0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, spritesheet);



		const Uint8 *keys = SDL_GetKeyboardState(NULL);

		modelMatrix = glm::mat4(1.0f);

		//move player
		if (keys[SDL_SCANCODE_LEFT]) {
			playerx -= elapsed;
			// go left!
		}
		else if (keys[SDL_SCANCODE_RIGHT]) {
			// go right!
			playerx += elapsed;
		}

		modelMatrix = glm::translate(modelMatrix, glm::vec3(playerx, -.70f, 1.0f));
		bulletx = playerx;
		textured_program.SetModelMatrix(modelMatrix);
		player.DrawSprite(textured_program, 68, 12, 8);


		float elapsed2;
		//bullety = -1.f;

	
		float xt = -1.15;
		float yt = .9;
		bool collision;

	
	
		/*bool isAlive = true;*/
		for (int i = 0; i < 10; i++) {
			xt += .2f;
			yt = .9;
			for (int j = 0; j < 5; j++) {
				//freq = rand() % 100;
			//	freq2 = rand() % 100;
				yt -= .15f;
				modelMatrix = glm::mat4(1.0f);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(xt, yt, 1.0f));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(.75, .5, 1.0f));
				textured_program.SetModelMatrix(modelMatrix);
				collision = enemies[i][j].isColliding(xt,yt,bulletx,bullety);
				
				if (collision == true && enemies[i][j].isAlive == true && bulletalive == true) {
					enemies[i][j].isAlive = false;
					bulletalive = false;
					wins++;
				}

				enemies[i][j].DrawSprite(textured_program, 58, 12, 8);
				ammopos_x[i][j] = xt;
				ammopos_y[i][j] = .75 - (j*.15);

				ammopos_y[i][j] -= .001;
			
				if (j == 5 && enemies[i][j].isAlive ) {
					ammoalive = true;
				}

				else {
					ammoalive = false;
				}

				if (ammopos_y[i][j] < -1.f) {
					ammoalive = false;
				}
			
				enemyammo[i][j].drawBullet(untext_program, modelMatrix, xt, yt, ammoalive);
				//ammoalive = false;
			}

		}

	
	
		//for (int v = 0; v < 10; v++) {
		//	for (int b = 0; b < 10; b++) {

		//		ammopos_y[v][b] = ammopos_y[v][b] - .001;
		//		freq = rand() % 500;
		//		freq2 = rand() % 500;

		//		if ((freq == freq2) && (enemies[v][b].isAlive == true)) {
		//			ammoalive = true;
		//		}

		//		enemyammo[v][b].drawBullet(untext_program, modelMatrix, ammopos_x[v][b], ammopos_y[v][b], ammoalive);
		//					   				 			
		//	}
		//}


		bulletx = playerx;

		if (trigger == true) {

			//use elapsed here
			bullety += .001f;
			//bulletx = playerx;

			if (bullety > 1.7f || bulletalive == false) {
				trigger = false;
				bullety = -1.f;
			}

			glUseProgram(untext_program.programID);

				ammo.drawBullet(untext_program, modelMatrix, bulletx, bullety, bulletalive);
			
		}
	
		glUseProgram(untext_program.programID);
		untext_program.SetColor(1.f, .2f, .2f, 1.0f);

		bullety += elapsed;

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		glDisableVertexAttribArray(textured_program.positionAttribute);
		glDisableVertexAttribArray(textured_program.texCoordAttribute);
		
		SDL_GL_SwapWindow(displayWindow);


    }
    
    SDL_Quit();
    return 0;
}

Entity::Entity() {
	
};

bool Entity::isColliding(float x, float y, float bulletx, float bullety) {

	if (bulletx > (x - .1) && bulletx<(x + .1) && bullety>(y - .1) && bullety < (y + .1)) {
		return true;
	}
	else
		return false;
}

void Entity::DrawSprite(ShaderProgram &program, int i, int scX, int scY) {

	if (isAlive == true) {
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(.0f, .70f, 1.0f));

		index = i;
		spriteCountX = scX;
		spriteCountY = scY;

		float u = (float)(((int)index) % spriteCountX) / (float)spriteCountX;
		float v = (float)(((int)index) / spriteCountY) / (float)spriteCountY;
		float spriteWidth = 1.0 / (float)spriteCountX;
		float spriteHeight = 1.0 / (float)spriteCountY;

		float texCoords[] = {
			u, v + spriteHeight,
			u + spriteWidth, v,
			u, v,
			u + spriteWidth, v,
			u, v + spriteHeight,
			u + spriteWidth, v + spriteHeight };

		float vertices[] = { -0.1f, -0.1f, 0.1f, 0.1f, -0.1f, 0.1f, 0.1f, 0.1f,  -0.1f,-0.1f, 0.1f, -0.1f };

		glUseProgram(program.programID);

		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
		glEnableVertexAttribArray(program.texCoordAttribute);
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
		glEnableVertexAttribArray(program.positionAttribute);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(program.positionAttribute);
	}
	}



Bullet::Bullet() {
}



void Bullet::drawBullet(ShaderProgram &untext_program, glm::mat4 modelMatrix, float bulletx, float bullety, bool bulletalive) {

	if (bulletalive == true) {
		//bulletalive = false;
		modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(bulletx, bullety, 1.0f));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(.1f, .5f, 1.0f));
		untext_program.SetModelMatrix(modelMatrix);

		float bullet[] = { .1f,-.1f,.1f,.1f,-.1f,-.1f,-.1f,-.1f,.1f,.1f,-.1f,.1f };
		glVertexAttribPointer(untext_program.positionAttribute, 2, GL_FLOAT, false, 0, bullet);
		glEnableVertexAttribArray(untext_program.positionAttribute);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(untext_program.positionAttribute);
	}
}