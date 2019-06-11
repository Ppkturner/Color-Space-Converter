#pragma once
#ifndef COLORSPACETRIPLET_H
#define COLORSPACETRIPLET_H

#include "CommonIncludes.h"
#include "Matrix_3x3.h"

#pragma region Chromasticities, Gamut, Content Light Levels, and White Point

typedef enum GAMUT
{
	None		= -1,
	StudioRange = 0,
	FullRange	= 1
} Gamut;

struct Chromasticities
{
	double
		xr, yr, zr,
		xg, yg, zg,
		xb, yb, zb;
};

struct WhitePoint
{
	double xw, yw, zw;
};

struct ContentLightLevels
{
	double LW, LB;
};

bool operator==(ContentLightLevels &l1, ContentLightLevels &l2);

bool operator!=(ContentLightLevels &l1, ContentLightLevels &l2);

#pragma endregion The Chromasticities, Gamut, Conetent Light Levels, and White Point associated with a color space

#pragma region TransferFunction

class TransferFunction
{
private:
	std::function<double(double)> tf;

	static double DefaultTF(double component) { return component; }

public:

	virtual double TF(double component);

	TransferFunction(std::function<double(double)> f) : tf(f) {};

	TransferFunction() : tf(TransferFunction::DefaultTF) {};

	// clamps values to the range [0.0, 1.0];
	static void Clamp(double *component) { *component = min(max(*component, 0.0), 1.0); }
};

#pragma endregion A wrapper class for containing a transfer function

#pragma region Color Space Factory

class ColorSpaceFactory
{
protected:
	virtual ColorSpaceFactory *Create() const = 0;

	virtual ColorSpaceFactory *Clone() const = 0;

	virtual std::string GetIdentifier() = 0;;

	virtual void Initialize() = 0;
};

#pragma endregion Represents the Factory interface for all color space triplet elements

#pragma region ColorPrimaries

class ColorPrimaries : public ColorSpaceFactory
{
protected:
	Chromasticities chromasticities;
	
	WhitePoint whitePoint;

public:
	virtual ColorPrimaries *Create() const = 0;

	virtual ColorPrimaries *Clone() const = 0;

	virtual std::string GetIdentifier() = 0;

	virtual void Initialize() = 0;

	Chromasticities GetChromasticities();

	WhitePoint GetWhitePoint();
};

bool operator==(ColorPrimaries &p1, ColorPrimaries &p2);

bool operator!=(ColorPrimaries &p1, ColorPrimaries &p2);

#pragma endregion Wrapper class for the Chromasticity and White Point of a Color Space

#pragma region TransferCharacteristic

/*
	Reference EOTF is given by the following equation (from BT.1886) for HDTV studio production:
		L = a(max[(V + b), 0])^gamma
	where:
		L:		screen luminance in cd/m^2
		
		LW:		screen luminance for white
		
		LB:		screen luminance for black
		
		V:		Input video signal level (normalized, black at V = 0, to white at V = 1. For 
				content mastered per Rec ITU BT.709, 10-bit digital code values "D" map into 
				values of V per the following equation: V = (D - 64) / 876
		
		gamma:	Exponent of power function, gamma = 2.40
		
		a:		Variable for user gain (legacy "contrast" control)
					
					a = (LW^(1 / gamma) - (LB^(1 / gamma))^gamma
		
		b:		Variable for user black level lift (legacy "brightness" control)
					
					b = LB^(1 / gamma) / (LW^(1 / gamma) - LB^(1 / gamma))
	
	Variables a and b are calculated from solving the folowing equations in order that V = 1
	gives L = LW and V = 0 gives L = LB:
					
					LB = a * b^gamma
					LW = a * (1 + b)^gamma
*/
class TransferCharacteristic : public ColorSpaceFactory
{
protected:
	TransferFunction EOTF;

	TransferFunction InvEOTF;
	
	TransferFunction OETF;
	
	TransferFunction InvOETF;

	TransferFunction OOTF;

	TransferFunction InvOOTF;

public:
	virtual TransferCharacteristic *Create() const = 0;

	virtual TransferCharacteristic *Clone() const = 0;

	virtual std::string GetIdentifier() = 0;

	virtual void Initialize() = 0;

	virtual void Linearize(std::vector<double> &input) = 0;

	virtual void ApplyTransferFunction(std::vector<double> &input) = 0;
};

bool operator==(TransferCharacteristic &t1, TransferCharacteristic &t2);

bool operator!=(TransferCharacteristic &t1, TransferCharacteristic &t2);

#pragma endregion Wrapper Class for all Transfer Functions defined in a color space

#pragma region ColorMatrix

class ColorMatrix : public ColorSpaceFactory
{
protected:
	Matrix_3x3 RgbToYCbCr;

	Matrix_3x3 YCbCrToRgb;

	Matrix_3x3 RgbToXyz;

	Matrix_3x3 XyzToRgb;

public:
	virtual void Initialize() = 0;

	virtual std::string GetIdentifier() = 0;

	virtual ColorMatrix *Create() const = 0;

	virtual ColorMatrix *Clone() const = 0;

	Matrix_3x3 GetRgbToYCbCr();

	Matrix_3x3 GetYCbCrToRgb();

	Matrix_3x3 GetRgbToXyz();

	Matrix_3x3 GetXyzToRgb();
};

bool operator==(ColorMatrix &m1, ColorMatrix &m2);

bool operator!=(ColorMatrix &m1, ColorMatrix &m2);

#pragma endregion Wrapper Class for Matrices specific to a Color Space

#pragma region Tone Mapping

/*
	Tone Mapping involves the application of an Electro-Electronic Transfer Function (EETF)
	to convert between different ranges of Linear Light (EX. BT.709 nominal range of 0-100 nits (cd/m^2)
	to HLG 0.005 to 1000 nits;

	This is independent of the OOTF, which accounts for artistic intent and render intent for displays.
	Additionally, this step is not always applied; it only should be done when converting between color
	spaces of different absolute luminance ranges, otherwise it is defunct.
*/

class ToneMapping
{
private:
	double KS;

	// Target min and max absolute luminances
	ContentLightLevels lightLevels;

private:
	void CalculateKneeStart();

	// The Hermite Spline
	double P(double luminance);

	// Util function for the Hermite Spline
	double T(double luminance);

public:
	ToneMapping(double minLum, double maxLum) : lightLevels({ minLum, maxLum }) { CalculateKneeStart(); }

	// Apply tone mapping to the input absolute luminance
	void ApplyToneMapping(double &luminance);
};

#pragma endregion Represents a class for mapping luminance levels between Color Spaces

#pragma region Color Space Triplet

class ColorSpaceTriplet
{
public:
	ColorPrimaries *primaries;

	ColorMatrix *matrix;

	TransferCharacteristic *transferCharacteristic;

	ContentLightLevels lightLevels;

	Gamut range;

public:
	// Default Constructor
	ColorSpaceTriplet() : primaries(nullptr), matrix(nullptr), transferCharacteristic(nullptr), range(Gamut::None) { this->lightLevels = { -1.0, -1.0 }; }

	// Copy Constructor
	ColorSpaceTriplet(const ColorSpaceTriplet &other);

	// copy-swap
	// friend void swap(ColorSpaceTriplet &first, ColorSpaceTriplet &second);
	
	// Assignment Operator
	ColorSpaceTriplet& operator=(ColorSpaceTriplet &other);
	
	// Move Constructor
	ColorSpaceTriplet(ColorSpaceTriplet &&other);

	// Destructor
	virtual ~ColorSpaceTriplet();

	void Initialize();

	Matrix_3x3 GenerateRgbYCbCrMatrices();

	Matrix_3x3 GenerateRgbXyzMatrices();

	void GenerateSrSgSb(double *SrSgSb);

	Matrix_3x3 GenerateReferenceChroma();

	void SetDisplayLuminanceLevels(double LW, double LB);
};

#pragma endregion Represents a Color Space

#pragma region BT709
// BT.709 System
class BT709Primaries : public ColorPrimaries
{
public:
	virtual void Initialize();

	virtual std::string GetIdentifier();

	virtual BT709Primaries *Create() const;

	virtual BT709Primaries *Clone() const;
};

// This can be used for BT.709, BT.601, and BT.2020 10-bit systems
double BT709Oetf (double component);

// Inverse OETF can be used as EOTF for BT.709
double BT709OetfInv (double component);

// EOTF as defined in BT.1886
double BT709EotfBT1886 (double component);

class BT709Matrix : public ColorMatrix
{
public:
	virtual void Initialize();

	virtual std::string GetIdentifier();

	virtual BT709Matrix *Create() const;

	virtual BT709Matrix *Clone() const;
};

class BT709TransferFunction : public TransferCharacteristic
{
public:
	virtual void Initialize();

	virtual std::string GetIdentifier();

	virtual BT709TransferFunction *Create() const;

	virtual BT709TransferFunction *Clone() const;

	virtual void Linearize(std::vector<double> &input);

	virtual void ApplyTransferFunction(std::vector<double> &input);
};

#pragma endregion The BT.709 Color Space Triplet

#pragma region BT.601
// BT.601 System
class BT601Primaries : public ColorPrimaries
{
public:
	virtual void Initialize();

	virtual std::string GetIdentifier();

	virtual BT601Primaries *Create() const;

	virtual BT601Primaries *Clone() const;
};

class BT601625Primaries : public ColorPrimaries
{
public:
	virtual void Initialize();

	virtual std::string GetIdentifier();

	virtual BT601625Primaries *Create() const;

	virtual BT601625Primaries *Clone() const;
};

class BT601525Primaries : public ColorPrimaries
{
public:
	virtual void Initialize();

	virtual std::string GetIdentifier();

	virtual BT601525Primaries *Create() const;

	virtual BT601525Primaries *Clone() const;
};

class BT601Matrix : public ColorMatrix
{
public:
	virtual void Initialize();

	virtual std::string GetIdentifier();

	virtual BT601Matrix *Create() const;

	virtual BT601Matrix *Clone() const;
};

class BT601625Matrix : public ColorMatrix
{
public:
	virtual void Initialize();

	virtual std::string GetIdentifier();

	virtual BT601625Matrix *Create() const;

	virtual BT601625Matrix *Clone() const;
};

class BT601525Matrix : public ColorMatrix
{
public:
	virtual void Initialize();

	virtual std::string GetIdentifier();

	virtual BT601525Matrix *Create() const;

	virtual BT601525Matrix *Clone() const;
};

class BT601TransferFunction : public TransferCharacteristic
{
public:
	virtual void Initialize();

	virtual std::string GetIdentifier();

	virtual BT601TransferFunction *Create() const;

	virtual BT601TransferFunction *Clone() const;

	virtual void Linearize(std::vector<double> &input);
	
	virtual void ApplyTransferFunction(std::vector<double> &input);
};

#pragma endregion The BT.601 Color Space Triplet

#pragma region BT.2020
// BT.2020 System
class BT2020Primaries : public ColorPrimaries
{
public:
	virtual void Initialize();

	virtual std::string GetIdentifier();

	virtual BT2020Primaries *Create() const;

	virtual BT2020Primaries *Clone() const;
};

class BT2020Matrix : public ColorMatrix
{
public:
	virtual void Initialize();

	virtual std::string GetIdentifier();

	virtual BT2020Matrix *Create() const;

	virtual BT2020Matrix *Clone() const;
};

// 12-bit variant of transfer function defined in the BT.2020 specification
double BT2020Oetf12Bit (double component);

// The EOTF for BT.2020 is the same as the one for BT.709

// The 10-bit OETF for BT.2020 is also the same as the one for BT.709

#pragma endregion The BT.2020 Color Space Triplet

#pragma region HLG

double HlgOetf(double component);

double InvHlgOetf(double component);

class HLGTransferCharacteristic : public TransferCharacteristic
{
public:
	HLGTransferCharacteristic() : lightLevels({ 1000.0, 0.005 }), alpha(1.0) {};

	HLGTransferCharacteristic(double Lw, double Lb, double alpha) : lightLevels({ Lw, Lb }), alpha(alpha) {};

	virtual void Initialize();

	virtual std::string GetIdentifier();

	virtual HLGTransferCharacteristic *Create() const;

	virtual HLGTransferCharacteristic *Clone() const;

	// Not needed for now
	/*
	void SetAlpha(double alpha) { this->alpha = alpha; }
	*/

	void SetPeakNominalDisplayLuminance(double LW) { this->lightLevels.LW = LW; }

	void SetBlackDisplayLuminance(double LB) { this->lightLevels.LB = LB; }

	virtual void Linearize(std::vector<double> &input);

	virtual void ApplyTransferFunction(std::vector<double> &input);

	void SetLuminance(double luminance) { this->luminance = luminance; }

	void SetLuminance(double R, double G, double B);

protected:

	void GenerateEotfAndInverse();

	void GenerateOotfAndInverse();

	void GenerateGamma();

private:
	// the Peak and Black luminances
	ContentLightLevels lightLevels;

	// The gamma based on the display peak luminance
	double gamma; 

	// The Y luminance component generated from linear RGB values
	double luminance;

	// The alpha coefficient of the OOTF. Represents the user gain in cd/m^2
	double alpha;
};

#pragma endregion The Hybrid Log Gamma Transfer Characteristic (Defined in BT.2100)

#pragma region PQ

double PqEotf(double component);

double InvPqEotf(double component);

double PqOotf(double component);

// double InvPqOotf(double component);

// double PqOetf(double component);

class PQTransferCharacteristic : public TransferCharacteristic
{
protected:
	void GenerateOETFAndInverse();

public:
	virtual void Initialize();

	virtual std::string GetIdentifier();

	virtual PQTransferCharacteristic *Create() const;

	virtual PQTransferCharacteristic *Clone() const;

	virtual void Linearize(std::vector<double> &input);

	virtual void ApplyTransferFunction(std::vector<double> &input);
};

#pragma endregion The Procedural Quantizer Transfer Characteristic (Defined in BT.2100)

#pragma region No Transformation
//class IdentityPrimaries : public ColorPrimaries
//{
//public:
//	virtual void Initialize();
//
//	virtual std::string GetIdentifier();
//};
//
//class IdentityMatrices : public ColorMatrix
//{
//public:
//	virtual void Initialize();
//
//	virtual std::string GetIdentifier();
//};

double LinearTransferFunction(double component);

class LinearTransferCharacteristic : public TransferCharacteristic
{
public: 
	virtual void Initialize();

	virtual std::string GetIdentifier();

	virtual LinearTransferCharacteristic *Create() const;

	virtual LinearTransferCharacteristic *Clone() const;

	virtual void Linearize(std::vector<double> &input);

	virtual void ApplyTransferFunction(std::vector<double> &input);
};

#pragma endregion Primaries, Matrices, and Transfer Characteristic representing no transformation 

#endif