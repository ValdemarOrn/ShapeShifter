#include "EffectKernel.h"

namespace DspAmp
{
	EffectKernel::EffectKernel(double fs, int bufferSize)
	{
		this->fs = fs;
		this->bufferSize = bufferSize;
	}

	EffectKernel::~EffectKernel()
	{
	}

	void EffectKernel::Process(float** inputs, float** outputs, int bufferSize)
	{
		for (int i = 0; i < bufferSize; i++)
		{
			outputs[0][i] = inputs[0][i];
			outputs[1][i] = inputs[1][i];
		}
	}
}
