
#include "stdafx.h"
#include "InteractorCB_MPR.h"

using namespace std;

CInteractorCB_MPR::CInteractorCB_MPR()
{

}

// ���콺 �� Ű���� ���� �ڵ鷯 �Լ�
void CInteractorCB_MPR::Execute(vtkObject *Caller, unsigned long event, void *callData)
{
	auto pInter = GetInteractor();
	if (nullptr == pInter) return;

	auto style = GetIStyle();
	if (0 == style) return;

	auto pRenderer = GetRenderer();
	if (nullptr == pRenderer) return;

	bool bRedraw = false;

	if (vtkCommand::KeyPressEvent == event)
	{
		char key = pInter->GetKeyCode();
		key;
	}
	else if (vtkCommand::LeftButtonPressEvent == event) // ���콺 �� ��ư ����
	{
		//StartMouseAction(EMB_ROTATE); // ȸ�� Operation ����		

		//style->OnLeftButtonDown();
	}
	else if (vtkCommand::LeftButtonReleaseEvent == event) // ���콺 �� ��ư ����
	{
		//EndMouseAction();

		//style->OnLeftButtonUp();
	}
	else if (vtkCommand::MiddleButtonPressEvent == event) // ���콺 M ��ư ����
	{
		StartMouseAction(EMB_PAN); // �̵� Operation ����

		style->OnMiddleButtonDown();
	}
	else if (vtkCommand::MiddleButtonReleaseEvent == event) // ���콺 M ��ư ����
	{
		EndMouseAction();

		style->OnMiddleButtonUp();
	}
	else if (vtkCommand::RightButtonPressEvent == event) // ���콺 ���� ��ư ����
	{
		StartMouseAction(EMB_ZOOM); // Ȯ��, ��� Operation ����

		style->OnRightButtonDown();
	}
	else if (vtkCommand::RightButtonReleaseEvent == event) // ���콺 ���� ��ư ����
	{
		EndMouseAction();

		style->OnRightButtonUp();
	}
	else if (vtkCommand::MouseMoveEvent == event) // ���콺 ������ ó��
	{
		MoveMouseAction();

		style->OnMouseMove();
	}

	if (bRedraw)
	{
		pRenderer->ResetCameraClippingRange();
		pInter->Render();
	}
}

