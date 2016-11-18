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
		std::map<int, Parameter> IndexToParameter;
		std::map<int, ParameterState> ParameterStates;
	public:
		
		EffectKernel(double fs, int bufferSize);
		~EffectKernel();

		void Process(float** inputs, float** outputs, int bufferSize);

		ParameterState GetParameterState(Parameter parameter);
		ParameterState GetParameterState(int index);
		void SetParameter(Parameter parameter, float value);
		void SetParameter(int index, float value);
	private:
		void MessageListener();
		void SetupParameters();
	};

	
}
