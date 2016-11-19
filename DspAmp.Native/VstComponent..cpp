#include "VstComponent.h"
#include "ExternalEditor.h"
#include "AudioLib/Utils.h"
#include "AudioLib/ValueTables.h"

using namespace AudioLib;

const char* PluginName = "DSP Amp";
const char* DeveloperName = "Valdemar Erlingsson";

AudioEffect* createEffectInstance (audioMasterCallback audioMaster)
{
	Utils::Initialize();
	ValueTables::Init();
	return new VstComponent(audioMaster);
}

// ------------------------------------------------------------------------------------

VstComponent::VstComponent(audioMasterCallback audioMaster)
	: AudioEffectX (audioMaster, 1, Parameters::GetParameterCount())
{
	instance = 0;
	setNumInputs(2); // 1x stereo in
	setNumOutputs(2); // 1x stereo out
	setUniqueID(91572787); // Random ID
	canProcessReplacing();
	canDoubleReplacing(false);
	isSynth(false);
	vst_strncpy (programName, PluginName, kVstMaxProgNameLen);

	// LEAKS!!!
	//ExternalEditor* ed = new ExternalEditor(this);
	//this->setEditor(ed);

	sampleRate = 48000;
	createDevice();
}

VstComponent::~VstComponent()
{
}

bool VstComponent::getInputProperties(VstInt32 index, VstPinProperties* properties)
{
	if (index == 0 || index == 1) // 0-1 = Main in
	{
		properties->arrangementType = kSpeakerArrStereo;
		properties->flags = kVstPinIsStereo;
		sprintf(properties->shortLabel, "Main In");
		sprintf(properties->label, "Main Input");
		return true;
	}
	else
	{
		return false;
	}
}

bool VstComponent::getOutputProperties(VstInt32 index, VstPinProperties* properties)
{
	if (index == 0 || index == 1) // 0-1 = Main out
	{
		properties->arrangementType = kSpeakerArrStereo;
		properties->flags = kVstPinIsStereo;
		sprintf(properties->shortLabel, "Output");
		sprintf(properties->label, "Main Output");
		return true;
	}
	else
	{
		return false;
	}
}

void VstComponent::setProgramName(char* name)
{
	vst_strncpy(programName, name, kVstMaxProgNameLen);
}

void VstComponent::getProgramName(char* name)
{
	vst_strncpy(name, programName, kVstMaxProgNameLen);
}

void VstComponent::setParameter(VstInt32 index, float value)
{
	instance->SetParameter(index, value, false, true);
}

float VstComponent::getParameter(VstInt32 index)
{
	return instance->GetParameterState(index).Value;
}

void VstComponent::getParameterName(VstInt32 index, char* label)
{
	strcpy(label, instance->GetParameterState(index).Name.c_str());
}

void VstComponent::getParameterDisplay(VstInt32 index, char* text)
{
	strcpy(text, instance->GetParameterState(index).Display.c_str());
}

void VstComponent::getParameterLabel(VstInt32 index, char* label)
{
	strcpy(label, "");
}

bool VstComponent::getEffectName(char* name)
{
	vst_strncpy(name, PluginName, kVstMaxEffectNameLen);
	return true;
}

bool VstComponent::getProductString(char* text)
{
	vst_strncpy(text, PluginName, kVstMaxProductStrLen);
	return true;
}

bool VstComponent::getVendorString(char* text)
{
	vst_strncpy(text, DeveloperName, kVstMaxVendorStrLen);
	return true;
}

VstInt32 VstComponent::getVendorVersion()
{ 
	return 1000; 
}

void VstComponent::processReplacing(float** inputs, float** outputs, VstInt32 sampleFrames)
{
	instance->Process(inputs, outputs, sampleFrames);
}

void VstComponent::setSampleRate(float sampleRate)
{
	this->sampleRate = sampleRate;
	createDevice();
}

void VstComponent::createDevice()
{
	std::vector<float> values;

	if (instance != 0)
	{
		for (size_t i = 0; i < numParams; i++)
			values.push_back(getParameter(i));

		delete instance;
	}

	instance = new EffectKernel(sampleRate, 64);
	instance->ParameterUpdateCallback = [&](Parameter para, float val)
	{
		// this maps the PArameter back to the vst param index and sends update to host (From gui, usually)
		auto key = para.GetKey();
		int vstParam = -1;
		for (auto kvp : instance->IndexToParameter)
		{
			if (kvp.second.GetKey() == key)
			{
				vstParam = kvp.first;
				break;
			}
		}

		if (vstParam != -1)
			this->setParameterAutomated(vstParam, val);

	};
	// re-apply parameters
	for (size_t i = 0; i < values.size(); i++)
		setParameter(i, values[i]);
}

