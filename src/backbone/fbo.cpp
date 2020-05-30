//--- --- --- --- --- --- --- --- ---
//	* 	L 	I 	E 	B 	R 	A 	N 	C 	A 	*
//
//	-programmer, cg artist.
//	-contact: dantepaola25@gmail.com
//
//--- --- --- --- --- --- --- --- ---

//--- --- --- --- --- --- --- --- ---
//	CRK BLOCK
//--- --- --- --- --- --- --- --- ---
//		0.	LIBRARIES
//	--- 	--- 	--- 	--- 	---
#include "fbo.h"
	
//		1.0		CONTS
//	--- 	--- 	--- 	--- 	---
namespace paint {
	std::vector<FBO*> frameBuffers;
	CANVAS* canvas;

	extern const unsigned int SHADOWRES = 256;

//--- --- --- --- --- --- --- --- ---
//		2.	FUNKS
//	--- 	--- 	--- 	--- 	---

//		2.1		CREATION
//	--- 	--- 	--- 	--- 	---
	FBO* createColorBuffer() {
		unsigned int i = frameBuffers.size();
		frameBuffers.push_back(new FBO());

		glGenFramebuffers(1, &frameBuffers[i]->loc);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffers[i]->loc);

		frameBuffers[i]->color = new DS_TEXTURE();

		unsigned int& rbo = frameBuffers[i]->rbo;
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1366, 768);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		return frameBuffers[i];
	}

	FBO* createDepthBuffer() {
		unsigned int i = frameBuffers.size();
		frameBuffers.push_back(new FBO());

		glGenFramebuffers(1, &frameBuffers[i]->loc);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffers[i]->loc);

		frameBuffers[i]->color = new DS_TEXTURE(false);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		return frameBuffers[i];
	}

	void initCanvas() {
		float quadVertices[] = {
			// positions   // texCoords
			-1.0f,  1.0f,  0.0f, 1.0f,
			-1.0f, -1.0f,  0.0f, 0.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,

			-1.0f,  1.0f,  0.0f, 1.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,
			 1.0f,  1.0f,  1.0f, 1.0f
		};

		unsigned int quadVAO, quadVBO;
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

		canvas = new CANVAS({ quadVAO, quadVBO });

	}

//		2.2		DELETION
//	--- 	--- 	--- 	--- 	---
	void deleteFrameBuffers() {

		for (unsigned int i = 0; i < frameBuffers.size(); i++) {
			FBO* fbo = frameBuffers[i];
			glDeleteRenderbuffers(1, &fbo->rbo);
			delete fbo->color;
			glDeleteFramebuffers(1, &fbo->loc);

			delete frameBuffers[i]; frameBuffers[i] = nullptr;

		}
	}

	void deleteCanvas() {
		glDeleteVertexArrays(1, &canvas->vao);
		glDeleteBuffers(1, &canvas->vbo);
		delete canvas;
	}

//		666.	DONE	.999 || ->DS
//--- END CRK BLOCK --- --- --- --- --- --- --- ---
};