
#include "stdafx.h"
#include "InteractorCB_Base.h"

using namespace std;

CInteractorCB_Base::CInteractorCB_Base() // 생성자 및 초기화 루틴
	: vtkCommand()
	, _pInteractor(nullptr)
	, m_nMouseState(EMB_NONE)
{

}

void CInteractorCB_Base::SetInteractor(vtkRenderWindowInteractor *pInter)
{
	_pInteractor = pInter; 
	
	C_VTK(vtkInteractorStyleImage, pStyle);
	pStyle->AddObserver(vtkCommand::LeftButtonPressEvent, this, 1.0f);
	pStyle->AddObserver(vtkCommand::LeftButtonReleaseEvent, this, 1.0f);
	pStyle->AddObserver(vtkCommand::RightButtonPressEvent, this, 1.0f);
	pStyle->AddObserver(vtkCommand::RightButtonReleaseEvent, this, 1.0f);
	pStyle->AddObserver(vtkCommand::MiddleButtonPressEvent, this, 1.0f);
	pStyle->AddObserver(vtkCommand::MiddleButtonReleaseEvent, this, 1.0f);
	pStyle->AddObserver(vtkCommand::MouseMoveEvent, this, 1.0f);
	pStyle->AddObserver(vtkCommand::MouseWheelForwardEvent, this, 1.0f);
	pStyle->AddObserver(vtkCommand::MouseWheelBackwardEvent, this, 1.0f);
	pStyle->AddObserver(vtkCommand::KeyPressEvent, this, 1.0f);
	pStyle->AddObserver(vtkCommand::KeyReleaseEvent, this, 1.0f);
	pStyle->AddObserver(vtkCommand::CharEvent, this, 1.0f);
	if (_pInteractor) _pInteractor->SetInteractorStyle(pStyle);
}

vtkRenderer* CInteractorCB_Base::GetRenderer(int idx)
{
	if (nullptr == _pInteractor) return nullptr;
	
	auto pRenWin = _pInteractor->GetRenderWindow();
	if (nullptr == pRenWin) return nullptr;

	auto renderers = pRenWin->GetRenderers();
	if (nullptr == renderers) return nullptr;
	
	vtkRenderer* pRen = renderers->GetFirstRenderer();
	for (int i = 0; i < idx && nullptr != pRen; i++)
	{
		pRen = renderers->GetNextItem();
	}
	return pRen;
}

vtkInteractorStyle* CInteractorCB_Base::GetIStyle()
{
	auto pInter = GetInteractor();
	if (pInter)
	{
		return vtkInteractorStyle::SafeDownCast(pInter->GetInteractorStyle());
	}
	return nullptr;
}

vtkRenderWindowInteractor* CInteractorCB_Base::GetInteractor()
{
	return _pInteractor;
}

void CInteractorCB_Base::StartMouseAction(E_MOUSE_ACTION_STATE nState)
{
	m_nMouseState = nState;

	auto style = vtkInteractorStyle::SafeDownCast(_pInteractor->GetInteractorStyle());
	if (0 == style) return;

	switch (m_nMouseState)
	{
	case EMB_ROTATE: style->StartRotate(); // 회전 시작
		break;
	case EMB_PAN: style->StartPan(); // 이동 시작
		break;
	case EMB_ZOOM: style->StartDolly(); // 확대 시작
		break;
	}
}

bool CInteractorCB_Base::MoveMouseAction()
{
	auto style = vtkInteractorStyle::SafeDownCast(_pInteractor->GetInteractorStyle());
	if (0 == style) return false;

	switch (m_nMouseState)
	{
	case EMB_ROTATE: style->Rotate(); // 회전
		break;
	case EMB_PAN: style->Pan(); // 이동
		break;
	case EMB_ZOOM: style->Dolly(); // 확대
		break;
	default:
		return false;
	}
	return true;
}

void CInteractorCB_Base::EndMouseAction(bool bResetState)
{
	auto style = vtkInteractorStyle::SafeDownCast(_pInteractor->GetInteractorStyle());
	if (0 == style) return;

	switch (m_nMouseState)
	{
	case EMB_ROTATE: style->EndRotate(); // 회전 해제
		break;
	case EMB_PAN: style->EndPan(); // 이동 해제
		break;
	case EMB_ZOOM: style->EndDolly(); // 확대 해제
		break;
	}

	if (bResetState)
	{
		m_nMouseState = EMB_NONE;
	}
}

CInteractorCB_Base::E_MOUSE_ACTION_STATE CInteractorCB_Base::GetMouseAction() const
{
	return m_nMouseState;
}

void CInteractorCB_Base::Execute(vtkObject *Caller, unsigned long event, void *callData)
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
		StartMouseAction(EMB_ROTATE); // 회전 Operation 설정		
		
		style->OnLeftButtonDown();
	}	
	else if (vtkCommand::LeftButtonReleaseEvent == event) // 마우스 왼 버튼 떼임
	{
		EndMouseAction();
		
		style->OnLeftButtonUp();
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
