#pragma once
#include "../AudioLib/Biquad.h"
#include "../AudioLib/TfUtil.h"
#include "../AudioLib/Butterworth.h"
#include "../AudioLib/Freqz.h"

#include <sstream>
using namespace AudioLib;

namespace IoFilter
{
	enum class CutType
	{
		None = 0,

		OnePole = 1,
		TwoPole,
		Shelf,
	};

	class IoFilterKernel
	{
	private:
		double fs;

		Biquad lowCutShelf;
		Biquad highCutShelf;

		TfUtil lowCut1Pole;
		TfUtil lowCut2Pole;
		TfUtil highCut1Pole;
		TfUtil highCut2Pole;

	public:
		CutType LowCut;
		CutType HighCut;

		double LowCutFreq;
		double LowCutdB;

		double HighCutFreq;
		double HighCutdB;

		

		IoFilterKernel(double fs, int bufferSize)
			: lowCutShelf(Biquad::FilterType::LowShelf, fs)
			, highCutShelf(Biquad::FilterType::HighShelf, fs)
			, lowCut1Pole(1)
			, lowCut2Pole(2)
			, highCut1Pole(1)
			, highCut2Pole(2)
		{
			this->fs = fs;
			LowCut = CutType::OnePole;
			HighCut = CutType::OnePole;

			LowCutFreq = 50.0;
			LowCutdB = 12.0;

			HighCutFreq = 15000.0;
			HighCutdB = 12.0;

			UpdateAll();
		}

		void UpdateAll()
		{
			auto onePoleLowCut = Butterworth::ComputeCoefficients(fs, true, LowCutFreq, 1);
			auto twoPoleLowCut = Butterworth::ComputeCoefficients(fs, true, LowCutFreq, 2);
			auto onePoleHighCut = Butterworth::ComputeCoefficients(fs, false, HighCutFreq, 1);
			auto twoPoleHighCut = Butterworth::ComputeCoefficients(fs, false , HighCutFreq, 2);

			lowCut1Pole.Update(get<0>(onePoleLowCut), get<1>(onePoleLowCut));
			lowCut2Pole.Update(get<0>(twoPoleLowCut), get<1>(twoPoleLowCut));
			highCut1Pole.Update(get<0>(onePoleHighCut), get<1>(onePoleHighCut));
			highCut2Pole.Update(get<0>(twoPoleHighCut), get<1>(twoPoleHighCut));

			lowCutShelf.Frequency = LowCutFreq;
			lowCutShelf.SetGainDb(-LowCutdB);
			lowCutShelf.Update2();

			highCutShelf.Frequency = HighCutFreq;
			highCutShelf.SetGainDb(-HighCutdB);
			highCutShelf.Update2();

			auto sLow = Utils::ToMatlabString(fs, lowCutShelf.GetB(), lowCutShelf.GetA());
			auto sHigh = Utils::ToMatlabString(fs, highCutShelf.GetB(), highCutShelf.GetA());

			auto respo = GetMagnitudeString(20);
		}

		vector<FreqzPoint> GetResponse (int pointsPerDecade)
		{
			auto f1 = Freqz::Compute(lowCutShelf.GetB(), lowCutShelf.GetA(), 30.0, fs, pointsPerDecade);
			auto f2 = Freqz::Compute(highCutShelf.GetB(), highCutShelf.GetA(), 30.0, fs, pointsPerDecade);
			
			vector<FreqzPoint> output;

			for (size_t i = 0; i < f1.size(); i++)
			{
				double mag = f1[i].Magnitude * f2[i].Magnitude;
				double phase = f1[i].Phase + f2[i].Phase;
				double w = f1[i].W;

				output.push_back(FreqzPoint(w, mag, phase));
			}

			return output;
		}

		std::string GetMagnitudeString(int pointsPerDecade)
		{
			auto resp = GetResponse(pointsPerDecade);

			std::stringstream str;

			for (auto val : resp)
			{
				str << Utils::Float2String(Utils::Gain2DB(val.Magnitude), 3);
				str << ", ";
			}

			return str.str();
		}
	};

}
