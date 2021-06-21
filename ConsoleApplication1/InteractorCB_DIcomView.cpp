
#include "stdafx.h"
#include "InteractorCB_DIcomView.h"

#include "DataModel.h"
#include "MPROverlayGenerator.h"

// use for M_PI
#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;

CInteractorCB_DicomView::CInteractorCB_DicomView() // 생성자 및 초기화 루틴
	: CInteractorCB_MPR()
	, m_pDataset(nullptr)
	, m_nBrushValue(1)
	, m_nBrushSize(3)
{

}

// 데이터 모델을 Interactor 에 설정함과 함께 Interactor 의 초기화 수행
bool CInteractorCB_DicomView::InitDataModel(CDataModel* pData)
{
	m_pDataset = pData;

	if (nullptr == m_pDataset) return false;

	auto pImage = m_pDataset->GetImage();
	auto pLabelImage = m_pDataset->GetLabelImage();

	// 데이터가 유효하지 않을 경우 초기화 실패
	if (nullptr == pImage) return false;
	
	C_VTK(vtkMatrix4x4, pAlignMat);
	pAlignMat->DeepCopy(&m_pDataset->m_alignMatrix[0]);
		
	// 세 개의 MPR Generator 에 데이터 assign.
	for (int i = 0; i < 3; i++)
	{
		m_pDataset->GetMPRGenerator(i)->SetImage(pImage, (E_DIRECTION)((int)ED_AXIAL + i), pAlignMat);
		m_pDataset->GetMPRGenerator(i)->SetLabelColorTable(m_pDataset->GetLabelColorTable());
		m_pDataset->GetMPRGenerator(i)->SetLabelImage(pLabelImage);		
	}
		
	// 3D View 의 경우 추가 카메라 보정, Orientation Marker 추가 필요
	{
		auto pRen_3D = m_pDataset->GetRenderer(3);

		auto pCam4 = pRen_3D->GetActiveCamera();
		pCam4->SetPosition(0, 1, 0);
		pCam4->SetViewUp(0, 0, -1);
		pCam4->SetFocalPoint(0, 0, 0);

		// 볼륨 데이터를 Volume Rendering 으로 추가.
		CInteractorCB_DicomView::Add_Volume_Rendering_Data_To_Renderer(pImage, pAlignMat, pRen_3D);

		// Orientation Marker 추가
		C_VTK(vtkAxesActor, axesActor);
		m_pOrientMarker = vtkSP<vtkOrientationMarkerWidget>::New();		
		m_pOrientMarker->SetOrientationMarker(axesActor);
		m_pOrientMarker->SetCurrentRenderer(pRen_3D);
		m_pOrientMarker->SetInteractor(GetInteractor());
		m_pOrientMarker->SetViewport(0.8, 0.0, 1.0, 0.2);
		m_pOrientMarker->SetEnabled(1);
		m_pOrientMarker->InteractiveOff();
	}
	
	// 모든 View 의 카메라를 초기화한다.
	for (int i = 0; i < m_pDataset->GetRendererCount(); i++)
	{
		auto pRen = m_pDataset->GetRenderer(i);
		if (pRen) pRen->ResetCamera();
	}
	
	CInteractorCB_DicomView::PrintUsage();

	// 활성화 된 m_nIdxFocusedView 인덱스의 View 에 Focus 표시를 해 줄 수 있다.

	return true;
}

// 활성화 View Index 가 변경될 때마다 자동 실행되는 함수.
void CInteractorCB_DicomView::OnChangeFocusedView(int prev, int cur)
{
	m_nIdxFocusedView = cur;

	printf("Focused View Changed : %d -> %d\n", prev, cur);
}

// 마우스 휠이 조작될 때마다 자동 실행되는 함수.
void CInteractorCB_DicomView::OnMouseWheel(int delta)
{
	// Focus View 가 0, 1, 2 (Axial, Sagittal, Coronal) 일때만 동작
	if (m_nIdxFocusedView < 0 || m_nIdxFocusedView > 2) return;

	auto pInter = GetInteractor();

	// 장면 관리 객체에 접근
	auto pMPR = m_pDataset->GetMPRGenerator(m_nIdxFocusedView);
	if (pMPR)
	{
		// 슬라이스를 이동한다.
		pMPR->MoveSlice(delta);

		// 화면 갱신
		pInter->Render();
	}	
}

// Label 자료구조의 내용을 3D View 에 적용
void CInteractorCB_DicomView::OnUpdateLabelTo3DView()
{
	if (nullptr == m_pDataset) return;

	auto pLabelImage = m_pDataset->GetLabelImage();
	if (nullptr == pLabelImage) return;

	auto pRen3D = m_pDataset->GetRenderer(3);
	if (nullptr == pRen3D) return;

	auto& objLabelColorTable = *m_pDataset->GetLabelColorTable();

	const int nCntColor = (int)objLabelColorTable.size() - 1;
	
	// 기존 가시화 내용 초기화
	for (auto& s : m_pVisLabel3D)
	{
		if (nullptr == s) continue;
		pRen3D->RemoveActor(s);
		 
		s = nullptr;
	}
	m_pVisLabel3D.clear();

	const int nMaxLabel = 3;
	
	// 3D Label 영상을 보인다.
	vector<int> labelTable;
	for (int i = 1; i <= nMaxLabel; i++)
	{
		labelTable.push_back(i);
	}
	
	const int nCountLabel = (int)labelTable.size();

	const bool bDoSmoothing = false;

	const int iso_mode = 0;

	m_pVisLabel3D = vector<vtkSP<vtkActor>>(nCountLabel, nullptr);

#pragma omp parallel for
	for (int i = 0; i < nCountLabel; i++)
	{
		const int nTargetValue = labelTable[i];

		C_VTK(vtkImageData, cpData);
		cpData->DeepCopy(pLabelImage);

		// Scalar 데이터를 얻는다.
		int CountNonZero = 0;

		auto pScalarArray = cpData->GetPointData()->GetScalars();
		if (pScalarArray)
		{
			vtkIdType nCntTuple = pScalarArray->GetNumberOfValues();
			for (vtkIdType j = 0; j < nCntTuple; j++)
			{
				auto pVal = (unsigned char*)pScalarArray->GetVoidPointer(j);
				if (*pVal != nTargetValue) *pVal = 0;
				else CountNonZero++;
			}
		}

		// 영역이 10 복셀보다 작으면 무시
		if (CountNonZero < 10) continue;

		// 색상을 결정한다.		
		const auto& pCol = objLabelColorTable[(i % nCntColor) + 1];

		C_VTK(vtkPolyData, cpMesh);

		if (iso_mode == 0)
		{
			C_VTK(vtkMarchingCubes, filter);
			filter->SetInputData(cpData);
			filter->GenerateValues(1, 1.0, nMaxLabel);
			filter->ComputeGradientsOn();
			filter->ComputeScalarsOff();
			filter->Update();
			cpMesh->DeepCopy(filter->GetOutput());
		}
		else if (iso_mode == 1)
		{
			C_VTK(vtkContourFilter, filter);
			filter->SetInputData(cpData);
			filter->ComputeGradientsOn();
			filter->ComputeScalarsOff();
			filter->GenerateTrianglesOn();
			filter->SetValue(0, nMaxLabel);
			filter->Update();
			cpMesh->DeepCopy(filter->GetOutput());
		}

		if (bDoSmoothing)
		{
			C_VTK(vtkSmoothPolyDataFilter, sfilter);
			sfilter->SetInputData(cpMesh);
			sfilter->SetNumberOfIterations(40);
			sfilter->SetRelaxationFactor(0.1);
			sfilter->FeatureEdgeSmoothingOff();
			sfilter->BoundarySmoothingOn();
			sfilter->Update();

			C_VTK(vtkPolyDataNormals, normalGenerator);
			normalGenerator->SetInputData(sfilter->GetOutput());
			normalGenerator->ComputePointNormalsOn();
			normalGenerator->ComputeCellNormalsOn();
			normalGenerator->Update();

			cpMesh->DeepCopy(normalGenerator->GetOutput());
		}

		C_VTK(vtkPolyDataMapper, pMapper);
		pMapper->SetInputData(cpMesh);

		m_pVisLabel3D[i] = vtkSP<vtkActor>::New();

		auto pActor = m_pVisLabel3D[i];
		pActor->SetMapper(pMapper);

		auto pProp = pActor->GetProperty();

		pProp->SetShading(1);
		pProp->SetDiffuseColor(pCol.rgba[0], pCol.rgba[1], pCol.rgba[2]);
		pProp->SetDiffuse(0.7);
		pProp->SetAmbientColor(pCol.rgba[0] * 0.6, pCol.rgba[1] * 0.7, pCol.rgba[2] * 0.4);
		pProp->SetAmbient(0.2);
		pProp->SetSpecularColor(pCol.rgba[0] * 1.5, pCol.rgba[1] * 1.5, pCol.rgba[2] * 1.5);
		pProp->SetSpecular(0.2);
		pProp->SetSpecularPower(50.0);

		// 데이터에 User Matrix 를 설정함.
		C_VTK(vtkMatrix4x4, pAlignMat);
		pAlignMat->DeepCopy(&m_pDataset->m_alignMatrix[0]);		
		pActor->SetUserMatrix(pAlignMat);
	}

	for (auto& s : m_pVisLabel3D)
	{
		pRen3D->AddActor(s);
	}

	pRen3D->ResetCameraClippingRange();

}

// 그리기 액션
void CInteractorCB_DicomView::OnDrawAction(int nScrPos[2])
{
	if (nullptr == m_pDataset) return;

	if (!(m_nIdxFocusedView >= 0 && m_nIdxFocusedView <= 2)) return;
		
	// Label 데이터에 반영한다.
	auto pLabelImage = m_pDataset->GetLabelImage();
	if (nullptr == pLabelImage) return;

	auto pMPR = m_pDataset->GetMPRGenerator(m_nIdxFocusedView);
	if (nullptr == pMPR) return;

	// nScrPos 좌표를 logical 단위의 좌표로 변환 필요.

	int voxelPt[3] = { 0, };
	if (!pMPR->Convert_ScrPos(nScrPos, nullptr, voxelPt)) return;

	//const int curSlice = pMPR->GetSlicePos();
	
	//printf("Voxel : %d, %d, %d, Slice : %d\n", voxelPt[0], voxelPt[1], voxelPt[2], curSlice);

	// 구를 모델링하는 함수
	CInteractorCB_DicomView::Modeling_Cube(pLabelImage, m_nBrushSize, voxelPt, m_nBrushValue);

	pLabelImage->Modified();
}

void CInteractorCB_DicomView::OnOverDrawAction(int nScrPos[2])
{
	// 현재까지 그려진 내용을 3D View 에 반영한다.
	if (!(m_nIdxFocusedView >= 0 && m_nIdxFocusedView <= 2)) return;
	
	// 모든 MPR View 에 대하여 순회
	for (int i = 0; i < 3; i++)
	{
		auto pMPR = m_pDataset->GetMPRGenerator(i);
		if (pMPR)
		{
			pMPR->UpdateLabel();
		}
	}	

	OnUpdateLabelTo3DView();

	auto pInter = GetInteractor();

	pInter->Render(); // 화면 갱신
}

void CInteractorCB_DicomView::PrintUsage()
{
	string str = "\n\n\n"
		"=== 3D DICOM Viewer ===\n"
		"\n"
		"Show Help Text : h"
		"\n"
		"Brush Label Value Change :\n"
		"	1 : Set 1 Label (Red)\n"
		"	2 : Set 2 Label (Green)\n"
		"	3 : Set 3 Label (Blue)\n"
		"\n"
		"Brush Size Change :\n"
		"	+ : Increase brush size\n"
		"	- : Decrease brush size\n"		
		"\n"
		"View Control :\n"
		"	Mouse L Button Dragging in MPR View : Draw Label.\n"
		"	Mouse M Button Dragging in MPR View : Image Pan.\n"
		"	Mouse R Button Dragging in MPR View : Image Zoom In, Out.\n"
		"	Mouse L Button Dragging in 3D View : Camera Rotation.\n"
		"	Mouse M Button Dragging in 3D View : Camera Pan.\n"
		"	Mouse R Button Dragging in 3D View : Camera Zoom In, Out.\n"
		"	Mouse Wheel : Focused MPR View's slicing.\n"
		"\n"
		"";

	printf("%s", str.c_str());
}

// 3D Image 데이터를 렌더러에 볼륨 렌더링 파이프라인으로 추가.
void CInteractorCB_DicomView::Add_Volume_Rendering_Data_To_Renderer(vtkImageData* pData, 
	vtkMatrix4x4* pMat, vtkRenderer* pRenderer)
{
	C_VTK(vtkSmartVolumeMapper, mapper);
	mapper->SetInputData(pData);

	C_VTK(vtkVolume, volume);
	volume->SetMapper(mapper);

	// Scalar 데이터의 Min, Max 를 구한다.
	//double *fMinMax = pData->GetPointData()->GetScalars()->GetRange();
	double *fMinMax = pData->GetScalarRange();
	mapper->SetBlendModeToComposite();

	// Volume 의 속성 설정
	C_VTK(vtkVolumeProperty, volProperty);
	volProperty->ShadeOn();
	volProperty->SetInterpolationTypeToLinear();
	volProperty->SetDiffuse(0.7);
	volProperty->SetAmbient(0.2);
	volProperty->SetSpecular(0.3);
	volProperty->SetSpecularPower(30.0);

	double fTerm = fMinMax[1] - fMinMax[0];

	double c1_pos = 0.4;

	// OTF 설정
	C_VTK(vtkPiecewiseFunction, OTF1);
	printf("fMin=%lf\n", fMinMax[0]);
	printf("fMax=%lf\n", fMinMax[1]);
	printf("First point = %lf\n", fMinMax[0] + fTerm * c1_pos);  
	OTF1->AddPoint(fMinMax[0], 0.0); // start	
	OTF1->AddPoint(335.434, 0);
	OTF1->AddPoint(850.220, 0.1);
	OTF1->AddPoint(1536.603, 0.356);
	OTF1->AddPoint(1536.603, 0);
	OTF1->AddPoint(2852.169, 0.581);
	OTF1->AddPoint(3938.941, 0);
	OTF1->AddPoint(7199.257, 0);
	OTF1->AddPoint(7199.257, 1);
	//OTF1->AddPoint(315.449, 0);
	OTF1->AddPoint(fMinMax[1], 0.4); // end*/

	
	
	/*OTF1->AddPoint(fMinMax[0], 0);
	
	OTF1->AddPoint(408.166,0);
	OTF1->AddPoint(408.166,1);	
	OTF1->AddPoint(fMinMax[1], 1);*/
	
	volProperty->SetScalarOpacity(0, OTF1);

	const int nCountItem_CTF = 4;

	double fbase[nCountItem_CTF];
	for (int i = 0; i < nCountItem_CTF; i++)
	{
		fbase[i] = fMinMax[0] + fTerm * i * 0.25; //  25% 씩 증가.
		printf("fabase[%d] = %lf\n", i, fbase[i]);
	}

	double col[nCountItem_CTF][3] =
	{
		{ 0.0, 0.0, 0.0 },
		{ 0.3, 0.4, 0.1 },
		{ 0.4, 0.6, 0.2 },
		{ 1.0, 1.0, 1.0 }
	};

	// CTF 설정
	C_VTK(vtkColorTransferFunction, CTF1);
	for (int i = 0; i < nCountItem_CTF; i++) {
		CTF1->AddRGBPoint(fbase[i], col[i][0], col[i][1], col[i][2]);
	}
	volProperty->SetColor(0, CTF1);
	volume->SetProperty(volProperty);

	volume->SetUserMatrix(pMat);


	pRenderer->AddVolume(volume);
}

// 구를 모델링하는 함수
void CInteractorCB_DicomView::Modeling_Sphere(vtkImageData* pLabelImage, float radius, int pt3d[3],
	unsigned char nLabel)
{
	const auto anDim = pLabelImage->GetDimensions();

	// determines how far apart the pixels are
	const float density = 1.0f;

	for (float i = 0.0f; i < 360.0f; i += density)
	{
		const float x1 = radius * cos(i * (float)M_PI / 180.0f);
		const float y1 = radius * sin(i * (float)M_PI / 180.0f);

		for (float j = 0.0f; j < 360.0f; j += density)
		{
			const float x2 = x1 * cos(j * (float)M_PI / 180.0f);
			const float y2 = x1 * sin(j * (float)M_PI / 180.0f);

			// round 연산 적용
			const int x = (int)(x2 + 0.5f) + pt3d[0];
			const int y = (int)(y1 + 0.5f) + pt3d[1];
			const int z = (int)(y2 + 0.5f) + pt3d[2];

			CInteractorCB_DicomView::SetLabelValue(pLabelImage, x, y, z, anDim, nLabel);
			CInteractorCB_DicomView::SetLabelValue(pLabelImage, x, y, -z, anDim, nLabel);
			CInteractorCB_DicomView::SetLabelValue(pLabelImage, -x, y, z, anDim, nLabel);
			CInteractorCB_DicomView::SetLabelValue(pLabelImage, -x, y, -z, anDim, nLabel);

			CInteractorCB_DicomView::SetLabelValue(pLabelImage, z, y, x, anDim, nLabel);
			CInteractorCB_DicomView::SetLabelValue(pLabelImage, z, y, -x, anDim, nLabel);
			CInteractorCB_DicomView::SetLabelValue(pLabelImage, -z, y, x, anDim, nLabel);
			CInteractorCB_DicomView::SetLabelValue(pLabelImage, -z, y, -x, anDim, nLabel);

			CInteractorCB_DicomView::SetLabelValue(pLabelImage, x, -y, z, anDim, nLabel);
			CInteractorCB_DicomView::SetLabelValue(pLabelImage, x, -y, -z, anDim, nLabel);
			CInteractorCB_DicomView::SetLabelValue(pLabelImage, -x, -y, z, anDim, nLabel);
			CInteractorCB_DicomView::SetLabelValue(pLabelImage, -x, -y, -z, anDim, nLabel);

			CInteractorCB_DicomView::SetLabelValue(pLabelImage, z, -y, x, anDim, nLabel);
			CInteractorCB_DicomView::SetLabelValue(pLabelImage, z, -y, -x, anDim, nLabel);
			CInteractorCB_DicomView::SetLabelValue(pLabelImage, -z, -y, x, anDim, nLabel);
			CInteractorCB_DicomView::SetLabelValue(pLabelImage, -z, -y, -x, anDim, nLabel);
		}
	}
}

// 큐브를 모델링하는 함수
void CInteractorCB_DicomView::Modeling_Cube(vtkImageData* pLabelImage, int radius, int pt3d[3], unsigned char nLabel)
{
	const auto anDim = pLabelImage->GetDimensions();

	for (int k = -radius; k <= radius; k++)
	{
		for (int j = -radius; j <= radius; j++)
		{
			for (int i = -radius; i <= radius; i++)
			{
				const int x = pt3d[0] + i;
				const int y = pt3d[1] + j;
				const int z = pt3d[2] + k;

				CInteractorCB_DicomView::SetLabelValue(pLabelImage, x, y, z, anDim, nLabel);
			}
		}
	}
}

// vtkImageData 자료구조에 값을 쓰는 함수
void CInteractorCB_DicomView::SetLabelValue(vtkImageData* pLabelImage, int x, int y, int z,
	int anDim[3], unsigned char nLabel)
{
	if (nullptr == pLabelImage) return;
	
	if (x < 0 || x >= anDim[0]) return; 
	if (y < 0 || y >= anDim[1]) return;
	if (z < 0 || z >= anDim[2]) return;
		
	auto ptr = (unsigned char*)pLabelImage->GetScalarPointer(x, y, z);
	*ptr = nLabel;	
}

// 마우스 및 키보드 조작 핸들러 함수
void CInteractorCB_DicomView::Execute(vtkObject *Caller, unsigned long event, void *callData)
{
	auto pInter = GetInteractor();
	if (nullptr == pInter) return;

	auto style = GetIStyle();
	if (0 == style) return;

	auto pRenWin = pInter->GetRenderWindow();
	if (nullptr == pRenWin) return;
		
	bool bRedraw = false;
	
	if (vtkCommand::KeyPressEvent == event)
	{
		char key = pInter->GetKeyCode();
		
		if (key == '1')
		{
			m_nBrushValue = 1;

			printf("Brush Value : %d\n", m_nBrushValue);
		}
		else if (key == '2')
		{
			m_nBrushValue = 2;
			
			printf("Brush Value : %d\n", m_nBrushValue);

		}
		else if (key == '3')
		{
			m_nBrushValue = 3;

			printf("Brush Value : %d\n", m_nBrushValue);
		}
		else if (key == '+')
		{
			m_nBrushSize ++;

			printf("Brush Size Increase: %d\n", m_nBrushSize);
		}
		else if (key == '-')
		{
			m_nBrushSize--;

			if (m_nBrushSize < 1)
			{
				m_nBrushSize = 1;
			}

			printf("Brush Size Decrease : %d\n", m_nBrushSize);
		}
		else if (key == 'h')
		{
			CInteractorCB_DicomView::PrintUsage();
		}
	}
	else if (vtkCommand::LeftButtonPressEvent == event) // 마우스 왼 버튼 눌림
	{
		// Focus View Index 업데이트
		const int hitTestView = m_pDataset->CheckRendererIndex(pRenWin, pInter->GetEventPosition());
		if (hitTestView != m_nIdxFocusedView)
		{
			OnChangeFocusedView(m_nIdxFocusedView, hitTestView);			
		}
		
		m_bLBtnDown = true;

		// 그리기 동작 수행
		OnDrawAction(pInter->GetEventPosition());
	}
	else if (vtkCommand::LeftButtonReleaseEvent == event) // 마우스 왼 버튼 떼임
	{
		m_bLBtnDown = false;

		// 그리기 종료 동작 수행
		OnOverDrawAction(pInter->GetEventPosition());
	}
	else if (vtkCommand::MiddleButtonPressEvent == event) // 마우스 가운데 버튼 눌림
	{
		// Focus View Index 업데이트
		const int hitTestView = m_pDataset->CheckRendererIndex(pRenWin, pInter->GetEventPosition());
		if (hitTestView != m_nIdxFocusedView)
		{
			OnChangeFocusedView(m_nIdxFocusedView, hitTestView);
		}
	}
	else if (vtkCommand::RightButtonPressEvent == event) // 마우스 오른 버튼 눌림
	{
		// Focus View Index 업데이트
		const int hitTestView = m_pDataset->CheckRendererIndex(pRenWin, pInter->GetEventPosition());
		if (hitTestView != m_nIdxFocusedView)
		{
			OnChangeFocusedView(m_nIdxFocusedView, hitTestView);
		}
	}
	else if (vtkCommand::MouseMoveEvent == event) // 마우스 움직임 처리
	{
		if (m_bLBtnDown)
		{
			// 그리기 동작 수행
			OnDrawAction(pInter->GetEventPosition());
		}
	}
	else if (vtkCommand::MouseWheelForwardEvent == event) // 마우스 순방향 휠 조작
	{
		OnMouseWheel(1);
	}
	else if (vtkCommand::MouseWheelBackwardEvent == event) // 마우스 역방향 휠 조작
	{
		OnMouseWheel(-1);
	}

	if (bRedraw)
	{		
		_pInteractor->Render();
	}

	// 3D View 에서 마우스 조작이 시작된 경우는 base 의 조작을 따른다.
	if (m_nIdxFocusedView == 3)
	{
		CInteractorCB_Base::Execute(Caller, event, callData);
	}
	else
	{
		CInteractorCB_MPR::Execute(Caller, event, callData);
	}
}