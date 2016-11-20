#pragma once

#include <iostream>
#include <cmath>

#include "../AudioLib/Sse.h"
#include "Expander.h"
#include "EnvelopeFollower.h"
#include "SlewLimiter.h"

using namespace AudioLib;

namespace NoiseInvader
{
	class NoiseGateKernel
	{
	private:

		double fs;

		EnvelopeFollower envelopeFollower;
		Expander expander;
		SlewLimiter slewLimiter;

	public:

		// Gain Settings
		double DetectorGain;
		
		// Noise Gate Settings
		double ReductionDb;
		double ThresholdDb;
		double Slope;
		double ReleaseMs;

		// for readouts
		double currentGainDb;

		NoiseGateKernel(int fs)
			: envelopeFollower(fs, 100)
			, expander()
			, slewLimiter(fs)
		{
			this->fs = fs;
			
			DetectorGain = 1.0f;
			ReductionDb = -150;
			ThresholdDb = -20;
			Slope = 3;
			ReleaseMs = 100;
			UpdateAll();
		}

		inline ~NoiseGateKernel()
		{

		}

		inline void UpdateAll()
		{
			expander.Update(ThresholdDb, ReductionDb, Slope);
			envelopeFollower.SetRelease(ReleaseMs);
			slewLimiter.UpdateDb60(2.0, ReleaseMs);
		}

		inline void Process(
			float* input, 
			float* detectorInput, 
			float* output, 
			int len)
		{
			Sse::PreventDernormals();
			double gainDb = 1.0;
			double currGain = -1000;

			for (size_t i = 0; i < len; i++)
			{
				auto x = detectorInput[i] * DetectorGain;
				envelopeFollower.ProcessEnvelope(x);
				auto env = envelopeFollower.GetOutput();

				expander.Expand(Utils::Gain2DB(env));
				gainDb = expander.GetOutput();
				gainDb = slewLimiter.Process(gainDb);

				if (gainDb > currGain)
					currGain = gainDb;

				auto gain = Utils::DB2gain(gainDb);
				output[i] = input[i] * gain;
			}

			currentGainDb = currGain;
		}

	};
}

