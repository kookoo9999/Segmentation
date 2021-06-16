
#pragma once

#include <vector>

enum E_DIRECTION
{
	ED_AXIAL = 0,
	ED_SAGITTAL,
	ED_CORONAL,
	ED_3D,
};

class CMPROverlayGenerator;

struct stLabelItem
{
	stLabelItem(int pv, double pr, double pg, double pb, double pa)
		: value(pv)
	{
		rgba[0] = pr;
		rgba[1] = pg;
		rgba[2] = pb;
		rgba[3] = pa;
	}

	int value;
	double rgba[4];	
};

// VTK DICOM Rendering Interactor 에서 사용할 데이터 모델
class CDataModel
{
public:

	CDataModel();
	virtual ~CDataModel();

	// 3D Image 데이터 지정, 조회 함수
	void SetImage(vtkImageData* pImage);
	vtkImageData* GetImage();

	// Label Image 에 접근하기 위한 자료구조
	vtkImageData* GetLabelImage();

	// 관리 렌더러 추가, 조회, 개수 리턴 함수	
	void AddRenderer(vtkRenderer* pRen, E_DIRECTION viewMode);
	vtkRenderer* GetRenderer(int idx);
	int GetRendererCount() const;

	// MPR Scene Generator 에 접근
	CMPROverlayGenerator* GetMPRGenerator(int idx);

	// 스크린 좌표의 위치로 어느 View port 의 렌더러를 클릭하였는지 인덱스 리턴. 실패의 경우 -1 리턴
	int CheckRendererIndex(vtkRenderWindow* pRenWin, int nScreenPos[2]);

	std::vector<stLabelItem>* GetLabelColorTable();

public:

	std::vector<double> m_alignMatrix; // Main 데이터의 Align Matrix

protected:

	vtkSP<vtkImageData> m_pMainImage; // Main 데이터
	vtkSP<vtkImageData> m_pLabelImage; // Label 데이터

	std::vector<stLabelItem> m_arColorTable; // Color Table 데이터
	
	std::vector<vtkRenderer*> m_arRenderer; // 4분할 렌더러 관리. 좌상, 우상, 좌하, 우하 순서.
	std::vector<CMPROverlayGenerator*> m_arMPRGen; // MPR Image 생성 클래스. Axial, Sagittal, Coronal.

};