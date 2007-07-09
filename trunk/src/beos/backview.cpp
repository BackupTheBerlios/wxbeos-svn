#include <Screen.h>
#include <Bitmap.h>
#include "backview.h"
#include "wx/wx.h"
#include "wx/beos/utilsbeos.h"

BBackView::BBackView(wxWindow *parent, const wxPoint& pos, const wxSize& size, uint32 flags, const wxString& name) :
	BView(BRect(pos.x, pos.y, pos.x+size.x, pos.y+size.y), name.c_str(), flags, B_WILL_DRAW | B_FRAME_EVENTS),
	cachedBounds(Bounds()),
	fwxParent(parent)
{
	backView = new BView(Bounds(), "backView", B_FOLLOW_NONE, B_WILL_DRAW);
	backView->SetViewColor(wx2be(*wxLIGHT_GREY));	
	Create();
}

BBackView::BBackView(wxWindow *parent, const wxRect& frame, uint32 flags, const wxString& name)	:
	BView(BRect(frame.x, frame.y, frame.x+frame.width, frame.y+frame.height), name.c_str(), flags, B_WILL_DRAW | B_FRAME_EVENTS),
	cachedBounds(Bounds()),
	fwxParent(parent)	
{
	backView = new BView(Bounds(), "backView", B_FOLLOW_NONE, B_WILL_DRAW);
	backView->SetViewColor(wx2be(*wxLIGHT_GREY));
	Create();
}

void BBackView::Create()
{
	BScreen * screen = new BScreen();
	color_space clrmap = screen->ColorSpace(); 
	back = new BBitmap(Bounds(), clrmap, true);
	delete screen;	
}

void BBackView::ResetBack(float width, float height)
{	
	if(back)
	{
		back->RemoveChild(backView);
		delete back;
		back = NULL;
	}	
	
	Create();
	backView->ResizeTo(width, height);
	cachedBounds = backView->Bounds();
	rgb_color clr = backView->HighColor();
	back->AddChild(backView);
	backView->SetHighColor(clr);
}

void BBackView::FrameResized(float width, float height)
{
	ResetBack(width, height);
	fwxParent->Draw();	
}

void BBackView::AttachedToWindow()
{
	ResetBack(Bounds().Width(), Bounds().Height());
}

void BBackView::flush()
{
	BBitmap * bmp = Bitmap();
	if(LockLooper())
	{	
		backView->Flush();
		SetViewBitmap(bmp, B_FOLLOW_NONE, 0);
		Invalidate();
		UnlockLooper();
	}
}

BRect BBackView::bounds()
{
	return cachedBounds;
}

void BBackView::Clear()
{
	if(!backView->LockLooper())
		return;
	rgb_color clr = backView->HighColor();
	backView->SetHighColor(backView->ViewColor());
	backView->FillRect(bounds());
	backView->SetHighColor(clr);
	backView->UnlockLooper();
}

BBackView::~BBackView()
{
	if(back)
		back->RemoveChild(backView);
}

void BBackView::MessageReceived(BMessage * msg)
{
	switch(msg->what)
	{
		case B_MOUSE_DOWN:
			if(msg->FindInt32("buttons") == B_SECONDARY_MOUSE_BUTTON)
			{
				BPoint where = msg->FindPoint("where");
//				fwxParent->DoPopupMenu(NULL, where.x, where.y);
			}
			break;
		default:
			BView::MessageReceived(msg);
	}
}
