
#include "stdafx.h"
#include "DataModel.h"

#include "MPROverlayGenerator.h"

using namespace std;


CDataModel::CDataModel()
	: m_pMainImage(nullptr)
	, m_pLabelImage(nullptr)
{

	m_arColorTable = {
		stLabelItem(0, 0, 0, 0, 0),
		stLabelItem(1, 1, 0, 0, 0.3),
		stLabelItem(2, 0, 1, 0, 0.3),
		stLabelItem(3, 0, 0, 1, 0.3),
	};

}

CDataModel::~CDataModel()
{
	for (auto& s : m_arMPRGen)
	{
		if (s) 
		{
			delete s;
			s = nullptr;
		}
	}

	m_arMPRGen.clear();
}

// 3D Image 데이터 지정, 조회 함수
void CDataModel::SetImage(vtkImageData* pImage)
{
	m_pMainImage = nullptr;
	m_pLabelImage = nullptr;

	if (pImage)
	{
		m_pMainImage = vtkSP<vtkImageData>::New();
		m_pMainImage->DeepCopy(pImage);

		// 동일 dimension 으로 버퍼 생성
		auto anDim = m_pMainImage->GetDimensions();
		auto afRes = m_pMainImage->GetSpacing();

		m_pLabelImage = vtkSP<vtkImageData>::New();
		m_pLabelImage->SetDimensions(anDim);
		m_pLabelImage->SetSpacing(afRes);
		m_pLabelImage->AllocateScalars(VTK_UNSIGNED_CHAR, 1);
		m_pLabelImage->GetPointData()->GetScalars()->Fill(0); // 초기화
	}
}

vtkImageData* CDataModel::GetImage()
{
	return m_pMainImage;
}

// Label Image 에 접근하기 위한 자료구조
vtkImageData* CDataModel::GetLabelImage()
{
	return m_pLabelImage;
}

// 관리 렌더러 추가, 조회, 개수 리턴 함수
void CDataModel::AddRenderer(vtkRenderer* pRen, E_DIRECTION viewMode)
{
	if (pRen)
	{
		m_arRenderer.push_back(pRen);

		if (viewMode >= ED_AXIAL && viewMode <= ED_CORONAL)
		{
			auto objRenderMPR = new CMPROverlayGenerator();
			objRenderMPR->SetRenderer(pRen);
			m_arMPRGen.push_back(objRenderMPR);
		}
	}
}

vtkRenderer* CDataModel::GetRenderer(int idx)
{
	if (idx >= 0 && idx < GetRendererCount())
	{
		return m_arRenderer[idx];
	}
	return nullptr;
}

int CDataModel::GetRendererCount() const
{
	return (int)m_arRenderer.size();
}

// MPR Scene Generator 에 접근
CMPROverlayGenerator* CDataModel::GetMPRGenerator(int idx)
{
	if (idx >= 0 && idx < (int)m_arMPRGen.size())
	{
		return m_arMPRGen[idx];
	}
	return nullptr;
}

// 스크린 좌표의 위치로 어느 View port 의 렌더러를 클릭하였는지 인덱스 리턴. 실패의 경우 -1 리턴
int CDataModel::CheckRendererIndex(vtkRenderWindow* pRenWin, int nScreenPos[2])
{
	if (nullptr == pRenWin) return -1;

	auto pRenSize = pRenWin->GetSize();

	int idx = 0;
	for (auto& s : m_arRenderer)
	{
		auto vp = s->GetViewport(); // xmin, ymin, xmax, ymax

		int xmin = (int)((double)pRenSize[0] * vp[0]);
		int xmax = (int)((double)pRenSize[0] * vp[2]);
		int ymin = (int)((double)pRenSize[1] * vp[1]);
		int ymax = (int)((double)pRenSize[1] * vp[3]);

		if (xmin <= nScreenPos[0] && xmax >= nScreenPos[0] &&
			ymin <= nScreenPos[1] && ymax >= nScreenPos[1])
		{
			return idx;
		}

		idx++;
	}
	return -1;
}

vector<stLabelItem>* CDataModel::GetLabelColorTable()
{
	return &m_arColorTable;
}
