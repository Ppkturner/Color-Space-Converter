#include "ColorSpaceTriplet.h"

void LinearTransferCharacteristic::Initialize()
{
	this->EOTF = TransferFunction(LinearTransferFunction);

	this->OETF = TransferFunction(LinearTransferFunction);
}

std::string LinearTransferCharacteristic::GetIdentifier()
{
	return "Linear Transfer Characteristic";
}

LinearTransferCharacteristic * LinearTransferCharacteristic::Create() const
{
	return new LinearTransferCharacteristic();
}

LinearTransferCharacteristic * LinearTransferCharacteristic::Clone() const
{
	return new LinearTransferCharacteristic(*this);
}

void LinearTransferCharacteristic::Linearize(std::vector<double>& input)
{
	for (int i = 0; i < input.size(); i++)
	{
		input[i] = this->EOTF.TF(input[i]);
	}
}

void LinearTransferCharacteristic::ApplyTransferFunction(std::vector<double>& input)
{
	for (int i = 0; i < input.size(); i++)
	{
		input[i] = this->OETF.TF(input[i]);
	}
}
