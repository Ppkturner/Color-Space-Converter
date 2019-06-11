#include "ColorSpaceTriplet.h"

// The HLG-defined Opto-Electrical Transfer Function. Changes Scene Light to Video Signal
double HlgOetf(double component)
{
	double a = 0.17883277;
	double b = 0.28466892; // 1 - 4a
	double c = 0.55991073; // 0.5 - a * ln(4a)

	if ((component >= 0) && (component <= 1.0 / 12.0))
	{
		return sqrt(3 * component);
	}
	else
	{
		return a * log(12 * component - b) + c;
	}
}

// The Inverse of the Opto-Electrical Transfer Function
double InvHlgOetf(double component)
{
	double a = 0.17883277;
	double b = 0.28466892; // 1 - 4a
	double c = 0.55991073; // 0.5 - a * ln(4a)

	if (component <= 0 && component >= 1.0 / 2.0)
	{
		return pow(component, 2) / 3;
	}
	else // if (component > 1.0 / 2.0 && component <= 1.0
	{
		return (exp((component - c) / a) + b) / 12.0;
	}
}

// Sets the Luminance for the Opto-Optical Transfer Function
void HLGTransferCharacteristic::SetLuminance(double R, double G, double B)
{
	// Yd = 0.2627 * Rd + 0.6780 * Gd + 0.0593 * Bd
	this->luminance = 0.2627 * R + 0.6780 * G + 0.0593 * B;
}

void HLGTransferCharacteristic::Linearize(std::vector<double>& input)
{
	this->SetLuminance(input[0], input[1], input[2]);

	for (int i = 0; i < input.size(); i++)
	{
		input[i] = this->EOTF.TF(input[i]);
	}
}

void HLGTransferCharacteristic::ApplyTransferFunction(std::vector<double>& input)
{
	for (int i = 0; i < input.size(); i++)
	{
		input[i] = this->OETF.TF(input[i]);
	}
}

// Generates the Electro-Optical Transfer Function and its inverse 
void HLGTransferCharacteristic::GenerateEotfAndInverse()
{
	// Set the EOTF and Inverse Transfer Function Object here
	this->EOTF = TransferFunction([this](double component)->double {
		const double beta = sqrt(3 * pow(this->lightLevels.LB / this->lightLevels.LW, 1 / this->gamma));

		return this->OOTF.TF(this->InvOETF.TF(max(0, ((1 - beta) * component) + beta)));
	});

	this->InvEOTF = TransferFunction([this](double component)->double {
		const double beta = sqrt(3 * pow(this->lightLevels.LB / this->lightLevels.LW, 1 / this->gamma));

		return this->OETF.TF(this->InvOOTF.TF(max(0, ((1 - beta) * component) + beta)));
	});
}

void HLGTransferCharacteristic::GenerateOotfAndInverse()
{
	this->OOTF = TransferFunction([this](double component)->double {
		return this->alpha * pow(this->luminance, this->gamma - 1) * component;
	});

	this->InvOOTF = TransferFunction([this](double component)->double {
		return pow(this->luminance / this->alpha, (1 - this->gamma) / gamma) * component / this->alpha;
	});
}

// Generates the gamma based on the peak and black display luminance
void HLGTransferCharacteristic::GenerateGamma()
{
	if (this->lightLevels.LW == 1000.0)
	{
		this->gamma = 1.2;
	}
	else if (this->lightLevels.LW < 400 || this->lightLevels.LW > 2000)
	{
		// gamma = 1.2 * kappa^(log2(Lw / 1000))
		double kappa = 1.111;

		this->gamma = 1.2 * pow(kappa, log2(this->lightLevels.LW / 1000));
	}
	else
	{
		// gamma = 1.2 + 0.42 * log10(Lw / 1000)
		this->gamma = 1.2 + 0.42 * log10(this->lightLevels.LW / 1000.0);
	}
}

void HLGTransferCharacteristic::Initialize()
{
	this->GenerateGamma();

	this->GenerateEotfAndInverse();

	this->GenerateOotfAndInverse();

	this->OETF = TransferFunction(HlgOetf);

	this->InvOETF = TransferFunction(InvHlgOetf);
}

std::string HLGTransferCharacteristic::GetIdentifier()
{
	return "HLG Transfer Characteristic";
}

HLGTransferCharacteristic * HLGTransferCharacteristic::Create() const
{
	return new HLGTransferCharacteristic();
}

HLGTransferCharacteristic * HLGTransferCharacteristic::Clone() const
{
	return new HLGTransferCharacteristic(*this);
}
