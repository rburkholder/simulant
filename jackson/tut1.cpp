/* 
 * File:   tut1.cpp
 * Author: rpb
 * 
 * Created on March 18, 2015, 11:12 PM
 */

//Copyright (C) 2010-2012 by Jason L. McKesson
//This file is licensed under the MIT License.



#include <algorithm>
#include <string>
#include <vector>
#include <stdio.h>

#define GL_GLEXT_PROTOTYPES

#ifdef __WXMAC__
#include "OpenGL/glu.h"
#include "OpenGL/gl.h"
#else
#include <GL/glu.h>
#include <GL/gl.h>
#include <GL/glext.h>
#endif

#include "tut1.h"

const std::string strVertexShader(
	"#version 330\n"
	"layout(location = 0) in vec4 position;\n"
	"void main(void) {\n"
	"   gl_Position = position;\n"
	"}\n"
);

const std::string strFragmentShader(
	"#version 330\n"
	"out vec4 outputColor;\n"
	"void main(void) {\n"
	"   outputColor = vec4(1.0f, 0.0f, 1.0f, 1.0f);\n"
	"}\n"
);

const GLfloat vertexPositions[] = {
	0.75f, 0.75f, 0.0f, 1.0f,
	0.75f, -0.75f, 0.0f, 1.0f,
	-0.75f, -0.75f, 0.0f, 1.0f,
};

void tut1::InitializeVertexBuffer() {
	glGenBuffers(1, &boPosition);

	glBindBuffer(GL_ARRAY_BUFFER, boPosition);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

//Called after the window and OpenGL are initialized. Called exactly once, before the main loop.
void tut1::OnPaintInit() {
  
  CanvasBase::AddShader( GL_VERTEX_SHADER, strVertexShader );
  CanvasBase::AddShader( GL_FRAGMENT_SHADER, strFragmentShader );
	InitializeProgram();
  
	InitializeVertexBuffer();

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
}

//Called to update the display.
//You should call glutSwapBuffers after all of your rendering to display what you rendered.
//If you need continuous updates of the screen, call glutPostRedisplay() at the end of the function.
void tut1::OnPaint() {
  
	glClearColor(0.0f, 0.0f, 1.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(m_program);

	glBindBuffer(GL_ARRAY_BUFFER, boPosition);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glDisableVertexAttribArray(0);
	glUseProgram(0);

}

//Called whenever the window is resized. The new window size is given, in pixels.
//This is an opportunity to call glViewport or glScissor to keep up with the change in size.
void tut1::OnResize( int w, int h) {
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
}
