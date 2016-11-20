#pragma once
#include <vector>
#include <sstream>
#include <algorithm>

namespace AudioLib
{
	class Splines
	{
	public:
		static std::string JfetStageLinear;
		static std::string JfetStageHighGain;
		static std::string ZenerClipper;
		static std::string D1N914Clipper;
		static std::string LedClipper;
	};

	class SplineInterpolator
	{
	private:
		std::vector<double> xs;
		std::vector<double> ys;
		std::vector<double> ks;
		double min;
		double max;

		inline std::vector<std::string> SplitString(const std::string s, char delim)
		{
			std::vector<std::string> elems;

			std::stringstream ss(s);
			std::string item;

			while (std::getline(ss, item, delim))
			{
				elems.push_back(item);
			}
			return elems;
		}

	public:

		double Bias;

		SplineInterpolator(std::string dataString)
		{
			if (dataString == "")
				return;

			auto bits = SplitString(dataString, ';');
			auto xStrings = SplitString(bits.at(0), ',');
			auto yStrings = SplitString(bits.at(1), ',');
			auto kStrings = SplitString(bits.at(2), ',');

			for (int i = 0; i < xStrings.size(); i++)
			{
				auto value = std::stod(xStrings[i]);
				xs.push_back(value);
			}

			for (int i = 0; i < yStrings.size(); i++)
			{
				auto value = std::stod(yStrings[i]);
				ys.push_back(value);
			}

			for (int i = 0; i < kStrings.size(); i++)
			{
				auto value = std::stod(kStrings[i]);
				ks.push_back(value);
			}

			Bias = 0.0;
			max = *std::max_element(std::begin(xs), std::end(xs));
			min = *std::min_element(std::begin(xs), std::end(xs));
		}

		/// <summary>
		/// Arrays must be sorted from lowest x value to highest
		/// </summary>
		/// <param name="xs"></param>
		/// <param name="ys"></param>
		/// <param name="ks"></param>
		SplineInterpolator(std::vector<double> xs, std::vector<double> ys, std::vector<double> ks)
		{
			this->xs = xs;
			this->ys = ys;
			this->ks = ks;

			Bias = 0.0;
			max = *std::max_element(std::begin(xs), std::end(xs));
			min = *std::min_element(std::begin(xs), std::end(xs));
		}

		~SplineInterpolator()
		{
			
		}

		void ProcessInPlace(double* inputs, int length)
		{
			for (int i = 0; i < length; i++)
			{
				auto x = inputs[i] + Bias;
				if (x > max) x = max - 0.00001;
				if (x < min) x = min + 0.00001;
				auto y = InterpolateNoBias(x);
				inputs[i] = y;
			}
		}

		__forceinline double Process(double input)
		{
			auto x = input + Bias;
			if (x > max) x = max - 0.00001;
			if (x < min) x = min + 0.00001;
			auto y = InterpolateNoBias(x);
			return y;
		}

		__forceinline double InterpolateNoBias(double x)
		{
			auto i = 1;
			while (xs[i] < x)
				i++;

			auto t = (x - xs[i - 1]) / (xs[i] - xs[i - 1]);

			auto a = ks[i - 1] * (xs[i] - xs[i - 1]) - (ys[i] - ys[i - 1]);
			auto b = -ks[i] * (xs[i] - xs[i - 1]) + (ys[i] - ys[i - 1]);

			auto q = (1 - t) * ys[i - 1] + t * ys[i] + t * (1 - t) * (a * (1 - t) + b * t);
			return q;
		}

	};


}