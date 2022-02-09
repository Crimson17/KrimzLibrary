#include "KrimzLib/gl/texture.h"


// Constructors/destructor
kl::texture::texture(const kl::image& image) {
	// Setting the texture type
	this->textureType = GL_TEXTURE_2D;

	// Flipping the image verticaly
	kl::image temp = image;
	temp.flipVertical();

	// Generating the texture
	glGenTextures(1, &textureID);

	// Binding the texture object
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Setting the texture parameters
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

	// Loading the pixel data
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, temp.gWidth(), temp.gHeight(), 0, GL_BGR, GL_UNSIGNED_BYTE, temp.pointer());

	// Generating the texture mipmaps
	glGenerateMipmap(GL_TEXTURE_2D);

	// Unbinding the texture object
	glBindTexture(GL_TEXTURE_2D, NULL);
}
kl::texture::texture(const kl::image& front, const kl::image& back, const kl::image& left, const kl::image& right, const kl::image& top, const kl::image& bottom) {
	// Setting the texture type
	this->textureType = GL_TEXTURE_CUBE_MAP;

	// Generating the texture
	glGenTextures(1, &textureID);

	// Binding the texture object
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	// Setting the texture parameters
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	// Loading the pixel data
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, right.gWidth(), right.gHeight(), 0, GL_BGR, GL_UNSIGNED_BYTE, right.pointer());
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, left.gWidth(), left.gHeight(), 0, GL_BGR, GL_UNSIGNED_BYTE, left.pointer());
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, top.gWidth(), top.gHeight(), 0, GL_BGR, GL_UNSIGNED_BYTE, top.pointer());
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, bottom.gWidth(), bottom.gHeight(), 0, GL_BGR, GL_UNSIGNED_BYTE, bottom.pointer());
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, front.gWidth(), front.gHeight(), 0, GL_BGR, GL_UNSIGNED_BYTE, front.pointer());
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, back.gWidth(), back.gHeight(), 0, GL_BGR, GL_UNSIGNED_BYTE, back.pointer());

	// Unbinding the texture object
	glBindTexture(GL_TEXTURE_CUBE_MAP, NULL);
}
kl::texture::~texture() {
	glDeleteTextures(1, &this->textureID);
}

// Binds the texture to the texture unit
void kl::texture::bind(kl::id textureUnit) const {
	// Activating the given image unit
	glActiveTexture(textureUnit);

	// Binding the texture object
	glBindTexture(textureType, textureID);
}
