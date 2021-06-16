
#pragma once

#include <vtkCommand.h>

#ifndef VTK_NEW_MACRO
#define VTK_NEW_MACRO(x) static x *New() { return new x; }
#endif

class vtkRenderer;
class vtkInteractorStyle;
class vtkRenderWindowInteractor;

// VTK Rendering Interator 의 Base Callback
class CInteractorCB_Base : public vtkCommand
{
public:
	VTK_NEW_MACRO(CInteractorCB_Base);
	void SetInteractor(vtkRenderWindowInteractor *pInter);
	
	vtkRenderer* GetRenderer(int idx = 0);
	vtkInteractorStyle* GetIStyle();
	vtkRenderWindowInteractor* GetInteractor();

	enum E_MOUSE_ACTION_STATE // 마우스 조작 상태 플래그
	{
		EMB_NONE = 0, // Idle
		EMB_ROTATE, // 회전
		EMB_PAN, // 이동
		EMB_ZOOM, // 확대, 축소
	};

	void StartMouseAction(E_MOUSE_ACTION_STATE nState); // 마우스 조작 상태 시작 설정
	bool MoveMouseAction(); // 설정된 마우스 조작 상태로 마우스 움직임 처리
	void EndMouseAction(bool bResetState = true); // 설정된 마우스 조작 상태의 종료 설정

	// 설정된 마우스 조작 상태 반환
	E_MOUSE_ACTION_STATE GetMouseAction() const;

protected:

	// 마우스 및 키보드 조작 핸들러 함수
	virtual void Execute(vtkObject *Caller, unsigned long event, void *callData);
	
	CInteractorCB_Base(); // 생성자 및 초기화 루틴
	
protected:
		
	vtkRenderWindowInteractor *_pInteractor;
	E_MOUSE_ACTION_STATE m_nMouseState;
};
