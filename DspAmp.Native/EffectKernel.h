#pragma once


#include "Osc/UdpTranceiver.h"
#include <thread>
#include "Parameters.h"

namespace DspAmp
{
	class ParameterState
	{
	public:
		int Index;
		float Value;
		std::string Name;
		std::string Display;
	};

	class EffectKernel
	{
	private:
		Osc::UdpTranceiver* udpTranceiver;
		std::thread messageListenerThread;
		double fs;
		int bufferSize;
		bool isClosing;

		int ParameterCount;
		std::map<int, ParameterState> ParameterStates;
	public:
		
		std::map<int, Parameter> IndexToParameter;
		std::function<void(Parameter, float)> ParameterUpdateCallback;

		EffectKernel(double fs, int bufferSize);
		~EffectKernel();

		void Process(float** inputs, float** outputs, int bufferSize);

		ParameterState GetParameterState(Parameter parameter);
		ParameterState GetParameterState(int index);
		void SetParameter(Parameter parameter, float value, bool triggerCallback);
		void SetParameter(int index, float value, bool triggerCallback);
	private:
		void MessageListener();
		void SetupParameters();
	};

	
}
