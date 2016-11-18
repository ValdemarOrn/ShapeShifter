#pragma once

#include "public.sdk/source/vst2.x/audioeffectx.h"
#include "EffectKernel.h"

using namespace DspAmp;

class VstComponent : public AudioEffectX
{
protected:
	char programName[kVstMaxProgNameLen + 1];
	EffectKernel* instance;
	
public:
	VstComponent(audioMasterCallback audioMaster);
	~VstComponent();
	
	bool VstComponent::getInputProperties(VstInt32 index, VstPinProperties* properties);
	bool VstComponent::getOutputProperties(VstInt32 index, VstPinProperties* properties);

	// Programs
	virtual void setProgramName (char* name);
	virtual void getProgramName (char* name);

	// Parameters
	virtual void setParameter (VstInt32 index, float value);
	virtual float getParameter (VstInt32 index);
	virtual void getParameterLabel (VstInt32 index, char* label);
	virtual void getParameterDisplay (VstInt32 index, char* text);
	virtual void getParameterName (VstInt32 index, char* text);

	// Metadata
	virtual bool getEffectName (char* name);
	virtual bool getVendorString (char* text);
	virtual bool getProductString (char* text);
	virtual VstInt32 getVendorVersion ();

	// Processing
	virtual void processReplacing(float** inputs, float** outputs, VstInt32 sampleFrames);
	virtual void setSampleRate(float sampleRate);
	void createDevice();


};

