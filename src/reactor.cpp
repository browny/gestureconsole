
#include "reactor.h"
#include <iostream>
using namespace std;

Reactor::Reactor() {}

void Reactor::react(GesName ges, int targ)
{
	/* Pre:  a gesture name 
	 * Post: trigger the corresponding action
	 */

	switch (ges)
	{
	case Up:		

		
		//cout << "up" << endl;
			
		// for PowerPoint (full screen)
		if (targ == 0) {
			/*keybd_event(VK_SHIFT, MapVirtualKey(VK_SHIFT, 0), 0, 0);
			keybd_event(VK_F5, MapVirtualKey(VK_F5, 0), 0, 0);
			keybd_event(VK_SHIFT, MapVirtualKey(VK_SHIFT, 0), KEYEVENTF_KEYUP, 0);
			keybd_event(VK_F5, MapVirtualKey(VK_F5, 0), KEYEVENTF_KEYUP, 0);*/
		}

		// for EV
		if (targ == 1) {

			/*::PostMessage(::FindWindow(L"EasyViewer", L"EasyViewer 3.0"),
				WM_KEYUP, VK_ESCAPE, NULL);*/

			/*mode = SendMessage(FindWindow(L"msi EasyViewer",NULL),WM_USER + 103,NULL,NULL);
			if (mode==3){
				::PostMessage(::FindWindow(L"msi EasyViewer",NULL), WM_KEYDOWN, 'W', NULL);
				::PostMessage(::FindWindow(L"msi EasyViewer",NULL), WM_KEYUP, 'W', NULL);
			} else{
				::PostMessage(::FindWindow(L"msi EasyViewer",NULL), WM_KEYDOWN, 38, NULL);
				::PostMessage(::FindWindow(L"msi EasyViewer",NULL), WM_KEYUP, 38, NULL);
			}*/
		}

		// for media
		if (targ == 2) {
			/*
			keybd_event(VK_F10, MapVirtualKey(VK_F10, 0), 0, 0);			
			keybd_event(VK_F10, MapVirtualKey(VK_F10, 0), KEYEVENTF_KEYUP, 0);
			*/
		}

		

		break;

	case Down:

		
		//cout << "down" << endl;

	
		// for PowerPoint (exit full screen)
		if (targ == 0) {
			/*keybd_event(VK_ESCAPE, MapVirtualKey(VK_ESCAPE, 0), 0, 0);
			keybd_event(VK_ESCAPE, MapVirtualKey(VK_ESCAPE, 0), KEYEVENTF_KEYUP, 0);*/
		}

		// for EV
		if (targ == 1) {

			/*::PostMessage(::FindWindow(L"EasyViewer", L"EasyViewer 3.0"),
				WM_KEYUP, VK_RETURN, NULL);*/

			/*mode = SendMessage(FindWindow(L"msi EasyViewer",NULL),WM_USER + 103,NULL,NULL);
			if (mode==3){
				::PostMessage(::FindWindow(L"msi EasyViewer",NULL), WM_KEYDOWN, 'C', NULL);
				::PostMessage(::FindWindow(L"msi EasyViewer",NULL), WM_KEYUP, 'C', NULL);
			} else{
				::PostMessage(::FindWindow(L"msi EasyViewer",NULL), WM_KEYDOWN, 40, NULL);
				::PostMessage(::FindWindow(L"msi EasyViewer",NULL), WM_KEYUP, 40, NULL);
			}*/
		}

		// for media
		if (targ == 2) {
			/*
			keybd_event(VK_F9, MapVirtualKey(VK_F9, 0), 0, 0);			
			keybd_event(VK_F9, MapVirtualKey(VK_F9, 0), KEYEVENTF_KEYUP, 0);
			*/
		}
		

		break;

	case Left:

		//cout << "left" << endl;				
		

		// for Powerpoint (simulate a key press for page up)
		if (targ == 0) {
			/*
			keybd_event( VK_PRIOR, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0 );		
			keybd_event( VK_PRIOR, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);			
			*/
		}

		// for EV
		if (targ == 1) {
			/*::PostMessage(::FindWindow(L"EasyViewer", L"EasyViewer 3.0"),
				WM_KEYUP, VK_LEFT, NULL);*/

			/*::PostMessage(::FindWindow(L"msi EasyViewer",NULL), WM_KEYDOWN, 39, NULL);
			::PostMessage(::FindWindow(L"msi EasyViewer",NULL), WM_KEYUP, 39, NULL);*/
		}

		// for media
		if (targ == 2) {
			/*
			keybd_event(VK_CONTROL, MapVirtualKey(VK_CONTROL, 0), 0, 0);
			keybd_event(0x42, MapVirtualKey(0x46, 0), 0, 0);
			keybd_event(VK_CONTROL, MapVirtualKey(VK_CONTROL, 0), KEYEVENTF_KEYUP, 0);
			keybd_event(0x42, MapVirtualKey(0x46, 0), KEYEVENTF_KEYUP, 0);
			*/
		}
					
		break;

	case Right:

		//cout << "right" << endl;				
		

		// for Powerpoint (simulate a key press for page down)
		if (targ == 0) {
			/*
			keybd_event( VK_NEXT, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0 );
			keybd_event( VK_NEXT, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);			
			*/
		}

		// for EV
		if (targ == 1) {

			/*::PostMessage(::FindWindow(L"EasyViewer", L"EasyViewer 3.0"),
				WM_KEYUP, VK_RIGHT, NULL);*/
			/*::PostMessage(::FindWindow(L"msi EasyViewer",NULL), WM_KEYDOWN, 37, NULL);
			::PostMessage(::FindWindow(L"msi EasyViewer",NULL), WM_KEYUP, 37, NULL);*/
		}		

		// for media
		if (targ == 2) {
			/*
			keybd_event(VK_CONTROL, MapVirtualKey(VK_CONTROL, 0), 0, 0);
			keybd_event(0x46, MapVirtualKey(0x46, 0), 0, 0);
			keybd_event(VK_CONTROL, MapVirtualKey(VK_CONTROL, 0), KEYEVENTF_KEYUP, 0);
			keybd_event(0x46, MapVirtualKey(0x46, 0), KEYEVENTF_KEYUP, 0);
			*/
		}

		break;

	case Clock:
		
		//cout << "clock" << endl;
		
		
		// for EV
		/*
		mode = SendMessage(FindWindow(L"msi EasyViewer",NULL),WM_USER + 103,NULL,NULL);
		if (mode==0 || mode == 1 || mode==3){ //進下一個mode
			::PostMessage(::FindWindow(L"msi EasyViewer",NULL), WM_KEYDOWN, 13, NULL);
			::PostMessage(::FindWindow(L"msi EasyViewer",NULL), WM_KEYUP, 13, NULL);
		} else if (mode==3) {
			
		}
		*/
		

		break;

	case UnClock:

		//cout << "counter-clock" << endl;

		
		// for EV
		/*mode = SendMessage(FindWindow(L"msi EasyViewer",NULL),WM_USER + 103,NULL,NULL);
		if (mode==1){ 
		::PostMessage(::FindWindow(L"msi EasyViewer",NULL), WM_KEYDOWN, 8, NULL);
		::PostMessage(::FindWindow(L"msi EasyViewer",NULL), WM_KEYUP, 8, NULL);
		}
		if (mode==3){ 
		::PostMessage(::FindWindow(L"msi EasyViewer",NULL), WM_KEYDOWN, 13, NULL); 
		::PostMessage(::FindWindow(L"msi EasyViewer",NULL), WM_KEYUP, 13, NULL);

		}*/
		
		

		break;	

	case Wrong:

		//cout << "wrong" << endl;

		break;

	case None:

		//cout << "none" << endl;

		break;

	}
	
}
