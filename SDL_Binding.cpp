#define REVENGINE_GL

#ifndef UNIX
#include "SDL.h"
#include "SDL_mixer.h"
#include "SDL_image.h"
#ifdef REVENGINE_GL
#include <glew.h>
#include <SDL_opengl.h>
#include <gl/glu.h>
#endif
#else
#include "SDL/SDL.H"
#include "SDL/SDL_mixer.h"
#include "SDL/SDL_image.h"
#endif

#include "Revengine.h"
#include "Event.h"
#include "Player.h"
#include "Sprite.h"
#include "Graphics.h"
#include "Blocking.h"

SDL_Window *window;
#ifndef REVENGINE_GL
SDL_Renderer *renderer;
#else
SDL_GLContext glContext;
#endif
Hashmap<int, bool> keys;
SDL_DisplayMode displayMode;
Hashmap<string, Mix_Chunk*> sounds;
Mix_Music *music;
string song;
Sprite ***chars;
List<SDL_Joystick*> joysticks;
struct PlayerMap
{
	bool a = false, b = false, up = false, down = false, left = false, right = false,
		up2 = false, down2 = false, left2 = false, right2 = false;
	bool playing = false;
};

PlayerMap playerMap[MAX_PLAYERS];

bool fullscreen = false;
const int JOYSTICK_DEAD_ZONE = 12000;


class File
{
public:
	SDL_RWops *file;
};

class Texture
{
public:
#ifndef REVENGINE_GL
	SDL_Texture *texture;
#else
	GLuint texture;
	int width, height;
#endif
};

void setWindowFullscreen(bool b)
{
	SDL_SetWindowFullscreen(window, b ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
}

void setWindowSize(int width, int height)
{
	SDL_SetWindowSize(window, width, height);
}

void centerWindow()
{
	SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
}

int getScreenWidth()
{
	return displayMode.w;
}

int getScreenHeight()
{
	return displayMode.h;
}

void getWindowSize(int *w, int *h)
{
	SDL_GetWindowSize(window, w, h);
}

#ifndef REVENGINE_GL
void setDrawColor(int r, int g, int b, int a)
{
	SDL_SetRenderDrawColor(renderer, r, g, b, a);
}

void clearScreen()
{
	SDL_RenderClear(renderer);
}

void display()
{
	SDL_RenderPresent(renderer);
}

void startDrawingPlayer(int index)
{
	if (index == -1)
	{
		SDL_SetRenderTarget(renderer, nullptr);
	}
	else {
		SDL_SetRenderTarget(renderer, players[index]->texture->texture);
	}
}

void drawTexture(Texture *texture, Rect *rect)
{
	SDL_Rect r;
	r.w = rect->w;
	r.h = rect->h;
	r.x = rect->x;
	r.y = rect->y;
	SDL_RenderCopy(renderer, texture->texture, nullptr, &r);
}

void drawTexture(Texture *texture, Rect *source, Rect *dest)
{
	SDL_Rect s, d;
	s.w = source->w;
	s.h = source->h;
	s.x = source->x;
	s.y = source->y;
	d.w = dest->w;
	d.h = dest->h;
	d.x = dest->x;
	d.y = dest->y;
	SDL_RenderCopy(renderer, texture->texture, &s, &d);
}

void drawTexture(Texture *texture, Rect *source, Rect *dest, int alpha)
{
	SDL_Rect s, d;
	s.w = source->w;
	s.h = source->h;
	s.x = source->x;
	s.y = source->y;
	d.w = dest->w;
	d.h = dest->h;
	d.x = dest->x;
	d.y = dest->y;
	SDL_SetTextureAlphaMod(texture->texture, (unsigned char)alpha);
	SDL_RenderCopy(renderer, texture->texture, &s, &d);
	SDL_SetTextureAlphaMod(texture->texture, 255);
}

Texture *createTexture(int width, int height)
{
	Texture * t = new Texture();
	t->texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, width, height);
	return t;
}

Texture *loadTexture(string filename)
{
	Texture * t = new Texture();
	t->texture = IMG_LoadTexture(renderer, ("image/" + filename + ".png").c_str());
	return t;
}

void getTextureSize(Texture *t, int *w, int *h)
{
	SDL_QueryTexture(t->texture, nullptr, nullptr, w, h);
}

void drawCharacter(char c, int x, int y, int r, int g, int b)
{
	chars[0][c]->draw(x, y, 255);
	if(r>0)chars[1][c]->draw(x, y, r);
	if(g>0)chars[2][c]->draw(x, y, g);
	if(b>0)chars[3][c]->draw(x, y, b);
}

void destroyTexture(Texture *texture)
{
	SDL_DestroyTexture(texture->texture);
	delete texture;
}
void fillRect(Rect *rect)
{
	SDL_Rect r;
	r.w = rect->w;
	r.h = rect->h;
	r.x = rect->x;
	r.y = rect->y;
	SDL_RenderFillRect(renderer, &r);
}

void fillRect(Rect *rect, int blendMode)
{
	SDL_Rect r;
	r.w = rect->w;
	r.h = rect->h;
	r.x = rect->x;
	r.y = rect->y;
	if (blendMode == BLENDMODE_ADD)
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);
	else if (blendMode == BLENDMODE_BLEND)
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	else if (blendMode == BLENDMODE_MOD)
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_MOD);
	else SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
	SDL_RenderFillRect(renderer, &r);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

#else

std::string vertexShader = std::string() +
"#version 330 core\n" +
"layout(location=0) in vec3 vertexPosition_modelspace;" +
"layout(location=1) in vec2 vertexUV;" +
"out vec2 UV;" +
"uniform mat4 MVP;" +
"void main(){" +
"gl_Position = MVP*vec4(vertexPosition_modelspace,1);" +
"UV = vertexUV;" +
"}";

std::string fragmentShader = std::string() +
"#version 330 core\n" +
"layout(location = 0) out vec4 color;" +
"in vec2 UV;" +
"uniform sampler2D sprite;" +
"void main(){" +
"color = texture(sprite,UV);" +
"}";

std::string fragmentShader2 = std::string() +
"#version 330 core\n" +
"layout(location = 0) out vec4 color;" +
"in vec2 UV;" +
"uniform sampler2D sprite;" +
"void main(){" +
"vec4 t = texture(sprite,UV);" + 
"color = vec4(1.0-t.x, t.y, 1.0-t.z, t.w);" +
"}";

std::string textShader = std::string() +
"#version 330 core\n" +
"layout(location=0) out vec4 color;" +
"in vec2 UV;" +
"uniform sampler2D sprite;" +
"uniform vec4 tcolor;" +
"void main(){" +
"vec4 t = texture(sprite,UV);" +
"if(t.w>.5){" + 
"color = tcolor;" +
"} else {" +
"color = t;" +
"}" +
"}";

std::string matteVertex = std::string() +
"#version 330 core\n" +
"layout(location=0) in vec3 vertexPosition_modelspace;" +
"uniform mat4 MVP;" +
"void main(){" +
"gl_Position = MVP*vec4(vertexPosition_modelspace,1);" +
"}";

std::string matteFragment = std::string() +
"#version 330 core\n" +
"layout(location=0) out vec4 color;" +
"uniform vec4 tcolor;"+
"void main(){" +
"color = tcolor;" +
"}";

void translateMatrix(GLfloat*matrix, GLfloat x, GLfloat y, GLfloat z);
void scaleMatrix(GLfloat*matrix, GLfloat x, GLfloat y, GLfloat z);

GLuint pixelProgram, pixelProgram2;
GLuint textProgram;
GLuint pixelTextureUniform;
GLuint pixelVertexBuffer, pixelUVBuffer;
GLfloat * matrix;
GLuint matrixID;

GLuint matteProgram;

int paddingX = 0;

static const GLfloat gVertexBufferData[] = {
	0.0f, 1.0F, 0.0F,
	0.0F, 0.0F, 0.0F,
	1.0F, 0.0F, 0.0F,
	0.0F,1.0F,0.0F,
	1.0F,0.0F,0.0F,
	1.0F,1.0F,0.0F
};

static const GLfloat matteVertexBufferData[] = {
	0.0f, 240.0F, 0.0F,
	0.0F, 0.0F, 0.0F,
	320.0F, 0.0F, 0.0F,
	0.0F,240.0F,0.0F,
	320.0F,0.0F,0.0F,
	320.0F,240.0F,0.0F
};

static GLfloat interVertexBufferData[] = {
	0.0f, 1.0F, 0.0F,
	0.0F, 0.0F, 0.0F,
	1.0F, 0.0F, 0.0F,
	0.0F,1.0F,0.0F,
	1.0F,0.0F,0.0F,
	1.0F,1.0F,0.0F
};

static const GLfloat gUVBufferData[] =
{
	0.0F, 1.0F,
	0.0F, 0.0F,
	1.0F, 0.0F,
	0.0F, 1.0F,
	1.0F,0.0F,
	1.0F,1.0F
};

static GLfloat interUVBufferData[] =
{
	0.0F, 0.0F,
	0.0F, 1.0F,
	1.0F, 1.0F,
	0.0F, 0.0F,
	1.0F,1.0F,
	1.0F,0.0F
};

void translateMatrix(GLfloat*matrix, GLfloat x, GLfloat y, GLfloat z)
{
	matrix[12] += x;
	matrix[13] += y;
	matrix[14] += z;
}

void scaleMatrix(GLfloat*matrix, GLfloat x, GLfloat y, GLfloat z)
{
	matrix[0] *= x;
	matrix[5] *= y;
	matrix[10] *= z;
}

GLuint lastShader;

void changeShader(GLuint shader)
{
	if (shader != lastShader)
	{
		glUseProgram(shader);
		pixelTextureUniform = glGetUniformLocation(shader, "sprite");
		matrixID = glGetUniformLocation(shader, "MVP");
		lastShader = shader;
		glUniformMatrix4fv(matrixID, 1, GL_FALSE, matrix);
	}
}

GLuint loadShaders(std::string vertex, std::string fragment)
{
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	char const * vertexSourcePointer = vertex.c_str();
	char const * fragmentSourcePointer = fragment.c_str();

	glShaderSource(vertexShaderID, 1, &vertexSourcePointer, nullptr);
	glCompileShader(vertexShaderID);

	glShaderSource(fragmentShaderID, 1, &fragmentSourcePointer, nullptr);
	glCompileShader(fragmentShaderID);

	int infoLength;
	glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &infoLength);
	if (infoLength > 0) {
		char *FragmentShaderErrorMessage = new char[infoLength + 1];
		glGetShaderInfoLog(fragmentShaderID, infoLength, NULL,FragmentShaderErrorMessage);
		debug(FragmentShaderErrorMessage);
		delete[] FragmentShaderErrorMessage;
	}

	GLuint program = glCreateProgram();
	glAttachShader(program, vertexShaderID);
	glAttachShader(program, fragmentShaderID);
	glLinkProgram(program);

	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLength);
	if (infoLength > 0) {
		char *FragmentShaderErrorMessage = new char[infoLength + 1];
		glGetProgramInfoLog(program, infoLength, NULL, FragmentShaderErrorMessage);
		debug(FragmentShaderErrorMessage);
		delete[]FragmentShaderErrorMessage;
	}

	glDetachShader(program, vertexShaderID);
	glDetachShader(program, fragmentShaderID);

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	return program;
}

GLuint loadShaders(std::string fragment)
{
	return loadShaders(vertexShader, fragment);
}

float drawR, drawG, drawB, drawA;

void setDrawColor(int r, int g, int b, int a)
{
	drawR = r / 255.0;
	drawG = g / 255.0;
	drawB = b / 255.0;
	drawA = a / 255.0;
	glClearColor(r / 255.0, g / 255.0, b / 255.0, a / 255.0);
}

void clearScreen()
{
	glClear(GL_COLOR_BUFFER_BIT);
}

void display()
{
	SDL_GL_SwapWindow(window);
}

GLuint frameBuffer;

void resetMatrix()
{
	for (int x = 0; x < 4; x++)
	{
		for (int y = 0; y < 4; y++)
		{
			if (x == y) matrix[x + 4 * y] = 1.0F;
			else
				matrix[x + 4 * y] = 0.0F;
		}
	}
}

void startDrawingPlayer(int index)
{
	bool doWidescreen = false;
	if (numPlayers == 1)
	{
		int width, height;
		if (fullscreen)
		{
			width = displayMode.w;
			height = displayMode.h;
		}
		else {
			getWindowSize(&width, &height);
		}
		if (width >= 3* height/2)
		{
			doWidescreen = true;
		}
	}
	if (doWidescreen)
	{
		if (index == -1)
		{
			paddingX = 0;
			int width, height;
			if (fullscreen)
			{
				width = displayMode.w;
				height = displayMode.h;
			}
			else {
				getWindowSize(&width, &height);
			}
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(0,0,width, height);
			resetMatrix();
			scaleMatrix(matrix, 1.0, -1.0, 1.0);
			lastShader = pixelProgram - 1;
			changeShader(pixelProgram);

			Rect r;
			r.x = -1;
			r.y = -1;
			r.w = 2;
			r.h = 2;
			drawTexture(players[0]->texture, &r);
			setDrawColor(0, 0, 0, 0);
			glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, players[0]->texture->texture, 0);
			glClear(GL_COLOR_BUFFER_BIT);
			//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		else {
			float width,height;
			if (fullscreen)
			{
				width = displayMode.w;
				height = displayMode.h;
			}
			else {
				int w, h;
				getWindowSize(&w, &h);
				width = w;
				height = h;
			}
			float width2 = width/(height/(float)HEIGHT);
			int iWidth = (int)(width2 + .5);
			Texture *t = players[0]->texture;
			if (t->width != iWidth)
			{
				glDeleteTextures(1, &t->texture);
				delete t;
				players[0]->texture = createTexture(iWidth, HEIGHT);
				t = players[0]->texture;
			}
			glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, players[0]->texture->texture, 0);

			glViewport(0, 0, players[0]->texture->width, players[0]->texture->height);
			paddingX = (int)((width2 - (float)WIDTH) / 2.0 + .5);
			resetMatrix();
			translateMatrix(matrix, -1.0F, -1.0F, -1.0F);
			scaleMatrix(matrix, 2 /  width2, 2 / (float)HEIGHT, 1.0F);
			lastShader = pixelProgram - 1;
			changeShader(pixelProgram);
		}
	}
	else {
		paddingX = 0;
		if (index == -1)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			if (fullscreen)
			{
				glViewport(0, 0, displayMode.w, displayMode.h);
				resetMatrix();
				translateMatrix(matrix, -1.0F, 1.0F, -1.0F);
				scaleMatrix(matrix, 2.0F / displayMode.w, -2.0F / displayMode.h, 1.0F);
			}
			else {
				int w, h;
				getWindowSize(&w, &h);
				glViewport(0, 0, w, h);
				resetMatrix();
				translateMatrix(matrix, -1.0F, 1.0F, -1.0F);
				scaleMatrix(matrix, 2.0 / w, -2.0F / h, 1.0F);
			}
			lastShader = pixelProgram - 1;
			changeShader(pixelProgram);
		}
		else {
			Texture *t = players[index]->texture;
			if (t->width != WIDTH || t->height != HEIGHT)
			{
				glDeleteTextures(1, &t->texture);
				delete t;
				players[index]->texture = createTexture(WIDTH, HEIGHT);
				t = players[index]->texture;
			}
			glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, players[index]->texture->texture, 0);

			glViewport(0, 0, players[index]->texture->width, players[index]->texture->height);

			resetMatrix();
			translateMatrix(matrix, -1.0F, -1.0F, -1.0F);
			scaleMatrix(matrix, 2 / (float)WIDTH, 2 / (float)HEIGHT, 1.0F);
			lastShader = pixelProgram - 1;
			changeShader(pixelProgram);
		}
	}
}

Texture *postBuffer;
void postProcess(Player *p)
{
	if (postBuffer->width != p->texture->width || postBuffer->height != p->texture->height)
	{
		delete postBuffer;
		postBuffer = createTexture(p->texture->width, p->texture->height);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, postBuffer->texture, 0);
	glViewport(0, 0, postBuffer->width, postBuffer->height);
	Rect r;
	r.x = -paddingX;
	r.y = 0;
	r.w = postBuffer->width;
	r.h = postBuffer->height;
	drawTexture(p->texture, &r);
	Texture *temp = postBuffer;
	postBuffer = p->texture;
	p->texture = temp;
}

void drawTexture(Texture *texture, Rect *rect)
{
	Rect s;
	s.x = 0;
	s.y = 0;
	s.w = texture->width;
	s.h = texture->height;
	drawTexture(texture, &s, rect);
}

void drawTexture(Texture *texture, Rect *source, Rect *dest)
{
	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			float v = gVertexBufferData[i * 3 + j];

			if (j == 0)
			{
				v = (v * dest->w) + dest->x + paddingX;
			}
			else if (j == 1)
			{
				v = (v * dest->h) + dest->y;
			}

			interVertexBufferData[i * 3 + j] = v;
		}

		float w = gUVBufferData[i * 2];
		if (w == 0.0) w = (float)source->x / texture->width;
		else w = (float)(source->x + source->w) / texture->width;
		interUVBufferData[i * 2] = w;

		w = gUVBufferData[i * 2 + 1];
		if (w == 0.0) w = (float)source->y / texture->height;
		else w = (float)(source->y + source->h) / texture->height;
		interUVBufferData[i * 2 + 1] = w;
	}

	glBindBuffer(GL_ARRAY_BUFFER, pixelVertexBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(interVertexBufferData), interVertexBufferData);

	glBindBuffer(GL_ARRAY_BUFFER, pixelUVBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(interUVBufferData), interUVBufferData);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture->texture);
	glUniform1i(pixelTextureUniform, 0);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, pixelVertexBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, pixelUVBuffer);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	glDrawArrays(GL_TRIANGLES, 0, 6 * 3);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

int getPaddingX()
{
	return paddingX;
}

Texture *createTexture(int width, int height)
{
	Texture * t = new Texture();

	GLuint renderedTexture;
	glGenTextures(1, &renderedTexture);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, renderedTexture);

	// Give an empty image to OpenGL ( the last "0" )
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	// Poor filtering. Needed !
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	t->texture = renderedTexture;
	t->width = width;
	t->height = height;
	return t;
}

Texture *loadTexture(string filename)
{
	Texture * t = new Texture();
	GLuint TextureID = 0;

	SDL_Surface* Surface = IMG_Load(("image/" + filename + ".png").c_str());

	if (Surface == nullptr||Surface->h*Surface->w==0)
	{
		delete t;
		return nullptr;
	}

	glGenTextures(1, &TextureID);
	glBindTexture(GL_TEXTURE_2D, TextureID);

	int Mode = GL_RGB;

	if (Surface->format->BytesPerPixel == 4) {
		Mode = GL_RGBA;
	}

	if (Surface->format->BytesPerPixel == 1) {
		SDL_Surface *converted = SDL_ConvertSurfaceFormat(
			Surface, SDL_PIXELFORMAT_ARGB8888, 0);
		SDL_FreeSurface(Surface);
		Surface = converted;
		Mode = GL_RGBA;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, Mode, Surface->w, Surface->h, 0, Mode, GL_UNSIGNED_BYTE, Surface->pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	t->texture = TextureID;
	t->width = Surface->w;
	t->height = Surface->h;

	SDL_FreeSurface(Surface);
	return t;
}

void getTextureSize(Texture *t, int *w, int *h)
{
	*w = t->width;
	*h = t->height;
}

void drawCharacter(unsigned char c, int x, int y, int r, int g, int b)
{
	GLuint s = lastShader;
	changeShader(textProgram);
	GLuint tcolorUniform = glGetUniformLocation(textProgram, "tcolor");
	glUniform4f(tcolorUniform, r / 255.0, g / 255.0, b / 255.0, 1.0);
	chars[0][c]->draw(x, y);
	changeShader(s);
}

void destroyTexture(Texture *texture)
{
	glDeleteTextures(1, &texture->texture);
	delete texture;
}
void fillRect(Rect *rect)
{
	GLuint s = lastShader;
	changeShader(matteProgram);

	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			float v = gVertexBufferData[i * 3 + j];

			if (j == 0)
			{
				v = (v * rect->w) + rect->x + paddingX;
			}
			else if (j == 1)
			{
				v = (v * rect->h) + rect->y;
			}

			interVertexBufferData[i * 3 + j] = v;
		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, pixelVertexBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(interVertexBufferData), interVertexBufferData);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, pixelVertexBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	GLuint tcolorUniform = glGetUniformLocation(matteProgram, "tcolor");
	glUniform4f(tcolorUniform, drawR, drawG, drawB, drawA);

	glDrawArrays(GL_TRIANGLES, 0, 6 * 3);
	glDisableVertexAttribArray(0);

	changeShader(s);
}

#endif

int ticks = SDL_GetTicks();

//delay is now sort of deprecated because VSYNC apparently locks the frame rate for us
//in the eventuality that it doesn't do its job we skip, though
bool delay(int millis)
{
	int ticks_ = SDL_GetTicks();
	bool o = true;
	if(ticks_-ticks<millis)
		SDL_Delay(millis-(ticks_-ticks));
	//debug(ticks_ - ticks);
	if (ticks_ - ticks >= 2 * millis)
	{
		o = false;
	}
	ticks = ticks_;
	return o;
}

void shadersInit();

int startGame()
{
	if (SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK) < 0)
	{
		return -2;
	}
	if (IMG_Init(IMG_INIT_PNG) < 0)
	{
		exitGame();
		return -3;
	}
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048)<0)
	{
		exitGame();
		return -4;
	}
	SDL_GetDesktopDisplayMode(0, &displayMode);
#ifdef REVENGINE_GL
	window = SDL_CreateWindow("Revengine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, displayMode.w, displayMode.h, SDL_WINDOW_SHOWN |SDL_WINDOW_OPENGL);
#else
	window = SDL_CreateWindow("Revengine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, displayMode.w, displayMode.h, SDL_WINDOW_SHOWN);
#endif
	if (window == nullptr)
	{
		exitGame();
		return -1;
	}
	{
		setWindowFullscreen(false);
		int scaleX = getScreenWidth() / WIDTH;
		int scaleY = getScreenHeight() / HEIGHT;
		int scalar = scaleX;
		if (scaleY < scaleX) scalar = scaleY;
		setWindowSize(scalar * WIDTH, scalar * HEIGHT);
		centerWindow();
	}
#ifndef REVENGINE_GL
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == nullptr)
	{
		exitGame();
		return -1;
	}
#else
	glewExperimental = true;
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	glContext = SDL_GL_CreateContext(window);
	if (glContext == 0)
	{
		debug(SDL_GetError());
		exitGame();
		return -5;
	}
	if (glewInit() != GLEW_OK)
	{
		exitGame();
		return -6;
	}
	if (SDL_GL_SetSwapInterval(1) < 0)
	{
		debug("No VSync here!");
	}
	glEnable(GL_BLEND);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);

	GLuint vertexArrayID;
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	glGenBuffers(1, &pixelVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, pixelVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(gVertexBufferData), gVertexBufferData, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &pixelUVBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, pixelUVBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(gUVBufferData), gUVBufferData, GL_DYNAMIC_DRAW);

	pixelProgram = loadShaders(vertexShader, fragmentShader);
	pixelProgram2 = loadShaders(vertexShader, fragmentShader2);
	matteProgram = loadShaders(matteVertex, matteFragment);
	textProgram = loadShaders(vertexShader, textShader);

	shadersInit();
	postBuffer = createTexture(WIDTH, HEIGHT);

	glClearColor(0.0F, 0.0F, 0.0F, 1.0F);

	matrix = new GLfloat[16];
	resetMatrix();

	translateMatrix(matrix, -1.0F, 1.0F, -1.0F);
	scaleMatrix(matrix, 2 / 320.0F, -2 / 240.0F, 1.0F);

	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	const GLenum draw_buffer = GL_COLOR_ATTACHMENT0;
	glDrawBuffers(1, &draw_buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	lastShader = pixelProgram - 1;
	changeShader(pixelProgram);
#endif
	for (int i = 0; i < SDL_NumJoysticks(); i++)
	{
		joysticks.add(SDL_JoystickOpen(i));
	}
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
	for (int i = SDLK_a; i <= SDLK_z; i++)
		keys.add(i, false);
	keys.add(SDLK_F4, false);
	keys.add(SDLK_F11, false);
	keys.add(SDLK_LSHIFT, false);
	keys.add(SDLK_RSHIFT, false);
	keys.add(SDLK_SPACE, false);
	keys.add(SDLK_ESCAPE, false);
	playerMap[0].playing = true;
#ifndef REVENGINE_GL
	chars = new Sprite**[4];
	for (int j = 0; j < 4; j++)
	{
		Sprite *s = new Sprite("charset" + to_string(j), 0, 0);
		chars[j] = new Sprite*[256];
		for (int i = 0; i < 256; i++)
		{
			Sprite *sprite = new Sprite(s, 8 * (i % 16), 8 * (i / 16), 8, 8, 0, 0);
			chars[j][i] = sprite;
			if(j!=0)SDL_SetTextureBlendMode(sprite->sprite->texture, SDL_BLENDMODE_ADD);
		}
	}
#else
	chars = new Sprite**[1];
	for (int j = 0; j < 1;j++)
	{
		Sprite *s = new Sprite("charset" + to_string(j), 0, 0);
		chars[j] = new Sprite*[256];
		for (int i = 0; i < 256; i++)
		{
			Sprite *sprite = new Sprite(s, 8 * (i % 16), 8 * (i / 16), 8, 8, 0, 0);
			chars[j][i] = sprite;
		}
	}
#endif
	return 0;
}

void exitGame()
{
	if (window != nullptr) SDL_DestroyWindow(window);
#ifndef REVENGINE_GL
	if (renderer != nullptr) SDL_DestroyRenderer(renderer);
#endif
	for (int i = 0; i < sprites.length(); i++)
	{
		delete sprites[i];
	}
	for (int i = 0; i < numPlayers; i++)
	{
		delete players[i];
	}
	if (Mix_PlayingMusic() == 1)
	{
		Mix_HaltMusic();
	}
	Mix_FreeMusic(music);
	music = nullptr;
	List<Mix_Chunk*> soundList = sounds.values();
	for (int i = 0; i < soundList.length(); i++)
	{
		Mix_FreeChunk(soundList[i]);
	}
	for (int i = 0; i < joysticks.length(); i++)
	{
		SDL_JoystickClose(joysticks[i]);
		joysticks[i] = nullptr;
	}
	IMG_Quit();
	Mix_Quit();
	SDL_Quit();
}

File * openFile(string s, bool writing)
{
	File * file = new File();
	file->file = nullptr;
	if (writing)
	{
		file->file = SDL_RWFromFile(s.c_str(), "w+b");
	}
	else {
		file->file = SDL_RWFromFile(s.c_str(), "r+b");
	}
	if (file->file == nullptr)
	{
		delete file;
		return nullptr;
	}
	return file;
}

void closeFile(File *file)
{
	SDL_RWclose(file->file);
	delete file;
}

unsigned char readByte(File * file)
{
	unsigned char c;
	SDL_RWread(file->file, &c, sizeof(unsigned char), 1);
	return c;
}
char readChar(File * file)
{
	char c;
	SDL_RWread(file->file, &c, sizeof(char), 1);
	return c;
}
short readShort(File * file)
{
	return SDL_ReadBE16(file->file);
}
int readInt(File * file)
{
	return SDL_ReadBE32(file->file);
}
long long readLong(File * file)
{
	return SDL_ReadBE64(file->file);
}
unsigned short readUShort(File * file)
{
	return SDL_ReadBE16(file->file);
}
unsigned int readUInt(File * file)
{
	return SDL_ReadBE32(file->file);
}
unsigned long long readULong(File * file)
{
	return SDL_ReadBE64(file->file);
}
string readString(File * file)
{
	int length = readShort(file);
	char * c = new char[length + 1];
	c[length] = 0;
	SDL_RWread(file->file, c, length, 1);
	string s(c);
	delete[] c;
	return s;
}
void writeByte(File * file, unsigned char v)
{
	SDL_RWwrite(file->file, &v, sizeof(unsigned char), 1);
}
void writeChar(File * file, char v)
{
	SDL_RWwrite(file->file, &v, sizeof(char), 1);
}
void writeShort(File * file, short v)
{
	SDL_WriteBE16(file->file, v);
}
void writeInt(File * file, int v)
{
	SDL_WriteBE32(file->file, v);
}
void writeLong(File * file, long long v)
{
	SDL_WriteBE64(file->file, v);
}
void writeString(File * file, string v)
{
	writeShort(file, (short)v.length());
	SDL_RWwrite(file->file, v.c_str(), v.length(), 1);
}

//Key press related stuff

void switchKey(int k, bool state)
{
	switch (k)
	{
	case SDLK_w:
		playerMap[0].up = state;
		break;
	case SDLK_d:
		playerMap[0].right = state;
		break;
	case SDLK_s:
		playerMap[0].down = state;
		break;
	case SDLK_a:
		playerMap[0].left = state;
		break;
	case SDLK_SPACE:
		playerMap[0].a = state;
		break;
	case SDLK_LSHIFT:
	case SDLK_RSHIFT:
		playerMap[0].b = state;
		break;
	case SDLK_UP:
	case SDLK_i:
		playerMap[0].up2 = state;
		break;
	case SDLK_DOWN:
	case SDLK_k:
		playerMap[0].down2 = state;
		break;
	case SDLK_LEFT:
	case SDLK_j:
		playerMap[0].left2 = state;
		break;
	case SDLK_RIGHT:
	case SDLK_l:
		playerMap[0].right2 = state;
		break;
	default:
		break;
	}
}

bool getEvent(Event *e)
{
	SDL_Event sdl;
	if (SDL_PollEvent(&sdl))
	{
		if (sdl.type == SDL_QUIT)
		{
			e->type = EVENT_QUIT;
		}
		else if (sdl.type==SDL_KEYDOWN&&!sdl.key.repeat)
		{
			if (keys.contains(sdl.key.keysym.sym))
			{
				if (sdl.key.keysym.sym == SDLK_F11&&!keys[SDLK_F11])
				{
					if (fullscreen)
					{
						setWindowFullscreen(false);
						int scaleX = getScreenWidth() / WIDTH;
						int scaleY = getScreenHeight() / HEIGHT;
						int scalar = scaleX;
						if (scaleY < scaleX) scalar = scaleY;
						setWindowSize(scalar * WIDTH, scalar * HEIGHT);
						centerWindow();
					}
					else {
						setWindowFullscreen(true);
						setWindowSize(getScreenWidth(), getScreenHeight());
					}
					fullscreen = !fullscreen;
				}
				else if (sdl.key.keysym.sym == SDLK_ESCAPE)
				{
					e->type = EVENT_QUIT;
				} else {
					switchKey(sdl.key.keysym.sym, true);
				}
				keys[sdl.key.keysym.sym] = true;
				e->keys.key = KEY_UNKNOWN;
			}
			else {
				e->keys.key = KEY_UNKNOWN;
			}
		}
		else if (sdl.type==SDL_KEYUP&&!sdl.key.repeat)
		{
			if (keys.contains(sdl.key.keysym.sym))
			{
				keys[sdl.key.keysym.sym] = false;
				e->keys.key = KEY_UNKNOWN;
				switchKey(sdl.key.keysym.sym, false);
			}
			else {
				e->keys.key = KEY_UNKNOWN;
			}
		}
		else if (sdl.type == SDL_JOYAXISMOTION)
		{
			if (sdl.jaxis.which < MAX_PLAYERS)
			{
				if (!playerMap[sdl.jaxis.which].playing)
				{
					addPlayer(sdl.jaxis.which);
					playerMap[sdl.jaxis.which].playing = true;
				}
				if (sdl.jaxis.axis == 0)
				{
					if (sdl.jaxis.value < -JOYSTICK_DEAD_ZONE)
					{
						playerMap[sdl.jaxis.which].left = true;
						playerMap[sdl.jaxis.which].right = false;
					}
					else if (sdl.jaxis.value > JOYSTICK_DEAD_ZONE)
					{
						playerMap[sdl.jaxis.which].right = true;
						playerMap[sdl.jaxis.which].left = false;
					}
					else {
						playerMap[sdl.jaxis.which].right = false;
						playerMap[sdl.jaxis.which].left = false;
					}
				}
				else {
					if (sdl.jaxis.value < -JOYSTICK_DEAD_ZONE)
					{
						playerMap[sdl.jaxis.which].up = true;
						playerMap[sdl.jaxis.which].down = false;
					}
					else if (sdl.jaxis.value > JOYSTICK_DEAD_ZONE)
					{
						playerMap[sdl.jaxis.which].down = true;
						playerMap[sdl.jaxis.which].up = false;
					}
					else {
						playerMap[sdl.jaxis.which].up = false;
						playerMap[sdl.jaxis.which].down = false;
					}
				}
			}
		}
		else if (sdl.type == SDL_JOYBUTTONDOWN)
		{
			if (sdl.jbutton.which < MAX_PLAYERS)
			{
				if (!playerMap[sdl.jaxis.which].playing)
				{
					addPlayer(sdl.jaxis.which);
					playerMap[sdl.jaxis.which].playing = true;
				}
				if (sdl.jbutton.button % 2 == 0)
				{
					playerMap[sdl.jbutton.which].a = true;
				}
				else {
					playerMap[sdl.jbutton.which].b = true;
				}
			}
		}
		else if (sdl.type == SDL_JOYBUTTONUP)
		{
			if (sdl.jbutton.which < MAX_PLAYERS)
			{
				if (sdl.jbutton.button % 2 == 0)
				{
					playerMap[sdl.jbutton.which].a = false;
				}
				else {
					playerMap[sdl.jbutton.which].b = false;
				}
			}
		}
		else if (sdl.type == SDL_JOYHATMOTION)
		{
			if (sdl.jhat.which < MAX_PLAYERS)
			{
				if (!playerMap[sdl.jaxis.which].playing)
				{
					addPlayer(sdl.jaxis.which);
					playerMap[sdl.jaxis.which].playing = true;
				}
				playerMap[sdl.jhat.which].up = sdl.jhat.value&1;
				playerMap[sdl.jhat.which].down = sdl.jhat.value&4;
				playerMap[sdl.jhat.which].left = sdl.jhat.value&8;
				playerMap[sdl.jhat.which].right = sdl.jhat.value&2;
			}
		}
		else {
			e->type = EVENT_UNKNOWN;
		}
		return true;
	}
	else {
		e->type = EVENT_UNKNOWN;
		return false;
	}
}

bool getKey(Player *p, EventKey key)
{
	PlayerMap* m = &playerMap[p->id];
	switch (key)
	{
	case KEY_A:
		return m->a;
	case KEY_B:
		return m->b;
	case KEY_UP:
		return m->up;
	case KEY_DOWN:
		return m->down;
	case KEY_LEFT:
		return m->left;
	case KEY_RIGHT:
		return m->right;
	case KEY_UP2:
		return m->up2;
	case KEY_DOWN2:
		return m->down2;
	case KEY_LEFT2:
		return m->left2;
	case KEY_RIGHT2:
		return m->right2;
	default:
		break;
	}
	return false;
}

void createSound(string name)
{
	sounds.add(name, Mix_LoadWAV(("sound/" + name + ".wav").c_str()));
}

void createSong(string name)
{

}

void setBackgroundMusic(string name)
{
	if (song != name)
	{
		if (Mix_PlayingMusic() == 1)
		{
			Mix_HaltMusic();
		}
		if (music != nullptr)
		{
			Mix_FreeMusic(music);
		}
		music = Mix_LoadMUS(("sound/" + name + ".wav").c_str());
		song = name;
		if (music != nullptr)
		{
			Mix_PlayMusic(music, -1);
		}
	}
}

void playSound(string name)
{
	if (sounds[name] != nullptr)
	{
		Mix_PlayChannel(-1, sounds[name], 0);
	}
}

#ifdef main
#undef main
#endif
int main(int argc, char **argv)
{
	debug("Debugging mode active");

	int _startGame = startGame();
	if (_startGame != 0)
	{
		return _startGame;
	}

	addPlayer(0);

	init();

	try {
		while (theLoop());
	}
	catch (ApplicationClosingException &a)
	{
		debug("Application closing exception caught");
	}
	exitGame();
	return 0;
}