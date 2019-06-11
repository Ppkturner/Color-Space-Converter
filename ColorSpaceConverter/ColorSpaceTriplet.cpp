#include "ColorSpaceTriplet.h"

Chromasticities ColorPrimaries::GetChromasticities()
{
	return this->chromasticities;
}

WhitePoint ColorPrimaries::GetWhitePoint()
{
	return this->whitePoint;
}

bool operator==(ContentLightLevels & l1, ContentLightLevels & l2)
{
	return (l1.LB == l2.LB) && (l1.LW == l2.LW);
}

bool operator!=(ContentLightLevels & l1, ContentLightLevels & l2)
{
	return !(l1 == l2);
}

bool operator==(ColorPrimaries & p1, ColorPrimaries & p2)
{
	return p1.GetIdentifier() == p2.GetIdentifier();
}

bool operator!=(ColorPrimaries & p1, ColorPrimaries & p2)
{
	return !(p1 == p2);
}

bool operator==(TransferCharacteristic & t1, TransferCharacteristic & t2)
{
	return t1.GetIdentifier() == t2.GetIdentifier();
}

bool operator!=(TransferCharacteristic & t1, TransferCharacteristic & t2)
{
	return !(t1 == t2);
}

bool operator==(ColorMatrix & m1, ColorMatrix & m2)
{
	return m1.GetIdentifier() == m2.GetIdentifier();
}

bool operator!=(ColorMatrix & m1, ColorMatrix & m2)
{
	return !(m1 == m2);
}

//void swap(ColorSpaceTriplet & first, ColorSpaceTriplet & second)
//{
//	using std::swap;
//
//	swap(*first.primaries, *second.primaries);
//
//	swap(*first.matrix, *second.matrix);
//
//	swap(*first.transferCharacteristic, *second.transferCharacteristic);
//
//	swap(first.range, second.range);
//}

Matrix_3x3 ColorMatrix::GetRgbToYCbCr()
{
	return this->RgbToYCbCr;
}

Matrix_3x3 ColorMatrix::GetYCbCrToRgb()
{
	return this->YCbCrToRgb;
}

Matrix_3x3 ColorMatrix::GetRgbToXyz()
{
	return this->RgbToXyz;
}

Matrix_3x3 ColorMatrix::GetXyzToRgb()
{
	return this->XyzToRgb;
}

double TransferFunction::TF(double component)
{
	if (this->tf)
	{
		// Force incoming values to be within the range [0.0, 1.0]
		//Clamp(&component);

		// Call the corresponding transfer function
		return this->tf(component);
	}
}

void ColorSpaceTriplet::Initialize()
{
	if(this->primaries != nullptr) 
		this->primaries->Initialize();
	
	if(this->matrix != nullptr) 
		this->matrix->Initialize();
	
	if(this->transferCharacteristic != nullptr)
		this->transferCharacteristic->Initialize();
}

Matrix_3x3 ColorSpaceTriplet::GenerateRgbYCbCrMatrices()
{
	Chromasticities chroma = this->primaries->GetChromasticities();

	WhitePoint white = this->primaries->GetWhitePoint();

	double chromaMatrix[9] =
	{
		chroma.xr, chroma.xg, chroma.xb,
		chroma.yr, chroma.yg, chroma.yb,
		chroma.zr, chroma.zg, chroma.zb
	};

	Matrix_3x3 chromaMatrixInverse = Matrix_3x3(chromaMatrix).Invert();

	double referenceWhite[3] =
	{
		white.xw / white.yw,
		1.0,
		white.zw / white.yw
	};

	std::vector<double> KrKgKb = chromaMatrixInverse * referenceWhite;

	double Kr = KrKgKb[0] * chroma.yr;
	double Kg = KrKgKb[1] * chroma.yg;
	double Kb = KrKgKb[2] * chroma.yb;

	double RgbToYCbCr[9] =
	{
		Kr, Kg, Kb,
		(1.0 / 2.0) * (-1.0 * Kr / (1.0 - Kb)), (1.0 / 2.0) * (-1.0 * Kg / (1.0 - Kb)), 1.0 / 2.0,
		1.0 / 2.0, (1.0 / 2.0) * (-1.0 * Kg / (1.0 - Kr)), (1.0 / 2.0) * (-1.0 * Kb / (1.0 - Kr))
	};

	return Matrix_3x3(RgbToYCbCr);
}

Matrix_3x3 ColorSpaceTriplet::GenerateRgbXyzMatrices()
{
	// Get Sr, Sg, and Sb
	double S[3];
	
	GenerateSrSgSb(S);

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

	Matrix_3x3 M(temp);

	return M;
}

void ColorSpaceTriplet::GenerateSrSgSb(double *SrSgSb)
{
	Matrix_3x3 referenceChromaInverse(GenerateReferenceChroma().Invert());

	WhitePoint white = this->primaries->GetWhitePoint();

	double referenceWhite[3] =
	{
		white.xw / white.yw,
		1.0,
		white.zw / white.yw
	};

	std::vector<double> temp = referenceChromaInverse * referenceWhite;

	for (int ii = 0; ii < 3; ii++)
		SrSgSb[ii] = temp[ii];
}

Matrix_3x3 ColorSpaceTriplet::GenerateReferenceChroma()
{
	Chromasticities chroma = this->primaries->GetChromasticities();

	double refChroma[] =
	{
		chroma.xr / chroma.yr, chroma.xg / chroma.yg, chroma.xb / chroma.yb,
		1.0, 1.0, 1.0,
		chroma.zr / chroma.yr, chroma.zg / chroma.yg, chroma.zb / chroma.yb
	};

	return Matrix_3x3(refChroma);
}

void ColorSpaceTriplet::SetDisplayLuminanceLevels(double LW, double LB)
{
	this->lightLevels = { LW, LB };
}

ColorSpaceTriplet::~ColorSpaceTriplet()
{
	if (this->primaries != nullptr)
	{
		delete this->primaries;

		this->primaries = nullptr;
	}

	if (this->matrix != nullptr)
	{
		delete this->matrix;

		this->matrix = nullptr;
	}

	if (this->transferCharacteristic != nullptr)
	{
		delete this->transferCharacteristic;

		this->transferCharacteristic = nullptr;
	}
}

ColorSpaceTriplet::ColorSpaceTriplet(const ColorSpaceTriplet & other) :
	range(range)
{
	*primaries = *other.primaries;

	*matrix = *other.matrix;

	*transferCharacteristic = *other.transferCharacteristic;

	range = other.range;
}

ColorSpaceTriplet & ColorSpaceTriplet::operator=(ColorSpaceTriplet & other)
{
	this->primaries = other.primaries->Clone();

	this->matrix = other.matrix->Clone();

	this->transferCharacteristic = other.transferCharacteristic->Clone();

	this->range = other.range;

	return *this;
}

void ToneMapping::CalculateKneeStart()
{
	this->KS = 1.5 * this->lightLevels.LW - 0.5;
}

double ToneMapping::P(double luminance)
{
	double Tcubic = pow(T(luminance), 3);
	double Tsquare = pow(T(luminance), 2);

	return (2 * Tcubic - 3 * Tsquare + 1) * this->KS
		+ (Tcubic - 2 * Tsquare + T(luminance)) * (1 - this->KS)
		+ (-2 * Tcubic + 2 * Tsquare) * this->lightLevels.LW;
}

double ToneMapping::T(double luminance)
{
	return (luminance - this->KS) / (1 - this->KS);
}

void ToneMapping::ApplyToneMapping(double & luminance)
{
	double E2 = luminance;

	if(luminance >= this->KS)
	{
		E2 = P(luminance);
	}

	luminance = E2 + this->lightLevels.LB * pow(1 - E2, 4);
}
