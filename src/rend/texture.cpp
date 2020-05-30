#include "texture.h"
#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include <cassert>
#include "../lymath/utils.h"
#include "joj.h"


DS_TEXTURE::DS_TEXTURE(bool pita) {
	glGenTextures(1, &hTex);
	glBindTexture(GL_TEXTURE_2D, hTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		256, 256, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, hTex, 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

}

DS_TEXTURE::DS_TEXTURE() { 

	glGenTextures(1, &hTex);
	glBindTexture(GL_TEXTURE_2D, hTex);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1366, 768, 0,
		GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hTex, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
}

DS_TEXTURE::DS_TEXTURE(const std::string& fileName)
{
	std::vector<unsigned char> buff, imageData = lybyte::frombin(fileName);

	unsigned int count;

	int xi = 0; unsigned int r, g, b; std::vector<bool> col;
	for (unsigned int i = 0; i < imageData.size(); i+= 3) {
		count = unsigned int(imageData[i + 2]);

		col = lybyte::cfile_sec(i, i + 2);
		r = lybyte::todecimal(lybyte::takebits(col, 0, 6)) * 16;
		g = lybyte::todecimal(lybyte::takebits(col, 6, 12)) * 16;
		b = lybyte::todecimal(lybyte::takebits(col, 12, 16)) * 32;

		for (unsigned int rep = 0; rep < count; rep++) {
			buff.push_back(r); buff.push_back(g); buff.push_back(b);
			xi++;
		}
	}

	glGenTextures(1, &hTex);
	glBindTexture(GL_TEXTURE_2D, hTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256,
		0, GL_RGB, GL_UNSIGNED_BYTE, &buff[0]);

	glBindTexture(GL_TEXTURE_2D, 0);
	lybyte::cfile_close();
}

DS_TEXTURE::~DS_TEXTURE()
{
	glDeleteTextures(1, &hTex);
}

void DS_TEXTURE::bind(unsigned int unit)
{
	assert(unit >= 0 && unit <= 31);
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, hTex);
}