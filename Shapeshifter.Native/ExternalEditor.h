#pragma once

#ifndef __aeffeditor__
#include "aeffeditor.h"
#endif

//#include "VstComponent.h"
#include <windows.h>

class ExternalEditor : public AEffEditor
{
public:
	//VstComponent* instance;

	ExternalEditor(AudioEffect* effect);
	virtual ~ExternalEditor();

	virtual bool open(void* ptr);
	virtual void close();
	virtual bool isOpen() { return Open; }

	bool Open;
	bool getRect(ERect** rect);

protected:
	ERect rect;
};
