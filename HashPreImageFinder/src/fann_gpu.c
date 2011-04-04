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


#include "fann_gpu.h"

void initOpenGL(){
	/*init opengl through GLUT, GLUT on windows, openglut on linux and mac*/
	int count = 1;
	char *args[1] = {""};
	glutInit ( &count, args );
	glutWindowHandle = glutCreateWindow("FANN - GPU");
}


char* test(){
	char *extensionList;
	char *version;
	char *vend;
	/*check vendor*/
	vend = (char*)glGetString(GL_VENDOR);
	if (strstr(vend, "NVIDIA") != NULL) vendor = NVIDIA;
	else if (strstr(vend, "ATI") != NULL) vendor = ATI;
	else vendor = OTHER;
	/*else return strcat("Vendor not supported: ", vend);*/

	/*check for right OpenGL version, at least 2.0*/
	version = (char*)glGetString(GL_VERSION);
	if (version[0] < 50) /*50 = "2"*/
		return strcat("Wrong OpenGL version: ", version);

	/*check for required extensions...*/
	extensionList = (char*)glGetString(GL_EXTENSIONS);

	/*used to write to texture*/
	if (strstr(extensionList, "GL_EXT_framebuffer_object") == NULL)
		return "GL_EXT_framebuffer_object not available";
	/*using internal texture format GL_RGBA32F_ARB*/
	if (strstr(extensionList, "GL_ARB_texture_float") == NULL)
		return "GL_ARB_texture_float not available";
	return 0;
}


int init(){
#ifdef WIN_OR_X11
	/*get function pointers*/
	/*opengl 2.0 glsl api functions*/
	if ((glGetProgramiv = (PFNGLGETPROGRAMIVPROC)getProcAddress("glGetProgramiv")) == 0)
		return 0;
	if ((glCreateProgram = (PFNGLCREATEPROGRAMPROC)getProcAddress("glCreateProgram")) == 0)
		return 0;
	if ((glDeleteProgram = (PFNGLDELETEPROGRAMPROC)getProcAddress("glDeleteProgram")) == 0)
		return 0;
	if ((glGetShaderiv = (PFNGLGETSHADERIVPROC)getProcAddress("glGetShaderiv")) == 0)
		return 0;
	if ((glCreateShader = (PFNGLCREATESHADERPROC)getProcAddress("glCreateShader")) == 0)
		return 0;
	if ((glDeleteShader = (PFNGLDELETESHADERPROC)getProcAddress("glDeleteShader")) == 0)
		return 0;
	if ((glShaderSource = (PFNGLSHADERSOURCEPROC)getProcAddress("glShaderSource")) == 0)
		return 0;
	if ((glCompileShader = (PFNGLCOMPILESHADERPROC)getProcAddress("glCompileShader")) == 0)
		return 0;
	if ((glAttachShader = (PFNGLATTACHSHADERPROC)getProcAddress("glAttachShader")) == 0)
		return 0;
	if ((glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)getProcAddress("glGetShaderInfoLog")) == 0)
		return 0;
	if ((glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)getProcAddress("glGetProgramInfoLog")) == 0)
		return 0;
	if ((glLinkProgram = (PFNGLLINKPROGRAMPROC)getProcAddress("glLinkProgram")) == 0)
		return 0;
	if ((glUseProgram = (PFNGLUSEPROGRAMPROC)getProcAddress("glUseProgram")) == 0)
		return 0;
	if ((glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)getProcAddress("glGetUniformLocation")) == 0)
		return 0;
	if ((glUniform1f = (PFNGLUNIFORM1FPROC)getProcAddress("glUniform1f")) == 0)
		return 0;
	if ((glUniform1i = (PFNGLUNIFORM1IPROC)getProcAddress("glUniform1i")) == 0)
		return 0;
	/*Extensions*/
	if ((glGenFramebuffersEXT = (PFNGLGENFRAMEBUFFERSEXTPROC)getProcAddress("glGenFramebuffersEXT")) == 0)
		return 0;
	if ((glBindFramebufferEXT = (PFNGLBINDFRAMEBUFFEREXTPROC)getProcAddress("glBindFramebufferEXT")) == 0)
		return 0;
	if ((glFramebufferTexture2DEXT = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)getProcAddress("glFramebufferTexture2DEXT")) == 0)
		return 0;
	if ((glCheckFramebufferStatusEXT = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC)getProcAddress("glCheckFramebufferStatusEXT")) == 0)
		return 0;
#ifdef WIN32
	if ((glActiveTexture = (PFNGLACTIVETEXTUREPROC)getProcAddress("glActiveTexture")) == 0)
		return 0;
#endif
#endif
	/*set formats.*/
	if (vendor == NVIDIA){
		texInternalFormat = GL_RGBA32F_ARB; /*Test if this works on ATI*/
	}
	else{ /*if above works on ATI, remove this.*/
		texInternalFormat = GL_RGBA_FLOAT32_ATI;
	}
	texTarget = GL_TEXTURE_2D;
	texFormat = GL_RGBA;

	/* create a Framebuffer object*/
	glGenFramebuffersEXT(1, &output_fb); 
	/* bind the fb obejct to the framebuffer. ie. write to object instead of window*/
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, output_fb);
#ifdef CHECK_ERROR
	if (glGetError() != GL_NO_ERROR) {
		printf("init():\t\t\t\t Failed...\n");
		return 0;
	} else{
		/*printf("init():\t\t\t\t Success...\n");*/
	}
#endif

	return 1;
}


/* creates a layer representation given shader source, input/output sizes*/
layer* generateLayer(char* source, int input_size, int output_size, int output_offset){
	layer * lay;
	lay = malloc(sizeof(layer));
	lay->program = 0;

	/*input vector sizes*/
	lay->size = input_size;
	/*since the actual width of a vector on the texture is a multiple of 4 the rgba size is divided by 4*/
	lay->size_rgba = input_size / 4;
	/* and to compensate for the integer rounding, we may need to add 1.*/
	if (input_size % 4 != 0)
		lay->size_rgba+=1;

	/*output vector sizes*/
	lay->out_size = output_size;
	lay->out_size_rgba = output_size / 4;
	if (output_size % 4 != 0)
		lay->out_size_rgba+=1;

	/*the offset suffers from the multiple of 4 issue. This means that any offset must be an multiple of 4.*/
	lay->out_offset_x = output_offset/4;

	/*generate textures to hold data.*/
	glGenTextures(1, &lay->input_texture);
	/*store texture for clean removal.*/
	lay->original_texture = lay->input_texture;
	setupTexture(lay->input_texture, lay->size_rgba, 1, texTarget, texFormat, texInternalFormat);
	
	glGenTextures(1, &lay->weight_texture);
	setupTexture(lay->weight_texture, lay->size_rgba, output_size, texTarget, texFormat, texInternalFormat);
	
	glGenTextures(1, &lay->mask_texture);
	setupTexture(lay->mask_texture, lay->size_rgba, output_size, texTarget, texFormat, texInternalFormat);

	/*if source is provided compile it and attach it to the source*/
	if (source != 0)
		lay->program = attachShaderProgram(lay, source);
	return lay;
}

/*clean up*/
void destroyLayer(layer * target){
	glDeleteTextures(1, &target->original_texture);
	glDeleteTextures(1, &target->weight_texture);
	glDeleteTextures(1, &target->mask_texture);
	glDeleteProgram(target->program);
	free(target);
}

/*sets the input vector in the target layer to the output vector of the input-layer.*/
void setInput(layer *target, layer *input){
	target->input_texture = input->out_texture;
}

/*sets the output vector of target layer to the input of output layer.*/
void setOutput(layer *target, layer *output){
	target->out_texture = output->input_texture;
}

/*compiles and links a shader source and attaches the resulting shader program to the target layer*/
GLuint attachShaderProgram(layer *target, char *source_file) {
	char * source = malloc(10000* sizeof(char));

	if (parseShader(source_file, source, target) != 0){
		GLuint programObj;
		GLuint shaderObj;
		GLint success = 0;
		programObj = glCreateProgram();

		/*create and attach a fragment shader*/
		shaderObj = glCreateShader(GL_FRAGMENT_SHADER);
		glAttachShader(programObj, shaderObj);
		/*set source to shader*/
		glShaderSource(shaderObj, 1, &source, 0);

		/*compile and link*/
		glCompileShader(shaderObj);
		printLog(shaderObj);
		glLinkProgram(programObj);
		printLog(programObj);
		/*check status*/
		glGetProgramiv(programObj, GL_LINK_STATUS, &success);
		if (!success) {
			printf("Shader could not be linked!\n");
			return -1;
		}

		/* Get memory location of the shader input.*/
		target->shader_input_vector = glGetUniformLocation(programObj, "input_vector");
		target->shader_weight_matrix = glGetUniformLocation(programObj, "weights");
		target->shader_mask_matrix = glGetUniformLocation(programObj, "mask");
		free (source);

#ifdef CHECK_ERROR
		if (glGetError() != GL_NO_ERROR) {
			printf("createShaderProgram():\t\t Failed...\n");
			return 0;
		} else{
			/*printf("createShaderProgram():\t\t Success...\n");*/
		}
#endif
		/*ev delete old program*/
		if (target->program != 0)
			glDeleteProgram(target->program);
		/*set new program and delete shader object since we dont need it anymore*/
		target->program = programObj;
		glDeleteShader(shaderObj);
		return programObj;
	}
	printf("could not read file: \"%s\"\n", source_file);
	return 0;
}


/*Copy the data from the vector data to texture texID of given size */
void copyVectorToTexture(float * data, layer * target){
	/*vendor specific code*/
	if (vendor == NVIDIA){
		/*bind the texture and fill it.*/
		glBindTexture(texTarget, target->input_texture);
		glTexSubImage2D(texTarget, 0, 0, 0, target->size_rgba, 1, texFormat, GL_FLOAT, data);
	}
	else {
		/*set texture as framebuffer object and draw to it, the sam manner as the shaders run.*/
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, texTarget, target->input_texture, 0);
		glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
		glRasterPos2i(0, 0);
		glDrawPixels(target->size_rgba, 1, texFormat, GL_FLOAT, data);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, texTarget, 0, 0);
	}
#ifdef CHECK_ERROR
	if (glGetError() != GL_NO_ERROR) {
		printf("copyVectorToTexture():\t\t Failed...\n");
	} else{
		/*printf("copyVectorToTexture():\t\t Success...\n");*/
	}
#endif
}

/*read data back from texture*/
void copyVectorFromTexture(float* data, layer* target) {
	/*bind texture and read*/
	glBindTexture(texTarget ,target->input_texture); 
	glGetTexImage(texTarget, 0, texFormat, GL_FLOAT, data);
#ifdef CHECK_ERROR
	if (glGetError() != GL_NO_ERROR) {
		printf("copyVectorFromTexture():\t Failed...\n");
	} else{
		/*printf("copyVectorFromTexture():\t Success...\n");*/
	}
#endif
}

/*copy 2d weigth matrix to the weight texture*/
/*the same as for vector but in 2d*/
void copyWeightsToTexture(float* data, layer* target){
	/*resize the weight vector to match the real size of the texture*/
	float * newdata = malloc(sizeof(float)*target->out_size*target->size_rgba*4);
	int index = 0, i,j;
	for(i=0; i<target->out_size; i++)
		for(j=0; j<target->size_rgba*4; j++)
			newdata[j+i*target->size_rgba*4] = j < target->size ? data[index++] : 0.0f;

	/*upload the values*/
	if (vendor == NVIDIA){
		glBindTexture(texTarget, target->weight_texture);
		glTexSubImage2D(texTarget, 0, 0, 0, target->size_rgba, target->out_size, texFormat, GL_FLOAT, newdata);
	}
	else {
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, texTarget, target->weight_texture, 0);
		glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
		glRasterPos2i(0, 0);
		glDrawPixels(target->size_rgba, target->out_size, texFormat, GL_FLOAT, newdata);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, texTarget, 0, 0);
	}
	free(newdata);
#ifdef CHECK_ERROR
	if (glGetError() != GL_NO_ERROR) {
		printf("copyWeightsToTexture():\t\t Failed...\n");
	} else{
		/*printf("copyWeightsToTexture():\t\t Success...\n");*/
	}
#endif
}

/*copy 2d mask matrix to the mask texture*/
/*the same as for weights*/
void copyMaskToTexture(float* data, layer* target){
	float * newdata = malloc(sizeof(float)*target->out_size_rgba*target->size_rgba*16);
	int index = 0, i,j;
	for(i=0; i<target->out_size_rgba*4; i++)
		for(j=0; j<target->size_rgba*4; j++)
			newdata[j+i*target->size_rgba*4] = j < target->size && i < target->out_size ? data[index++] : 0.0f;
	if (vendor == NVIDIA){
		glBindTexture(texTarget, target->mask_texture);
		glTexSubImage2D(texTarget, 0, 0, 0, target->size_rgba, target->out_size, texFormat, GL_FLOAT, newdata);
	}
	else {
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, texTarget, target->mask_texture, 0);
		glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
		glRasterPos2i(0, 0);
		glDrawPixels(target->size_rgba, target->out_size, texFormat, GL_FLOAT, newdata);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, texTarget, 0, 0);
	}
#ifdef CHECK_ERROR
	if (glGetError() != GL_NO_ERROR) {
		printf("copyMaskToTexture():\t\t Failed...\n");
	} else{
		/*printf("copyMaskToTexture():\t\t Success...\n");*/
	}
#endif
}




/*setup a given texture. //once per texture, private method*/
void setupTexture(const GLuint texID, GLuint width, GLuint height, GLenum tex_target, GLenum tex_format, GLenum internal_format){
	/*bind and set modes*/
	glBindTexture(tex_target, texID);
	glTexParameteri(tex_target, GL_TEXTURE_MIN_FILTER, GL_NEAREST); /* no interpolation*/
	glTexParameteri(tex_target, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
	glTexParameteri(tex_target, GL_TEXTURE_WRAP_S, GL_CLAMP); /*texture does not repeat beyond the borders.*/
	glTexParameteri(tex_target, GL_TEXTURE_WRAP_T, GL_CLAMP);
	/* define texture with floating point format*/
	glTexImage2D(tex_target, 0 ,internal_format, width, height, 0, tex_format, GL_FLOAT, 0);
	/*Error check*/
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
#ifdef CHECK_ERROR
	if (glGetError() != GL_NO_ERROR) {
		printf("setupTexture():\t\t\t Failed...\n");
	} else{
		/*printf("setupTexture():\t\t\t Success...\n");*/
	}
#endif
}

/*print the data that lies in the input vector of a layer*/
void printLayer(layer *target){
	int i;
	float * data = malloc(sizeof(float) * target->size_rgba*4);
	copyVectorFromTexture(data, target);
	printf("\nData currently in layer:\n");
	for (i=0; i<target->size; i++) 
		printf("%f\n",data[i]);
	free(data);
}

/*parses path(file) to text, adding necessary #defines and to the front of */
//text or after first ¤ character if present
char* parseShader(char* file, char* text, layer * target){
	char * tmp_text = calloc(10000, sizeof(char));
	char * value = calloc(10, sizeof(char));
	char * code_start;
	int len;
	int index;
	FILE * f = fopen(file, "r");
	if (f){
		int ch;
		int ind;
		ind = 0;
		/*readfile*/
		while((ch = fgetc( f )) != EOF) {
			tmp_text[ind] = ch;
			ind++;
		}
		fclose(f);
		tmp_text[ind] = 0;

		/*add defines*/
		strcpy(text, "#version 110\n");
		index = 13;

		/*define the input size*/
		len = sprintf(value, "%d", target->size_rgba);
		strcpy(text+index, "#define i_size ");
		index+=15;
		strncpy(text+index, value, len);
		index += len;
		strcpy(text+index, ".0\n");
		index+=3;

		/*define the output size*/
		len = sprintf(value, "%d", target->out_size);
		strcpy(text+index, "#define o_size ");
		index+=15;
		strncpy(text+index, value, len);
		index += len;
		strcpy(text+index, ".0\n");
		index+=3;

		/*define the offset length*/
		len = sprintf(value, "%d", target->out_offset_x);
		strcpy(text+index, "#define offset ");
		index+=15;
		strncpy(text+index, value, len);
		index += len;
		strcpy(text+index, ".0\n");
		index+=3;

		/*add source*/
		if ((code_start = strstr(tmp_text,"¤")) != 0)
			strcpy(text+index, code_start+1);
		else
			strcpy(text+index, tmp_text);
		free(value);
		free(tmp_text);
		return text;
	}
	return 0;
}

/*prints log of shader or program*/
void printLog(GLuint obj) {
	int logLength = 0;
	int chars  = 0;
	char *infoLog;
	/*check if there is a log for a shader*/
	glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength > 1) {
		/*get full log and print if*/
		infoLog = (char *)malloc(logLength);
		glGetShaderInfoLog(obj, logLength, &chars, infoLog);
		printf(infoLog);
		printf("\n");
		free(infoLog);
		logLength = 0;
	}
	/*check if there is a program log*/
	glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength > 1) {
		/*get full log and print if*/
		infoLog = (char *)malloc(logLength);
		glGetProgramInfoLog(obj, logLength, &chars, infoLog);
		printf(infoLog);
		printf("\n");
		free(infoLog);
		logLength = 0;
	}
	/*clear error state, either getProgram or getShader renders error.*/
	glGetError();
}

void run(layer * target){
	/* setup 1:1 texture - pixel mapping*/
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (GLdouble)target->out_size_rgba, 0.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	/*Move the renderers focus to the offset position*/
	glViewport(target->out_offset_x, 0, target->out_size_rgba, 1);

	/*render to output tex*/
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, texTarget, target->out_texture, 0);

	/*glLinkProgram(target->program);*/
	glUseProgram(target->program);

	/*input vector*/
	/*let texture 0 be input_vector*/
	glActiveTexture(GL_TEXTURE0);	
	glBindTexture(texTarget, target->input_texture);
	/*tell shader that input_vector_param is tex 0*/
	glUniform1i(target->shader_input_vector, 0); 

	glActiveTexture(GL_TEXTURE1);	
	glBindTexture(texTarget, target->weight_texture);
	glUniform1i(target->shader_weight_matrix, 1);

	glActiveTexture(GL_TEXTURE2);	
	glBindTexture(texTarget, target->mask_texture);
	glUniform1i(target->shader_mask_matrix, 2); 
	/*check status.*/
	if (glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) != GL_FRAMEBUFFER_COMPLETE_EXT){
		printf("Framebuffer incomplete, status: %d\n", glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT));
		return;
	}
	/*COMPUTE!*/
	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
	glPolygonMode(GL_FRONT,GL_FILL);
	
	glBegin(GL_QUADS);
	/*if(texTarget == GL_TEXTURE_2D){*/
		glTexCoord2f(0.0f, 0.0f); 
		glVertex2f(0.0f, 0.0f);
		glTexCoord2f(1.0f, 0.0f); 
		glVertex2f((GLfloat)target->out_size_rgba, 0.0f);
		glTexCoord2f(1.0f, 1.0f); 
		glVertex2f((GLfloat)target->out_size_rgba, 1.0f);
		glTexCoord2f(0.0f, 1.0f); 
		glVertex2f(0.0f, 1.0f);
	/*}*/
	glEnd(); 
	
#ifdef CHECK_ERROR
	if (glGetError() != GL_NO_ERROR) {
		printf("run():\t\t\t\t Failed...\n");
	} else{
		/*printf("run():\t\t\t\t Success...\n");*/
	}
#endif
}

