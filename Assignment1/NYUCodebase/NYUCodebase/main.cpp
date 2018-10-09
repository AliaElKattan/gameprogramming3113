#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

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
		


	GLuint image = LoadTexture(RESOURCE_FOLDER"elementStone021.png");
	GLuint background = LoadTexture(RESOURCE_FOLDER"backgrounds.png");
	GLuint bee = LoadTexture(RESOURCE_FOLDER"bee_fly.png");

	glm::mat4 projectionMatrix = glm::mat4(1.0f);
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	glm::mat4 viewMatrix = glm::mat4(1.0f);

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
		
		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;

		textured_program.SetModelMatrix(modelMatrix);
		textured_program.SetProjectionMatrix(projectionMatrix);
		textured_program.SetViewMatrix(viewMatrix);

		untext_program.SetModelMatrix(modelMatrix);
		untext_program.SetProjectionMatrix(projectionMatrix);
		untext_program.SetViewMatrix(viewMatrix);

		glClearColor(.91f, .91f, 1.f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);



		//textured polygon
		glUseProgram(textured_program.programID);

		glBindTexture(GL_TEXTURE_2D, background);

		float background[] = { -2.f,-1.f,2.f,-1.f,2.f,.98f,-2.f,-1.f,2.f,.98f,-2.f,.98f };
		glVertexAttribPointer(textured_program.positionAttribute, 2, GL_FLOAT, false, 0, background);
		glEnableVertexAttribArray(textured_program.positionAttribute);

		float texCoords2[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
		glVertexAttribPointer(textured_program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords2);
		glEnableVertexAttribArray(textured_program.texCoordAttribute);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		//untextured polygons
		glUseProgram(untext_program.programID);

		untext_program.SetColor(.3f, .2f, .2f, 1.0f);

		float building[] = { .5f,-.5f,.5f,.7f,-.5f,-.5f,-.5f,-.5f,.5f,.7f,-.5f,.7f };
		glVertexAttribPointer(untext_program.positionAttribute, 2, GL_FLOAT, false, 0, building);
		glEnableVertexAttribArray(untext_program.positionAttribute);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(untext_program.positionAttribute);


	//textured polygon

		glUseProgram(textured_program.programID);
		

		//glDisableVertexAttribArray(textured_program.positionAttribute);
		//glDisableVertexAttribArray(textured_program.texCoordAttribute);

		//---


		glBindTexture(GL_TEXTURE_2D, image);
		
		float door[] = { -.13f,-.5f,.13f,-.5f,.13f,.0f,-.13f,-.5f,.13f,.0f,-.13f,0.f};
		//float vertices1[] = { .4f, -.75f, .4f, -.5f, -.4f, -.5f, .4f, -.75f, -.4f, -.5f, -.4f, -.75f };
		//float vertices2[] = {1.2f, -.5f, .4f, -.5f, .4f, -.75f, 1.2f, -.75f, 1.2f, -.5f, .4f, -.75f };
			
		glVertexAttribPointer(textured_program.positionAttribute, 2, GL_FLOAT, false, 0, door);
		glEnableVertexAttribArray(textured_program.positionAttribute);
		
		float texCoords[] = {0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
		glVertexAttribPointer(textured_program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
		glEnableVertexAttribArray(textured_program.texCoordAttribute);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		
	/*
		
		float door[] = { .13f,-.5f,.13f,-.5f,.13f,.0f,-.13f,-.5f,.13f,.0f,-.13f,0.f };
		
		//float vertices1[] = { .4f, -.75f, .4f, -.5f, -.4f, -.5f, .4f, -.75f, -.4f, -.5f, -.4f, -.75f };
		//float vertices2[] = {1.2f, -.5f, .4f, -.5f, .4f, -.75f, 1.2f, -.75f, 1.2f, -.5f, .4f, -.75f };
			
		glVertexAttribPointer(textured_program.positionAttribute, 2, GL_FLOAT, false, 0, door);
		glEnableVertexAttribArray(textured_program.positionAttribute);
		
		float texCoords[] = {0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
		glVertexAttribPointer(textured_program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
		glEnableVertexAttribArray(textured_program.texCoordAttribute);
		glDrawArrays(GL_TRIANGLES, 0, 6);*/
		
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		charx += elapsed;
		glBindTexture(GL_TEXTURE_2D, bee);

		float posx_left = charx - .13f;
		float posx_right = charx + .13f;
		float beechar[] = { posx_right,-.2f,posx_left,-.2f,posx_left,.0f,posx_right,-.2f,posx_left,.0f,posx_right,0.f };

		if (posx_right > 2.f || posx_left < -2.f) {
			charx = -1.8f;
		}
			
		//float vertices1[] = { .4f, -.75f, .4f, -.5f, -.4f, -.5f, .4f, -.75f, -.4f, -.5f, -.4f, -.75f };
		//float vertices2[] = {1.2f, -.5f, .4f, -.5f, .4f, -.75f, 1.2f, -.75f, 1.2f, -.5f, .4f, -.75f };

		glVertexAttribPointer(textured_program.positionAttribute, 2, GL_FLOAT, false, 0, beechar);
		glEnableVertexAttribArray(textured_program.positionAttribute);

		float texCoords3[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
		glVertexAttribPointer(textured_program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords3);
		glEnableVertexAttribArray(textured_program.texCoordAttribute);
		glDrawArrays(GL_TRIANGLES, 0, 6);





		glDisableVertexAttribArray(textured_program.positionAttribute);
		glDisableVertexAttribArray(textured_program.texCoordAttribute);
		

		

		SDL_GL_SwapWindow(displayWindow);


    }
    
    SDL_Quit();
    return 0;
}
