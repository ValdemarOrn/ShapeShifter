#ifndef AUDIOLIB_UTILS
#define AUDIOLIB_UTILS

#include <cstring>
#include <cmath>
#include "MathDefs.h"
#include <sstream>
#include <vector>
#include <iomanip>

namespace AudioLib
{
	class Utils
	{
	private:
		static const int TableSize = 20000;
		static float note2Freq[12800];
		static float sintable[TableSize];
		static float costable[TableSize];
		static float tableScaler;

		static const int TanhTableSize = 60000;
		static float tanhTable[TanhTableSize];

	public:
		static void Initialize();
		static float Note2Freq(float note);

		static inline float FastSin(float x)
		{
			int idx = ((int)(x * tableScaler) + 100 * TableSize) % TableSize;
			return sintable[idx];
		}

		static inline float FastCos(float x)
		{
			int idx = ((int)(x * tableScaler) + 100 * TableSize) % TableSize;
			return costable[idx];
		}

		static inline void ZeroBuffer(float* buffer, int len)
		{
			for (int i = 0; i < len; i++)
				buffer[i] = 0.0f;
		}
		
		static inline void PreventDenormal(float* buffer, int len)
		{
			static bool sign;
			auto offset = 0.00001f;
			if (sign) offset *= -1;
			sign = !sign;

			for (int i = 0; i < len; i++)
			{
				buffer[i] = offset;
				offset *= -1;
			}
		}

		static inline void Copy(float* source, float* dest, int len)
		{
			std::memcpy(dest, source, len * sizeof(float));
		}

		static inline void Gain(float* buffer, float gain, int len)
		{
			for (int i = 0; i < len; i++)
			{
				buffer[i] *= gain;
			}
		}

		static inline float LinInterp(float* table, int tableSize, float value)
		{
			if (value <= 0)
				return table[0];
			if (value >= 1.0)
				return table[tableSize - 1];

			float fIdx = value * tableSize;
			int idxA = (int)fIdx;
			int idxB = idxA + 1;
			if (idxA >= tableSize) idxA = tableSize - 1;
			if (idxB >= tableSize) idxB = tableSize - 1;
			float mix = fIdx - idxA;

			return table[idxA] * (1 - mix) + table[idxB] * mix;
		}

		static double Min(double* input, int len)
		{
			double min = input[0];
			for (int i = 1; i < len; i++)
				if (input[i] < min)
					min = input[i];

			return min;
		}

		static double Max(double* input, int len)
		{
			double max = input[0];
			for (int i = 1; i < len; i++)
				if (input[i] > max)
					max = input[i];

			return max;
		}
				
		static inline float Limit(float input, float min, float max)
		{
			int above = (input > min);
			int notAbove = above != 1;
			input = above * input + notAbove * min;

			int below = (input < max);
			int notBelow = below != 1;
			input = below * input + notBelow * max;

			return input;
		}

		static inline float LimitMin(float input, float min)
		{
			int above = (input > min);
			int notAbove = above != 1;
			return above * input + notAbove * min;
		}

		static inline float LimitMax(float input, float max)
		{
			int below = (input < max);
			int notBelow = below != 1;
			return below * input + notBelow * max;
		}

		static inline float TanhPoly(float x)
		{
			float sign = (float)(-1.0 + 2 * (x >= 0));

			x = x * sign;
			float xSquare = x * x;
			float xCube = xSquare * x;
			float result = 1.0f - 1.0f / (1.0f + x + xSquare + xCube);
			return result * sign;
		}

		static inline float TanhLookup(float x)
		{
			int i = (int)(x * 10000 + 30000);
			int overZero = i > 0;
			int underMax = i < TanhTableSize - 1;
			i = i * overZero;
			i = i * underMax + !underMax * (TanhTableSize - 1);
			return tanhTable[i];
		}

		static inline float CubicNonlin(float x)
		{
			// Eq1: x - a * x^3
			// derivative is 
			// Eq2: 1 - 3a * x^2
			// saturation at +-1.5
			// solving for Eq1 = 1, Eq2 = 0, we get
			// = 0.5 / (3*(1.5)^2-(1.5)^3)
			// == 0.148148148148148

			const float a = 0.148148148148148f;
			int s1 = x > -1.5f;
			int s2 = x > 1.5f;

			float y = x - a * x*x*x;
			float vals[3] = { -1.0f, y, 1.0f };
			return vals[s1 + s2];
		};

		static inline void GainAndSum(float* source, float* dest, float gain, int len)
		{
			for (int i = 0; i < len; i++)
				dest[i] += source[i] * gain;
		}

		template<typename T>
		static double DB2gain(T input)
		{
			return std::pow(10, input / 20.0);
		}

		template<typename T>
		static double Gain2DB(T input)
		{
			if (input < 0.0000001)
				return -100000;

			return 20.0f * std::log10(input);
		}

		template<typename T>
		static double Average(T* input, int len)
		{
			double ave = 0;
			for (int i = 0; i < len; i++)
				ave += input[i];

			return (T)(ave / (double)len);
		}

		template<typename T>
		static double Rms(T* data, int len)
		{
			double sum = 0.0;

			for (int i = 0; i < len; i++)
			{
				sum += data[i] * data[i];
			}

			return std::sqrt(sum / len);
		}

		template<typename T>
		static void ReverseInPlace(T* data, int len)
		{
			for (int i = 0; i < len / 2; i++)
			{
				auto temp = data[i];
				data[i] = data[len - 1 - i];
				data[len - 1 - i] = temp;
			}
		}

		static inline double ComputeLpAlpha(double fc, double ts)
		{
			return (2 * M_PI * ts * fc) / (2 * M_PI * ts * fc + 1);
		}

		template<typename T>
		static std::string ToMatlabString(double fs, std::vector<T> b, std::vector<T> a)
		{
			std::stringstream bStream;
			std::stringstream aStream;
			bStream << std::setprecision(15);
			aStream << std::setprecision(15);

			for (size_t i = 0; i < b.size(); i++)
			{
				if (i != 0)
					bStream << ", ";

				bStream << b.at(i);
			}

			for (size_t i = 0; i < a.size(); i++)
			{
				if (i != 0)
					aStream << ", ";

				aStream << a.at(i);
			}

			std::string str;
			str += "b = [" + bStream.str() + "];\r\n";
			str += "a = [" + aStream.str() + "];\r\n";
			str += "h = fvtool(b, a);\r\n";
			str += "set(h, 'Fs', " + std::to_string(fs) + ");\r\n";
			str += "set(h, 'FrequencyScale', 'log');\r\n";

			return str;
		}

		template<typename T>
		static std::string Float2String(T value, int precision)
		{
			std::stringstream str;
			str << std::fixed << std::setprecision(precision) << value;
			return str.str();
		}

		template<typename T>
		static std::vector<T> Linspace(double min, double max, int num)
		{
			double space = (max - min) / (num - 1);
			std::vector<T> output;
			output.push_back(min);
			T runningVal = min;
			for (int i = 1; i<num; i++)
			{
				runningVal = runningVal + space;
				output.push_back(runningVal);
			}

			return output;
		}

		template<typename T>
		static std::vector<T> Octavespace(double start, double max, int pointsPerOctave)
		{
			std::vector<T> output;
			auto pto = (double)pointsPerOctave;

			int i = 0;
			while (true)
			{
				double n = i / pto;
				auto val = start * std::pow(2, n);
				if (val > max)
					break;

				output.push_back(val);
				i++;
			}

			return output;
		}

		template<typename T>
		static std::vector<double> Decadespace(double start, double max, int pointsPerDecade)
		{
			std::vector<T> output;
			auto ptd = (double)pointsPerDecade;

			int i = 0;
			while (true)
			{
				double n = i / ptd;
				auto val = start * std::pow(10, n);
				if (val > max)
					break;

				output.push_back(val);
				i++;
			}

			return output;
		}
	};
}

#endif
