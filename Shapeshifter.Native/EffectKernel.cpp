#include "EffectKernel.h"
#include "Osc/OscMessage.h"
#include <iostream>
#include "Parameters.h"
#include "AudioLib/ValueTables.h"

namespace Shapeshifter
{
	EffectKernel::EffectKernel(double fs, int bufferSize)
		: noiseGate(fs)
		, boost(fs, bufferSize)
	{
		buffer1 = new float[bufferSize];
		buffer2 = new float[bufferSize];
		this->fs = fs;
		this->bufferSize = bufferSize;
		isClosing = false;
		udpTranceiver = new Osc::UdpTranceiver(1800, 1801);
		messageListenerThread = std::thread(&EffectKernel::MessageListener, this);

		SetupParameters();
	}

	EffectKernel::~EffectKernel()
	{
		isClosing = true;
		messageListenerThread.join();

		delete buffer1;
		delete buffer2;
		delete udpTranceiver;
		udpTranceiver = 0;
	}

	void EffectKernel::Process(float** inputs, float** outputs, int totalBufferSize)
	{
		for (int i = 0; i < totalBufferSize; i += bufferSize)
		{
			auto size = i + bufferSize > totalBufferSize ? totalBufferSize - i : bufferSize;

			float* outL = &outputs[0][i];
			float* outR = &outputs[1][i];
			float* inL = &inputs[0][i];
			float* inR = &inputs[1][i];
			ProcessBuffer(inL, inR, outL, outR, size);
		}
	}

	void EffectKernel::ProcessBuffer(float* inL, float* inR, float* outL, float* outR, int count)
	{
		//noiseGate.Process(inL, inL, buffer1, count);
		boost.Process(inL, buffer2, count);

		Utils::Copy(buffer2, outL, count);
		Utils::Copy(buffer2, outR, count);

		/*for (int i = 0; i < count; i++)
		{
			outL[i] = AudioLib::Utils::Limit(outL[i] * 20, -1, 1);
			outR[i] = AudioLib::Utils::Limit(outR[i] * 20, -1, 1);

			//outL[i] = inL[i];
			//outR[i] = inR[i];
		}*/
	}

	ParameterState EffectKernel::GetParameterState(Parameter parameter)
	{
		return ParameterStates[parameter.GetKey()];
	}

	ParameterState EffectKernel::GetParameterState(int index)
	{
		return GetParameterState(IndexToParameter[index]);
	}

	void EffectKernel::SetParameter(Parameter parameter, float value, bool updateHost, bool updateGui)
	{
		auto key = parameter.GetKey();
		ParameterStates[key].Value  = value;
		bool updated = false;

		if (parameter.Module == EffectModule::NoiseGate)
		{
			updated = ApplyNoiseGateParameter(parameter.ParameterIndex, value);
		}
		else if (parameter.Module == EffectModule::Boost)
		{
			updated = ApplyBoostParameter(parameter.ParameterIndex, value);
		}

		if (!updated)
			return;

		if (updateHost && ParameterUpdateCallback)
		{
			ParameterUpdateCallback(parameter, value);
		}

		if (updateGui)
		{
			std::string address = "/";
			address += Parameters::ModuleNames[parameter.Module];
			address += "/";
			address += std::to_string(parameter.ParameterIndex);

			Osc::OscMessage msg(address);
			msg.SetFloat(ParameterStates[key].Value);
			msg.SetString(ParameterStates[key].Display);
			udpTranceiver->Send(msg.GetBytes());
		}
	}

	void EffectKernel::SetParameter(int index, float value, bool updateHost, bool updateGui)
	{
		SetParameter(IndexToParameter[index], value, updateHost, updateGui);
	}

	// ------------------------------------------------------------------------------------


	bool EffectKernel::ApplyNoiseGateParameter(int index, float value)
	{
		auto key = Parameter(EffectModule::NoiseGate, index).GetKey();
		auto p = static_cast<ParametersNoiseGate>(index);
		double tempVal = 0.0;
		bool update = true;

		switch (p)
		{
		case ParametersNoiseGate::DetectorGain:
			tempVal = 40 * value - 20;
			noiseGate.DetectorGain = Utils::DB2gain(tempVal);
			ParameterStates[key].Display = Utility::SPrint("%.1f dB", tempVal);
			break;
		case ParametersNoiseGate::ReductionDb:
			noiseGate.ReductionDb = -value * 100;
			ParameterStates[key].Display = Utility::SPrint("%.1f dB", noiseGate.ReductionDb);
			break;
		case ParametersNoiseGate::ReleaseMs:
			noiseGate.ReleaseMs = 10 + ValueTables::Get(value, ValueTables::Response2Dec) * 990;
			ParameterStates[key].Display = Utility::SPrint("%.0f ms", noiseGate.ReleaseMs);
			break;
		case ParametersNoiseGate::Slope:
			noiseGate.Slope = 1.0f + ValueTables::Get(value, ValueTables::Response2Dec) * 49;
			ParameterStates[key].Display = Utility::SPrint("%.2f", noiseGate.Slope);
			break;
		case ParametersNoiseGate::ThresholdDb:
			noiseGate.ThresholdDb = -ValueTables::Get(1 - value, ValueTables::Response2Oct) * 80;
			ParameterStates[key].Display = Utility::SPrint("%.1f dB", noiseGate.ThresholdDb);
			break;
		default:
			update = false;
		}

		if (update)
			noiseGate.UpdateAll();

		return update;
	}

	bool EffectKernel::ApplyBoostParameter(int index, float value)
	{
		auto key = Parameter(EffectModule::Boost, index).GetKey();
		auto p = static_cast<ParametersBoost>(index);
		double tempVal = 0.0;
		bool update = true;

		switch (p)
		{
		case ParametersBoost::Drive:
			boost.SetDrive(value);
			ParameterStates[key].Display = Utility::SPrint("%.2f", value * 10);
			break;
		case ParametersBoost::Tone:
			boost.SetTone(value);
			ParameterStates[key].Display = Utility::SPrint("%.2f", value * 10);
			break;
		case ParametersBoost::Mix:
			boost.Mix = value;
			ParameterStates[key].Display = Utility::SPrint("%.0f %", value * 100);
			break;
		case ParametersBoost::Tightness:
			boost.SetTightness(value);
			ParameterStates[key].Display = Utility::SPrint("%.2f", value * 10);
			break;
		case ParametersBoost::Clipper:
			boost.SetClipper((Boost::ClipperType)(int)(value * 3.999));
			ParameterStates[key].Display = boost.GetClipperName();
			break;
		
		default:
			update = false;
		}

		if (update)
			noiseGate.UpdateAll();

		return update;
	}


	// ------------------------------------------------------------------------------------

	void EffectKernel::MessageListener()
	{
		auto sleepTime = std::chrono::milliseconds(2);

		while (!isClosing)
		{
			try
			{
				while (true)
				{
					auto data = udpTranceiver->Receive();
					if (data.size() == 0)
						break;

					try
					{
						auto oscMsgs = Osc::OscMessage::ParseRawBytes(data);
						auto oscMsg = oscMsgs.at(0);

						float value = oscMsg.GetFloat(0);
						auto param = Parameters::ParseOsc(oscMsg.Address);
						SetParameter(param, value, true, true);
					}
					catch (std::exception ex)
					{
						try
						{
							std::cout << ex.what() << std::endl;
							Osc::OscMessage oscMsg("/Control/ErrorMessage");
							oscMsg.SetString(std::string("An Error occurred while processing an OSC message:\n") + ex.what());
							udpTranceiver->Send(oscMsg.GetBytes());
						}
						catch (std::exception ex2)
						{
							std::cout << ex2.what() << std::endl;
						}
					}
				}
			}
			catch (std::exception ex)
			{
				std::cout << ex.what() << std::endl;
			}

			std::this_thread::sleep_for(sleepTime);
		}
	}

	void EffectKernel::SetupParameters()
	{
		auto index = 0;
		
		for (int i = (int)EffectModule::NoiseGate; i <= (int)EffectModule::Global; i++)
		{
			auto module = (EffectModule)i;

			for (size_t i = 0; i < Parameters::ModuleParamCount[module]; i++)
			{
				Parameter p(module, i);
				auto key = p.GetKey();

				ParameterStates[key].Index = index;
				ParameterStates[key].Name = Parameters::ModuleNames[module] + " " + std::to_string(i);
				IndexToParameter[index] = key;
				SetParameter(p, 0.0, true, true);
				index++;
			}
		}

		ParameterCount = index;
	}
}
