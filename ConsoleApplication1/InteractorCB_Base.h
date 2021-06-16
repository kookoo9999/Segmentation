
#pragma once

#include <vtkCommand.h>

#ifndef VTK_NEW_MACRO
#define VTK_NEW_MACRO(x) static x *New() { return new x; }
#endif

class vtkRenderer;
class vtkInteractorStyle;
class vtkRenderWindowInteractor;

// VTK Rendering Interator �� Base Callback
class CInteractorCB_Base : public vtkCommand
{
public:
	VTK_NEW_MACRO(CInteractorCB_Base);
	void SetInteractor(vtkRenderWindowInteractor *pInter);
	
	vtkRenderer* GetRenderer(int idx = 0);
	vtkInteractorStyle* GetIStyle();
	vtkRenderWindowInteractor* GetInteractor();

	enum E_MOUSE_ACTION_STATE // ���콺 ���� ���� �÷���
	{
		EMB_NONE = 0, // Idle
		EMB_ROTATE, // ȸ��
		EMB_PAN, // �̵�
		EMB_ZOOM, // Ȯ��, ���
	};

	void StartMouseAction(E_MOUSE_ACTION_STATE nState); // ���콺 ���� ���� ���� ����
	bool MoveMouseAction(); // ������ ���콺 ���� ���·� ���콺 ������ ó��
	void EndMouseAction(bool bResetState = true); // ������ ���콺 ���� ������ ���� ����

	// ������ ���콺 ���� ���� ��ȯ
	E_MOUSE_ACTION_STATE GetMouseAction() const;

protected:

	// ���콺 �� Ű���� ���� �ڵ鷯 �Լ�
	virtual void Execute(vtkObject *Caller, unsigned long event, void *callData);
	
	CInteractorCB_Base(); // ������ �� �ʱ�ȭ ��ƾ
	
protected:
		
	vtkRenderWindowInteractor *_pInteractor;
	E_MOUSE_ACTION_STATE m_nMouseState;
};
