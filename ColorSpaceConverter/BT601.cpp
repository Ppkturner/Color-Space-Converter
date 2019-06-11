#pragma once

#include "ColorSpaceTriplet.h"

void BT601Primaries::Initialize()
{
	this->chromasticities =
	{
		0.67, // xr
		0.33, // yr
		1 - 0.67 - 0.33, // zr = 1 - xr - yr
		0.21, // xg
		0.71, // yg
		1 - 0.21 - 0.71, // zg
		0.14,// xb
		0.08,// yb
		1 - 0.14 - 0.08 // zb
	};

	this->whitePoint =
	{
		0.3127,
		0.3290,
		1 - 0.3127 - 0.3290
	};
}

std::string BT601Primaries::GetIdentifier()
{
	return "BT.601 Primaries";
}

BT601Primaries * BT601Primaries::Create() const
{
	return new BT601Primaries();
}

BT601Primaries * BT601Primaries::Clone() const
{
	return new BT601Primaries(*this);
}

void BT601625Primaries::Initialize()
{
	this->chromasticities =
	{
		0.640, // xr
		0.330, // yr
		1 - 0.640 - 0.330, // zr = 1 - xr - yr
		0.290, // xg
		0.600, // yg
		1 - 0.290 - 0.600, // zg
		0.150, // xb
		0.060, // yb
		1 - 0.150 - 0.060 // zb
	};

	this->whitePoint =
	{
		0.3127, // xw
		0.3290, // yw
		1 - 0.3127 - 0.3290 // zw
	};
}

std::string BT601625Primaries::GetIdentifier()
{
	return "BT.601 625 Primaries";;
}

BT601625Primaries * BT601625Primaries::Create() const
{
	return new BT601625Primaries();
}

BT601625Primaries * BT601625Primaries::Clone() const
{
	return new BT601625Primaries(*this);
}

void BT601525Primaries::Initialize()
{
	this->chromasticities =
	{
		0.630, // xr
		0.340, // yr
		1 - 0.630 - 0.340, // zr = 1 - xr - yr
		0.310, // xg
		0.595, // yg
		1 - 0.310 - 0.595, // zg
		0.155, // xb
		0.070, // yb
		1 - 0.155 - 0.070 // zb
	};

	this->whitePoint =
	{
		0.3127, // xw
		0.3290, // yw
		1 - 0.3127 - 0.3290 // zw
	};
}

std::string BT601525Primaries::GetIdentifier()
{
	return "BT.601 525 Primaries";
}

BT601525Primaries * BT601525Primaries::Create() const
{
	return new BT601525Primaries();
}

BT601525Primaries * BT601525Primaries::Clone() const
{
	return new BT601525Primaries(*this);
}

void BT601Matrix::Initialize()
{
	double RgbToYCbCr[9] =
	{
		0.298967, 0.586421, 0.114612,
		-0.168834, -0.331166, 0.500000,
		0.500000, -0.418255, -0.081745
	};

	this->RgbToYCbCr = Matrix_3x3(RgbToYCbCr);

	double YCbCrToRgb[9] =
	{
		1.000000, -0.000000, 1.402067,
		1.000000, -0.346086, -0.714795,
		1.000000, 1.770776, 0.000000
	};

	this->YCbCrToRgb = Matrix_3x3(YCbCrToRgb);

	double RgbToXyz[9] =
	{
		0.588099, 0.179133, 0.183223,
		0.289661, 0.605640, 0.104699,
		-0.000000, 0.068241, 1.020817
	};

	this->RgbToXyz = Matrix_3x3(RgbToXyz);

	double XyzToRgb[9] =
	{
		1.971026, -0.549468, -0.297418,
		-0.953709, 1.936318, -0.027418,
		0.063755, -0.129442, 0.981441
	};

	this->XyzToRgb = Matrix_3x3(XyzToRgb);
}

std::string BT601Matrix::GetIdentifier()
{
	return "BT.601 Matrices";
}

BT601Matrix * BT601Matrix::Create() const
{
	return new BT601Matrix();
}

BT601Matrix * BT601Matrix::Clone() const
{
	return new BT601Matrix(*this);
}

void BT601625Matrix::Initialize()
{
	double RgbToYCbCr[9] =
	{
		0.298967, 0.586421, 0.114612,
		-0.168834, -0.331166, 0.500000,
		0.500000, -0.418255, -0.081745
	};

	this->RgbToYCbCr = Matrix_3x3(RgbToYCbCr);

	double YCbCrToRgb[9] =
	{
		1.000000, -0.000000, 1.402067,
		1.000000, -0.346086, -0.714795,
		1.000000, 1.770776, 0.000000
	};

	this->YCbCrToRgb = Matrix_3x3(YCbCrToRgb);

	double RgbToXyz[9] =
	{
		0.430554, 0.341550, 0.178352,
		0.222004, 0.706655, 0.071341,
		0.020182, 0.129553, 0.939322
	};

	this->RgbToXyz = Matrix_3x3(RgbToXyz);

	double XyzToRgb[9] =
	{
		3.063361, -1.393390, -0.475824,
		-0.969244, 1.875968, 0.041555,
		0.067861, -0.228799, 1.069090
	};

	this->XyzToRgb = Matrix_3x3(XyzToRgb);
}

std::string BT601625Matrix::GetIdentifier()
{
	return "BT.601 625 Matrices";
}

BT601625Matrix * BT601625Matrix::Create() const
{
	return new BT601625Matrix();
}

BT601625Matrix * BT601625Matrix::Clone() const
{
	return new BT601625Matrix(*this);
}

void BT601525Matrix::Initialize()
{
	double RgbToYCbCr[9] =
	{
		0.298967, 0.586421, 0.114612,
		-0.168834, -0.331166, 0.500000,
		0.500000, -0.418255, -0.081745
	};

	this->RgbToYCbCr = Matrix_3x3(RgbToYCbCr);

	double YCbCrToRgb[9] =
	{
		1.000000, -0.000000, 1.402067,
		1.000000, -0.346086, -0.714795,
		1.000000, 1.770776, 0.000000
	};

	this->YCbCrToRgb = Matrix_3x3(YCbCrToRgb);

	double RgbToXyz[9] =
	{
		0.393521, 0.365258, 0.191677,
		0.212376, 0.701060, 0.086564,
		0.018739, 0.111934, 0.958385
	};

	this->RgbToXyz = Matrix_3x3(RgbToXyz);

	double XyzToRgb[9] =
	{
		3.506003, -1.739791, -0.544058,
		-1.069048, 1.977779, 0.035171,
		0.056307, -0.196976, 1.049952
	};
}

std::string BT601525Matrix::GetIdentifier()
{
	return "BT.601 525 Matrices";
}

BT601525Matrix * BT601525Matrix::Create() const
{
	return new BT601525Matrix();
}

BT601525Matrix * BT601525Matrix::Clone() const
{
	return new BT601525Matrix(*this);
}

void BT601TransferFunction::Initialize()
{
	this->EOTF = TransferFunction(); // just use Inverse OETF

	this->InvEOTF = TransferFunction();

	this->OETF = TransferFunction(BT709Oetf);

	this->InvOETF = TransferFunction(BT709OetfInv);

	this->OOTF = this->InvOOTF = TransferFunction(); // No OOTF for BT.601 is defined as of now.
}

std::string BT601TransferFunction::GetIdentifier()
{
	return "BT.601 Transfer Characteristic";
}

BT601TransferFunction * BT601TransferFunction::Create() const
{
	return new BT601TransferFunction();
}

BT601TransferFunction * BT601TransferFunction::Clone() const
{
	return new BT601TransferFunction(*this);
}

void BT601TransferFunction::Linearize(std::vector<double>& input)
{
	for (int i = 0; i < input.size(); i++)
	{
		input[i] = this->InvOETF.TF(input[i]);
	}
}

void BT601TransferFunction::ApplyTransferFunction(std::vector<double>& input)
{
	for (int i = 0; i < input.size(); i++)
	{
		input[i] = this->OETF.TF(input[i]);
	}
}
