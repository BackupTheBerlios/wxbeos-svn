#ifndef _WX_UTILS_BEOS_H
#define _WX_UTILS_BEOS_H

class BRect;
class wxColour;
class wxRect;

rgb_color wx2be(wxColour wxClr);
wxColour be2wx(rgb_color clr);

wxRect be2wx(BRect brect);

#endif