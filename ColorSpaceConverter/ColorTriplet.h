#pragma once
#ifndef COLORTRIPLET_H
#define COLORTRIPLET_H

#include "CommonIncludes.h"

typedef enum COLOROMODEL
{
	YCbCr = 0,
	RGB = 1,
	XYZ = 2,
	ICtCp = 3
} ColorModel;

template <typename T>
class ColorTriplet
{
private:
	// The color triplet. 
	T triplet[3];

	// Indicates the triplet contain an alpha channel
	bool hasAlpha;

	// Bit Depth of each component in the triplet
	unsigned int bitDepth;

	ColorModel colorModel;

public:
	ColorTriplet(T element1, T element2, T element3, unsigned int bitDepth, bool hasAlpha, ColorModel colorModel) : 
		bitDepth(bitDepth),
		hasAlpha(hasAlpha),
		colorModel(colorModel)
	{
		triplet[0] = element1;
		triplet[1] = element2;
		triplet[2] = element3;
	};

	ColorTriplet(T elements[3], unsigned int bitDepth, bool hasAlpha, ColorModel colorModel) : 
		bitDepth(bitDepth),
		hasAlpha(hasAlpha),
		colorModel(colorModel)
	{
		for (int ii = 0; ii < 3; ii++)
			triplet[ii] = elements[ii];
	}

	// Get this triplet as an array
	void GetArray(T *output);
	
	// Get this triplet as a vector
	std::vector<T> GetArray();

	unsigned int GetBitDepth();

	ColorModel GetColorModel();

	bool GetAlpha();

	void SetArray(std::vector<double> pixel);
};

#endif

template<typename T>
inline void ColorTriplet<T>::GetArray(T * output)
{
	if (output != nullptr)
	{
		for (int ii = 0; ii < 3; ii++)
			output[ii] = triplet[ii];
	}
}

template<typename T>
inline std::vector<T> ColorTriplet<T>::GetArray()
{
	std::vector<T> output(3);

	for (int ii = 0; ii < 3; ii++)
	{
		output[ii] = triplet[ii];
	}

	return output;
}

template<typename T>
inline unsigned int ColorTriplet<T>::GetBitDepth()
{
	return this->bitDepth;
}

template<typename T>
inline ColorModel ColorTriplet<T>::GetColorModel()
{
	return this->colorModel;
}

template<typename T>
inline bool ColorTriplet<T>::GetAlpha()
{
	return this->hasAlpha;
}

template<typename T>
inline void ColorTriplet<T>::SetArray(std::vector<double> pixel)
{
	for (int i = 0; i < 3; i++)
		this->triplet[i] = pixel[i];
}
