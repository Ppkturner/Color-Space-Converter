#include "ColorSpace.h"
#include "ColorSpaceTriplet.h"

#if 0
void ColorSpace::Initialize()
{
	SetPrimaries();

	SetWhitePoint();

	SetTransferFunctions();

	SetXyzConversionMatrices();

	SetRgbYCbCrConversionMatrices();
}

ColorSpace::~ColorSpace()
{
	if (this->primaries != nullptr)
		delete this->primaries;

	if (this->matrix != nullptr)
		delete this->matrix;
	
	if (this->transferFunction != nullptr)
		delete this->transferFunction;
}

Matrix_3x3 ColorSpace::GenerateXyzConversionMatrix()
{
	// Get Sr, Sg, and Sb
	double *S = GenerateSrSgSb();

	// Get Reference Chroma Matrix
	double *refChroma = this->GenerateReferenceChroma().matrix;

	/*
				| Xr * Sr, Xg * Sg, Xb * Sb |
			M =	| Yr * Sr, Yg * Sg, Yb * Sb |
				| Zr * Sr, Zg * Sg, Zb * Sb |
	*/

	double temp[9] =
	{
		refChroma[0] * S[0], refChroma[1] * S[1], refChroma[2] * S[2],
		refChroma[3] * S[0], refChroma[4] * S[1], refChroma[5] * S[2],
		refChroma[6] * S[0], refChroma[7] * S[1], refChroma[8] * S[2]
	};

	delete[] S;

	Matrix_3x3 M(temp);

	return M;
}

double *ColorSpace::GenerateSrSgSb()
{
	Matrix_3x3 referenceChromaInverse(GenerateReferenceChroma().Invert());

	double referenceWhite[3] =
	{
		this->whitePoint.xw / this->whitePoint.yw,
		1.0,
		this->whitePoint.zw / this->whitePoint.yw
	};

	double *SrSgSb = new double[3];

	std::vector<double> temp = referenceChromaInverse * referenceWhite;

	for (int ii = 0; ii < 3; ii++)
		SrSgSb[ii] = temp[ii];

	return SrSgSb;
}

Matrix_3x3 ColorSpace::GenerateReferenceChroma()
{
	// Get Reference Chroma Matrix
	// 
	//					   | Xr Xg Xb |
	//	Reference Chroma = | Yr Yg Yb |
	//					   | Zr Zg Zb |
	// Xr = xr / yr
	// Yr = 1
	// Zr = zr / yr = (1 - xr - yr) / yr
	// Etc. For the other values
	double refChroma[] =
	{
		this->primaries.xr / this->primaries.yr, this->primaries.xg / this->primaries.yg, this->primaries.xb / this->primaries.yb,
		1.0, 1.0, 1.0,
		this->primaries.zr / this->primaries.yr, this->primaries.zg / this->primaries.yg, this->primaries.zb / this->primaries.yb
	};

	return Matrix_3x3(refChroma);
}

Matrix_3x3 ColorSpace::GenerateRgbToYuvMatrix()
{
	// Generate Kr, Kg, and Kb
	double chromaMatrix[9] =
	{
		this->primaries.xr, this->primaries.xg, this->primaries.xb,
		this->primaries.yr, this->primaries.yg, this->primaries.yb,
		this->primaries.zr, this->primaries.zg, this->primaries.zb
	};

	Matrix_3x3 chromaMatrixInverse = Matrix_3x3(chromaMatrix).Invert();

	double referenceWhite[3] =
	{
		this->whitePoint.xw / this->whitePoint.yw,
		1.0,
		this->whitePoint.zw / this->whitePoint.yw
	};

	std::vector<double> KrKgKb = chromaMatrixInverse * referenceWhite;

	double Kr = KrKgKb[0] * this->primaries.yr;
	double Kg = KrKgKb[1] * this->primaries.yg;
	double Kb = KrKgKb[2] * this->primaries.yb;

	double RgbToYuv[9] =
	{
		Kr, Kg, Kb,
		(1.0 / 2.0) * (-1.0 * Kr / (1.0 - Kb)), (1.0 / 2.0) * (-1.0 * Kg / (1.0 - Kb)), 1.0 / 2.0,
		1.0 / 2.0, (1.0 / 2.0) * (-1.0 * Kg / (1.0 - Kr)), (1.0 / 2.0) * (-1.0 * Kb / (1.0 - Kr))
	};

	return Matrix_3x3(RgbToYuv);
}

void BT601_525::SetPrimaries()
{
	this->primaries.xr = 0.630;
	this->primaries.yr = 0.340;
	this->primaries.zr = 1 - this->primaries.xr - this->primaries.yr;

	this->primaries.xg = 0.310;
	this->primaries.yg = 0.595;
	this->primaries.zg = 1 - this->primaries.xg - this->primaries.yg;
	
	this->primaries.xb = 0.155;
	this->primaries.yb = 0.070;
	this->primaries.zb = 1 - this->primaries.xb - this->primaries.yb;
}

void BT601_525::SetWhitePoint()
{
	this->whitePoint.xw = 0.3127;
	this->whitePoint.yw = 0.3290;
	this->whitePoint.zw = 1 - this->whitePoint.xw - this->whitePoint.yw;
}

void BT601_525::SetTransferFunctions()
{
	return;
}

void BT601_525::SetXyzConversionMatrices()
{
	double temp[9] =
	{
		0.393521, 0.365258, 0.191677,
		0.212376, 0.701060, 0.086564,
		0.018739, 0.111934, 0.958385
	};

	this->ToXyzMatrix = Matrix_3x3(temp);

	double tempInv[9] =
	{
		3.506003, -1.739791, -0.544058,
		-1.069048, 1.977779, 0.035171,
		0.056307, -0.196976, 1.049952
	};

	this->FromXyzMatrix = Matrix_3x3(tempInv);
}

void BT601_525::SetRgbYCbCrConversionMatrices()
{
	double temp[9] =
	{
		0.212376, 0.701060, 0.086564,
		-0.116251, -0.383749, 0.500000,
		0.500000, -0.445047, -0.054953
	};

	this->ToYCbCrMatrix = Matrix_3x3(temp);

	double tempInv[9] = 
	{
		1.000000, -0.000000, 1.575247,
		1.000000, -0.225574, -0.477199,
		1.000000, 1.826872, 0.000000 
	};

	this->ToRgbMatrix = Matrix_3x3(tempInv);
}

void BT601_525::SetName()
{
	this->Name = "BT.601, 525-Line";
}

void BT709::SetPrimaries()
{
	this->primaries.xr = 0.640;
	this->primaries.yr = 0.330;
	this->primaries.zr = 1 - this->primaries.xr - this->primaries.yr;

	this->primaries.xg = 0.300;
	this->primaries.yg = 0.600;
	this->primaries.zg = 1 - this->primaries.xg - this->primaries.yg;

	this->primaries.xb = 0.150;
	this->primaries.yb = 0.060;
	this->primaries.zb = 1 - this->primaries.xb - this->primaries.yb;
}

void BT709::SetWhitePoint()
{
	this->whitePoint.xw = 0.3127;
	this->whitePoint.yw = 0.3290;
	this->whitePoint.zw = 1 - this->whitePoint.xw - this->whitePoint.yw;
}

void BT709::SetTransferFunctions()
{
	return;
}

void BT709::SetXyzConversionMatrices()
{
	double temp[9] = 
	{
		 0.412391, 0.357584, 0.180481,
		 0.212639, 0.715169, 0.072192,
		 0.019331, 0.119195, 0.950532
	};

	this->ToXyzMatrix = Matrix_3x3(temp);

	double tempInv[9] =
	{
		3.240970, -1.537383, -0.498611,
		-0.969244, 1.875968, 0.041555,
		0.055630, -0.203977, 1.056972
	};

	this->FromXyzMatrix = Matrix_3x3(tempInv);

	return;
}

void BT709::SetRgbYCbCrConversionMatrices()
{
	double temp[9] =
	{
		0.212639, 0.715169, 0.072192,
		-0.114592, -0.385408, 0.500000,
		0.500000, -0.454156, -0.045844
	};

	this->ToYCbCrMatrix = Matrix_3x3(temp);

	double tempInv[9] =
	{
		 1.000000, -0.000000, 1.574722,
		 1.000000, -0.187314, -0.468207,
		 1.000000, 1.855615, 0.000000
	};

	this->ToRgbMatrix = Matrix_3x3(tempInv);

	return;
}

void BT709::SetName()
{
	this->Name = "BT.709";
}

void sRGB::SetPrimaries()
{
	this->primaries.xr = 0.6400;
	this->primaries.yr = 0.3300;
	this->primaries.zr = 1 - this->primaries.xr - this->primaries.yr;

	this->primaries.xg = 0.3000;
	this->primaries.yg = 0.6000;
	this->primaries.zg = 1 - this->primaries.xg - this->primaries.yg;

	this->primaries.xb = 0.1500;
	this->primaries.yb = 0.0600;
	this->primaries.zb = 1 - this->primaries.xb - this->primaries.yb;
}

void sRGB::SetWhitePoint()
{
	// CIE Illuminant D65
	this->whitePoint.xw = 0.3127;
	this->whitePoint.yw = 0.3290;
	this->whitePoint.zw = 1 - this->whitePoint.xw - this->whitePoint.yw;
}

void sRGB::SetTransferFunctions()
{
	return;
}

void sRGB::SetXyzConversionMatrices()
{
	return;
}

void sRGB::SetRgbYCbCrConversionMatrices()
{
	return;
}

void sRGB::SetName()
{
	this->Name = "sRGB";
}

void BT601_NTSC_Legacy::SetPrimaries()
{
	this->primaries.xr = 0.67;
	this->primaries.yr = 0.33;
	this->primaries.zr = 1 - this->primaries.xr - this->primaries.yr;

	this->primaries.xg = 0.21;
	this->primaries.yg = 0.71;
	this->primaries.zg = 1 - this->primaries.xg - this->primaries.yg;

	this->primaries.xb = 0.14;
	this->primaries.yb = 0.08;
	this->primaries.zb = 1 - this->primaries.xb - this->primaries.yb;
}

void BT601_NTSC_Legacy::SetWhitePoint()
{
	// CIE Standard Illuminant C, 6774K
	this->whitePoint.xw = 0.310;
	this->whitePoint.yw = 0.316;
	this->whitePoint.zw = 1 - this->whitePoint.xw - this->whitePoint.yw;
}

void BT601_NTSC_Legacy::SetTransferFunctions()
{
	return;
}

void BT601_NTSC_Legacy::SetXyzConversionMatrices()
{
	double temp[9] = 
	{
		0.606993, 0.173449, 0.200571,
		0.298967, 0.586421, 0.114612,
		-0.000000, 0.066076, 1.117469
	};

	this->ToXyzMatrix = Matrix_3x3(temp);

	double tempInv[9] = 
	{
		1.909675, -0.532365, -0.288161,
		-0.984965, 1.999777, -0.028317,
		0.058241, -0.118246, 0.896554
	};

	this->FromXyzMatrix = Matrix_3x3(tempInv);
}

void BT601_NTSC_Legacy::SetRgbYCbCrConversionMatrices()
{
	double temp[9] = 
	{
		0.298967, 0.586421, 0.114612,
		-0.168834, -0.331166, 0.500000,
		0.500000, -0.418255, -0.081745
	};

	this->ToYCbCrMatrix = Matrix_3x3(temp);

	double tempInv[9] = 
	{
		1.000000, -0.000000, 1.402067,
		1.000000, -0.346086, -0.714795,
		1.000000, 1.770776, 0.000000
	};

	this->ToRgbMatrix = Matrix_3x3(tempInv);
}

void BT601_NTSC_Legacy::SetName()
{
	this->Name = "BT.601 NTSC (1953)";
}

void BT2020::SetPrimaries()
{
	this->primaries.xr = 0.708;
	this->primaries.yr = 0.292;
	this->primaries.zr = 1 - this->primaries.xr - this->primaries.yr;

	this->primaries.xg = 0.170;
	this->primaries.yg = 0.797;
	this->primaries.zg = 1 - this->primaries.xg - this->primaries.yg;

	this->primaries.xb = 0.131;
	this->primaries.yb = 0.046;
	this->primaries.zb = 1 - this->primaries.xb - this->primaries.yb;
}

void BT2020::SetWhitePoint()
{
	this->whitePoint.xw = 0.3127;
	this->whitePoint.yw = 0.3290;
	this->whitePoint.zw = 1 - this->whitePoint.xw - this->whitePoint.yw;
}

void BT2020::SetTransferFunctions()
{
	return;
}

void BT2020::SetXyzConversionMatrices()
{
	double temp[9] =
	{
		0.636958, 0.144617, 0.168881,
		0.262700, 0.677998, 0.059302,
		0.000000, 0.028073, 1.060985
	};

	this->ToXyzMatrix = Matrix_3x3(temp);

	double tempInv[9] =
	{
		1.716651, -0.355671, -0.253366,
		-0.666684, 1.616481, 0.015769,
		0.017640, -0.042771, 0.942103
	};

	this->FromXyzMatrix = Matrix_3x3(tempInv);
}

void BT2020::SetRgbYCbCrConversionMatrices()
{
	double temp[9] = 
	{
		0.262700, 0.677998, 0.059302,
		-0.139630, -0.360370, 0.500000,
		0.500000, -0.459785, -0.040215
	};

	this->ToYCbCrMatrix = Matrix_3x3(temp);

	double tempInv[9] = 
	{
		1.000000, -0.000000, 1.474600,
		1.000000, -0.164558, -0.571355,
		1.000000, 1.881397, -0.000000
	};

	this->ToRgbMatrix = Matrix_3x3(tempInv);
}

void BT2020::SetName()
{
	this->Name = "BT.2020";
}

// Transfer Functions

// This transfer function is used for BT.601, BT.709, and BT.2020 (in part)
double BT601TransferFunction::transferFunction(const double & component)
{
	double output;

	if (component < 0.018)
	{
		output = 4.500 *  component;
	}
	else // if (component >= 0.018)
	{
		output = 1.099 * pow(component, 0.45) - 0.099;
	}

	return output;
}

double BT2020TransferFunction::transferFunction(const double & component)
{
	double output;

	if (component < 0.0181)
	{
		output = 4.5 * component;
	}
	else // if (component >= 0.0181)
	{
		output = 1.0993 * pow(component, 0.45) - (1.0993 - 1);
	}

	return output;
}

double PqTransferFunction::transferFunction(const double & component)
{
	double output;

	double c1 = 0.8359375,
		c2 = 18.8515625,
		c3 = 18.6875,
		m = 78.84375,
		n = 0.1593017578125;

	output = pow((c1 + c2 * pow(component, n)) / (1 + c3 * pow(component, n)), m);

	return output;
}

double HlgTransferFunction::transferFunction(const double & component)
{
	double boundary = 1.0 / 12.0;

	double a = 0.17883277;

	double b = 1 - 4 * a;

	double c = 0.5 - a * log(4 * a);

	double output;

	if (component <= boundary)
	{
		output = pow(3 * component, 0.5);
	}
	else // if (component > boundary)
	{
		output = a * log(12 * component - b) + c;
	}

	return output;
}
#endif