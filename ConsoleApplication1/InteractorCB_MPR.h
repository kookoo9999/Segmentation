
#pragma once

#include "InteractorCB_Base.h"

// MPR View 전용 Interactor Class
class CInteractorCB_MPR : public CInteractorCB_Base
{
public:
	VTK_NEW_MACRO(CInteractorCB_MPR);

protected:

	// 마우스 및 키보드 조작 핸들러 함수
	virtual void Execute(vtkObject *Caller, unsigned long event, void *callData);

	CInteractorCB_MPR();

};