#include "EffectKernel.h"
#include "Osc/OscMessage.h"
#include <iostream>
#include "Parameters.h"
#include "AudioLib/ValueTables.h"

namespace DspAmp
{
	EffectKernel::EffectKernel(double fs, int bufferSize)
		: noiseGate(fs)
	{
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
		noiseGate.Process(inL, inR, inL, outL, outR, count);

		for (int i = 0; i < count; i++)
		{
			outL[i] = AudioLib::Utils::Limit(outL[i] * 20, -1, 1);
			outR[i] = AudioLib::Utils::Limit(outR[i] * 20, -1, 1);

			//outL[i] = inL[i];
			//outR[i] = inR[i];
		}
	}

	ParameterState EffectKernel::GetParameterState(Parameter parameter)
	{
		return ParameterStates[parameter.GetKey()];
	}

	ParameterState EffectKernel::GetParameterState(int index)
	{
		return GetParameterState(IndexToParameter[index]);
	}

	void EffectKernel::SetParameter(Parameter parameter, float value, bool triggerCallback)
	{
		ParameterStates[parameter.GetKey()].Value  = value;
		ParameterStates[parameter.GetKey()].Display = Utility::SPrint("%.2f", value);
		bool updated = false;

		if (parameter.Module == EffectModule::NoiseGate)
		{
			updated = ApplyNoiseGateParameter(parameter.ParameterIndex, value);
		}


		if (updated && triggerCallback && ParameterUpdateCallback)
			ParameterUpdateCallback(parameter, value);
	}

	void EffectKernel::SetParameter(int index, float value, bool triggerCallback)
	{
		SetParameter(IndexToParameter[index], value, triggerCallback);
	}

	// ------------------------------------------------------------------------------------


	bool EffectKernel::ApplyNoiseGateParameter(int index, float value)
	{
		auto p = static_cast<ParametersNoiseGate>(index);
		bool update = true;
		switch (p)
		{
		case ParametersNoiseGate::DetectorGain:
			noiseGate.DetectorGain = Utils::DB2gain(40 * value - 20);
			break;
		case ParametersNoiseGate::ReductionDb:
			noiseGate.ReductionDb = -value * 100;
			break;
		case ParametersNoiseGate::ReleaseMs:
			noiseGate.ReleaseMs = 10 + ValueTables::Get(value, ValueTables::Response2Dec) * 990;
			break;
		case ParametersNoiseGate::Slope:
			noiseGate.Slope = 1.0f + ValueTables::Get(value, ValueTables::Response2Dec) * 50;
			break;
		case ParametersNoiseGate::ThresholdDb:
			noiseGate.ThresholdDb = -ValueTables::Get(1 - value, ValueTables::Response2Oct) * 80;
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
						auto oscMsgs = Polyhedrus::OscMessage::ParseRawBytes(data);
						auto oscMsg = oscMsgs.at(0);

						float value = oscMsg.GetFloat(0);
						auto param = Parameters::ParseOsc(oscMsg.Address);
						SetParameter(param, value, true);
					}
					catch (std::exception ex)
					{
						try
						{
							std::cout << ex.what() << std::endl;
							Polyhedrus::OscMessage oscMsg("/Control/ErrorMessage");
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
				SetParameter(p, 0.0, true);
				index++;
			}
		}

		ParameterCount = index;
	}
}
