
#include "stdafx.h"
#include "MPROverlayGenerator.h"

// use for E_DIRECTION
#include "DataModel.h"

using namespace std;

CMPROverlayGenerator::CMPROverlayGenerator()
	: CMPRGenerator()	
	, m_pImageLabel(nullptr)
	, m_pResliceLabel(nullptr)
	, m_pTableLabel(nullptr)
	, m_pColorLabel(nullptr)
	, m_pActorLabel(nullptr)
	, m_pLabelColorTable(nullptr)
{
	
}

bool CMPROverlayGenerator::SetLabelImage(vtkImageData *pImage)	
{	
	auto pRenderer = GetRenderer();
	if (pRenderer == nullptr) return false;
	
	if (nullptr == m_pLabelColorTable)
	{
		// You should call SetLabelColorTable() before call SetLabelImage().
		return false;
	}

	pRenderer->DrawOff();

	if (m_pActorLabel)
	{
		// 기존 렌더러에서 Actor 를 제거한다.
		pRenderer->RemoveActor(m_pActorLabel);
		m_pActorLabel = nullptr;
		m_pResliceLabel = nullptr;
		m_pTableLabel = nullptr;
		m_pColorLabel = nullptr;		
	}
		
	m_pImageLabel = pImage;
	
	// MPR View 에 Rendering 할 데이터가 있으면 Render Pipeline 을 설정
	vtkImageData *mpr_image = m_pImageLabel;
	if (mpr_image)
	{
		double *center = mpr_image->GetCenter();

		// Matrices for axial, coronal, sagittal, oblique view orientations	

		auto resliceAxes = CMPRGenerator::GetResliceMatrix(_nSliceMode, m_pMatAlign);

		// Set the slice orientation		
		resliceAxes->SetElement(0, 3, center[0]); // Set the point through which to slice
		resliceAxes->SetElement(1, 3, center[1]);
		resliceAxes->SetElement(2, 3, center[2]);
				
		m_pResliceLabel = vtkSP<vtkImageReslice>::New();
		m_pTableLabel = vtkSP<vtkLookupTable>::New();
		m_pColorLabel = vtkSP<vtkImageMapToColors>::New();
		m_pActorLabel = vtkSP<vtkImageActor>::New();

		auto reslice = m_pResliceLabel;
		auto table = m_pTableLabel;
		auto color = m_pColorLabel;
		auto actor = m_pActorLabel;

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
				
		table->SetRange(0, 3); // 
		table->SetNumberOfTableValues(4);

		for (auto& s : *m_pLabelColorTable)
		{			
			table->SetTableValue(s.value, s.rgba[0], s.rgba[1], s.rgba[2], s.rgba[3]);
		}

		table->Build();

		// Map the image through the lookup table		
		color->SetLookupTable(table);
		color->SetInputConnection(reslice->GetOutputPort());
		color->Update();

		actor->SetInputData(color->GetOutput());
		pRenderer->AddActor(actor);
	}

	pRenderer->DrawOn();

	pRenderer->Render();

	return true;

}

void CMPROverlayGenerator::SetLabelColorTable(std::vector<stLabelItem>* pTable)
{
	m_pLabelColorTable = pTable;
}

void CMPROverlayGenerator::MoveSlice(int amount)
{
	CMPRGenerator::MoveSlice(amount);

	if (m_pResliceLabel == nullptr) return;

	vtkMatrix4x4 *matrix = m_pResliceLabel->GetResliceAxes();
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

	if (m_pColorLabel)
	{
		m_pColorLabel->Update();
	}
}

void CMPROverlayGenerator::UpdateLabel()
{
	if (nullptr == m_pImageLabel) return;
	
	if (m_pColorLabel)
	{
		m_pColorLabel->Update();
	}
}
