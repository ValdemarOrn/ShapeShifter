#pragma once
#include <vector>

namespace AudioLib
{
	class TfUtil
	{
	private:
		int order;
		std::vector<double> zb;
		std::vector<double> za;

		double x1, x2, x3, x4, x5, x6 ;
		double y1, y2, y3, y4, y5, y6;

	public:

		TfUtil(int order)
		{
			auto zb = std::vector<double>(order + 1);
			auto za = std::vector<double>(order + 1);
			this->order = zb.size() - 1;

			if (za.size() != zb.size())
				throw new std::exception("lengths of arrays must match");

			this->zb = zb;
			this->za = za;
			x1 = x2 = x3 = x4 = x5 = x6 = 0;
			y1 = y2 = y3 = y4 = y5 = y6 = 0;
		}

		TfUtil(std::vector<double> zb, std::vector<double> za)
		{
			order = zb.size() - 1;
			if (za.size() != zb.size())
				throw new std::exception("lengths of arrays must match");

			this->zb = zb;
			this->za = za;
			x1 = x2 = x3 = x4 = x5 = x6 = 0;
			y1 = y2 = y3 = y4 = y5 = y6 = 0;
		}

		int GetOrder() { return order; }

		void Update(std::vector<double> newZb, std::vector<double> newZa)
		{
			if (newZb.size() - 1 != order || newZb.size() != newZa.size())
				throw new std::exception("Length must match order must match");

			zb = newZb;
			za = newZa;
		}

		double Process1(double x0, bool updateInternalState = true)
		{
			double y0 = zb[0] * x0 + zb[1] * x1
				- za[1] * y1;

			if (updateInternalState)
			{
				y1 = y0;

				x1 = x0;
			}

			return y0;
		}

		double Process2(double x0, bool updateInternalState = true)
		{
			double y0 = zb[0] * x0 + zb[1] * x1 + zb[2] * x2
				- za[1] * y1 - za[2] * y2;

			if (updateInternalState)
			{
				y2 = y1;
				y1 = y0;

				x2 = x1;
				x1 = x0;
			}

			return y0;
		}

		double Process3(double x0, bool updateInternalState = true)
		{
			double y0 = zb[0] * x0 + zb[1] * x1 + zb[2] * x2 + zb[3] * x3
				- za[1] * y1 - za[2] * y2 - za[3] * y3;

			if (updateInternalState)
			{
				y3 = y2;
				y2 = y1;
				y1 = y0;

				x3 = x2;
				x2 = x1;
				x1 = x0;
			}

			return y0;
		}

		double Process4(double x0, bool updateInternalState = true)
		{
			double y0 = zb[0] * x0 + zb[1] * x1 + zb[2] * x2 + zb[3] * x3 + zb[4] * x4
				- za[1] * y1 - za[2] * y2 - za[3] * y3 - za[4] * y4;

			if (updateInternalState)
			{
				y4 = y3;
				y3 = y2;
				y2 = y1;
				y1 = y0;

				x4 = x3;
				x3 = x2;
				x2 = x1;
				x1 = x0;
			}

			return y0;
		}

		double Process5(double x0, bool updateInternalState = true)
		{
			double y0 = zb[0] * x0 + zb[1] * x1 + zb[2] * x2 + zb[3] * x3 + zb[4] * x4 + zb[5] * x5
				- za[1] * y1 - za[2] * y2 - za[3] * y3 - za[4] * y4 - za[5] * y5;

			if (updateInternalState)
			{
				y5 = y4;
				y4 = y3;
				y3 = y2;
				y2 = y1;
				y1 = y0;

				x5 = x4;
				x4 = x3;
				x3 = x2;
				x2 = x1;
				x1 = x0;
			}

			return y0;
		}

		double Process6(double x0, bool updateInternalState = true)
		{
			double y0 = zb[0] * x0 + zb[1] * x1 + zb[2] * x2 + zb[3] * x3 + zb[4] * x4 + zb[5] * x5 + zb[6] * x6
				- za[1] * y1 - za[2] * y2 - za[3] * y3 - za[4] * y4 - za[5] * y5 - za[6] * y6;

			if (updateInternalState)
			{
				y6 = y5;
				y5 = y4;
				y4 = y3;
				y3 = y2;
				y2 = y1;
				y1 = y0;

				x6 = x5;
				x5 = x4;
				x4 = x3;
				x3 = x2;
				x2 = x1;
				x1 = x0;
			}

			return y0;
		}
	};


}