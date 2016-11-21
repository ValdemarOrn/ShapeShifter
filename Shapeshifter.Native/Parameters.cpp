#include "Parameters.h"
#include <map>

std::map<EffectModule, int> Parameters::ModuleParamCount =
{ 
	{ EffectModule::NoiseGate, (int)ParametersNoiseGate::Count },
	{ EffectModule::Compressor, (int)6 },
	{ EffectModule::InputFilter, (int)ParametersIoFilter::Count },
	{ EffectModule::Boost, (int)ParametersBoost::Count },
	{ EffectModule::Preamplifier, 6 },
	{ EffectModule::OutputFilter, 6 },
	{ EffectModule::OutputStage, 6 },
	{ EffectModule::GraphicEq, 6 },
	{ EffectModule::Cabinet, 6 },
	{ EffectModule::Global, 6 }
};

std::map<EffectModule, std::string> Parameters::ModuleNames =
{
	{ EffectModule::NoiseGate, "NoiseGate" },
	{ EffectModule::Compressor, "Compressor" },
	{ EffectModule::InputFilter, "InputFilter" },
	{ EffectModule::Boost, "Boost" },
	{ EffectModule::Preamplifier, "Preamplifier" },
	{ EffectModule::OutputFilter, "OutputFilter" },
	{ EffectModule::OutputStage, "OutputStage" },
	{ EffectModule::GraphicEq, "GraphicEq" },
	{ EffectModule::Cabinet, "Cabinet" },
	{ EffectModule::Global, "Global" }
};

int Parameters::GetParameterCount()
{
	int sum = 0;
	for (auto kvp : ModuleParamCount)
		sum += kvp.second;

	return sum;
}
