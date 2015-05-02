#ifndef  _CSTATICUI_H
#define  _CSTATICUI_H

#include "CControlUI.h"

class CStaticUI : public CControlUI
{
public:
	//CStaticUI(void);
	CStaticUI(CDialogUI* pParentDialog, int ID, LPCTSTR strText, int x, int y, UINT width, UINT height);
	CStaticUI(std::istream& inputFile);

	virtual ~CStaticUI(void);

	void			setText		( const char strText[]);
	//void			setFont		( UINT fontIndex); Replaced by elementFonts....
	void			setTextColor( D3DXCOLOR textColor);

	const char*		getText		() const;

	void			Render		( CAssetManager& assetManger);

	virtual bool    SaveToFile	( std::ostream& SaveFile );			

protected:
	char	    m_strText[MAX_PATH]; 

	//UINT	    m_fontIndex;
	D3DXCOLOR	m_textColor;
};

#endif  //_CSTATICUI_H