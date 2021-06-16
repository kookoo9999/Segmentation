
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

// �����̽� ��带 ��´�.
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
		// ���� ���������� Actor �� �����Ѵ�.
		pRenderer->RemoveActor(m_pActorMPR);
		m_pActorMPR = nullptr;
		m_pResliceMPR = nullptr;
		m_pTableMPR = nullptr;
		m_pColorMPR = nullptr;		
	}
		
	m_pImageMPR = pImage;
	
	_nSliceMode = nSliceMode;

	// MPR View �� Rendering �� �����Ͱ� ������ Render Pipeline �� ����
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

		reslice->SetInputData(mpr_image); // ���� �����͸� �Է��Ѵ�.	
		reslice->SetOutputDimensionality(2);

		// reslice ������ �Է��Ѵ�.
		// ù��° �÷� : x axis vector
		// �ι�° �÷� : y axis vector
		// ����° �÷� : z axis vector
		reslice->SetResliceAxes(resliceAxes);

		reslice->SetInterpolationModeToNearestNeighbor(); // ���� ���

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

	pRenderer->ResetCamera(); // ���ο��� ResetCameraClippingRange() �Լ� �ڵ� ȣ��.

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

// ���� �����̽��� ��ġ�� ��´�.
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

// Screen ��ǥ�踦 World, Voxel ��ǥ��� ��ȯ�Ѵ�.
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
		
	// �ڿ��� Origin ������ ����� ��츦 ����Ͽ� ����� �� Origin �� ������Ʈ.			
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

// Axial, Sagittal, Coronal Slice �� Matrix �� ��´�.
// mode : 0 - Axial, 1 - Sagittal, 2 - Coronal
vtkSP<vtkMatrix4x4> CMPRGenerator::GetResliceMatrix(E_DIRECTION mode, vtkMatrix4x4 *rotMat /*= nullptr*/)
{
	// �Ʒ��� ����� ù��° column �� right vector
	// �Ʒ��� ����� �ι�° column �� up vector
	// �Ʒ��� ����� ����° column �� direction vector �̴�.

	C_VTK(vtkMatrix4x4, matOrient);
	matOrient->Identity();

	switch (mode)
	{
	case ED_AXIAL:
	{
		double Orientation[16] =
		{
			-1, 0, 0, 0, // Axial �̹����� �¿� ����
			0, 1, 0, 0, // ��, �� ������ �ٲ�
			0, 0, -1, 0, // �����̽� ������ �ٲ�
			0, 0, 0, 1
		};
		matOrient->DeepCopy(Orientation);
	}
	break;

	case ED_SAGITTAL:
	{
		double Orientation[16] =
		{
			0, -1, 0, 0, // ��, �� �ٶ󺸴� ������ �ٲ�
			0, 0, -1, 0, // ��, �� ������ �ٲ�
			-1, 0, 0, 0, // �����̽� ������ �ٲ�
			0, 0, 0, 1
		};
		matOrient->DeepCopy(Orientation);
	}
	break;

	case ED_CORONAL:
	{
		double Orientation[16] =
		{
			-1, 0, 0, 0, // ��, �� �ٶ󺸴� ������ �ٲ�
			0, 0, -1, 0, // ��, �� ������ �ٲ�
			0, 1, 0, 0, // �����̽� ������ �ٲ�
			0, 0, 0, 1
		};
		matOrient->DeepCopy(Orientation);
	}
	break;
	}

	// rotMat �� �����Ѵ�.
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

// World -> Screen ��ǥ ��ȯ
bool CMPRGenerator::WorldToScr(const double worldPos3D[3], int scrPos2D[2], vtkRenderer *pRen)
{
	if (nullptr == pRen) return false;

	C_VTK(vtkCoordinate, coord);
	coord->SetCoordinateSystemToWorld(); // World ��ǥ��� ����.
	coord->SetValue(worldPos3D[0], worldPos3D[1], worldPos3D[2]);
	coord->SetViewport(pRen);
	int* pRes = coord->GetComputedDisplayValue(nullptr); // �Էµ� ��ǥ�踦 Display ��ǥ��� ��ȯ.

	memcpy(scrPos2D, pRes, sizeof(int) * 2);
	return true;
}
