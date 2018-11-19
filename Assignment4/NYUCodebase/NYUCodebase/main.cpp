#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <cstdlib>
#include <vector>


#define LEVEL_HEIGHT 32
#define LEVEL_WIDTH 40
#define SPRITE_COUNT_X 16
#define SPRITE_COUNT_Y 8
#define TILE_SIZE .2f

//using namespace std;

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

#include "ShaderProgram.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "flare_map/FlareMap.h"
#include "flare_map/FlareMap.cpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"



SDL_Window* displayWindow;

void DrawSprite(ShaderProgram &program, int index, int spriteCountX, int spriteCountY, float playerY);
void worldToTileCoordinates(float worldX, float worldY, int *gridX, int *gridY);

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


int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Alia's Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
	glewInit();
#endif
	bool keyPressed = false;
	ShaderProgram textured_program;
	ShaderProgram chars_program;
	ShaderProgram untext_program;
	textured_program.Load(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl"); //for textured
	chars_program.Load(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl"); //for textured
	untext_program.Load(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl"); //for untextured
		
	int numberOfBlocks = 0;

	GLuint image = LoadTexture(RESOURCE_FOLDER"elementStone021.png");
	GLuint background = LoadTexture(RESOURCE_FOLDER"backgrounds.png");
	GLuint bee = LoadTexture(RESOURCE_FOLDER"bee_fly.png");

	GLuint spritesheet = LoadTexture(RESOURCE_FOLDER"arne_sprites.png");
	GLuint characters = LoadTexture(RESOURCE_FOLDER"characters_1.png");

	glm::mat4 projectionMatrix = glm::mat4(1.0f);
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	glm::mat4 modelMatrix2 = glm::mat4(1.0f);
	glm::mat4 viewMatrix = glm::mat4(1.0f);
	glm::mat4 viewMatrix2 = glm::mat4(1.0f);
	viewMatrix = glm::translate(viewMatrix, glm::vec3(-1.77f, 1.f, 0.0f)); //centering tiles
	/*textured_program.SetViewMatrix(viewMatrix);*/

	projectionMatrix = glm::ortho(-1.777f, 1.777f, -1.0f, 1.0f, -1.0f, 1.0f);

	float gravity = -.01;
	float velocity = 0;
	float jumpVelocity = .07f;
	float playerY = 0;
	

	float lastFrameTicks = 0.0f;
	float charx = -2.f;
	glEnable(GL_BLEND);

    SDL_Event event;
    bool done = false;
    while (!done) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
        }

		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;


		textured_program.SetModelMatrix(modelMatrix);
		textured_program.SetProjectionMatrix(projectionMatrix);
		textured_program.SetViewMatrix(viewMatrix);

		chars_program.SetModelMatrix(modelMatrix2);
		chars_program.SetProjectionMatrix(projectionMatrix);
		chars_program.SetViewMatrix(viewMatrix2);

		/*untext_program.SetModelMatrix(modelMatrix);
		untext_program.SetProjectionMatrix(projectionMatrix);
		untext_program.SetViewMatrix(viewMatrix);*/

		glClearColor(1.f, 1.f, 1.f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);


		
		//check for collisions

		if (playerY > -.3) {

			playerY += velocity;
			velocity += gravity;
		}

		if (playerY <= -.3) {
			playerY = -.3;
		}

		
		const Uint8 *keys = SDL_GetKeyboardState(NULL);

		modelMatrix = glm::mat4(1.0f);

		glBindTexture(GL_TEXTURE_2D, characters);
		//move screen
		if (keys[SDL_SCANCODE_LEFT]) {
			
			keyPressed = true;
			DrawSprite(chars_program, 15, 12, 8, playerY);
			viewMatrix = glm::translate(viewMatrix, glm::vec3(elapsed, .0f, 0.0f));
			textured_program.SetViewMatrix(viewMatrix);
			// go left
		}
		else if (keys[SDL_SCANCODE_RIGHT]) {
			// go right
			DrawSprite(chars_program, 17, 12, 8, playerY);
			keyPressed = true;
			viewMatrix = glm::translate(viewMatrix, glm::vec3(-elapsed, .0f, 0.0f));
			textured_program.SetViewMatrix(viewMatrix);
		}
		else if (keys[SDL_SCANCODE_UP]) {
			keyPressed = true;

			while (jumpVelocity > 0) {
				playerY += jumpVelocity;
				jumpVelocity += gravity;
				modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(0.f, playerY, 0.f));
			}
			DrawSprite(chars_program, 4, 12, 8, playerY);

			jumpVelocity = .07f;
		}

		else { keyPressed = false; }


		
		FlareMap map;
		map.Load("map.txt");
	

		glm::mat4 projectionMatrix = glm::mat4(1.0f);
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		glm::mat4 viewMatrix = glm::mat4(1.0f);

		
		numberOfBlocks = 0;

		std::vector<float> vertexData;
		std::vector<float> texCoordData;
		for (int y = 0; y < map.mapHeight; y++) {
			for (int x = 0; x < map.mapWidth; x++) {

				if (map.mapData[y][x] != 0) {
					numberOfBlocks++;
					float u = (float)(((int)map.mapData[y][x]) % SPRITE_COUNT_X) / (float)SPRITE_COUNT_X;
					float v = (float)(((int)map.mapData[y][x]) / SPRITE_COUNT_X) / (float)SPRITE_COUNT_Y;
					float spriteWidth = 1.0f / (float)SPRITE_COUNT_X;
					float spriteHeight = 1.0f / (float)SPRITE_COUNT_Y;
					vertexData.insert(vertexData.end(), {
					TILE_SIZE * x, -TILE_SIZE * y,
					TILE_SIZE * x, (-TILE_SIZE * y) - TILE_SIZE,
					(TILE_SIZE * x) + TILE_SIZE, (-TILE_SIZE * y) - TILE_SIZE,
					TILE_SIZE * x, -TILE_SIZE * y,
					(TILE_SIZE * x) + TILE_SIZE, (-TILE_SIZE * y) - TILE_SIZE,
					(TILE_SIZE * x) + TILE_SIZE, -TILE_SIZE * y
						});
					texCoordData.insert(texCoordData.end(), {
					u, v,
					u, v + (spriteHeight),
					u + spriteWidth, v + (spriteHeight),
					u, v,
					u + spriteWidth, v + (spriteHeight),
					u + spriteWidth, v
						});
				}
			}


	
			glBindTexture(GL_TEXTURE_2D, spritesheet); //map

			glUseProgram(textured_program.programID); //shader program

			glVertexAttribPointer(textured_program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
			glEnableVertexAttribArray(textured_program.texCoordAttribute);
			glVertexAttribPointer(textured_program.positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
			glEnableVertexAttribArray(textured_program.positionAttribute);
			glDrawArrays(GL_TRIANGLES, 0, (numberOfBlocks * 6)); //map size
			glDisableVertexAttribArray(textured_program.positionAttribute);
		}




		glBindTexture(GL_TEXTURE_2D, characters);


		if (keyPressed == false) {

			DrawSprite(chars_program, 4, 12, 8, playerY);
			/*worldToTileCoordinates(float worldX, float worldY, int *gridX, int *gridY);*/
		}

		//textured polygon
		glUseProgram(textured_program.programID);
		

		glBindTexture(GL_TEXTURE_2D, characters);
		modelMatrix = glm::mat4(1.0f);

		DrawSprite(textured_program, 70, 12, 8, 0);
	
		//textured_program.SetViewMatrix(viewMatrix);

		SDL_GL_SwapWindow(displayWindow);


    }
    
    SDL_Quit();
    return 0;
}

void worldToTileCoordinates(float worldX, float worldY, int *gridX, int *gridY) {
	*gridX = (int)(worldX / TILE_SIZE);
	*gridY = (int)(worldY / -TILE_SIZE);
}

void DrawSprite(ShaderProgram &program, int index, int spriteCountX, int spriteCountY, float playerY) {

		glm::mat4 modelMatrix = glm::mat4(1.0f);

		if (playerY == 0) {
			modelMatrix = glm::translate(modelMatrix, glm::vec3(2.3f, -1.2f, 1.0f));
		}

		else {
			modelMatrix = glm::translate(modelMatrix, glm::vec3(0.f, playerY, 1.0f));
		}
		program.SetModelMatrix(modelMatrix);
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

