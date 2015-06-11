#pragma once

#include "CMyObject.h"
#include "..\CAssetManager.h"
//#include "CMyMesh.h"
#include "CMyVertex.h"
#include <vector>
#include <string>
#include "d3d.h"
//const vars
const char	Textures[2][256] = {"black.jpg","white.jpg"};

struct TERRAIN_PREFS //groups together all the preferences of the terrian
{
	OBJECT_PREFS objPrefs;
	D3DXVECTOR3 minBounds;
	D3DXVECTOR3 maxBounds;
	UINT numCellsWide;
	UINT numCellsHigh;
	
};

class CTerrain : public CMyObject
{
public:
	enum ATTRIBSQUARE{ATTACK,MOVE,SELECT,FRAME};

	CTerrain(LPDIRECT3DDEVICE9 pDevice, CAssetManager& assetManger,CMyMesh *pTerrainMesh,TERRAIN_PREFS& TerrianPref);
	virtual ~CTerrain(void);
	
	HRESULT		createTerrain	(LPDIRECT3DDEVICE9 pDevice, CAssetManager& assetManger, D3DXVECTOR3 minBounds, D3DXVECTOR3 maxBounds ,UINT numCellsWide, UINT numCellsHigh, D3DXVECTOR3 vecScale );

	void		drawSubset		(IDirect3DDevice9* pd3dDevice, ULONG AttributeID, ID3DXEffect * effect, UINT numPass, D3DXMATRIX ViewProj);

	void		setFrameSqaure	(DWORD faceCount);
	HRESULT		drawSquare		(ULONG squareNum, ID3DXEffect * effect, UINT numPass, D3DXMATRIX ViewProj);

protected:
	CMyMesh	*			m_squareMesh;
	CMyMesh *			m_boardFrameMesh;
	ULONG				m_boardFrameAttrib;
	std::vector<ULONG>	m_attribSquares[2];
	ULONG				m_selectSquare;
	ULONG				m_fraemSqaure;

	float				m_stepX;
	float				m_stepZ;
	D3DXVECTOR3		    m_meshScale; //stores the scale vector that was used during the creation of the board mesh

	ULONG				m_attribIDs[4];

	UINT				m_numCellsWide;
	UINT				m_numCellsHigh;
	D3DXVECTOR3			m_scaleVec;
};
