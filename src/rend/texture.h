#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include "GL/glew.h"

class DS_TEXTURE
{
	public:
		DS_TEXTURE();
		DS_TEXTURE(bool pita);
		DS_TEXTURE(const std::string& fileName);
		~DS_TEXTURE();

		void bind(unsigned int unit);

	private:
		GLuint hTex;

};

#endif // TEXTURE_H