#ifndef B_NATIVE_WINDOW
#define  B_NATIVE_WINDOW

#include <Window.h>

class BNativeWindow : public BWindow
{
	BNativeWindow(const wxString title, const wxPoint pos, const wxSize size, long style, const wxString name);
};

#endif