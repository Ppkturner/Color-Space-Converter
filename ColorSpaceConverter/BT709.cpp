#pragma once

#include "ColorSpaceTriplet.h"

void BT709Primaries::Initialize()
{
	this->chromasticities =
	{
		0.640, // xr
		0.330, // yr
		1 - 0.640 - 0.33,  // zr

		0.300, // xg
		0.600, // yg
		1 - 0.300 - 0.600,  // zg

		0.150, // xb
		0.060, // yb
		1 - 0.150 - 0.060 // zb
	};

	// D65 White Point
	this->whitePoint =
	{
		0.3127,
		0.3290,
		0.3583
	};
}

std::string BT709Primaries::GetIdentifier()
{
	return "BT.709 Primaries";
}

BT709Primaries * BT709Primaries::Create() const
{
	return new BT709Primaries();
}

BT709Primaries * BT709Primaries::Clone() const
{
	return new BT709Primaries(*this);
}

void BT709Matrix::Initialize()
{
	double RgbToYCbCr[9] =
	{
		0.212639, 0.715169, 0.072192,
		-0.114592, -0.385408, 0.500000,
		0.500000, -0.454156, -0.045844
	};

	this->RgbToYCbCr = Matrix_3x3(RgbToYCbCr);

	double YCbCrToRgb[9] =
	{
		1.000000, -0.000000, 1.574722,
		1.000000, -0.187314, -0.468207,
		1.000000, 1.855615, 0.000000
	};

	this->YCbCrToRgb = Matrix_3x3(YCbCrToRgb);

	double RgbToXyz[9] =
	{
		0.412391, 0.357584, 0.180481,
		0.212639, 0.715169, 0.072192,
		0.019331, 0.119195, 0.950532
	};

	this->RgbToXyz = Matrix_3x3(RgbToXyz);

	double XyzToRgb[9] =
	{
		3.240970, -1.537383, -0.498611,
		-0.969244, 1.875968, 0.041555,
		0.055630, -0.203977, 1.056972
	};

	this->XyzToRgb = Matrix_3x3(XyzToRgb);
}

std::string BT709Matrix::GetIdentifier()
{
	return "BT.709 Matrices";
}

BT709Matrix * BT709Matrix::Create() const
{
	return new BT709Matrix();
}

BT709Matrix * BT709Matrix::Clone() const
{
	return new BT709Matrix(*this);
}

// As Defined in BT.709
// Can be used for BT.601, BT.709, and BT.2020 10-bit
double BT709Oetf(double component)
{
	double output;

	if(component < 0.018)// && component >= 0)
	{
		// V = 4.500 L
		output = 4.500 * component;
	}
	else // if (component >= 0.018 && component <= 1.0)
	{
		// V = 1.099 L^0.45 - 0.099
		output = 1.099 * pow(component, 0.45) - 0.099;
	}

	return output;
}

double BT709OetfInv(double component)
{
	double output = 0.0;

	if (component < 0.081) // && component >= 0
	{
		output = component / 4.500;
	}
	else
	{
		output = std::pow((component + 0.099) / 1.099, 1.0 / 0.45);
	}

	return output;
}

// The BT.709 EOTF function as defined in BT.1886
double BT709EotfBT1886(double component)
{
	// Lw here is 100 cd/m^2, Lb = 0.1 cd/m^2
	// double Lw = 100, Lb = 0.1;

	double gamma = 2.40;

	// double gammaInv = 1.0 / gamma;

	double a = 87.03105; // std::pow(std::pow(Lw, gammaInv) - std::pow(Lb, gammaInv), gamma);

	double b = 0.05958; // std::pow(Lb, gammaInv) / (std::pow(Lw, gammaInv) - std::pow(Lb, gammaInv));

	return (a * std::pow(max(component + b, 0), gamma)) / 100.0; // divide by max luminance to get values in range [0, 1]
}

void BT709TransferFunction::Initialize()
{
	this->EOTF = TransferFunction(BT709EotfBT1886); // use the BT.1886 defined EOTF instead of inversion OETF

	// I havent't seen this defined in BT.1886. This would effectively be the OETF for BT.709, as BT.709 does not define and OOTF or artistic augmentation
	this->InvEOTF = TransferFunction();

	this->OETF = TransferFunction(BT709Oetf);

	this->InvOETF = TransferFunction(BT709OetfInv);

	this->OOTF = this->InvOOTF = this->InvEOTF = TransferFunction(); // no defined OOTF for BT.709 Transfer functions
}

std::string BT709TransferFunction::GetIdentifier()
{
	return "BT.709 Transfer Characteristic";
}

BT709TransferFunction * BT709TransferFunction::Create() const
{
	return new BT709TransferFunction(*this);
}

BT709TransferFunction * BT709TransferFunction::Clone() const
{
	return new BT709TransferFunction(*this);
}

void BT709TransferFunction::Linearize(std::vector<double>& input)
{
	for (int i = 0; i < input.size(); i++)
	{
		input[i] = this->InvOETF.TF(input[i]);
	}
}

void BT709TransferFunction::ApplyTransferFunction(std::vector<double>& input)
{
	for (int i = 0; i < input.size(); i++)
	{
		input[i] = this->OETF.TF(input[i]);
	}
}