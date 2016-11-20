#pragma once
#include <vector>

namespace AudioLib
{
	class Conv
	{
	
	public:

		static std::vector<double> Convolution(std::vector<double> h, std::vector<double> g)
		{
			std::vector<double> output(h.size() + g.size() - 1);

			// To minimize the number of MAC-operations, split the loop in two
			// parts, use different algorithms on each side
			int lg = g.size();
			int lh = h.size();
			for (int i = 0; i < output.size(); i++)
			{
				for (int j = 0; j <= i; j++)
				{
					if (j < lh && (i - j) < lg)
						output[i] += h[j] * g[i - j];
				}

				// System.out.println(""+output[i]);
			}

			return output;
		}

	};


}