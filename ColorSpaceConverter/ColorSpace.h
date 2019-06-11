#pragma once
#ifndef COLORSPACE_H
#define COLORSPACE_H

#if 0
#include "CommonIncludes.h"
#include "Matrix_3x3.h"

// Transfer Function Enum
enum TransferFunctionEnum
{
	Bt601Tf,
	Bt202012BitTf,
	PqTf,
	HlgTf
};

enum ColorSpaceEnum
{
	Bt601Cs, 
	Bt709Cs,
	Bt2020Cs
};

// Transfer Functions : These are encapsulated in structs for ease of use. 

// Interface for all transfer functions
struct TransferFunction 
{
	virtual double transferFunction(const double &component) = 0;
};

// Used for BT.601, BT.709, and BT.2020 10-bit systems
struct BT601TransferFunction : public TransferFunction
{
	virtual double transferFunction(const double &component);
};

struct BT2020TransferFunction : public TransferFunction
{
	// holds the BT2020 transfer function for 12-bit components
	virtual double transferFunction(const double &component);
};

struct PqTransferFunction : public TransferFunction
{
	virtual double transferFunction(const double &component);
};

struct HlgTransferFunction : public TransferFunction
{
	virtual double transferFunction(const double &component);
};

// Note: z = 1 - x - y
struct Primaries
{
	double xr, yr, zr; // Red
	double xg, yg, zg; // Green
	double xb, yb, zb; // Blue
};

// Note: z = 1 - x - y
struct WhitePoint
{
	double xw, yw, zw; // White Point
};

// Note: These are given for 8-bit digital values
// and need to be converted to higher bit-depth 
// equivalents
struct Offset
{
	static const unsigned int LumaOffset = 16;
	static const unsigned int ChromaOffset = 128;
};

// static array of all TransferFunctions

//static const TransferFunction *transferFunctions[4] =
//{
//	new BT601TransferFunction(),
//	new BT2020TransferFunction(),
//	new PqTransferFunction(),
//	new HlgTransferFunction()
//};

// abstract class for all color spaces to inherit from
class ColorSpace
{
	/*
	 * We define a color space in terms of 2 properties:
	 * 1. It's primaries defined in the CIE 1931 XYZ Color Space.
		  This includes the white point for the color space.
	 * 2. It's opto-electrical transfer function(s).
	 * Conversion matrices are dependent on the primaries, and so 
	 * do not define the color space. They are listed here nonetheless
	 * as an optimization. The process to derive said matrices will be 
	 * listed here as a reference
	*/
	
protected:
	// The primaries for the color space
	Primaries primaries; 

	// The white point for the color space
	WhitePoint whitePoint;

	// Offsets are constant unsigned ints
	static const Offset offsets;

	// Transfer functions related to a color space
	// Some color spaces make use of more than 1.
	TransferFunction **transferFunctions;

	// Convert From Linear RGB to XYZ
	Matrix_3x3 ToXyzMatrix;

	// Convert From Xyz to Linear RGB
	Matrix_3x3 FromXyzMatrix;

	// Convert From Non-Linear RGB to YCbCr
	Matrix_3x3 ToYCbCrMatrix;

	// Convert From Non-Linear YCbCr to RGB
	Matrix_3x3 ToRgbMatrix;

	std::string Name;

public:
	ColorSpace() {};

	virtual void Initialize();

	// Conversion matrix to the CIEXYZ color space
	// NOT used for conversions; this is just a 
	// reference to generate the matrices
	Matrix_3x3 GenerateXyzConversionMatrix();

	// Generate RGB to YUV Matrix
	Matrix_3x3 GenerateRgbToYuvMatrix();

	Matrix_3x3 GetToXyzMatrix() { return this->ToXyzMatrix; }

	Matrix_3x3 GetFromXyzMatrix() { return this->FromXyzMatrix; }

	Matrix_3x3 GetToYCbCrMatrix() { return this->ToYCbCrMatrix; }
	
	Matrix_3x3 GetToRgbMatrix() { return this->ToRgbMatrix; }

	std::string GetName() { return this->Name; }

private:
	virtual void SetPrimaries() = 0;

	virtual void SetWhitePoint() = 0;

	virtual void SetTransferFunctions() = 0;

	virtual void SetXyzConversionMatrices() = 0;

	virtual void SetRgbYCbCrConversionMatrices() = 0;

	virtual void SetName() = 0;

protected:
	// Used for generating RGB <-> YCbCr matrices
	double *GenerateSrSgSb();

	// Generates Reference Chroma from primaries
	Matrix_3x3 GenerateReferenceChroma();
};

class BT601_525 : public ColorSpace
{
public:
	BT601_525() {};

private:
	virtual void SetPrimaries();

	virtual void SetWhitePoint();
		
	virtual void SetTransferFunctions();
	
	virtual void SetXyzConversionMatrices();
	
	virtual void SetRgbYCbCrConversionMatrices();

	virtual void SetName();
};

class BT601_NTSC_Legacy : public ColorSpace
{
public:
	BT601_NTSC_Legacy() {};

private:
	virtual void SetPrimaries();

	virtual void SetWhitePoint();

	virtual void SetTransferFunctions();

	virtual void SetXyzConversionMatrices();

	virtual void SetRgbYCbCrConversionMatrices();

	virtual void SetName();
};

class BT709 : public ColorSpace
{
public:
	BT709() {};

private:
	virtual void SetPrimaries();

	virtual void SetWhitePoint();

	virtual void SetTransferFunctions();

	virtual void SetXyzConversionMatrices();

	virtual void SetRgbYCbCrConversionMatrices();

	virtual void SetName();
};

class sRGB : public ColorSpace
{
public:
	sRGB() {};

private:
	virtual void SetPrimaries();

	virtual void SetWhitePoint();

	virtual void SetTransferFunctions();

	virtual void SetXyzConversionMatrices();

	virtual void SetRgbYCbCrConversionMatrices();

	virtual void SetName();
};


class BT2020 : public ColorSpace
{
public:
	BT2020() {};

private:
	virtual void SetPrimaries();

	virtual void SetWhitePoint();

	virtual void SetTransferFunctions();

	virtual void SetXyzConversionMatrices();

	virtual void SetRgbYCbCrConversionMatrices();

	virtual void SetName();
};

//class BT2020NCL : public ColorSpace
//{
//public:
//	BT2020NCL() {};
//
//private:
//	virtual void SetPrimaries();
//
//	virtual void SetWhitePoint();
//
//	virtual void SetTransferFunctions();
//
//	virtual void SetXyzConversionMatrices();
//
//	virtual void SetRgbYCbCrConversionMatrices();
//
//	virtual void SetName();
//};
#endif

#endif