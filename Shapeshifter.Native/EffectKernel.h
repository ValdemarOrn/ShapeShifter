#pragma once


#include "Osc/UdpTranceiver.h"
#include <thread>
#include "Parameters.h"
#include "NoiseGate/NoiseGateKernel.h"
#include "Boost/BoostKernel.h"
#include "IoFilter/IoFilter.h"

namespace Shapeshifter
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
		float* buffer1;
		float* buffer2;
		std::map<int, ParameterState> ParameterStates;

		NoiseInvader::NoiseGateKernel noiseGate;
		Boost::BoostKernel boost;
		IoFilter::IoFilterKernel inputFilter;
	public:
		
		std::map<int, Parameter> IndexToParameter;
		std::function<void(Parameter, float)> ParameterUpdateCallback;

		EffectKernel(double fs, int bufferSize);
		~EffectKernel();

		void Process(float** inputs, float** outputs, int totalBufferSize);
		void ProcessBuffer(float* inL, float* inR, float* outL, float* outR, int count);

		ParameterState GetParameterState(Parameter parameter);
		ParameterState GetParameterState(int index);
		void SetParameter(Parameter parameter, float value, bool updateHost, bool updateGui);
		void SetParameter(int index, float value, bool updateHost, bool updateGui);

	private:
		bool ApplyNoiseGateParameter(int parameter_index, float value);
		bool ApplyBoostParameter(int parameter_index, float value);
		
		void MessageListener();
		void SetupParameters();
	};

	
}
