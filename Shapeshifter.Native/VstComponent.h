#pragma once

#include "public.sdk/source/vst2.x/audioeffectx.h"
#include "EffectKernel.h"

using namespace Shapeshifter;

class VstComponent : public AudioEffectX
{
protected:
	char programName[kVstMaxProgNameLen + 1];
	EffectKernel* instance;
	
public:
	VstComponent(audioMasterCallback audioMaster);
	~VstComponent();
	
	bool VstComponent::getInputProperties(VstInt32 index, VstPinProperties* properties) override;
	bool VstComponent::getOutputProperties(VstInt32 index, VstPinProperties* properties) override;

	// Programs
	virtual void setProgramName (char* name) override;
	virtual void getProgramName (char* name) override;

	// Parameters
	virtual void setParameter (VstInt32 index, float value) override;
	virtual float getParameter (VstInt32 index) override;
	virtual void getParameterLabel (VstInt32 index, char* label) override;
	virtual void getParameterDisplay (VstInt32 index, char* text) override;
	virtual void getParameterName (VstInt32 index, char* text) override;

	// Metadata
	virtual bool getEffectName (char* name) override;
	virtual bool getVendorString (char* text) override;
	virtual bool getProductString (char* text) override;
	virtual VstInt32 getVendorVersion () override;

	// Processing
	virtual void processReplacing(float** inputs, float** outputs, VstInt32 sampleFrames) override;
	virtual void setSampleRate(float sampleRate) override;
	void createDevice();


};

