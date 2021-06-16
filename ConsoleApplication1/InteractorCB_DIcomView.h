
#pragma once

#include "InteractorCB_MPR.h"

class CDataModel;

// VTK DICOM Rendering Interator 의 Interactor Callback
class CInteractorCB_DicomView: public CInteractorCB_MPR
{
public:
	VTK_NEW_MACRO(CInteractorCB_DicomView);
	
	// 데이터 모델을 Interactor 에 설정함과 함께 Interactor 의 초기화 수행
	bool InitDataModel(CDataModel* pData);

	// 활성화 View Index 가 변경될 때마다 자동 실행되는 함수.
	virtual void OnChangeFocusedView(int prev, int cur);

	// 마우스 휠이 조작될 때마다 자동 실행되는 함수.
	virtual void OnMouseWheel(int delta);

	// Label 자료구조의 내용을 3D View 에 적용
	virtual void OnUpdateLabelTo3DView();

	// 그리기 액션
	virtual void OnDrawAction(int nScrPos[2]);
	virtual void OnOverDrawAction(int nScrPos[2]);

	static void PrintUsage();

public: // Utility Class

	// 3D Image 데이터를 렌더러에 볼륨 렌더링 파이프라인으로 추가.
	static void Add_Volume_Rendering_Data_To_Renderer(vtkImageData* pData, vtkMatrix4x4* pMat,
		vtkRenderer* pRenderer);

	// 구를 모델링하는 함수
	static void Modeling_Sphere(vtkImageData* pLabelImage, float radius, int pt3d[3], unsigned char nLabel);

	// 큐브를 모델링하는 함수
	static void Modeling_Cube(vtkImageData* pLabelImage, int radius, int pt3d[3], unsigned char nLabel);

	// vtkImageData 자료구조에 값을 쓰는 함수
	static void SetLabelValue(vtkImageData* pLabelImage, int x, int y, int z,
		int anDim[3], unsigned char nLabel);
	
protected:

	// 마우스 및 키보드 조작 핸들러 함수
	virtual void Execute(vtkObject *Caller, unsigned long event, void *callData);

	CInteractorCB_DicomView(); // 생성자 및 초기화 루틴
		
protected:

	CDataModel* m_pDataset;
	int m_nIdxFocusedView; // 현재 활성화 된 View Index 를 보관

	// 3D Orientation Marker 표시를 위한 자료구조
	vtkSP<vtkOrientationMarkerWidget> m_pOrientMarker;

	bool m_bLBtnDown;

	// Label 내용을 3D View 로 표현할 때 Label 값 별 Object 자료구조
	std::vector<vtkSP<vtkActor>> m_pVisLabel3D;

	int m_nBrushValue;
	int m_nBrushSize;
	
};