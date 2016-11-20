#pragma once
#include <vector>
#include "Utils.h"
#include <complex>

namespace AudioLib
{
	using namespace std;

	class FreqzPoint
	{
	public:
		double W;
		double Magnitude;
		double Phase;

		FreqzPoint(double w, double mag, double phase)
		{
			W = w;
			Magnitude = mag;
			Phase = phase;
		}
	};

	class Freqz
	{
	public:
		
		template<typename T>
		static vector<FreqzPoint> Compute(vector<T> b, vector<T> a, int points)
		{
			double scaler = 1.0 / (double)points * M_PI;
			auto ws = Utils::Linspace<double>(0, points - 1, points);
			for (size_t i = 0; i < ws.size(); i++)
				ws[i] *= scaler;
			
			vector<FreqzPoint> output;

			for (int i = 0; i < ws.size(); i++)
			{
				auto wReal = ws[i];
				complex<double> w = std::polar<double>(1, wReal);
				complex<double> ww = 1;
				complex<double> bSum = 0;
				complex<double> aSum = 0;

				for(auto bb : b)
				{
					ww *= w;
					bSum += ((double)bb) * ww;
				}

				ww = 1;
				for(auto aa : a)
				{
					ww *= w;
					aSum += ((double)aa) * ww;
				}

				auto div = (bSum / aSum);
				double mag = std::abs(div);
				double ang = std::arg(div);
				
				output.push_back(FreqzPoint(wReal, mag, ang));
			}
			
			return output;
		}
	};
}