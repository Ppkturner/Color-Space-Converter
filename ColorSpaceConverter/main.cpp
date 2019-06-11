#include "ColorSpaceConverter.h"

int main()
{
	ColorSpaceConverter converter = ColorSpaceConverter();

	ColorSpaceTriplet inputColorSpace = ColorSpaceTriplet();

	inputColorSpace.primaries = new BT2020Primaries();

	inputColorSpace.matrix = new BT2020Matrix();

	inputColorSpace.transferCharacteristic = new HLGTransferCharacteristic();

	inputColorSpace.range = Gamut::StudioRange;

	inputColorSpace.lightLevels.LW = 1000;

	inputColorSpace.lightLevels.LB = 0.001;

	ColorSpaceTriplet outputColorSpace = ColorSpaceTriplet();

	outputColorSpace.primaries = new BT709Primaries();

	outputColorSpace.matrix = new BT709Matrix();

	outputColorSpace.transferCharacteristic = new BT709TransferFunction();

	outputColorSpace.range = Gamut::StudioRange;

	outputColorSpace.lightLevels.LW = 120;

	outputColorSpace.lightLevels.LB = 0;

	converter.SetInputColorSpace(inputColorSpace);

	converter.SetOutputColorSpace(outputColorSpace);

	converter.Initialize();

	ColorTriplet<double> input = ColorTriplet<double>(913, 314, 525, 10, false, ColorModel::YCbCr);

	ColorTriplet<double> output = ColorTriplet<double>(0, 0, 0, 10, false, ColorModel::YCbCr);

	converter.Convert(input, &output);

	return 0;
}