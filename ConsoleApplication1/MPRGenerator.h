
#pragma once

#include <vector>

enum E_DIRECTION;

// MPR Render Scene �� �������ִ� Ŭ����
class CMPRGenerator
{

public:

	CMPRGenerator();

	void SetRenderer(vtkRenderer* pRen);
	vtkRenderer* GetRenderer();

	// �����̽� ��带 ��´�.
	E_DIRECTION GetSliceMode() const;

	virtual bool SetImage(vtkImageData *pImage, E_DIRECTION nSliceMode, vtkMatrix4x4 *pMatAlign,
		double *fMinMax2D = nullptr);

	virtual void MoveSlice(int amount);

	// ���� �����̽��� logical index ��ġ�� ��´�.
	int GetSlicePos() const;

	// Screen ��ǥ�踦 World, Voxel ��ǥ��� ��ȯ�Ѵ�.
	bool Convert_ScrPos(const int scrPos[2], double* worldPos3D, int* voxelPos3D);

public: // Utility Class

	// Axial, Sagittal, Coronal Slice �� Matrix �� ����.
	static vtkSP<vtkMatrix4x4> GetResliceMatrix(E_DIRECTION mode, vtkMatrix4x4 *rotMat = nullptr);

	// Screen ��ǥ��� World ��ǥ���� ��ȯ ���
	static bool ScrToWorld(const int scrPos2D[2], double worldPos3D[3], vtkRenderer *pRen);
	static bool WorldToScr(const double worldPos3D[3], int scrPos2D[2], vtkRenderer *pRen);

protected:

	vtkRenderer* m_pRenderer;

	std::vector<int> _anDim;
	std::vector<double> _afRes;
	std::vector<double> _physicalDim;
	E_DIRECTION _nSliceMode;

	// MRI ����, �ڷᱸ���� �ܺο� �����ϸ� �����͸� ����.
	vtkImageData* m_pImageMPR;

	vtkSP<vtkImageReslice> m_pResliceMPR;
	vtkSP<vtkLookupTable> m_pTableMPR;
	vtkSP<vtkImageMapToColors> m_pColorMPR;
	vtkSP<vtkImageActor> m_pActorMPR;

	vtkSP<vtkMatrix4x4> m_pMatAlign;

};
