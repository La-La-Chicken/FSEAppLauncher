#include "pch.h"
#include "BaseDialog.h"



BEGIN_MESSAGE_MAP(CBaseDialog, CDialogEx)
	ON_MESSAGE(WM_NCCALCSIZE, &CBaseDialog::OnNcCalcSizeMessage)
END_MESSAGE_MAP()


LRESULT CBaseDialog::OnNcCalcSizeMessage(WPARAM wParam, LPARAM lParam) {
	if (wParam == TRUE) {	// 当 wParam 为 TRUE 时，表示需要计算客户区大小
		return 0;	// 返回 0 表示整个窗口区域都将作为客户区（即移除标准框架）
	}

	return Default();	// 否则调用默认处理（例如窗口最小化/最大化时的计算）
}
