
#pragma once

#include "InteractorCB_Base.h"

// MPR View ���� Interactor Class
class CInteractorCB_MPR : public CInteractorCB_Base
{
public:
	VTK_NEW_MACRO(CInteractorCB_MPR);

protected:

	// ���콺 �� Ű���� ���� �ڵ鷯 �Լ�
	virtual void Execute(vtkObject *Caller, unsigned long event, void *callData);

	CInteractorCB_MPR();

};