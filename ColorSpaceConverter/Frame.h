#pragma once
#ifndef FRAME_H
#define FRAME_H

class Frame
{
private:
	// Frame data, as a byte array
	unsigned char * data;

	// Width of the frame in pixels.
	unsigned int width; 

	// Height of the frame in pixels
	unsigned int height; 

	// The pixel format of the frame.
	unsigned int pixFmt;

public:
	// default constructor
	Frame() : data(nullptr), width(0), height(0), pixFmt(0) {}

	Frame(unsigned int width, unsigned int height, unsigned int pixFmt);

	// copy constructor
	Frame(Frame &other);

	// move constructor
	Frame(Frame &&other);

	// assignment operator
	Frame &operator=(Frame other);
};

#endif