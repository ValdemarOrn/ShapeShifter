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
		float* buffer;

		Biquad lowCutShelf;
		Biquad highCutShelf;

		Biquad peak1;
		Biquad peak2;

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

		double Peak1Gain;
		double Peak1Freq;
		double Peak1Q;

		double Peak2Gain;
		double Peak2Freq;
		double Peak2Q;
		

		IoFilterKernel(double fs, int bufferSize)
			: lowCutShelf(Biquad::FilterType::LowShelf, fs)
			, highCutShelf(Biquad::FilterType::HighShelf, fs)
			, peak1(Biquad::FilterType::Peak, fs)
			, peak2(Biquad::FilterType::Peak, fs)
			, lowCut1Pole(1)
			, lowCut2Pole(2)
			, highCut1Pole(1)
			, highCut2Pole(2)
		{
			this->fs = fs;
			buffer = new float[bufferSize];

			LowCut = CutType::OnePole;
			HighCut = CutType::OnePole;

			LowCutFreq = 50.0;
			LowCutdB = 0;

			HighCutFreq = 15000.0;
			HighCutdB = 0;

			Peak1Freq = 500;
			Peak1Gain = 0;
			Peak1Q = 0.7;

			Peak2Freq = 1200;
			Peak2Gain = 0;
			Peak2Q = 0.7;

			UpdateAll();
		}

		~IoFilterKernel()
		{
			delete buffer;
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
			lowCutShelf.SetGainDb(LowCutdB);
			lowCutShelf.Update2();

			highCutShelf.Frequency = HighCutFreq;
			highCutShelf.SetGainDb(HighCutdB);
			highCutShelf.Update2();

			peak1.Frequency = Peak1Freq;
			peak1.SetGainDb(Peak1Gain);
			peak1.SetQ(Peak1Q);
			peak1.Update2();

			peak2.Frequency = Peak2Freq;
			peak2.SetGainDb(Peak2Gain);
			peak2.SetQ(Peak2Q);
			peak2.Update2();
		}

		void Process(float* input, float* output, int count)
		{
			if (LowCut == CutType::OnePole)
			{
				for (size_t i = 0; i < count; i++)
					buffer[i] = lowCut1Pole.Process1(input[i]);
			}
			else if (LowCut == CutType::TwoPole)
			{
				for (size_t i = 0; i < count; i++)
					buffer[i] = lowCut2Pole.Process2(input[i]);
			}
			else if (LowCut == CutType::Shelf)
			{
				for (size_t i = 0; i < count; i++)
					buffer[i] = lowCutShelf.Process(input[i]);
			}
			else
			{
				for (size_t i = 0; i < count; i++)
					buffer[i] = input[i];
			}


			for (size_t i = 0; i < count; i++)
				buffer[i] = peak1.Process(buffer[i]);

			for (size_t i = 0; i < count; i++)
				buffer[i] = peak2.Process(buffer[i]);


			if (HighCut == CutType::OnePole)
			{
				for (size_t i = 0; i < count; i++)
					output[i] = highCut1Pole.Process1(buffer[i]);
			}
			else if (HighCut == CutType::TwoPole)
			{
				for (size_t i = 0; i < count; i++)
					output[i] = highCut2Pole.Process2(buffer[i]);
			}
			else if (HighCut == CutType::Shelf)
			{
				for (size_t i = 0; i < count; i++)
					output[i] = highCutShelf.Process(buffer[i]);
			}
			else
			{
				for (size_t i = 0; i < count; i++)
					output[i] = buffer[i];
			}
		}

		vector<FreqzPoint> GetResponse (int pointsPerDecade)
		{
			// todo: complete
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

		static std::string GetCutTypeName(CutType type)
		{
			if (type == CutType::None)
				return "None";
			if (type == CutType::OnePole)
				return "6dB";
			if (type == CutType::TwoPole)
				return "12dB";
			if (type == CutType::Shelf)
				return "Shelf";

			return "";
		}
	};

}
