#include "ColorSpaceTriplet.h"

void BT2020Primaries::Initialize()
{
	this->chromasticities =
	{
		0.708, // xr
		0.292, // yr
		1 - 0.708 - 0.292, // zr
		0.170, // xg
		0.797, // yg
		1 - 0.170 - 0.797, // zg
		0.131, // xb
		0.046, // yb
		1 - 0.131 - 0.046 // zb
	};

	this->whitePoint =
	{
		0.3127,
		0.3290,
		1 - 0.3127 - 0.3290
	};
}

std::string BT2020Primaries::GetIdentifier()
{
	return "BT.2020 Primaries";
}

BT2020Primaries * BT2020Primaries::Create() const
{
	return new BT2020Primaries();
}

BT2020Primaries * BT2020Primaries::Clone() const
{
	return new BT2020Primaries(*this);
}

void BT2020Matrix::Initialize()
{
	double RgbToYCbCr[9] =
	{
		0.262700, 0.677998, 0.059302,
		-0.139630, -0.360370, 0.500000,
		0.500000, -0.459785, -0.040215
	};

	this->RgbToYCbCr = Matrix_3x3(RgbToYCbCr);

	double YCbCrToRgb[9] =
	{
		1.000000, -0.000000, 1.474600,
		1.000000, -0.164558, -0.571355,
		1.000000, 1.881397, -0.000000
	};

	this->YCbCrToRgb = Matrix_3x3(YCbCrToRgb);

	double RgbToXyz[9] =
	{
		0.636958, 0.144617, 0.168881,
		0.262700, 0.677998, 0.059302,
		0.000000, 0.028073, 1.060985
	};

	this->RgbToXyz = Matrix_3x3(RgbToXyz);

	double XyzToRgb[9] =
	{
		1.716651, -0.355671, -0.253366,
		-0.666684, 1.616481, 0.015769,
		0.017640, -0.042771, 0.942103
	};

	this->XyzToRgb = Matrix_3x3(XyzToRgb);
}

std::string BT2020Matrix::GetIdentifier()
{
	return "BT.2020 Matrices";
}

BT2020Matrix * BT2020Matrix::Create() const
{
	return new BT2020Matrix();
}

BT2020Matrix * BT2020Matrix::Clone() const
{
	return new BT2020Matrix(*this);
}

double BT2020Oetf12Bit(double component)
{
	double output;

	// For 12-bit, a = 1.0993, b = 0.0181
	// For 10-bit, a= 1.099, b = 0.018 (Same as BT.709)

	if (component < 0.0181 && component >= 0.0)
	{
		// V = 4.500 L
		output = 4.500 * component;
	}
	else // if (component >= 0.0181 && component <= 1.0)
	{
		// V = a * E^0.45 - (a - 1), b <= E <= 1
		output = 1.0993 * pow(component, 0.45) - 0.0993;
	}

	return output;
}
