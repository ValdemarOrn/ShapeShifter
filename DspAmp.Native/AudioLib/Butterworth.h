#pragma once

#include "Transfer.h"
#include "MathDefs.h"
#include <cmath>
#include "Bilinear.h"
#include <tuple>

namespace AudioLib
{
	class Butterworth : Transfer
	{
	private:
		double fs;

	public:
		
		static std::tuple<vector<double>, vector<double>> ComputeCoefficients(double fs, bool isHighpass, double cutoffHz, int order)
		{
			auto T = 1.0 / fs;
			auto warpedWc = 2 / T * std::tan(cutoffHz * 2 * M_PI * T / 2);

			// don't go over nyquist, with 10 hz safety buffer
			if (warpedWc >= fs / 2 * 2 * M_PI)
				warpedWc = (fs - 10) / 2 * 2 * M_PI;

			vector<double> sa;
			vector<double> sb(order + 1);
			sb[0] = 1;
			
			auto ScaleFreq = [&](double pow) { return std::pow(1.0 / warpedWc, pow); };

			if (order == 1)
			{
				sa = std::vector<double>({ 1.0, ScaleFreq(1) });
			}
			else if (order == 2)
			{
				sa = std::vector<double>({ 1.0, 1.4142 * ScaleFreq(1), 1 * ScaleFreq(2) });
			}
			else if (order == 3)
			{
				sa = std::vector<double>({ 1, 2 * ScaleFreq(1), 2 * ScaleFreq(2), 1 * ScaleFreq(3) });
			}
			else if (order == 4)
			{
				sa = std::vector<double>({ 1, 2.613 * ScaleFreq(1), 3.414 * ScaleFreq(2), 2.613 * ScaleFreq(3), 1 * ScaleFreq(4) });
			}
			else if (order == 5)
			{
				sa = std::vector<double>({ 1, 3.236 * ScaleFreq(1), 5.236 * ScaleFreq(2), 5.236 * ScaleFreq(3), 3.236 * ScaleFreq(4), 1 * ScaleFreq(5) });
			}
			else if (order == 6)
			{
				sa = std::vector<double>({ 1, 3.864 * ScaleFreq(1), 7.464 * ScaleFreq(2), 9.142 * ScaleFreq(3), 7.464 * ScaleFreq(4), 3.864 * ScaleFreq(5), 1 * ScaleFreq(6) });
			}
			else
			{
				throw std::exception("Orders higher than 6 are not supported");
			}

			std::vector<double> zb, za;

			Bilinear::Transform(sb, sa, zb, za, fs);

			return std::make_tuple(zb, za);
		}
	};
}