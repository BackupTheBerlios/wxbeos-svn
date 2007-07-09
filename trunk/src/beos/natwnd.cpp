#include <View.h>
#include <String.h>
#include <StringView.h>
#include "wx/beos/natwnd.h"
#include "wx/beos/utilsbeos.h"
#include "wx/wx.h"

BNativeWindow::BNativeWindow(wxWindow * parent, const wxString title, const wxPoint pos, const wxSize size, long style, const wxString name) :
	BWindow(BRect(pos.x, pos.y, size.x+pos.x, size.y+pos.y), title.c_str(), B_DOCUMENT_WINDOW, 0),
	fParentWin(parent)
{
}

void BNativeWindow::MessageReceived(BMessage *msg)
{
//send message to a parent
	switch(msg->what)
	{
		default:
			((wxFrame*)fParentWin)->HandleMenuSelect(msg);
	}		
}

bool BNativeWindow::QuitRequested()
{
	if(fParentWin->GetEventHandler())
		return fParentWin->Close(true);
	return true;
}

void BNativeWindow::FrameResized(float w, float h)
{
	((wxTopLevelWindowBeOS*)fParentWin)->DoSetSize((int)Frame().left, (int)Frame().top, (int)w, (int)h);
}
