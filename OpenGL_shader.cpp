#include "Shader.h"
#include <string>
#include <glew.h>
#include <gl/GLU.h>
#include "Player.h"

GLuint loadShaders(std::string fragment);
void changeShader(GLuint shader);

Light::Light(float x, float y, float radius, float r, float g, float b)
{
	this->x = x;
	this->y = y;
	this->radius = radius;
	this->r = r;
	this->g = g;
	this->b = b;
}

Light::Light()
{
	this->x = 0.0;
	this->y = 0.0;
	this->radius = 0.0;
	this->r = 0.0;
	this->g = 0.0;
	this->b = 0.0;
}

namespace shader {
	GLuint sDefault, sInvert, sLightTest, sGrey;
	std::string default = std::string() +
		"#version 330 core\n" +
		"layout(location = 0) out vec4 color;" +
		"in vec2 UV;" +
		"uniform sampler2D sprite;" +
		"void main(){" +
		"color = texture(sprite,UV);" +
		"}";
	std::string invert = std::string() +
		"#version 330 core\n" +
		"layout(location = 0) out vec4 color;" +
		"in vec2 UV;" +
		"uniform sampler2D sprite;" +
		"void main(){" +
		"vec4 t = texture(sprite,UV);" +
		"color = vec4(1.0-t.x, 1.0-t.y, 1.0-t.z, t.w);" +
		"}";
	std::string grey = std::string() +
		"#version 330 core\n" +
		"layout(location = 0) out vec4 color;" +
		"in vec2 UV;" +
		"uniform sampler2D sprite;" +
		"void main(){" +
		"vec4 t = texture(sprite,UV);" +
		"float a = (t.x+t.y+t.z)/3.0;" +
		"color = vec4(a, a, a, t.w);" +
		"}";
	std::string lightTest = std::string() +
		"#version 330 core\n" +
		"layout(location = 0) out vec4 color;" +
		"uniform float lights[16*6];" +
		"in vec2 UV;" +
		"uniform sampler2D sprite;" +
		"uniform vec4 rect;" +
		"uniform vec3 ambient;" +
		"uniform int max;"
		"void main(){" +
		"color = texture(sprite,UV);" +
		"vec3 lc = ambient;" +
		"float x = rect.x + rect.z*UV.x;" +
		"float y = rect.y + rect.w*UV.y;" +
		"for(int i = 0; i < max; i++){" +
		"lc = lc + vec3(lights[6*i+3],lights[6*i+4],lights[6*i+5])*(1.0-clamp(((lights[6*i]-x)*(lights[6*i]-x)+(lights[6*i+1]-y)*(lights[6*i+1]-y))/lights[6*i+2]/lights[6*i+2],0.0,1.0));" +
		"}" +
		"color = color * vec4(lc,1.0);" +
		"}";
}

void shadersInit()
{
	shader::sDefault = loadShaders(shader::default);
	shader::sInvert = loadShaders(shader::invert);
	shader::sLightTest = loadShaders(shader::lightTest);
	shader::sGrey = loadShaders(shader::grey);
}

void sDefault()
{
	changeShader(shader::sDefault);
}

void sInvert()
{
	changeShader(shader::sInvert);
}

void sGrey()
{
	changeShader(shader::sGrey);
}

class Texture
{
public:
	GLuint texture;
	int width, height;
};

void sLighting(Player *p, float ambientR, float ambientG, float ambientB, List<Light>& lights)
{
	changeShader(shader::sLightTest);
	GLuint location = glGetUniformLocation(shader::sLightTest, "lights");
	GLfloat light[6 * 16];
	for (int i = 0; i < 16; i++)
	{
		if (i < lights.length())
		{
			light[6 * i] = lights[i].x;
			light[6 * i + 1] = lights[i].y;
			light[6 * i + 2] = lights[i].radius*(rand() / (float)RAND_MAX*.05F + .995F);
			light[6 * i + 3] = lights[i].r;
			light[6 * i + 4] = lights[i].g;
			light[6 * i + 5] = lights[i].b;
		}
		else {
			light[6 * i] = 0.0;
			light[6 * i + 1] = 0.0;
			light[6 * i + 2] = 0.0;
			light[6 * i + 3] = 0.0;
			light[6 * i + 4] = 0.0;
			light[6 * i + 5] = 0.0;
		}
	}
	glUniform1fv(location, 6*16, light);
	GLfloat rect[4] = {
		p->getCameraCenterX()-p->texture->width/2.0+p->width/2.0,p->getCameraCenterY()-p->texture->height/2.0+p->height/2.0,p->texture->width,p->texture->height
	};
	glUniform4f(glGetUniformLocation(shader::sLightTest, "rect"), rect[0], rect[1], rect[2], rect[3]);
	glUniform3f(glGetUniformLocation(shader::sLightTest, "ambient"), ambientR, ambientG, ambientB);
	glUniform1i(glGetUniformLocation(shader::sLightTest, "max"), lights.length());
}