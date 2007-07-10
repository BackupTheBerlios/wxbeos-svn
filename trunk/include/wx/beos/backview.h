#ifndef BACKVIEW
#define BACKVIEW

#include <View.h>

class BBitmap;
class wxString;
class wxPoint;
class wxSize;
class wxRect;
class wxWindow;

class BBackView : public BView
{
public:
	BBackView(wxWindow *parent, const wxPoint& pos, const wxSize& size, uint32 flags, const wxString& name);
	BBackView(wxWindow *parent, const wxRect& frame, uint32 flags, const wxString& name);
	~BBackView();
	virtual void FrameResized(float width, float height);	
	virtual void AttachedToWindow(void);
	virtual void MessageReceived(BMessage *msg);
	void ResetBack(float width, float height);
	BBitmap * Bitmap() {if(back) return back; else return NULL;};
	void flush();
	BRect bounds();
	BView * GetBack() { return backView; }; 
	void Clear();
protected:
	BRect cachedBounds;
	BBitmap * back;	
	BView * backView;	
	wxWindow * fwxParent;
private:
	void Create();
};

#endif