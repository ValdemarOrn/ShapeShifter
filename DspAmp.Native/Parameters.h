#pragma once
#include <string>
#include "Utility.h"
#include <map>

enum class EffectModule
{
	None = 0,

	NoiseGate = 1,
	Compressor,
	InputFilter,
	Boost,
	Preamplifier,
	OutputFilter,
	OutputStage,
	GraphicEq,
	Cabinet,
	Global
};

class Parameter
{
public:
	EffectModule Module;
	int ParameterIndex;

	Parameter()
	{
		Module = EffectModule::None;
		ParameterIndex = 0;
	}

	Parameter(int key)
	{
		Module = (EffectModule)(key >> 8);
		ParameterIndex = key & 0xFF;
	}

	Parameter(EffectModule module, int index)
	{
		Module = module;
		ParameterIndex = index;
	}



	int GetKey()
	{
		return ((int)Module) << 8 | ParameterIndex;
	}
};

class Parameters
{
	
public:

	static std::map<EffectModule, int> ModuleParamCount;
	static std::map<EffectModule, std::string> ModuleNames;
	
	static int GetParameterCount();

	static inline Parameter ParseOsc(std::string address)
	{
		auto parts = Utility::SplitString(address, '/');
		EffectModule module = EffectModule::None;

		auto moduleString = parts.at(1);
		int index = std::stoi(parts.at(2));
		
		for (auto kvp : ModuleNames)
		{
			if (kvp.second == moduleString)
				module = kvp.first;
		}

		Parameter output;
		output.Module = module;
		output.ParameterIndex = index;

		return output;
	}
};