#include "TrackBar.hpp"
#include <Windows.h>
#include <CommCtrl.h>

HWND WINAPI createTrackBar(
    const HWND& parent, //parent window handle
    const HINSTANCE& hInstance, //process handle
    LONG x, //x coordinate to place the trackbar
    LONG y, //y coordinate to place the trackbar
    UINT width, //horizontal size of the trackbar
    UINT height, //vertical size of the trackbar
    UINT iMin,     // minimum value in trackbar range 
    UINT iMax,     // maximum value in trackbar range 
    UINT iSelMin,  // minimum value in trackbar selection 
    UINT iSelMax)  // maximum value in trackbar selection  
{
    HWND trackBar;


    if (!(trackBar = CreateWindowEx(
        0,                              // Optional window styles.
        TRACKBAR_CLASS,                     // Window class
        L"Track Bar",       // Window text
        WS_CHILD | TBS_ENABLESELRANGE | WS_VISIBLE, // Window style

        // Position and size
        x, y, width, height,

        parent,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL
    )))
    {
        MessageBox(NULL, L"Failed to create track bar.", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return NULL;
    }

    SendMessage(trackBar, TBM_SETRANGE,
        (WPARAM)TRUE,                   // redraw flag 
        (LPARAM)MAKELONG(iMin, iMax));  // min. & max. positions

    SendMessage(trackBar, TBM_SETPAGESIZE,
        0, (LPARAM)4);                  // new page size 

    SendMessage(trackBar, TBM_SETSEL,
        (WPARAM)FALSE,                  // redraw flag 
        (LPARAM)MAKELONG(iSelMin, iSelMax));

    SendMessage(trackBar, TBM_SETPOS,
        (WPARAM)TRUE,                   // redraw flag 
        (LPARAM)10); //set pos to starting simulation size

    //SetFocus(trackBar);

    
    return trackBar;
}