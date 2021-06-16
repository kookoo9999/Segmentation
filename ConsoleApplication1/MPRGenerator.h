
#pragma once

#include <vector>

enum E_DIRECTION;

// MPR Render Scene 을 생성해주는 클래스
class CMPRGenerator
{

public:

	CMPRGenerator();

	void SetRenderer(vtkRenderer* pRen);
	vtkRenderer* GetRenderer();

	// 슬라이스 모드를 얻는다.
	E_DIRECTION GetSliceMode() const;

	virtual bool SetImage(vtkImageData *pImage, E_DIRECTION nSliceMode, vtkMatrix4x4 *pMatAlign,
		double *fMinMax2D = nullptr);

	virtual void MoveSlice(int amount);

	// 현재 슬라이스의 logical index 위치를 얻는다.
	int GetSlicePos() const;

	// Screen 좌표계를 World, Voxel 좌표계로 변환한다.
	bool Convert_ScrPos(const int scrPos[2], double* worldPos3D, int* voxelPos3D);

public: // Utility Class

	// Axial, Sagittal, Coronal Slice 의 Matrix 를 정의.
	static vtkSP<vtkMatrix4x4> GetResliceMatrix(E_DIRECTION mode, vtkMatrix4x4 *rotMat = nullptr);

	// Screen 좌표계와 World 좌표계의 변환 기능
	static bool ScrToWorld(const int scrPos2D[2], double worldPos3D[3], vtkRenderer *pRen);
	static bool WorldToScr(const double worldPos3D[3], int scrPos2D[2], vtkRenderer *pRen);

protected:

	vtkRenderer* m_pRenderer;

	std::vector<int> _anDim;
	std::vector<double> _afRes;
	std::vector<double> _physicalDim;
	E_DIRECTION _nSliceMode;

	// MRI 영상, 자료구조가 외부에 존재하며 포인터만 관리.
	vtkImageData* m_pImageMPR;

	vtkSP<vtkImageReslice> m_pResliceMPR;
	vtkSP<vtkLookupTable> m_pTableMPR;
	vtkSP<vtkImageMapToColors> m_pColorMPR;
	vtkSP<vtkImageActor> m_pActorMPR;

	vtkSP<vtkMatrix4x4> m_pMatAlign;

};
