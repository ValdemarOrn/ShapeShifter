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
	: AudioEffectX (audioMaster, 1, (int)Parameters::Count)
{
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

	parameters[(int)Parameters::Param0] = 0.0;
	parameters[(int)Parameters::Param1] = 0.0;
	
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
	parameters[index] = value;
	bool update = true;

	switch ((Parameters)index)
	{
	case Parameters::Param0:
		;
		break;
	case Parameters::Param1:
		;
		break;
	default:
		update = false;
	}

	if (update)
	{

	}
}

float VstComponent::getParameter(VstInt32 index)
{
	return parameters[index];
}

void VstComponent::getParameterName(VstInt32 index, char* label)
{
	switch ((Parameters)index)
	{
	case Parameters::Param0:
	case Parameters::Param1:
		strcpy(label, "Parameter");
		break;
	}
}

void VstComponent::getParameterDisplay(VstInt32 index, char* text)
{
	switch ((Parameters)index)
	{
	case Parameters::Param0:
	case Parameters::Param1:
		sprintf(text, "%.2f", parameters[index]);
		break;
	}
}

void VstComponent::getParameterLabel(VstInt32 index, char* label)
{
	switch ((Parameters)index)
	{
	case Parameters::Param0:
	case Parameters::Param1:
		strcpy(label, "");
		break;
	}
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
	//setParameterAutomated((int)Parameters::CurrentGain, kernel->currentGainDb / 150 + 1);
}

void VstComponent::setSampleRate(float sampleRate)
{
	this->sampleRate = sampleRate;
	createDevice();
}

void VstComponent::createDevice()
{
	delete instance;
	instance = new EffectKernel(sampleRate, 64);

	// re-apply parameters
	for (size_t i = 0; i < (int)Parameters::Count; i++)
	{
		setParameter(i, parameters[i]);
	}
}

