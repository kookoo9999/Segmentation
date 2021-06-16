
#include "stdafx.h"
#include "InteractorCB_MPR.h"

using namespace std;

CInteractorCB_MPR::CInteractorCB_MPR()
{

}

// 마우스 및 키보드 조작 핸들러 함수
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
	else if (vtkCommand::LeftButtonPressEvent == event) // 마우스 왼 버튼 눌림
	{
		//StartMouseAction(EMB_ROTATE); // 회전 Operation 설정		

		//style->OnLeftButtonDown();
	}
	else if (vtkCommand::LeftButtonReleaseEvent == event) // 마우스 왼 버튼 떼임
	{
		//EndMouseAction();

		//style->OnLeftButtonUp();
	}
	else if (vtkCommand::MiddleButtonPressEvent == event) // 마우스 M 버튼 눌림
	{
		StartMouseAction(EMB_PAN); // 이동 Operation 설정

		style->OnMiddleButtonDown();
	}
	else if (vtkCommand::MiddleButtonReleaseEvent == event) // 마우스 M 버튼 떼임
	{
		EndMouseAction();

		style->OnMiddleButtonUp();
	}
	else if (vtkCommand::RightButtonPressEvent == event) // 마우스 오른 버튼 눌림
	{
		StartMouseAction(EMB_ZOOM); // 확대, 축소 Operation 설정

		style->OnRightButtonDown();
	}
	else if (vtkCommand::RightButtonReleaseEvent == event) // 마우스 오른 버튼 떼임
	{
		EndMouseAction();

		style->OnRightButtonUp();
	}
	else if (vtkCommand::MouseMoveEvent == event) // 마우스 움직임 처리
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

