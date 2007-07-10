#ifndef B_NATIVE_WINDOW
#define  B_NATIVE_WINDOW

#include <Window.h>

class wxString;
class wxPoint;
class wxSize;
class wxWindow;

class BNativeWindow : public BWindow
{
public:
	BNativeWindow(wxWindow * parent, const wxString title, const wxPoint pos, const wxSize size, long style, const wxString name);
	virtual void MessageReceived(BMessage * msg);
friend class wxTopLevelWindowBeOS;
	virtual void FrameResized(float width, float height);
	virtual bool QuitRequested(void);
private:
	wxWindow * fParentWin;
};

#endif
