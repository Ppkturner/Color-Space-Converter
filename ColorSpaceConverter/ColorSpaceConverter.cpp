#include "ColorSpaceConverter.h"

void ColorSpaceConverter::Initialize()
{
	this->inputColorSpace.Initialize();
	
	this->outputColorSpace.Initialize();

	// this->toneMap = ToneMapping(outputColorSpace.lightLevels.LB, outputColorSpace.lightLevels.LW);
}

void ColorSpaceConverter::SetInputColorSpace(ColorSpaceTriplet & input)
{
	this->inputColorSpace = input;
}

void ColorSpaceConverter::SetInputColorSpace(ColorPrimaries * primaries,  ColorMatrix * matrix,  TransferCharacteristic * transferCharacteristic, Gamut range)
{
	this->inputColorSpace.primaries = primaries;

	this->inputColorSpace.matrix = matrix;

	this->inputColorSpace.transferCharacteristic = transferCharacteristic;

	this->inputColorSpace.range = range;
}

void ColorSpaceConverter::SetOutputColorSpace(ColorSpaceTriplet & output)
{
	this->outputColorSpace = output;
}

void ColorSpaceConverter::SetOutputColorSpace(ColorPrimaries * primaries,  ColorMatrix * matrix,  TransferCharacteristic * transferCharacteristic, Gamut range)
{
	this->outputColorSpace.primaries = primaries;

	this->outputColorSpace.matrix = matrix;
	
	this->outputColorSpace.transferCharacteristic = transferCharacteristic;

	this->outputColorSpace.range = range;
}

void ColorSpaceConverter::Convert(ColorTriplet<double>& input, ColorTriplet<double>* output)
{
	// Steps:
	// 1. Convert YCbCr to RGB
	// 2. Normalize RGB to [0.0, 1.0]
	// 3. Apply EOTF to RGB
	// 4. Convert RGB To XYZ
	// 5. Convert XYZ to Target RGB
	// 6. Apply Target Transfer Characteristic
	// 7. (Optional) Tone Mapping
	// 8. Scale RGB to target range
	// 9. Convert RGB to YCbCr

	std::vector<double> intermediateTriplet(input.GetArray());

	if (input.GetColorModel() == ColorModel::YCbCr)
	{
		ConvertYCbCrToRgb(input, intermediateTriplet);
	}

	if (*inputColorSpace.transferCharacteristic != *outputColorSpace.transferCharacteristic)
	{
		Normalize(intermediateTriplet, input.GetBitDepth());

		Linearize(intermediateTriplet);
	}

	if (*inputColorSpace.primaries != *outputColorSpace.primaries)
	{
		if (input.GetColorModel() != ColorModel::XYZ)
		{
			ConvertRgbToXyz(intermediateTriplet);
		}

		if (output->GetColorModel() != ColorModel::XYZ)
		{
			ConvertXyzToRgb(intermediateTriplet);
		}
	}
	
	if (*inputColorSpace.transferCharacteristic != *outputColorSpace.transferCharacteristic)
	{
		ApplyTransferFunction(intermediateTriplet);

		Quantize(intermediateTriplet, output->GetBitDepth());
	}

	if (output->GetColorModel() == ColorModel::YCbCr)
	{
		ConvertRgbToYCbCr(intermediateTriplet, output->GetBitDepth(), outputColorSpace.range);
	}

	output->SetArray(intermediateTriplet);
}

void ColorSpaceConverter::ConvertYCbCrToRgb(ColorTriplet<double> &input, std::vector<double>& intermediateTriplet)
{
	std::vector<double> inputTriplet = input.GetArray();

	// Convert YCbCr to RGB
	if (this->inputColorSpace.matrix != nullptr)
	{
		unsigned int offsetCbCr = 0;

		double chromaFactor = (this->inputColorSpace.range == Gamut::StudioRange) ? 219.0 / 224.0 : 1.0;

		if (input.GetBitDepth() != 0)
		{
			offsetCbCr = 1 << (input.GetBitDepth() - 1);
		}

		// Regardless of range, YCbCr always has a chroma offset of 2^(bitDepth - 1)
		inputTriplet[1] = (inputTriplet[1] - static_cast<double>(offsetCbCr)) * chromaFactor;

		inputTriplet[2] = (inputTriplet[2] - static_cast<double>(offsetCbCr)) * chromaFactor;

		intermediateTriplet = this->inputColorSpace.matrix->GetYCbCrToRgb() * inputTriplet;
	}
	else
	{
		// if the matrix is null, then the input does not need to be converted (i.e. is not YCbCr)
		intermediateTriplet = inputTriplet;
	}
}

void ColorSpaceConverter::Normalize(std::vector<double>& intermediateTriplet, unsigned int bitDepth)
{
	// Normalize RGB to [0.0, 1.0]. If the bit depth is 0 or the range is "None", then we are already normalized
	if (this->inputColorSpace.range != Gamut::None && bitDepth != 0)
	{
		double range = 0;

		// The default assumed range is full range regardless of the actual value
		if (this->inputColorSpace.range == Gamut::StudioRange)
		{
			range = static_cast<double>(1 << (bitDepth - 8));

			for (int i = 0; i < 3; i++)
			{
				intermediateTriplet[i] = ((intermediateTriplet[i] / range) - 16.0) / 219.0;
			}
		}
		else
		{
			range = static_cast<double>(1 << bitDepth) - 1;

			for (int i = 0; i < 3; i++)
			{
				intermediateTriplet[i] /= range;
			}
		}
	}
}

void ColorSpaceConverter::Linearize(std::vector<double>& intermediateTriplet)
{
	if(this->inputColorSpace.transferCharacteristic != nullptr)
		this->inputColorSpace.transferCharacteristic->Linearize(intermediateTriplet);
}

void ColorSpaceConverter::ConvertRgbToXyz(std::vector<double>& intermediateTriplet)
{
	if(this->inputColorSpace.matrix != nullptr)
		intermediateTriplet = this->inputColorSpace.matrix->GetRgbToXyz() * intermediateTriplet;
}

void ColorSpaceConverter::ConvertXyzToRgb(std::vector<double>& intermediateTriplet)
{
	if(this->outputColorSpace.matrix != nullptr)
		intermediateTriplet = this->outputColorSpace.matrix->GetXyzToRgb() * intermediateTriplet;
}

void ColorSpaceConverter::ApplyTransferFunction(std::vector<double>& intermediateTriplet)
{
	if(this->outputColorSpace.transferCharacteristic != nullptr)
		this->outputColorSpace.transferCharacteristic->ApplyTransferFunction(intermediateTriplet);
}

void ColorSpaceConverter::ConvertRgbToYCbCr(std::vector<double>& intermediateTriplet, unsigned int bitDepth, Gamut outputGamut)
{
	if (this->outputColorSpace.matrix != nullptr)
	{
		double offset = static_cast<double>(1 << (bitDepth - 1));

		double range = 1.0;

		intermediateTriplet = this->outputColorSpace.matrix->GetRgbToYCbCr() * intermediateTriplet;

		if (outputGamut == Gamut::StudioRange)
		{
			range = 224.0 / 219.0;
		}

		for (int i = 1; i < 3; i++)
		{
			intermediateTriplet[i] = intermediateTriplet[i] * range + offset;
		}
	}
}

void ColorSpaceConverter::ApplyToneMapping(std::vector<double>& intermediateTriplet)
{
	/*for (int i = 0; i < 3; i++)
		this->toneMap.ApplyToneMapping(intermediateTriplet[i]);*/
}

void ColorSpaceConverter::Quantize(std::vector<double>& intermediateTriplet, unsigned int bitDepth)
{
	if (this->outputColorSpace.range == Gamut::StudioRange)
	{
		for (int i = 0; i < 3; i++)
		{
			intermediateTriplet[i] = (intermediateTriplet[i] * 219.0 + 16.0) * (1 << (bitDepth - 8));
		}
	}
	else
	{
		for (int i = 0; i < 3; i++)
		{
			intermediateTriplet[i] *= ((1 << (bitDepth)) - 1);
		}
	}
}
