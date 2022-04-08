#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

class FrameBuffer
{
private:
	unsigned int FBO;
	unsigned int colourAttachment;
	unsigned int depthAttachment;
public:
	FrameBuffer(unsigned int width, unsigned int height);
	unsigned int getBuffer();
	unsigned int getTexture();
	unsigned int getDepthMap();
};

