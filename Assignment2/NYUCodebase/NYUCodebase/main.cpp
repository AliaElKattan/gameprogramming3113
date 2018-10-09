/*INSTRUCTIONS

PLAYER ONE: MOVE WITH UP AND DOWN ARROW KEYS.
PLAYER TWO: MOVE WITH W AND S KEYS.

POSITION RESTARTS WHEN BALL GOES OUT OF BOUNDS.

EACH PLAYER GAINS A POINT WHEN THEY HIT THE BALL, AND WHEN THE OPPONENT LOSES IT.


*/

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

void drawBall(ShaderProgram shader, float ball_x, float ball_y);


int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Alia's Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
	glewInit();
#endif

	//ShaderProgram textured_program;
	ShaderProgram untext_program;
	//textured_program.Load(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl"); //for textured
	untext_program.Load(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl"); //for untextured
		
	glm::mat4 projectionMatrix = glm::mat4(1.0f);
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	glm::mat4 viewMatrix = glm::mat4(1.0f);

	projectionMatrix = glm::ortho(-1.777f, 1.777f, -1.0f, 1.0f, -1.0f, 1.0f);

	float lastFrameTicks = 0.0f;
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	float elapsed = ticks - lastFrameTicks;
	lastFrameTicks = ticks;

	float yx1 = 0.f;
	float yx2 = 0.f;

	float ball_x = 0;
	float ball_y = 0;
	float ballSpeedy = elapsed * .002;
	float ballSpeedx = elapsed * .002;
	int player1Score = 0;//SCOREKEEPING
	int player2Score = 0;

	glEnable(GL_BLEND);

    SDL_Event event;
    bool done = false;
	while (!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
		}

	


		untext_program.SetModelMatrix(modelMatrix);
		untext_program.SetProjectionMatrix(projectionMatrix);
		untext_program.SetViewMatrix(viewMatrix);

		glClearColor(.91f, .91f, 1.f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		untext_program.SetColor(0.0f, 0.0f, 0.0f, 1.f);

		//untextured polygons

		//middle barrier line
		glUseProgram(untext_program.programID);

		float barrier[] = { .03f,-1.2f,.03f,1.2f,-.03f,-1.2f,-.03f,-1.2f,.03f,1.2f,-.03f,1.2f };
		glVertexAttribPointer(untext_program.positionAttribute, 2, GL_FLOAT, false, 0, barrier);
		glEnableVertexAttribArray(untext_program.positionAttribute);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(untext_program.positionAttribute);

		//right paddle


		float posy_up1 = yx1 + .12f; //top pos of paddle 1's y
		float posy_down1 = yx1 - .12f; //bottom pos of paddle 1's y

		float right_paddle[] = { 1.77f, posy_down1, 1.77f, posy_up1, 1.71f, posy_down1, 1.71f, posy_down1, 1.77f, posy_up1, 1.71f,posy_up1 };
		glVertexAttribPointer(untext_program.positionAttribute, 2, GL_FLOAT, false, 0, right_paddle);
		glEnableVertexAttribArray(untext_program.positionAttribute);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(untext_program.positionAttribute);


		//right paddle movement

		const Uint8 *keys = SDL_GetKeyboardState(NULL);
		if (keys[SDL_SCANCODE_UP]) {
			// go up
			if (posy_up1 < 1.f) { //only allow movement when inside the screen
				yx1 += .001f;
			}
		}
		else if (keys[SDL_SCANCODE_DOWN]) {
			// go right!

			if (posy_down1 > -1.f) {
				yx1 -= .001f;
			}
		}

		//left paddle


		float posy_up2 = yx2 + .12f;
		float posy_down2 = yx2 - .12f;


		float left_paddle[] = { -1.77f, posy_down2, -1.77f, posy_up2, -1.71f, posy_down2, -1.71f, posy_down2, -1.77f, posy_up2, -1.71f, posy_up2 };
		glVertexAttribPointer(untext_program.positionAttribute, 2, GL_FLOAT, false, 0, left_paddle);
		glEnableVertexAttribArray(untext_program.positionAttribute);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(untext_program.positionAttribute);

		//left paddle movement


		if (keys[SDL_SCANCODE_W]) {
			// go up
			if (posy_up2 < 1.f) { //only allow movement when inside the screen
				yx2 += .001f;
			}
		}
		else if (keys[SDL_SCANCODE_S]) {
			// go right!

			if (posy_down2 > -1.f) { //only allow movement when inside the screen
				yx2 -= .001f;
			}
		}

		//ball

		/*modelMatrix = glm::scale(modelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
		untext_program.SetModelMatrix(modelMatrix);
*/
//untext_program.SetColor(0.0f, 0.0f, 0.0f, 1.f);




		ball_x += ballSpeedx;
		ball_y += ballSpeedy;

		float ball_xr = ball_x + .06f;
		float ball_xl = ball_x - .06f;
		float ball_yt = ball_y + .06f; //top edge y pos
		float ball_yb = ball_y - .06f; //bottom edge y pos


		if (ball_y > 1.f || ball_y < -1.f) { //check when ball gets off the screen (top or bottom)

			ballSpeedy = ballSpeedy * -1;
		}


			if (ball_xr > 1.75f && ball_yb < posy_up1 && ball_yt>posy_down1) { //condition when hits RIGHT paddle
					ball_x = 1.65f; //move it out to make sure it doesnt get stuck inside the paddle
					player1Score++; //INCREASE PLAYER 1'S SCORE
					ballSpeedx = ballSpeedx * -1;
					ball_x += ballSpeedx;
				
				}
			
				
			if (ball_xl < -1.75f && ball_yb < posy_up2 && ball_yt>posy_down2) { //condition when hitting LEFT paddle
					ball_x = -1.65f;
					player2Score++; //INCREASE PLAYER 2'S SCORE
					ballSpeedx = ballSpeedx * -1;
					ball_x += ballSpeedx;
				
			}

			if (ball_xr > 1.9f) {

				ball_x = 0;
				ball_y = 0;
				player2Score++; //INCREASE PLAYER 1'S SCORE
			
			} 
			
			if (ball_xl < -1.9f) { //if out of bounds 
								
				ball_x = 0;
				ball_y = 0;
				player1Score++; //INCREASE PLAYER 2'S SCORE

			}
		



		drawBall(untext_program, ball_x, ball_y);

		

	/*	modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f, 1.0f, 1.0f));
		untext_program.SetModelMatrix(modelMatrix);*/
	
		/*modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f, 1.0f, 0.0f));
*/

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//if (posx_right > 2.f || posx_left < -2.f) {
		//	charx = -1.8f;
		//}

		

		SDL_GL_SwapWindow(displayWindow);


    }
    
    SDL_Quit();
    return 0;
}

void drawBall(ShaderProgram shader, float ball_x, float ball_y) {


	float ball_xr = ball_x + .06f;
	float ball_xl = ball_x - .06f;
	float ball_yt = ball_y + .06f; //top edge y pos
	float ball_yb = ball_y - .06f; //bottom edge y pos




	float ball[] = {ball_xr,ball_yb,ball_xr,ball_yt,ball_xl,ball_yb,ball_xl,ball_yb,ball_xr,ball_yt,ball_xl,ball_yt };
	glVertexAttribPointer(shader.positionAttribute, 2, GL_FLOAT, false, 0, ball);
	glEnableVertexAttribArray(shader.positionAttribute);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(shader.positionAttribute);


}