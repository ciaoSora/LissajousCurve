#include "UIController.h"

extern LPDIRECT3DDEVICE9 d3ddev;
extern const int WIDTH;
extern const int HEIGHT;

static LPD3DXFONT font;

static RECT fomulaRectX, fomulaRectY;
static RECT omega1Rect, omega2Rect;
static RECT phiRect;

void UIController::Init() {
	D3DXFONT_DESC df;
	ZeroMemory(&df, sizeof(D3DXFONT_DESC));
	df.Height = 25;
	df.Width = 10;
	df.Weight = 500;
	df.MipLevels = D3DX_DEFAULT;
	df.Italic = false;
	df.CharSet = DEFAULT_CHARSET;
	df.OutputPrecision = 0;
	df.Quality = 0;
	df.PitchAndFamily = 0;
	strcpy_s(df.FaceName, "Arial");
	D3DXCreateFontIndirect(d3ddev, &df, &font);
	
	const int margin = 20;
	int curveRightBound = Lissajou::GetOriginX() + Lissajou::GetA1();
	fomulaRectX.left = curveRightBound + 50;
	fomulaRectX.right = WIDTH - margin;
	fomulaRectX.top = 100;
	fomulaRectX.bottom = fomulaRectX.top + df.Height;

	fomulaRectY.left = fomulaRectX.left;
	fomulaRectY.right = fomulaRectX.right;
	fomulaRectY.top = fomulaRectX.bottom + margin;
	fomulaRectY.bottom = fomulaRectY.top + df.Height;

	omega1Rect.left = curveRightBound + 100;
	omega1Rect.right = WIDTH - margin;
	omega1Rect.top = fomulaRectY.bottom + 100;
	omega1Rect.bottom = omega1Rect.top + df.Height;

	omega2Rect.left = omega1Rect.left;
	omega2Rect.right = omega1Rect.right;
	omega2Rect.top = omega1Rect.bottom + margin;
	omega2Rect.bottom = omega2Rect.top + df.Height;

	phiRect.left = omega1Rect.left;
	phiRect.right = omega1Rect.right;
	phiRect.top = omega2Rect.bottom + margin;
	phiRect.bottom = phiRect.top + df.Height;
}

void UIController::Draw() {
	static char omega1String[20], omega2String[20], phiString[20];

	font->DrawText(NULL, "x = A1 * cos(¦Ø1 * t + ¦Õ)", -1, &fomulaRectX, DT_VCENTER | DT_SINGLELINE, 0xffffffff);
	font->DrawText(NULL, "y = A2 * cos(¦Ø2 * t)", -1, &fomulaRectY, DT_VCENTER | DT_SINGLELINE, 0xffffffff);
	
	sprintf_s(omega1String, "¦Ø1 = %d", Lissajou::GetOmega1());
	sprintf_s(omega2String, "¦Ø2 = %d", Lissajou::GetOmega2());
	sprintf_s(phiString, "   ¦Õ = %.3f ¦Ð", Lissajou::GetDeltaPhi() / D3DX_PI);

	font->DrawText(NULL, omega1String, -1, &omega1Rect, DT_VCENTER | DT_SINGLELINE, 0xffffffff);
	font->DrawText(NULL, omega2String, -1, &omega2Rect, DT_VCENTER | DT_SINGLELINE, 0xffffffff);
	font->DrawText(NULL, phiString, -1, &phiRect, DT_VCENTER | DT_SINGLELINE, 0xffffffff);

}
