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

	ShaderProgram textured_program;
	ShaderProgram untext_program;
	textured_program.Load(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl"); //for textured
	untext_program.Load(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl"); //for untextured
		
	int numberOfBlocks = 0;

	GLuint image = LoadTexture(RESOURCE_FOLDER"elementStone021.png");
	GLuint background = LoadTexture(RESOURCE_FOLDER"backgrounds.png");
	GLuint bee = LoadTexture(RESOURCE_FOLDER"bee_fly.png");

	GLuint spritesheet = LoadTexture(RESOURCE_FOLDER"arne_sprites.png");

	glm::mat4 projectionMatrix = glm::mat4(1.0f);
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	glm::mat4 viewMatrix = glm::mat4(1.0f);
	//viewMatrix = glm::translate(viewMatrix, glm::vec3(-1.77f, 1.f, 0.0f)); //centering tiles
	/*textured_program.SetViewMatrix(viewMatrix);*/

	projectionMatrix = glm::ortho(-1.777f, 1.777f, -1.0f, 1.0f, -1.0f, 1.0f);

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


		textured_program.SetModelMatrix(modelMatrix);
		textured_program.SetProjectionMatrix(projectionMatrix);
		textured_program.SetViewMatrix(viewMatrix);


		untext_program.SetModelMatrix(modelMatrix);
		untext_program.SetProjectionMatrix(projectionMatrix);
		untext_program.SetViewMatrix(viewMatrix);

		glClearColor(1.f, 1.f, 1.f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//unsigned int levelData[LEVEL_HEIGHT][LEVEL_WIDTH] =
		//{
		//{11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11},
		//{0,20,4,4,4,4,4,4,0,0,0,0,0,0,4,4,4,4,4,4,20,0},
		//{0,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,20,0},
		//{0,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,20,0},
		//{0,20,0,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,20,0},
		//{0,20,0,0,0,0,0,6,6,6,6,6,6,6,6,0,0,0,0,0,20,0},
		//{0,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,20,0},
		//{0,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,20,0},
		//{0,20,6,6,6,6,6,0,0,0,0,0,0,0,0,6,6,6,6,6,20,0},
		//{0,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,20,0},
		//{0,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,20,0},
		//{0,20,0,0,0,0,0,6,6,6,6,6,6,6,6,0,0,0,0,0,20,0},
		//{0,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,20,0},
		//{0,20,125,118,0,0,116,0,0,0,0,0,0,0,0,0,0,117,0,127,20,0},
		//{2,2,2,2,2,2,2,2,2,3,0,0,1,2,2,2,2,2,2,2,2,2},
		//{32,33,33,34,32,33,33,34,33,35,100,101,35,32,33,32,34,32,33,32,33,33}
		//};


		

		const Uint8 *keys = SDL_GetKeyboardState(NULL);

		modelMatrix = glm::mat4(1.0f);

		//move screen
		if (keys[SDL_SCANCODE_LEFT]) {
			viewMatrix = glm::translate(viewMatrix, glm::vec3(-.1f, .0f, 0.0f));
			textured_program.SetViewMatrix(viewMatrix);
			// go left
		}
		else if (keys[SDL_SCANCODE_RIGHT]) {
			// go right
			viewMatrix = glm::translate(viewMatrix, glm::vec3(.1f, .0f, 0.0f));
			textured_program.SetViewMatrix(viewMatrix);
		}


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



		//std::vector<float> vertexData;
		//std::vector<float> texCoordData;
		//for (int y = 0; y < LEVEL_HEIGHT; y++) {
		//	for (int x = 0; x < LEVEL_WIDTH; x++) {
		//		float u = (float)(((int)levelData[y][x]) % SPRITE_COUNT_X) / (float)SPRITE_COUNT_X;
		//		float v = (float)(((int)levelData[y][x]) / SPRITE_COUNT_X) / (float)SPRITE_COUNT_Y;
		//		float spriteWidth = 1.0f / (float)SPRITE_COUNT_X;
		//		float spriteHeight = 1.0f / (float)SPRITE_COUNT_Y;
		//		vertexData.insert(vertexData.end(), {
		//		TILE_SIZE * x, -TILE_SIZE * y,
		//		TILE_SIZE * x, (-TILE_SIZE * y) - TILE_SIZE,
		//		(TILE_SIZE * x) + TILE_SIZE, (-TILE_SIZE * y) - TILE_SIZE,
		//		TILE_SIZE * x, -TILE_SIZE * y,
		//		(TILE_SIZE * x) + TILE_SIZE, (-TILE_SIZE * y) - TILE_SIZE,
		//		(TILE_SIZE * x) + TILE_SIZE, -TILE_SIZE * y
		//			});
		//		texCoordData.insert(texCoordData.end(), {
		//		u, v,
		//		u, v + (spriteHeight),
		//		u + spriteWidth, v + (spriteHeight),
		//		u, v,
		//		u + spriteWidth, v + (spriteHeight),
		//		u + spriteWidth, v
		//			});
		//	}

		//	glBindTexture(GL_TEXTURE_2D, spritesheet); //map


		//	glUseProgram(textured_program.programID); //shader program

		//	glVertexAttribPointer(textured_program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
		//	glEnableVertexAttribArray(textured_program.texCoordAttribute);
		//	glVertexAttribPointer(textured_program.positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
		//	glEnableVertexAttribArray(textured_program.positionAttribute);
		//	glDrawArrays(GL_TRIANGLES, 0, (vertexData.size()/2)); //map size
		//	glDisableVertexAttribArray(textured_program.positionAttribute);
		//}


		//textured polygon
		glUseProgram(textured_program.programID);

		//glBindTexture(GL_TEXTURE_2D, background);

		//float texCoords2[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
		//glVertexAttribPointer(textured_program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords2);
		//glEnableVertexAttribArray(textured_program.texCoordAttribute);
		/*glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(textured_program.positionAttribute);
		glDisableVertexAttribArray(textured_program.texCoordAttribute);
		
*/
		

		SDL_GL_SwapWindow(displayWindow);


    }
    
    SDL_Quit();
    return 0;
}


//void DrawText(ShaderProgram &program, int fontTexture, std::string text, float size, float spacing) {
//	float character_size = 1.0 / 16.0f;
//	std::vector<float> vertexData;
//	std::vector<float> texCoordData;
//	for (int i = 0; i < text.size(); i++) {
//		int spriteIndex = (int)text[i];
//		float texture_x = (float)(spriteIndex % 16) / 16.0f;
//		float texture_y = (float)(spriteIndex / 16) / 16.0f;
//		vertexData.insert(vertexData.end(), {
//		((size + spacing) * i) + (-0.5f * size), 0.5f * size,
//		((size + spacing) * i) + (-0.5f * size), -0.5f * size,
//		((size + spacing) * i) + (0.5f * size), 0.5f * size,
//		((size + spacing) * i) + (0.5f * size), -0.5f * size,
//		((size + spacing) * i) + (0.5f * size), 0.5f * size,
//		((size + spacing) * i) + (-0.5f * size), -0.5f * size,
//			});
//		texCoordData.insert(texCoordData.end(), {
//		texture_x, texture_y,
//		texture_x, texture_y + character_size,
//		texture_x + character_size, texture_y,
//		texture_x + character_size, texture_y + character_size,
//		texture_x + character_size, texture_y,
//		texture_x, texture_y + character_size,
//			});
//	}
//	glBindTexture(GL_TEXTURE_2D, fontTexture);
//
//
//	glUseProgram(program.programID);
//
//	glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
//	glEnableVertexAttribArray(program.texCoordAttribute);
//	glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
//	glEnableVertexAttribArray(program.positionAttribute);
//	glDrawArrays(GL_TRIANGLES, 0, text.size() * 6); //map size
//	glDisableVertexAttribArray(program.positionAttribute);
//
//}