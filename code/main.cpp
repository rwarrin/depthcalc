#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#include "drillcalc_platform.h"
#include "drillcalc.cpp"

global_variable drill_data GlobalCenterDrillOptions[] = 
{
    {"#5-0", (1.0f/8.0f), 0.100f, 0.010f, 0.010f, 0.125f},
    {"#4-0", (1.0f/8.0f), 0.25f, 0.025f, 0.015f, 0.125f},
    {"#3-0", (1.0f/8.0f), 0.25f, 0.025f, 0.020f, 0.125f},
    {"#00", (1.0f/8.0f), 0.25f, 0.025f, 0.025f, 0.125f},
    {"#0", (1.0f/8.0f), (1.0f/32.0f), (1.0f/32.0f), 0.031f, 0.125f},
    {"#1", (1.0f/8.0f), (3.0f/64.0f), (3.0f/64.0f), 0.047f, 0.125f},
    {"#2", (3.0f/16.0f), (5.0f/64.0f), (5.0f/64.0f), 0.078f, 0.187f},
    {"#3", (1.0f/4.0f), (7.0f/64.0f), (7.0f/64.0f), 0.109f, 0.250f},
    {"#4", (5.0f/16.0f), (1.0f/8.0f), (1.0f/8.0f), 0.125f, 0.312f},
    {"#5", (7.0f/16.0f), (3.0f/16.0f), (3.0f/16.0f), 0.187f, 0.437f},
    {"#6", (1.0f/2.0f), (7.0f/32.0f), (7.0f/32.0f), 0.219f, 0.500f},
    {"#7", (5.0f/8.0f), (1.0f/4.0f), (1.0f/4.0f), 0.250f, 0.625f},
    {"#8", (3.0f/4.0f), (5.0f/16.0f), (5.0f/16.0f), 0.312f, 0.750f},
    {"#9", (7.0f/8.0f), (11.0f/32.0f), (11.0f/32.0f), 0.343f, 0.875f},
    {"#10", 1.0f, (3.0f/8.0f), (3.0f/8.0f), 0.375f, 1.0f},
};

global_variable drill_angle_data GlobalCountersinkOptions[] =
{
    {"60°", 60.0f},
    {"82°", 82.0f},
    {"90°", 90.0f},
    {"100°", 100.0f},
    {"120°", 120.0f},
};

#define CONTROL_EDIT_DIAMETER		500
#define CONTROL_EDIT_DRILLDEPTH		501
#define CONTROL_STATUS_BAR			502
#define CONTROL_EDIT_ANGLE_DIAMETER	503
#define CONTROL_EDIT_ANGLE_TIP_DIA	504
#define CONTROL_EDIT_ANGLEDEPTH		505
#define CONTROL_SELECT_DRILL		506
#define CONTROL_SELECT_ANGLE		507

#define APP_MENU_FILE_EXIT			1000
#define APP_MENU_HELP_ABOUT			1500

#define MESSAGE_CENTERDRILL_CHANGE  (WM_USER + 100)
#define MESSAGE_COUNTERSINK_CHANGE  (WM_USER + 110)

internal void
Win32CreateMenu(HWND Window)
{
    HMENU MainMenu = CreateMenu();

    HMENU FileMenu = CreateMenu();
    AppendMenuA(FileMenu, MF_STRING, APP_MENU_FILE_EXIT, "&Exit");

    HMENU HelpMenu = CreateMenu();
    AppendMenuA(HelpMenu, MF_STRING, APP_MENU_HELP_ABOUT, "About");

    AppendMenu(MainMenu, MF_POPUP, (UINT_PTR)FileMenu, "&File");
    AppendMenu(MainMenu, MF_POPUP, (UINT_PTR)HelpMenu, "&Help");
    SetMenu(Window, MainMenu);
}

static HWND
Win32CreateStaticLabel(char *Label, int X, int Y, int Width, int Height,
                       HWND ParentWindow, HINSTANCE Instance)
{
    HWND Result = CreateWindowExA(0, "static", 0,
                                  WS_CHILD | WS_VISIBLE,
                                  X, Y,
                                  Width, Height,
                                  ParentWindow, 0, Instance, 0);
    SetWindowText(Result, Label);

    return(Result);
}

static HWND
Win32CreateEditWindow(int X, int Y, int Width, int Height,
                      HMENU ControlID, DWORD ExtraClasses,
                      HWND ParentWindow, HINSTANCE Instance)
{
    HWND Result = CreateWindowExA(WS_EX_CLIENTEDGE, WC_EDIT, 0,
                                  ES_LEFT | WS_VISIBLE | WS_CHILD |
                                  WS_TABSTOP | ExtraClasses,
                                  X, Y,
                                  Width, Height,
                                  ParentWindow, ControlID,
                                  Instance, 0);
    return(Result);
}

static HWND
Win32CreateSelectWindow(int X, int Y, int Width, int Height, HMENU ControlID,
                        HWND ParentWindow, HINSTANCE Instance)
{
    HWND Result = CreateWindowExA(0, WC_COMBOBOX, 0,
                                  CBS_DROPDOWNLIST | CBS_HASSTRINGS | CBS_DISABLENOSCROLL |
                                  WS_VISIBLE | WS_CHILD | WS_VSCROLL | WS_TABSTOP,
                                  X, Y,
                                  Width, Height,
                                  ParentWindow, ControlID, Instance, 0);

    return(Result);
}

static void
Win32HandleCenterDrillChange(HWND ChamferEditWindow,
                             HWND SizeSelectWindow,
                             HWND DrillToolDepthWindow,
                             drill_data *CenterDrillOptions)
{
    char InputStr[32] = {};
    Edit_GetText(ChamferEditWindow, (LPSTR)InputStr, ArrayCount(InputStr));

    if(strlen(InputStr) != 0)
    {
        real32 InputDiameter = (real32)atof(InputStr);
        int32 SelectedDrill = ComboBox_GetCurSel(SizeSelectWindow);

        drill_data *Option = CenterDrillOptions + SelectedDrill;
        if((InputDiameter <= Option->Minimum) ||
           (InputDiameter >= Option->Maximum))
        {
            Edit_SetText(DrillToolDepthWindow, "");
        }
        else
        {
            real32 DrillDepth = CalculateDrillDepth(Option, InputDiameter);

            char OutputString[64] = {};
            _snprintf(OutputString, ArrayCount(OutputString), "%0.4f", DrillDepth);
            Edit_SetText(DrillToolDepthWindow, OutputString);
        }
    }
}

static void
Win32HandleCountersinkChange(HWND AngleEditWindow,
                             HWND ChamferEditWindow,
                             HWND TipDiameterWindow,
                             HWND ToolDepthWindow,
                             drill_angle_data *CountersinkOptions)
{
    int32 SelectedAngleIndex = ComboBox_GetCurSel(AngleEditWindow);
    drill_angle_data *AngleData = CountersinkOptions + SelectedAngleIndex;

    char BodyDiameterStr[32] = {};
    Edit_GetText(ChamferEditWindow, (LPSTR)BodyDiameterStr, ArrayCount(BodyDiameterStr));

    char TipDiameterStr[32] = {};
    Edit_GetText(TipDiameterWindow, (LPSTR)TipDiameterStr, ArrayCount(TipDiameterStr));

    if(strlen(BodyDiameterStr) != 0)
    {
        real32 BodyDiameter = atof(BodyDiameterStr);
        real32 TipDiameter = atof(TipDiameterStr);

        real32 DrillAngleDepth = CalculateDrillAngleDepth(AngleData, BodyDiameter, TipDiameter);
        char OutputString[64] = {};
        _snprintf(OutputString, ArrayCount(OutputString), "%0.4f", DrillAngleDepth);
        Edit_SetText(ToolDepthWindow, OutputString);
    }
}

LRESULT CALLBACK
WindowsCallback(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
    LRESULT Result = 0;
    switch(Message)
    {
        case WM_DESTROY:
            {
                PostQuitMessage(0);
            } break;
        case WM_COMMAND:
            {
                uint32 WParamHigh = HIWORD(WParam);
                uint32 WParamLow = LOWORD(WParam);
                if((WParamHigh == CBN_SELCHANGE) ||
                   (WParamHigh == EN_CHANGE))
                {
                    if((WParamLow == CONTROL_SELECT_DRILL) ||
                       (WParamLow == CONTROL_EDIT_DIAMETER))
                    {
                        PostMessageA(0, MESSAGE_CENTERDRILL_CHANGE, 0, 0);
                    }
                    else if((WParamLow == CONTROL_SELECT_ANGLE) ||
                            (WParamLow == CONTROL_EDIT_ANGLE_DIAMETER) ||
                            (WParamLow == CONTROL_EDIT_ANGLE_TIP_DIA))
                    {
                        PostMessageA(0, MESSAGE_COUNTERSINK_CHANGE, 0, 0);
                    }
                }
                else if(WParamLow == APP_MENU_FILE_EXIT)
                {
                    PostQuitMessage(0);
                }
                else if(WParamLow == APP_MENU_HELP_ABOUT)
                {
                    MessageBox(NULL, "DepthCalc ©2019", "About", MB_OK | MB_ICONINFORMATION);
                }
            } break;
        default:
            {
                Result = DefWindowProc(Window, Message, WParam, LParam);
            } break;
    }

    return(Result);
}

int WINAPI
WinMain(HINSTANCE Instance, HINSTANCE PrevInstance, LPSTR CmdLine, int CmdShow)
{
    WNDCLASSEXA WindowClass = {};
    WindowClass.cbSize = sizeof(WindowClass);
    WindowClass.style = CS_HREDRAW | CS_VREDRAW;
    WindowClass.lpfnWndProc = WindowsCallback;
    WindowClass.hInstance = Instance;
    WindowClass.hIcon = LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(1));
    WindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    WindowClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
    WindowClass.lpszClassName = "DrillCalcWindowClass";

    if(!RegisterClassEx(&WindowClass))
    {
        MessageBox(NULL, "Failed to register window class", "Error", MB_OK);
        return 1;
    }

    HWND Window = CreateWindowExA(0, WindowClass.lpszClassName,
                                  "Depth Calculator",
                                  (WS_OVERLAPPEDWINDOW | WS_VISIBLE) ^ (WS_SIZEBOX | WS_MAXIMIZEBOX),
                                  CW_USEDEFAULT, CW_USEDEFAULT,
                                  505, 380,
                                  0, 0, Instance, 0);

    if(!Window)
    {
        MessageBox(NULL, "Failed to create window", "Error", MB_OK);
        return 2;
    }

    Win32CreateMenu(Window);

    HWND CenterDrillSizeLabel = Win32CreateStaticLabel("Center Drill Size", 10, 10, 220, 20, Window, Instance);
    HWND CenterDrillSize = Win32CreateSelectWindow(10, 30, 240, 200, (HMENU)CONTROL_SELECT_DRILL, Window, Instance);

    HWND CenterDrillChamferLabel = Win32CreateStaticLabel("Chamfer", 260, 10, 220, 20, Window, Instance);
    HWND CenterDrillChamfer = Win32CreateEditWindow(260, 30, 220, 24, (HMENU)CONTROL_EDIT_DIAMETER, 0, Window, Instance);

    HWND CenterDrillToolDepthLabel = Win32CreateStaticLabel("Tool Depth", 260, 64, 220, 20, Window, Instance);
    HWND CenterDrillToolDepth = Win32CreateEditWindow(260, 84, 220, 24, (HMENU)CONTROL_EDIT_DRILLDEPTH,
                                                      ES_READONLY, Window, Instance);

    HWND CountersinkLabel = Win32CreateStaticLabel("Countersink", 10, 130, 220, 20, Window, Instance);
    HWND CountersinkAngle = Win32CreateSelectWindow(10, 150, 240, 200, (HMENU)CONTROL_SELECT_ANGLE, Window, Instance);

    HWND CountersinkChamferLabel = Win32CreateStaticLabel("Chamfer", 260, 130, 220, 20, Window, Instance);
    HWND CountersinkChamfer = Win32CreateEditWindow(260, 150, 220, 24, (HMENU)CONTROL_EDIT_ANGLE_DIAMETER, 0, Window, Instance);

    HWND TipDiameterLabel = Win32CreateStaticLabel("Tip Diameter", 260, 184, 220, 20, Window, Instance);
    HWND TipDiameter = Win32CreateEditWindow(260, 204, 220, 24, (HMENU)CONTROL_EDIT_ANGLE_TIP_DIA, 0, Window, Instance);

    HWND CountersinkToolDepthLabel = Win32CreateStaticLabel("Tool Depth", 260, 238, 220, 20, Window, Instance);
    HWND CountersinkToolDepth = Win32CreateEditWindow(260, 258, 220, 24, (HMENU)CONTROL_EDIT_ANGLEDEPTH,
                                                      ES_READONLY, Window, Instance);

    HWND StatusBarWindow = CreateWindowExA(0, STATUSCLASSNAME, "Ready",
                                           WS_CHILD | WS_VISIBLE,
                                           0, 0, 0, 0,
                                           Window, (HMENU)CONTROL_STATUS_BAR, Instance, 0);

    for(int Index = 0; Index < ArrayCount(GlobalCenterDrillOptions); ++Index)
    {
        drill_data *Option = GlobalCenterDrillOptions + Index;
        char NameBuffer[64] = {};
        _snprintf(NameBuffer, ArrayCount(NameBuffer), "%s (%0.3f - %0.3f)",
                  Option->Name, Option->Minimum, Option->Maximum);
        ComboBox_AddString(CenterDrillSize, NameBuffer);
    }
    SendMessage(CenterDrillSize, CB_SETCURSEL, 0, 0);

    for(int Index = 0; Index < ArrayCount(GlobalCountersinkOptions); ++Index)
    {
        ComboBox_AddString(CountersinkAngle, GlobalCountersinkOptions[Index].Name);
    }
    SendMessage(CountersinkAngle, CB_SETCURSEL, 0, 0);

    bool32 GetMessageResult = 0;
    MSG Message;
    for(;;)
    {
        GetMessageResult = GetMessage(&Message, 0, 0, 0);
        if(GetMessageResult <= 0)
        {
            break;
        }
        else
        {
            bool32 WasDialogMessage = IsDialogMessage(Window, &Message);
            if(!WasDialogMessage)
            {
                switch(Message.message)
                {
                    case MESSAGE_CENTERDRILL_CHANGE:
                    {
                        Win32HandleCenterDrillChange(CenterDrillChamfer,
                                                     CenterDrillSize,
                                                     CenterDrillToolDepth,
                                                     GlobalCenterDrillOptions);
                    } break;
                    case MESSAGE_COUNTERSINK_CHANGE:
                    {
                        Win32HandleCountersinkChange(CountersinkAngle,
                                                     CountersinkChamfer,
                                                     TipDiameter,
                                                     CountersinkToolDepth,
                                                     GlobalCountersinkOptions);
                    } break;
                    default:
                    {
                        TranslateMessage(&Message);
                        DispatchMessage(&Message);
                    } break;
                }
            }
        }
    }

    return 0;
}
