#include "ColorSpaceTriplet.h"

double PqEotf(double component)
{
	double m1 = 0.1593017578125;	// 2610 / 16384
	double m2 = 78.84375;			// 2523 / 4096
	double c1 = 0.8359375;			// 3424 / 4096 = c3 - c2 + 1
	double c2 = 18.8515625;			// 2413 / 4096 * 32
	double c3 = 18.6875;			// 2392 / 4096 * 32
	
	// Y = (max[E^(1/m2) - c1, 0] / (c2 - c3 * E^(1/m2)))^(1/m1)
	double Y = pow(max(pow(component, 1 / m2) - c1, 0) / (c2 - (c3 * pow(component, 1 / m2))), 1 / m1);

	// Fd = 10000 * Y
	return 10000.0 * Y;
}

double InvPqEotf(double component)
{
	double m1 = 0.1593017578125;	// 2610 / 16384
	double m2 = 78.84375;			// 2523 / 4096
	double c1 = 0.8359375;			// 3424 / 4096 = c3 - c2 + 1
	double c2 = 18.8515625;			// 2413 / 4096 * 32
	double c3 = 18.6875;			// 2392 / 4096 * 32

	double Y = component / 10000.0;

	return pow((c1 + (c2 * pow(Y, m1))) / (1 + (c3 * pow(Y, m1))), m2);
}

double PqOotf(double component)
{
	double comp709 = 0.0;

	if (component < 1 && component > 0.0003024)
	{
		comp709 = 1.099 * pow(59.5208 * component, 0.45) - 0.099;
	}
	else // if (component <= 0.03024 && component >= 0.0)
	{
		comp709 = 267.84 * component;
	}

	return 100.0 * pow(comp709, 2.4);
}

double LinearTransferFunction(double component)
{
	return component;
}

void PQTransferCharacteristic::GenerateOETFAndInverse()
{
	this->OETF = TransferFunction([this](double component)->double {
		return this->InvEOTF.TF(this->OOTF.TF(component));
	});

	// I'm not sure the inverse OETF is needed
}

void PQTransferCharacteristic::Initialize()
{
	this->GenerateOETFAndInverse();

	this->OOTF = TransferFunction(PqOotf);

	this->EOTF = TransferFunction(PqEotf);
	
	this->InvEOTF = TransferFunction(InvPqEotf);
}

std::string PQTransferCharacteristic::GetIdentifier()
{
	return "PQ Transfer Characteristic";
}

PQTransferCharacteristic * PQTransferCharacteristic::Create() const
{
	return new PQTransferCharacteristic();
}

PQTransferCharacteristic * PQTransferCharacteristic::Clone() const
{
	return new PQTransferCharacteristic(*this);
}

void PQTransferCharacteristic::Linearize(std::vector<double>& input)
{
	for (int i = 0; i < input.size(); i++)
	{
		input[i] = this->EOTF.TF(input[i]);
	}
}

void PQTransferCharacteristic::ApplyTransferFunction(std::vector<double>& input)
{
	for (int i = 0; i < input.size(); i++)
	{
		input[i] = this->OETF.TF(input[i]);
	}
}
