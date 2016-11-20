#pragma once
#include "../AudioLib/ValueTables.h"
#include <string>
#include "../AudioLib/TfUtil.h"
#include "../AudioLib/SplineInterpolator.h"
#include "../AudioLib/Butterworth.h"

//using namespace AudioLib;

namespace Boost
{
	enum class ClipperType
	{
		Diode = 0,
		Zener,
		Led,
		Jfet
	};

	class BoostKernel
	{
	private:
		double fs;
		ClipperType clipper;
		double tone;
		AudioLib::SplineInterpolator spline;
		AudioLib::TfUtil hpFilterGain;
		AudioLib::TfUtil hpFilterOut;
		AudioLib::TfUtil lpTone;
		double toneFreq;
		double drive;
		double effectiveDrive;
		int bufferSize;
		double* buffer;
		double tightness;

	public:
		double Mix;

	public:
		BoostKernel(double fs, int bufferSize)
			: hpFilterGain(1)
			, hpFilterOut(1)
			, lpTone(1)
			, spline("")
		{
			this->bufferSize = bufferSize;
			this->buffer = new double[bufferSize];
			this->fs = fs;
			SetClipper(ClipperType::Diode);
			
			auto hpOut = AudioLib::Butterworth::ComputeCoefficients(fs, true, 70, 1);
			hpFilterOut.Update(std::get<0>(hpOut), std::get<1>(hpOut));
			
			SetTightness(0.4);
			SetTone(1);
		}

		~BoostKernel()
		{
			delete buffer;
		}

		double GetDrive() { return drive; }

		void SetDrive(double value)
		{
			drive = value;
			effectiveDrive = AudioLib::ValueTables::Get(drive, AudioLib::ValueTables::Response2Dec) * 200;
		}
		

		double GetTone() { return tone; }
		
		void SetTone(double value)
		{
			this->toneFreq = 700 + AudioLib::ValueTables::Get(tone, AudioLib::ValueTables::Response2Dec) * 4300;
			tone = value;
			auto lp = AudioLib::Butterworth::ComputeCoefficients(fs, false, toneFreq, 1);
			lpTone.Update(std::get<0>(lp), std::get<1>(lp));
		}
		

		double GetTightness() { return tightness; }
		
		void SetTightness(double value)
		{
			auto freq = 200 + AudioLib::ValueTables::Get(tone, AudioLib::ValueTables::Response2Oct) * 1800;
			tightness = value;
			auto hpGain = AudioLib::Butterworth::ComputeCoefficients(fs, true, freq, 1);
			hpFilterGain.Update(get<0>(hpGain), get<1>(hpGain));
		}
		

		ClipperType GetClipper() { return clipper; }

		void SetClipper(ClipperType value)
		{
			clipper = value;

			switch (clipper)
			{
			case ClipperType::Diode:
				spline = AudioLib::SplineInterpolator(AudioLib::Splines::D1N914Clipper);
				break;
			case ClipperType::Jfet:
				spline = AudioLib::SplineInterpolator(AudioLib::Splines::JfetStageHighGain);
				break;
			case ClipperType::Led:
				spline = AudioLib::SplineInterpolator(AudioLib::Splines::LedClipper);
				break;
			case ClipperType::Zener:
				spline = AudioLib::SplineInterpolator(AudioLib::Splines::ZenerClipper);
				break;
			}
		}
		
		double GetToneFreq()
		{
			return toneFreq;
		}

		std::string GetClipperName()
		{
			switch (clipper)
			{
			case ClipperType::Diode:
				return "Diode";
			case ClipperType::Jfet:
				return "Jfet";
			case ClipperType::Led:
				return "Led";
			case ClipperType::Zener:
				return "Zener";
			default:
				return "";
			}
		}

		void Process(double* input, double* output, int count)
		{
			if (count > bufferSize)
			{
				throw std::exception();
			}

			for (int i = 0; i < count; i++)
				buffer[i] = hpFilterGain.Process1(input[i]) * effectiveDrive;

			spline.ProcessInPlace(buffer, count);
			double mix = Mix;

			for (int i = 0; i < bufferSize; i++)
				buffer[i] = buffer[i] * mix + input[i];

			for (int i = 0; i < bufferSize; i++)
				output[i] = lpTone.Process1(hpFilterOut.Process1(buffer[i])) * 0.3;
		}

		double Process(double input)
		{
			double buffer = 0.0;

			buffer = hpFilterGain.Process1(input) * effectiveDrive;
			buffer = spline.Process(buffer);
			double mix = Mix;

			buffer = buffer * mix + input;
			auto output = lpTone.Process1(hpFilterOut.Process1(buffer));
			return output;
		}
	};
}