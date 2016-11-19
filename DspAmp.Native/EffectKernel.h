#pragma once


#include "Osc/UdpTranceiver.h"
#include <thread>
#include "Parameters.h"
#include "NoiseGate/NoiseGateKernel.h"

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

		NoiseInvader::NoiseGateKernel noiseGate;
	public:
		
		std::map<int, Parameter> IndexToParameter;
		std::function<void(Parameter, float)> ParameterUpdateCallback;

		EffectKernel(double fs, int bufferSize);
		~EffectKernel();

		void Process(float** inputs, float** outputs, int totalBufferSize);
		void ProcessBuffer(float* inL, float* inR, float* outL, float* outR, int count);

		ParameterState GetParameterState(Parameter parameter);
		ParameterState GetParameterState(int index);
		void SetParameter(Parameter parameter, float value, bool triggerCallback);
		void SetParameter(int index, float value, bool triggerCallback);

	private:
		bool ApplyNoiseGateParameter(int parameter_index, float value);

		void MessageListener();
		void SetupParameters();
	};

	
}
