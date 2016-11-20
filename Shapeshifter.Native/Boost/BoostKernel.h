#pragma once
#include "../AudioLib/ValueTables.h"
#include <string>
#include "../AudioLib/TfUtil.h"
#include "../AudioLib/SplineInterpolator.h"
#include "../AudioLib/Butterworth.h"

using namespace AudioLib;

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
		shared_ptr<SplineInterpolator> spline;
		TfUtil hpFilterGain;
		TfUtil hpFilterOut;
		TfUtil lpTone;
		double toneFreq;
		double drive;
		double effectiveDrive;
		int bufferSize;
		double* buffer;
		double tightness;

	public:
		double Mix;
		double OutputGain;

	public:
		BoostKernel(double fs, int bufferSize)
			: hpFilterGain(1)
			, hpFilterOut(1)
			, lpTone(1)
		{
			this->bufferSize = bufferSize;
			this->buffer = new double[bufferSize];
			this->fs = fs;

			clipper = (ClipperType)-1;
			SetClipper(ClipperType::Diode);
			
			auto hpOut = Butterworth::ComputeCoefficients(fs, true, 70, 1);
			hpFilterOut.Update(get<0>(hpOut), get<1>(hpOut));
			
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
			effectiveDrive = ValueTables::Get(drive, ValueTables::Response2Dec) * 200;
		}
		

		double GetTone() { return tone; }
		
		void SetTone(double value)
		{
			tone = value;
			this->toneFreq = 700 + ValueTables::Get(tone, ValueTables::Response2Dec) * 4300;
			auto lp = Butterworth::ComputeCoefficients(fs, false, toneFreq, 1);
			lpTone.Update(get<0>(lp), get<1>(lp));
		}
		

		double GetTightness() { return tightness; }
		
		void SetTightness(double value)
		{
			tightness = value;
			auto freq = 200 + ValueTables::Get(tightness, ValueTables::Response2Oct) * 1800;
			auto hpGain = Butterworth::ComputeCoefficients(fs, true, freq, 1);
			hpFilterGain.Update(get<0>(hpGain), get<1>(hpGain));
		}
		

		ClipperType GetClipper() { return clipper; }

		void SetClipper(ClipperType value)
		{
			if (value == clipper)
				return; // no change

			clipper = value;

			switch (clipper)
			{
			case ClipperType::Diode:
				spline = make_shared<SplineInterpolator>(Splines::D1N914Clipper);
				break;
			case ClipperType::Jfet:
				spline = make_shared<SplineInterpolator>(Splines::JfetStageHighGain);
				break;
			case ClipperType::Led:
				spline = make_shared<SplineInterpolator>(Splines::LedClipper);
				break;
			case ClipperType::Zener:
				spline = make_shared<SplineInterpolator>(Splines::ZenerClipper);
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

		void Process(float* input, float* output, int count)
		{
			// Keep a reference to the current spline object so it won't get GCed
			auto splineHandle = spline;

			if (count > bufferSize)
			{
				throw std::exception();
			}

			for (int i = 0; i < count; i++)
				buffer[i] = hpFilterGain.Process1(input[i]) * effectiveDrive;

			splineHandle->ProcessInPlace(buffer, count);
			double mix = Mix;
			double g = OutputGain;

			for (int i = 0; i < bufferSize; i++)
				buffer[i] = input[i] + buffer[i] * mix;

			for (int i = 0; i < bufferSize; i++)
				output[i] = lpTone.Process1(hpFilterOut.Process1(buffer[i])) * g;
		}

		double Process(double input)
		{
			// Keep a reference to the current spline object so it won't get GCed
			auto splineHandle = spline;

			double buffer = 0.0;

			buffer = hpFilterGain.Process1(input) * effectiveDrive;
			buffer = splineHandle->Process(buffer);
			double mix = Mix;

			buffer = buffer * mix + input;
			auto output = lpTone.Process1(hpFilterOut.Process1(buffer));
			return output;
		}
	};
}