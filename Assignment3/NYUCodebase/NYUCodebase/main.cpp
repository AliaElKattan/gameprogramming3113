#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <string.h>
#include <vector>
#include <SDL_mixer.h>

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

void DrawText(ShaderProgram &program, int fontTexture, std::string text, float size, float spacing);


int main(int argc, char *argv[])
{

	//initialize audio
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

	Mix_Chunk *beepSound, *loseSound, *winSound;
	Mix_Music *introMusic;
	introMusic = Mix_LoadMUS("intro_music.wav");
	beepSound = Mix_LoadWAV("beep_sound.wav");
	loseSound = Mix_LoadWAV("lose.wav");
	winSound = Mix_LoadWAV("bonus.wav");


	//ASSIGN VARIABLES
	int maxrow;
	int gameState = 0; //gamestate 0 = opening, 1 = main game, 2 = you win, 3 = you lose
	int wins = 0; //number of insects killed
	int hits = 0; //number of times player is hit
	float enemyammo_x = 0;
	float enemyammo_y = 1.f;
	bool bulletalive = false;
	bool ammoalive = false;
	bool trigger = false;
	float bulletx = 0;
	float playerx = 0.;
	bool musicOn = false;


	//SETUP
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Alia's Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
	glewInit();
#endif

	//SHADERS

	ShaderProgram textured_program;
	ShaderProgram untext_program;
	textured_program.Load(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl"); //for textured
	untext_program.Load(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl"); //for untextured
		

	GLuint fontTexture = LoadTexture(RESOURCE_FOLDER"font2.png");

	glm::mat4 projectionMatrix = glm::mat4(1.0f);
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	glm::mat4 viewMatrix = glm::mat4(1.0f);

	projectionMatrix = glm::ortho(-1.777f, 1.777f, -1.0f, 1.0f, -1.0f, 1.0f);
	

	glEnable(GL_BLEND);

	float bullety = -.8f;

	GLuint spritesheet = LoadTexture(RESOURCE_FOLDER"characters_1.png");
	Entity player;
	Entity enemies[8][4];
	Bullet ammo;
	Bullet enemyammo[10];
	SDL_Event event;
	
	int bullindex = 0;
	float lastFrameTicks = 0.0f;
    bool done = false;
	while (!done) {

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}

			else if (event.type == SDL_KEYDOWN) {
				if (event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
					
					if (gameState == 0) {
						gameState = 1; //switch to main screen
					}

					if (gameState == 1) {
						trigger = true; //shoot bullet up
						bulletalive = true;
					}
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

		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;

		if (gameState == 0) {

			if (musicOn ==false){
				Mix_PlayMusic(introMusic, 1);
				musicOn = true;
			}
					glUseProgram(textured_program.programID);
					modelMatrix = glm::mat4(1.0f);
					modelMatrix = glm::translate(modelMatrix, glm::vec3(-1.05f, .3f, 1.0f));
					textured_program.SetModelMatrix(modelMatrix);
					DrawText(textured_program, fontTexture,"SPIDER INVADERS", .15f, .001f);

					modelMatrix = glm::mat4(1.0f);
					modelMatrix = glm::translate(modelMatrix, glm::vec3(-.7f, .0f, 1.0f));
					textured_program.SetModelMatrix(modelMatrix);
					DrawText(textured_program, fontTexture, "destroy the insects while", .07f, -.009f);

					modelMatrix = glm::mat4(1.0f);
					modelMatrix = glm::translate(modelMatrix, glm::vec3(-.77f, -.15f, 1.0f));
					textured_program.SetModelMatrix(modelMatrix);
					DrawText(textured_program, fontTexture, "avoiding the incoming lasers", .07f, -.009f);
					

					modelMatrix = glm::mat4(1.0f);
					modelMatrix = glm::translate(modelMatrix, glm::vec3(-.73f, -.5f, 1.0f));
					textured_program.SetModelMatrix(modelMatrix);
					DrawText(textured_program, fontTexture, "press space to start", .08f, -.001f);

		}

		if (gameState == 1) {
			
			glBindTexture(GL_TEXTURE_2D, spritesheet);

			const Uint8 *keys = SDL_GetKeyboardState(NULL);

			modelMatrix = glm::mat4(1.0f);

			//move player
			if (keys[SDL_SCANCODE_LEFT]) {
				if (playerx > -1.7f)
				playerx -= elapsed;
				// go left
			}
			else if (keys[SDL_SCANCODE_RIGHT]) {
				// go right

				if (playerx < 1.7f)
				playerx += elapsed;
			}

			//draw player
			modelMatrix = glm::translate(modelMatrix, glm::vec3(playerx, -.70f, 1.0f));
			bulletx = playerx;
			textured_program.SetModelMatrix(modelMatrix);
			player.DrawSprite(textured_program, 68, 12, 8);
		
			
			float xt = -1.15;
			float yt;
			bool collision;
					   
			
			//draw matrix of insects
			for (int i = 0; i < 8; i++) {
				xt += .25f;
				yt = .8;
				for (int j = 0; j < 4; j++) {
					yt -= .15f;
					modelMatrix = glm::mat4(1.0f);
					modelMatrix = glm::translate(modelMatrix, glm::vec3(xt, yt, 1.0f));
					modelMatrix = glm::scale(modelMatrix, glm::vec3(.9, .6, 1.0f));
					textured_program.SetModelMatrix(modelMatrix);
					collision = enemies[i][j].isColliding(xt, yt, bulletx, bullety);

					//check if bullet is colliding with 1 of the insects
					if (collision == true && enemies[i][j].isAlive == true && bulletalive == true) {
						Mix_PlayChannel(-1, beepSound, 0);
						enemies[i][j].isAlive = false;
						bulletalive = false;
						wins++;
					}

					enemies[i][j].DrawSprite(textured_program, 58, 12, 8);
				}

			}

				//shoot lasers down
				enemyammo_x = -.9f + (bullindex * .25f); 
				enemyammo_y -= elapsed * 3.5;
				modelMatrix = glm::mat4(1.0f);
				modelMatrix = glm::scale(modelMatrix, glm::vec3(1.f, .3, 1.0f));
				untext_program.SetModelMatrix(modelMatrix);
				enemyammo[0].drawBullet(untext_program, modelMatrix, enemyammo_x, enemyammo_y, true);

				if (enemyammo_y > -1.f && enemyammo_y < .6 && (enemyammo_x > (playerx - .1)) && (enemyammo_x < (playerx + .1))) {
					enemyammo_y = 1.f; //restart laser position if it collides so that the hits var only increments once
					bullindex += rand() % 7;

					if (bullindex > 6) {
						bullindex = 0;
						bullindex += rand() % 7;
					}

					hits++;
				}

				glUseProgram(textured_program.programID);
				modelMatrix = glm::mat4(1.0f);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(-1.6f, .85f, 1.0f));
				textured_program.SetModelMatrix(modelMatrix);

				if (hits == 0) {
					DrawText(textured_program, fontTexture, "LIVES: 3", .07f, -.005f);
				}

				if (hits == 1) {
					DrawText(textured_program, fontTexture, "LIVES: 2", .07f, -.005f);
				}

				if (hits == 2) {
					DrawText(textured_program, fontTexture, "LIVES: 1", .07f, -.005f);
				}

				//^read an int into a string instead?

			
			if (enemyammo_y < -1.77f) {
				enemyammo_y = 1.f;
				bullindex+=rand()%8; //randomize column laser comes down through

				if (bullindex > 7) { //make sure lasers don't go out of bounds
					bullindex = 0;
					bullindex += rand() % 8;
				}
			}

			bulletx = playerx;

			if (trigger == true) {

				bullety += elapsed; //bullet moves up
				
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

			//if all insects are killed, player wins
			if (wins == 32) {
				Mix_PlayChannel(-1, winSound,0);
				gameState = 3; 
			}

			//if player is hit 3 times, player loses
			if (hits == 3) {
				Mix_PlayChannel(-1, loseSound, 0);
				gameState = 4;
			}
			

		}

		if (gameState == 3) {

			glUseProgram(textured_program.programID);
			modelMatrix = glm::mat4(1.0f);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(-1.05f, .3f, 1.0f));
			textured_program.SetModelMatrix(modelMatrix);
			DrawText(textured_program, fontTexture, "CONGRATULATIONS", .15f, .001f);

			modelMatrix = glm::mat4(1.0f);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(-.3f, .0f, 1.0f));
			textured_program.SetModelMatrix(modelMatrix);
			DrawText(textured_program, fontTexture, "game over", .08f, .0f);

		}

		if (gameState == 4) {

			glUseProgram(textured_program.programID);
			modelMatrix = glm::mat4(1.0f);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(-.8f, .3f, 1.0f));
			textured_program.SetModelMatrix(modelMatrix);
			DrawText(textured_program, fontTexture, "GAME OVER", .18f, .001f);

			modelMatrix = glm::mat4(1.0f);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.35f, .0f, 1.0f));
			textured_program.SetModelMatrix(modelMatrix);
			DrawText(textured_program, fontTexture, "you lose", .08f, .0f);

		}

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glDisableVertexAttribArray(textured_program.positionAttribute);
		glDisableVertexAttribArray(textured_program.texCoordAttribute);


		SDL_GL_SwapWindow(displayWindow);
    }
    

	Mix_FreeChunk(beepSound);
	Mix_FreeChunk(loseSound);
	Mix_FreeMusic(introMusic);

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

void DrawText(ShaderProgram &program, int fontTexture, std::string text, float size, float spacing) {
	float character_size = 1.0 / 16.0f;
	std::vector<float> vertexData;
	std::vector<float> texCoordData;
	for (int i = 0; i < text.size(); i++) {
		int spriteIndex = (int)text[i];
		float texture_x = (float)(spriteIndex % 16) / 16.0f;
		float texture_y = (float)(spriteIndex / 16) / 16.0f;
		vertexData.insert(vertexData.end(), {
		((size + spacing) * i) + (-0.5f * size), 0.5f * size,
		((size + spacing) * i) + (-0.5f * size), -0.5f * size,
		((size + spacing) * i) + (0.5f * size), 0.5f * size,
		((size + spacing) * i) + (0.5f * size), -0.5f * size,
		((size + spacing) * i) + (0.5f * size), 0.5f * size,
		((size + spacing) * i) + (-0.5f * size), -0.5f * size,
			});
		texCoordData.insert(texCoordData.end(), {
		texture_x, texture_y,
		texture_x, texture_y + character_size,
		texture_x + character_size, texture_y,
		texture_x + character_size, texture_y + character_size,
		texture_x + character_size, texture_y,
		texture_x, texture_y + character_size,
			});
	}
	glBindTexture(GL_TEXTURE_2D, fontTexture);


	glUseProgram(program.programID);

	glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
	glEnableVertexAttribArray(program.texCoordAttribute);
	glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
	glEnableVertexAttribArray(program.positionAttribute);
	glDrawArrays(GL_TRIANGLES, 0, text.size() * 6);
	glDisableVertexAttribArray(program.positionAttribute);

}