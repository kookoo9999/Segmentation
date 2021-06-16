
#include "stdafx.h"
#include "MPRGenerator.h"

// use for E_DIRECTION
#include "DataModel.h"

using namespace std;

CMPRGenerator::CMPRGenerator()
	: m_pRenderer(nullptr)
	, _nSliceMode(ED_AXIAL)
	, m_pImageMPR(nullptr)
	, m_pResliceMPR(nullptr)
	, m_pTableMPR(nullptr)
	, m_pColorMPR(nullptr)
	, m_pActorMPR(nullptr)
	, m_pMatAlign(nullptr)
{

	_anDim = { 0, 0, 0 };
	_afRes = { 0, 0, 0 };
	_physicalDim = { 0, 0, 0 };

	m_pMatAlign = vtkSP<vtkMatrix4x4>::New();
	m_pMatAlign->Identity();

}

void CMPRGenerator::SetRenderer(vtkRenderer* pRen)
{
	m_pRenderer = pRen;
}

vtkRenderer* CMPRGenerator::GetRenderer()
{
	return m_pRenderer;
}

// 슬라이스 모드를 얻는다.
E_DIRECTION CMPRGenerator::GetSliceMode() const
{
	return _nSliceMode;
}

bool CMPRGenerator::SetImage(vtkImageData *pImage, E_DIRECTION nSliceMode, vtkMatrix4x4 *pMatAlign,
	double *fMinMax2D /*= nullptr*/)
{
	auto pRenderer = GetRenderer();
	if (pRenderer == nullptr) return false;

	m_pMatAlign->Identity();
	if (pMatAlign)
	{
		m_pMatAlign->DeepCopy(pMatAlign);
	}

	pRenderer->DrawOff();

	if (m_pActorMPR)
	{
		// 기존 렌더러에서 Actor 를 제거한다.
		pRenderer->RemoveActor(m_pActorMPR);
		m_pActorMPR = nullptr;
		m_pResliceMPR = nullptr;
		m_pTableMPR = nullptr;
		m_pColorMPR = nullptr;		
	}
		
	m_pImageMPR = pImage;
	
	_nSliceMode = nSliceMode;

	// MPR View 에 Rendering 할 데이터가 있으면 Render Pipeline 을 설정
	vtkImageData *mpr_image = m_pImageMPR;
	if (mpr_image)
	{
		double *center = mpr_image->GetCenter();

		// Matrices for axial, coronal, sagittal, oblique view orientations	

		auto resliceAxes = CMPRGenerator::GetResliceMatrix(_nSliceMode, m_pMatAlign);

		// Set the slice orientation		
		resliceAxes->SetElement(0, 3, center[0]); // Set the point through which to slice
		resliceAxes->SetElement(1, 3, center[1]);
		resliceAxes->SetElement(2, 3, center[2]);
				
		m_pResliceMPR = vtkSP<vtkImageReslice>::New();
		m_pTableMPR = vtkSP<vtkLookupTable>::New();
		m_pColorMPR = vtkSP<vtkImageMapToColors>::New();
		m_pActorMPR = vtkSP<vtkImageActor>::New();

		auto reslice = m_pResliceMPR;
		auto table = m_pTableMPR;
		auto color = m_pColorMPR;
		auto actor = m_pActorMPR;

		reslice->SetInputData(mpr_image); // 볼륨 데이터를 입력한다.	
		reslice->SetOutputDimensionality(2);

		// reslice 방향을 입력한다.
		// 첫번째 컬럼 : x axis vector
		// 두번째 컬럼 : y axis vector
		// 세번째 컬럼 : z axis vector
		reslice->SetResliceAxes(resliceAxes);

		reslice->SetInterpolationModeToNearestNeighbor(); // 보간 방법

														  // Create a greyscale lookup table
		auto fRange = mpr_image->GetPointData()->GetScalars()->GetRange();

		table->SetRange(fRange[0], fRange[1]); // image intensity range

		table->SetValueRange(0, 1); // from black to white
		table->SetSaturationRange(0, 0); // no color saturation
		table->SetHueRange(0, 0);
		table->SetRampToLinear();
		table->Build();

		// Map the image through the lookup table		
		color->SetLookupTable(table);
		color->SetInputConnection(reslice->GetOutputPort());
		color->Update();

		actor->SetInputData(color->GetOutput());
		pRenderer->AddActor(actor);

		mpr_image->GetDimensions(&_anDim[0]);
		mpr_image->GetSpacing(&_afRes[0]);

		for (int i = 0; i < 3; i++) _physicalDim[i] = _anDim[i] * _afRes[i];
	}

	pRenderer->ResetCamera(); // 내부에서 ResetCameraClippingRange() 함수 자동 호출.

	vtkCamera *pCamera = pRenderer->GetActiveCamera();
	pCamera->ParallelProjectionOn();
	pCamera->Zoom(1.2);

	pRenderer->DrawOn();

	pRenderer->Render();

	return true;
}

void CMPRGenerator::MoveSlice(int amount)
{
	if (m_pResliceMPR == nullptr) return;

	vtkMatrix4x4 *matrix = m_pResliceMPR->GetResliceAxes();
	if (matrix == nullptr) return;

	double spacing = 0.0;

	switch (GetSliceMode())
	{
	case ED_AXIAL:
		spacing = _afRes[2];
		break;
	case ED_SAGITTAL:
		spacing = _afRes[0];
		break;
	case ED_CORONAL:
		spacing = _afRes[1];
		break;
	}

	double pos[4] = { 0.0, 0.0, (double)amount * spacing, 1.0 };

	double res_pos[4] = { 0, };
	// move the center point that we are slicing through
	matrix->MultiplyPoint(pos, res_pos);
	matrix->SetElement(0, 3, res_pos[0]);
	matrix->SetElement(1, 3, res_pos[1]);
	matrix->SetElement(2, 3, res_pos[2]);

	if (m_pColorMPR)
	{
		m_pColorMPR->Update();
	}
}

// 현재 슬라이스의 위치를 얻는다.
int CMPRGenerator::GetSlicePos() const
{
	int res = 0;

	if (m_pResliceMPR == nullptr) return res;

	vtkMatrix4x4 *matrix = m_pResliceMPR->GetResliceAxes();
	if (matrix == nullptr) return res;

	double pos[4] = { 0.0, 0.0, 0.0, 1.0 };

	double res_pos[4] = { 0, };
	matrix->MultiplyPoint(pos, res_pos); // move the center point that we are slicing through

	switch (GetSliceMode())
	{
	case ED_AXIAL:
		res = (int)(res_pos[2] / _afRes[2] + 0.5);
		break;
	case ED_SAGITTAL:
		res = (int)(res_pos[0] / _afRes[0] + 0.5);
		break;
	case ED_CORONAL:
		res = (int)(res_pos[1] / _afRes[1] + 0.5);
		break;
	}

	return res;
}

// Screen 좌표계를 World, Voxel 좌표계로 변환한다.
bool CMPRGenerator::Convert_ScrPos(const int scrPos[2], double* worldPos3D, int* voxelPos3D)
{
	vtkMatrix4x4 *reslice_matrix = nullptr;
	if (m_pResliceMPR) reslice_matrix = m_pResliceMPR->GetResliceAxes();

	auto pRenderer = GetRenderer();
	if (reslice_matrix == nullptr || pRenderer == nullptr) return false;

	// Screen coord. to MPR World coord.
	double pt_world[3] = { 0, };
	CMPRGenerator::ScrToWorld(scrPos, pt_world, pRenderer);

	double p[4] = { pt_world[0], pt_world[1], pt_world[2], 1 }, p_res[4] = { 0, };

	// MPR world coord. to General World coord.	
	reslice_matrix->MultiplyPoint(p, p_res);
		
	// 뒤에서 Origin 정보를 사용할 경우를 대비하여 제대로 된 Origin 을 업데이트.			
	double origin[3] = { 0, };

	if (worldPos3D)
	{
		memcpy(worldPos3D, p_res, sizeof(double) * 3);
	}

	if (voxelPos3D)
	{
		for (int i = 0; i < 3; i++)
		{
			voxelPos3D[i] = (int)((p_res[i] - origin[i]) / _afRes[i] + 0.5);
		}
	}
	
	return true;
}

// Axial, Sagittal, Coronal Slice 의 Matrix 를 얻는다.
// mode : 0 - Axial, 1 - Sagittal, 2 - Coronal
vtkSP<vtkMatrix4x4> CMPRGenerator::GetResliceMatrix(E_DIRECTION mode, vtkMatrix4x4 *rotMat /*= nullptr*/)
{
	// 아래의 행렬은 첫번째 column 이 right vector
	// 아래의 행렬은 두번째 column 이 up vector
	// 아래의 행렬은 세번째 column 이 direction vector 이다.

	C_VTK(vtkMatrix4x4, matOrient);
	matOrient->Identity();

	switch (mode)
	{
	case ED_AXIAL:
	{
		double Orientation[16] =
		{
			-1, 0, 0, 0, // Axial 이미지의 좌우 방향
			0, 1, 0, 0, // 상, 하 방향이 바뀜
			0, 0, -1, 0, // 슬라이싱 방향이 바뀜
			0, 0, 0, 1
		};
		matOrient->DeepCopy(Orientation);
	}
	break;

	case ED_SAGITTAL:
	{
		double Orientation[16] =
		{
			0, -1, 0, 0, // 좌, 우 바라보는 방향이 바뀜
			0, 0, -1, 0, // 상, 하 방향이 바뀜
			-1, 0, 0, 0, // 슬라이싱 방향이 바뀜
			0, 0, 0, 1
		};
		matOrient->DeepCopy(Orientation);
	}
	break;

	case ED_CORONAL:
	{
		double Orientation[16] =
		{
			-1, 0, 0, 0, // 좌, 우 바라보는 방향이 바뀜
			0, 0, -1, 0, // 상, 하 방향이 바뀜
			0, 1, 0, 0, // 슬라이싱 방향이 바뀜
			0, 0, 0, 1
		};
		matOrient->DeepCopy(Orientation);
	}
	break;
	}

	// rotMat 를 적용한다.
	if (rotMat)
	{
		vtkMatrix4x4::Multiply4x4(matOrient, rotMat, matOrient);
	}

	matOrient->Invert();

	return matOrient;
}

// Coordinate Screen -> World
bool CMPRGenerator::ScrToWorld(const int scrPos2D[2], double worldPos3D[3], vtkRenderer *pRen)
{
	if (nullptr == pRen) return false;

	// Screen coord. to MPR World coord.
	C_VTK(vtkCoordinate, coord);
	coord->SetCoordinateSystemToDisplay();
	coord->SetValue(scrPos2D[0], scrPos2D[1], 0);
	coord->SetViewport(pRen);

	// third component is fake, must be initialized!!!
	double *pt_world = coord->GetComputedWorldValue(nullptr);
	pt_world[2] = 0.0;

	memcpy(worldPos3D, pt_world, sizeof(double) * 3);
	return true;
}

// World -> Screen 좌표 변환
bool CMPRGenerator::WorldToScr(const double worldPos3D[3], int scrPos2D[2], vtkRenderer *pRen)
{
	if (nullptr == pRen) return false;

	C_VTK(vtkCoordinate, coord);
	coord->SetCoordinateSystemToWorld(); // World 좌표계로 동작.
	coord->SetValue(worldPos3D[0], worldPos3D[1], worldPos3D[2]);
	coord->SetViewport(pRen);
	int* pRes = coord->GetComputedDisplayValue(nullptr); // 입력된 좌표계를 Display 좌표계로 변환.

	memcpy(scrPos2D, pRes, sizeof(int) * 2);
	return true;
}
