/*
Fast Artificial Neural Network Library (fann)
Copyright (C) 2003 Steffen Nissen (lukesky@diku.dk)

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/



//comment to disable error checking
//#define CHECK_ERROR

//get rid of deprecated warnings: fopen, strcat, strcpy etc...
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS 1
#endif



//include opengl stuff
#ifdef WIN32
#include <GL/glut.h>
#include <GL/glext.h>
#define getProcAddress(x) wglGetProcAddress(x) // OS dependent function pointers methods
#define WIN_OR_X11
#endif

#ifdef __APPLE__
#import <OpenGL/gl.h>
#import <OpenGL/glu.h>
#import <GLUT/glut.h>
#include <GL/openglut.h>
#include <OpenGL/glext.h>
#define POINTERS
#define LD_PREBIND
#endif

#ifdef X11
#include <openglut.h>
#include <GL/glext.h>
#define getProcAddress(x) glXGetProcAddressARB(x) //OS dependent function pointers methods
#define WIN_OR_X11
#endif


//standard includes
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>


#ifndef RAND
#define RAND (((float)rand()) / RAND_MAX ) //random i the range 0, 1
#endif

#ifndef RAND_UNI
#define RAND_UNI (2.0f*RAND - 1.0f) //random in the range -1, 1
#endif

//function pointers for non standard opengl functions
#ifndef POINTERS
#define POINTERS
//GLSL 
PFNGLGETPROGRAMIVPROC				glGetProgramiv;
PFNGLCREATEPROGRAMPROC		        glCreateProgram;
PFNGLCREATEPROGRAMOBJECTARBPROC		glCreateProgram2;
PFNGLDELETEPROGRAMPROC				glDeleteProgram;
PFNGLGETSHADERIVPROC				glGetShaderiv;
PFNGLCREATESHADERPROC				glCreateShader;
PFNGLDELETESHADERPROC				glDeleteShader;
PFNGLSHADERSOURCEPROC	            glShaderSource;
PFNGLCOMPILESHADERPROC				glCompileShader;
PFNGLATTACHSHADERPROC				glAttachShader;
PFNGLGETSHADERINFOLOGPROC			glGetShaderInfoLog;
PFNGLGETPROGRAMINFOLOGPROC			glGetProgramInfoLog;
PFNGLLINKPROGRAMPROC	            glLinkProgram;
PFNGLUSEPROGRAMPROC					glUseProgram;
PFNGLGETUNIFORMLOCATIONPROC		    glGetUniformLocation;
PFNGLUNIFORM1FPROC					glUniform1f;
PFNGLUNIFORM1IPROC					glUniform1i;

//extensions
PFNGLGENFRAMEBUFFERSEXTPROC			glGenFramebuffersEXT;
PFNGLBINDFRAMEBUFFEREXTPROC			glBindFramebufferEXT;
PFNGLFRAMEBUFFERTEXTURE2DEXTPROC	glFramebufferTexture2DEXT;
PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC	glCheckFramebufferStatusEXT;

#ifdef WIN32 // under windows
PFNGLACTIVETEXTUREPROC				glActiveTexture;
#endif
#endif

#define NVIDIA 1
#define ATI 2
#define OTHER 3

int vendor;

//Framebuffer (output)
GLuint output_fb;
GLuint glutWindowHandle;



//Layer representation - includes inputs and weights
typedef struct{
	int size;
	int size_rgba;
	GLuint input_texture;
	GLuint original_texture; 
	GLuint weight_texture;
	GLuint mask_texture;
	GLuint program;

	//output:
	int out_size;
	int out_size_rgba;
	GLuint out_texture;
	int out_offset_x;

	//shader params
	GLint shader_input_vector; 
	GLint shader_weight_matrix;
	GLint shader_mask_matrix;
	
} layer;


//Move into layer struct, or keep global??
GLenum texTarget;
GLenum texInternalFormat;
GLenum texFormat;


//Public functions
/* Function: initOpenGL

initializes the OpenGL context. Must be done befor any other OpenGL calls can be made.

*/ 
void initOpenGL();


/* Function: test

test if the system is compatible, returns a error string ow.

Returns:
returns a pointer to the error string it there was an error. Otherwise 0

Example:
> char * error;
> if ((error = test()) != 0)
>	printf("system not compatible: %s",error);

*/ 
char* test();

/* Function: init

Get function pointers and setup framebuffer and formats.

Returns:
returns 1 if successful otherwise 0

*/ 
int init();

/* Function: attachShaderProgram

creates a compiled and linked shader program from a source file. It attaches the program to the layer. In addition it also returnes the program handle.

Parameters:
source_file - pointer to the filename of the shader source
target - the layer to attach the program. For example see generateLayer.

Returns:
returns the shader program
*/ 
GLuint attachShaderProgram(char* source_file, layer * target);

/* Function: generateLayer

Creates a layer and returns a pointer to it

Parameters:
source - pointer to shader source file name.
input_size - number of input neurons
output_size - number of output neurons
output_offset - if the output vector offset

Returns:
returns a pointer to the new layer

Example:
> //create a layer with input 13 input and 5 output connections and no offset
> //layer *lay = generateLayer(0, 13, 5, 0);
> //attach shader "sum.fp"
> //attachShaderProgram("sum.fp", lay);
*/ 
layer* generateLayer(char* source, int input_size, int output_size, int output_offset);

/* Function: setInput

Sets the input of a Layer to be the output of another

Parameters:
target - the layer that receives the update
input - the new input layer

*/ 
void setInput(layer* target, layer* input);

/* Function: setOutput

Sets the output of a Layer to be the input of another

Parameters:
target - the layer that receives the update
input - the new output layer

*/ 
void setOutput(layer* target, layer* output);

/* Function: destroyLayer

Frees all textures and memory used by this layer

Parameters:
target - layer to be destroyed

*/ 
void destroyLayer(layer * target);

/* Function: run

Executes the shader program on the given layer.

Parameters:
target - the layer to execute

*/ 
void run(layer * target);

/* Function: printLayer

Prints the the values that currently are stored in the target layer in the input texture

Parameters:
target - the layer to print input on
*/
void printLayer(layer *target);

/* Function: copyVectorToTexture

Copies an array of floats onto the input texture of the given layer

Parameters:
data - the float array to transfer
target - the target layer that holds the input vector that will be filled

Example:
> float data[3] = {1.0f, 2.0f, 3.0f};
> //generate a layer with input size 3.
> layer *lay = generateLayer("shader.fp", 3, 5, 0);
> //copy the vector
> copyVectorToTexture(data, lay);

*/
void copyVectorToTexture(float * data, layer* target);

/* Function: copyVectorToTexture

Copies the current data from the input texture of the given layer to an array of floats

Parameters:
data - the target float array.
target - the target layer that holds the input vector that will be filled

Example:
> float data[3] = {1.0f, 2.0f, 3.0f};
> //generate a layer with input size 3.
> layer *lay = generateLayer("shader.fp", 3, 5, 0);
> //copy the vector
> copyVectorToTexture(data, lay);

*/
void copyVectorFromTexture(float* data, layer* target);

/* Function: copyWeightsToTexture

Copies a matrix represented as an array of floats to the weight matrix of the target layer

Parameters:
data - the target float array.
target - the target layer that holds the weights matrix that will be filled
*/

void copyWeightsToTexture(float* data, layer* target);

/* Function: copyMaskToTexture

Copies a matrix represented as an array of floats to the mask matrix of the target layer. 
It is actually a second weight array. For each element the product of the mask and the weight 
gives the true weight. It is supposed to only be filled with 0.0 or 1.0 to create sparse networks.

Parameters:
data - the target float array.
target - the target layer that holds the mask matrix that will be filled
*/
void copyMaskToTexture(float* data, layer* target);


//HELPERS - private
//prints the infolog of either a shader object or a program object
void printLog(GLuint obj);
char* parseShader(char* file, char* text, layer* target);
void setupTexture(const GLuint texID, GLuint width, GLuint height, GLenum tex_target, GLenum tex_format, GLenum internal_format);





