#include "EffectKernel.h"
#include "Osc/OscMessage.h"
#include <iostream>
#include "Parameters.h"

namespace DspAmp
{
	EffectKernel::EffectKernel(double fs, int bufferSize)
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

	void EffectKernel::Process(float** inputs, float** outputs, int bufferSize)
	{
		for (int i = 0; i < bufferSize; i++)
		{
			outputs[0][i] = inputs[0][i];
			outputs[1][i] = inputs[1][i];
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

		if (triggerCallback && ParameterUpdateCallback)
			ParameterUpdateCallback(parameter, value);
	}

	void EffectKernel::SetParameter(int index, float value, bool triggerCallback)
	{
		SetParameter(IndexToParameter[index], value, triggerCallback);
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
