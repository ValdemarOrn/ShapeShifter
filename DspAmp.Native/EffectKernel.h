#pragma once


#include "Osc/UdpTranceiver.h"
#include <thread>

namespace DspAmp
{
	enum class Parameters
	{
		Param0 = 0,
		Param1,

		Count
	};

	class EffectKernel
	{
	private:
		Osc::UdpTranceiver* udpTranceiver;
		std::thread messageListenerThread;
		double fs;
		int bufferSize;
	public:

		EffectKernel(double fs, int bufferSize);
		~EffectKernel();

		void Process(float** inputs, float** outputs, int bufferSize);

	};

	
}
