#pragma once
#ifndef COLORSPACECONVERTER_H
#define COLORSPACECONVERTER_H

#include "CommonIncludes.h"
#include "ColorSpaceTriplet.h"
#include "ColorTriplet.h"

class ColorSpaceConverter
{
private:
	ColorSpaceTriplet inputColorSpace;

	ColorSpaceTriplet outputColorSpace;

	//ToneMapping toneMap;

public:

	void Initialize();

	void SetInputColorSpace(ColorSpaceTriplet &input);

	void SetInputColorSpace( ColorPrimaries *primaries = nullptr,  ColorMatrix *matrix = nullptr,  TransferCharacteristic *transferCharacteristic = nullptr, Gamut range = Gamut::None);

	void SetOutputColorSpace(ColorSpaceTriplet &output);

	void SetOutputColorSpace( ColorPrimaries *primaries = nullptr,  ColorMatrix *matrix = nullptr,  TransferCharacteristic *transferCharacteristic = nullptr, Gamut range = Gamut::None);

	void Convert(ColorTriplet<double> &input, ColorTriplet<double> *output);

	// Utility functions

	// Use input color space
	void ConvertYCbCrToRgb(ColorTriplet<double> &input, std::vector<double> &intermediateTriplet);

	void Normalize(std::vector<double> &intermediateTriplet, unsigned int bitDepth);

	void Linearize(std::vector<double> &intermediateTriplet);

	void ConvertRgbToXyz(std::vector<double> &intermediateTriplet);

	// Use output Colorspace
	void ConvertXyzToRgb(std::vector<double> &intermediateTriplet);

	void ApplyTransferFunction(std::vector<double> &intermediateTriplet);

	// Tone mapping is done on the video signal, not the optical, following from report BT.2390-4 (2018)
	void ApplyToneMapping(std::vector<double> &intermediateTriplet);

	void Quantize(std::vector<double> &intermediateTriplet, unsigned int bitDepth);

	void ConvertRgbToYCbCr(std::vector<double> &intermediateTriplet, unsigned int bitDepth, Gamut outputGamut);
};

#endif