#ifndef __ASHITA_TimerInterface_H_INCLUDED__
#define __ASHITA_TimerInterface_H_INCLUDED__

#include <algorithm>
#include <chrono>
#include <list>
#include <map>
#include "C:\Program Files (x86)\Ashita 4\plugins\sdk\Ashita.h"
using namespace std::chrono;

enum class AnchorMode
{
    Fixed,
    Draggable,
    Left,
    Top,
    Right,
    Bottom
};
struct SingleTimerTemplate
{
    //Internal Usage
    IAshitaCore* m_AshitaCore;

    //Icon - Visual Only
    bool UseIcon;
    int32_t IconHeight;
    int32_t IconWidth;
    char IconPath[256];

    //Overlay - Visual Only
    bool UseOverlay;
    int32_t OverlayHeight;
    int32_t OverlayWidth;
    char OverlayPath[256];

    //Font - Visual Only(Font is set for whole panel in Text)
    char FontName[256];
    int32_t FontSize;

    //Bar - Visual Only
    bool BarOverlapsIcon;
    int32_t BarOutline;
    int32_t BarHeight;
    int32_t BarWidth;
    int32_t BarSpacing;

    //FillForward - Visual Only
    bool FillForward;

    //Appearance - Text and Visual
    int32_t BlinkFrameCount;
    int32_t BlinkThreshold;
    bool ShowMs;

    //Colors - Background and Font are visual only.  Rest are text and visual.
    D3DCOLOR FontColor;
    D3DCOLOR BackgroundColor;
    D3DCOLOR HighDurationColor;
    D3DCOLOR MiddleDurationColor;
    D3DCOLOR LowDurationColor;
    D3DCOLOR BlinkColor;

    //Timers - Text and visual.
    int32_t MiddleThreshold;
    int32_t LowThreshold;
    int32_t RemoveThreshold;

    SingleTimerTemplate(IAshitaCore* m_AshitaCore)
        : m_AshitaCore(m_AshitaCore)
        , UseIcon(true)
        , IconHeight(20)
        , IconWidth(20)
        , UseOverlay(false)
        , FontName("Courier\x00")
        , FontSize(10)
        , BarOverlapsIcon(true)
        , BarOutline(2)
        , BarHeight(6)
        , BarWidth(150)
        , BarSpacing(1)
        , FillForward(false)
        , BlinkFrameCount(15)
        , BlinkThreshold(0)
        , ShowMs(true)
        , FontColor(D3DCOLOR_ARGB(255, 255, 255, 255))
        , BackgroundColor(D3DCOLOR_ARGB(255, 0, 0, 0))
        , HighDurationColor(D3DCOLOR_ARGB(255, 0, 255, 0))
        , MiddleDurationColor(D3DCOLOR_ARGB(255, 255, 255, 0))
        , LowDurationColor(D3DCOLOR_ARGB(255, 255, 0, 0))
        , BlinkColor(D3DCOLOR_ARGB(255, 255, 0, 0))
        , MiddleThreshold(30000)
        , LowThreshold(10000)
        , RemoveThreshold(-5000)
    {
        sprintf_s(IconPath, 256, "%sresources\\timers\\buffs\\%%u.png", m_AshitaCore->GetInstallPath());
    }

};
class SingleTimerSettings
{
private:
    //Internal Usage
    IAshitaCore* m_AshitaCore;
    int32_t Duration;
    time_point<steady_clock> StartTime;
    time_point<steady_clock> EndTime;
    time_point<steady_clock> BlinkTime;
    time_point<steady_clock> MiddleTime;
    time_point<steady_clock> LowTime;
    time_point<steady_clock> RemoveTime;

    //Command - Text and Visual
    char ClickCommand[256];
    int32_t ClickMode = -1;
    bool ClickEnabled = false;

    char EndCommand[256];
    int32_t EndMode = -1;
    bool EndEnabled = false;


    //Icon - Visual Only
    bool UseIcon = false;
    char IconFile[256];
    int32_t IconHeight;
    int32_t IconWidth;

    //Overlay - Visual Only
    bool UseOverlay = false;
    char OverlayFile[256];
    int32_t OverlayHeight;
    int32_t OverlayWidth;

    //Text - Text and Visual
    char DisplayText[256];

    //Font - Visual Only(Font is set for whole panel in Text)
    char FontName[256] = "Courier";
    int32_t FontSize = 10;

    //Position - Visual Only(Internal use only)
    int32_t PositionX = 0;
    int32_t PositionY = 0;

    //Bar - Visual Only
    bool BarOverlapsIcon   = false;
    int32_t BarOutline = 2;
    int32_t BarHeight  = 6;
    int32_t BarWidth   = 150;
    int32_t BarSpacing = 1;

    //FillForward - Visual Only
    bool FillForward        = false;
    
    //Appearance - Text and Visual
    int32_t BlinkFrameCount = 15;
    int32_t BlinkThreshold  = 0;
    bool ShowMs             = true;

    //Colors - Background and Font are visual only.  Rest are text and visual.
    D3DCOLOR FontColor           = D3DCOLOR_ARGB(255, 255, 255, 255);
    D3DCOLOR BackgroundColor     = D3DCOLOR_ARGB(255, 0, 0, 0);
    D3DCOLOR HighDurationColor   = D3DCOLOR_ARGB(255, 0, 255, 0);
    D3DCOLOR MiddleDurationColor = D3DCOLOR_ARGB(255, 255, 255, 0);
    D3DCOLOR LowDurationColor    = D3DCOLOR_ARGB(255, 255, 0, 0);
    D3DCOLOR BlinkColor          = D3DCOLOR_ARGB(255, 255, 0, 0);

    //Timers - Text and visual.
    int32_t MiddleThreshold = 30000;
    int32_t LowThreshold    = 10000;
    int32_t RemoveThreshold = -5000;

public:
    SingleTimerSettings(IAshitaCore* AshitaCore, time_point<steady_clock> StartTime, int32_t Duration, const char* DisplayText)
    {
        m_AshitaCore = AshitaCore;
        strcpy_s(this->DisplayText, 256, DisplayText);
        SetDuration(StartTime, Duration);
    }

    SingleTimerSettings(SingleTimerTemplate Settings, time_point<steady_clock> StartTime, int32_t Duration, const char* DisplayText, uint32_t Id)
        : m_AshitaCore(Settings.m_AshitaCore)
        , UseIcon(Settings.UseIcon)
        , UseOverlay(Settings.UseOverlay)
        , FontSize(Settings.FontSize)
        , BarOverlapsIcon(Settings.BarOverlapsIcon)
        , BarOutline(Settings.BarOutline)
        , BarHeight(Settings.BarHeight)
        , BarWidth(Settings.BarWidth)
        , BarSpacing(Settings.BarSpacing)
        , FillForward(Settings.FillForward)
        , BlinkFrameCount(Settings.BlinkFrameCount)
        , BlinkThreshold(Settings.BlinkThreshold)
        , ShowMs(Settings.ShowMs)
        , FontColor(Settings.FontColor)
        , BackgroundColor(Settings.BackgroundColor)
        , HighDurationColor(Settings.HighDurationColor)
        , MiddleDurationColor(Settings.MiddleDurationColor)
        , LowDurationColor(Settings.LowDurationColor)
        , BlinkColor(Settings.BlinkColor)
        , MiddleThreshold(Settings.MiddleThreshold)
        , LowThreshold(Settings.LowThreshold)
        , RemoveThreshold(Settings.RemoveThreshold)
    {
        if (Settings.UseIcon)
        {
            UseIcon    = Settings.UseIcon;
            IconHeight = Settings.IconHeight;
            IconWidth  = Settings.IconWidth;
            sprintf_s(IconFile, 256, Settings.IconPath, Id);
        }
        if (Settings.UseOverlay)
        {
            UseOverlay = true;
            OverlayHeight = Settings.OverlayHeight;
            OverlayWidth  = Settings.OverlayWidth;
            strcpy_s(OverlayFile, 256, Settings.OverlayPath);
        }
        strcpy_s(FontName, 256, Settings.FontName);
        strcpy_s(this->DisplayText, 256, DisplayText);
        SetDuration(StartTime, Duration);
    }

    void SetDuration(time_point<steady_clock> StartTime, int32_t Duration)
    {
        this->StartTime = StartTime;
        this->Duration  = Duration;
        EndTime         = StartTime + milliseconds(Duration);
        BlinkTime       = EndTime - milliseconds(BlinkThreshold);
        MiddleTime      = EndTime - milliseconds(MiddleThreshold);
        LowTime         = EndTime - milliseconds(LowThreshold);
        RemoveTime      = EndTime - milliseconds(RemoveThreshold);
    }

    bool GetIsBlinking()
    {
        if (BlinkFrameCount == 0)
            return false;
        return (steady_clock::now() > BlinkTime);
    }
    bool GetIsFinished()
    {
        if (steady_clock::now() >= RemoveTime)
        {
            if (EndEnabled)
                m_AshitaCore->GetChatManager()->QueueCommand(EndMode, EndCommand);
            return true;
        }
        return false;
    }

    int32_t GetDurationMs()
    {
        return Duration;
    }

    int32_t GetElapsedTimeMs()
    {
        return duration_cast<milliseconds>(steady_clock::now() - StartTime).count();
    }

    int32_t GetRemainingTimeMs()
    {
        if (steady_clock::now() >= EndTime)
            return 0;

        return duration_cast<milliseconds>(EndTime - steady_clock::now()).count();
    }

    int32_t GetRemoveTimeMs()
    {
        if (steady_clock::now() >= RemoveTime)
            return 0;

        return duration_cast<milliseconds>(RemoveTime - steady_clock::now()).count();
    }

    D3DCOLOR GetCurrentColor()
    {
        if (steady_clock::now() > LowTime)
            return LowDurationColor;
        else if (steady_clock::now() > MiddleTime)
            return MiddleDurationColor;
        else
            return HighDurationColor;
    }

    int32_t GetTotalHeight()
    {
        return max(IconHeight, BarHeight + FontSize + (BarOutline * 2));
    }

    int32_t GetTotalWidth()
    {
        if ((UseIcon) && (!BarOverlapsIcon))
            return BarWidth + (BarOutline * 2) + IconWidth;
        else
            return BarWidth + (BarOutline * 2);
    }

    const char* GetEndCommand()
    {
        if (!EndEnabled)
            return NULL;
        return EndCommand;
    }
    int32_t GetEndMode()
    {
        return EndMode;
    }

    void DisableEndCommand()
    {
        EndEnabled = false;
    }
    void SetEndCommand(const char* Command, int32_t Mode)
    {
        strcpy_s(EndCommand, 256, Command);
        EndMode    = Mode;
        EndEnabled = true;
    }

    bool GetClickEnabled()
    {
        return ClickEnabled;
    }
    const char* GetClickCommand()
    {
        if (!ClickEnabled)
            return NULL;
        return ClickCommand;
    }
    int32_t GetClickMode()
    {
        return ClickMode;
    }

    void DisableClickCommand()
    {
        ClickEnabled = false;
    }
    void SetClickCommand(const char* Command, int32_t Mode)
    {
        strcpy_s(ClickCommand, 256, Command);
        ClickMode    = Mode;
        ClickEnabled = true;
    }

    void DisableIcon()
    {
        UseIcon = false;
    }
    void SetIcon(const char* Filename, int32_t Height, int32_t Width)
    {
        UseIcon = true;
        strcpy_s(IconFile, 256, Filename);
        IconHeight = Height;
        IconWidth  = Width;
    }
    bool GetIconEnabled()
    {
        return UseIcon;
    }
    const char* GetIconFile()
    {
        if (!UseIcon)
            return NULL;
        return IconFile;
    }
    int32_t GetIconHeight()
    {
        if (!UseIcon)
            return 0;
        return IconHeight;
    }
    int32_t GetIconWidth()
    {
        if (!UseIcon)
            return 0;
        return IconWidth;
    }
    bool GetOverlapIcon()
    {
        return BarOverlapsIcon;
    }
    void SetOverlapIcon(bool Setting)
    {
        BarOverlapsIcon = Setting;
    }

    bool GetOverlayEnabled()
    {
        return UseOverlay;
    }
    const char* GetOverlayFile()
    {
        if (!UseOverlay)
            return NULL;
        return OverlayFile;
    }
    void DisableOverlay()
    {
        UseOverlay = false;
    }
    void SetOverlay(const char* Filename)
    {
        UseOverlay = true;
        strcpy_s(OverlayFile, 256, Filename);
    }

    const char* GetDisplayText()
    {
        return DisplayText;
    }
    void SetDisplayText(const char* Text)
    {
        strcpy_s(DisplayText, 256, Text);
    }
    const char* GetFontName()
    {
        return FontName;
    }
    int32_t GetFontSize()
    {
        return FontSize;
    }
    void SetFont(const char* Name, int32_t Size)
    {
        strcpy_s(FontName, 256, Name);
        FontSize = Size;
    }

    int32_t GetPositionX()
    {
        return PositionX;
    }
    int32_t GetPositionY()
    {
        return PositionY;
    }
    void SetPosition(int32_t X, int32_t Y)
    {
        PositionX = X;
        PositionY = Y;
    }

    int32_t GetBarOutline()
    {
        return BarOutline;
    }
    int32_t GetBarHeight()
    {
        return BarHeight;
    }
    int32_t GetBarWidth()
    {
        return BarWidth;
    }
    int32_t GetBarSpacing()
    {
        return BarSpacing;
    }
    void SetBarSize(int32_t Outline, int32_t Height, int32_t Width, int32_t Spacing)
    {
        BarOutline = Outline;
        BarHeight  = Height;
        BarWidth   = Width;
        BarSpacing = Spacing;
    }

    bool GetFillForward()
    {
        return FillForward;
    }
    void SetFillForward(bool Enabled)
    {
        FillForward = Enabled;
    }
    int32_t GetBlinkFrameCount()
    {
        return BlinkFrameCount;
    }
    void DisableBlink()
    {
        BlinkFrameCount = 0;
    }
    void SetBlinkTiming(int32_t FrameCount, int32_t Threshold)
    {
        BlinkFrameCount = min(1, FrameCount);
        BlinkThreshold  = Threshold;
        BlinkTime       = EndTime - milliseconds(BlinkThreshold);
    }

    bool GetShowMs()
    {
        return ShowMs;
    }
    void SetShowMs(bool Setting)
    {
        ShowMs = Setting;
    }

    D3DCOLOR GetFontColor()
    {
        return FontColor;
    }
    void SetFontColor(D3DCOLOR Color)
    {
        FontColor = Color;
    }
    D3DCOLOR GetBackgroundColor()
    {
        return BackgroundColor;
    }
    void SetBackgroundColor(D3DCOLOR Color)
    {
        BackgroundColor = Color;
    }
    D3DCOLOR GetHighDurationColor()
    {
        return HighDurationColor;
    }
    void SetHighDurationColor(D3DCOLOR Color)
    {
        HighDurationColor = Color;
    }
    D3DCOLOR GetMiddleDurationColor()
    {
        return MiddleDurationColor;
    }
    void SetMiddleDurationColor(D3DCOLOR Color)
    {
        MiddleDurationColor = Color;
    }
    D3DCOLOR GetLowDurationColor()
    {
        return LowDurationColor;
    }
    void SetLowDurationColor(D3DCOLOR Color)
    {
        LowDurationColor = Color;
    }
    D3DCOLOR GetBlinkColor()
    {
        return BlinkColor;
    }
    void SetBlinkColor(D3DCOLOR Color)
    {
        BlinkColor = Color;
    }

    void SetMiddleThreshold(int32_t ms)
    {
        MiddleThreshold = ms;
        MiddleTime      = EndTime - milliseconds(MiddleThreshold);
    }
    void SetLowThreshold(int32_t ms)
    {
        LowThreshold = ms;
        LowTime      = EndTime - milliseconds(LowThreshold);
    }
    void SetRemoveThreshold(int32_t ms)
    {
        RemoveThreshold = ms;
        RemoveTime      = EndTime - milliseconds(RemoveThreshold);
    }
};
class TimerPanelSettings
{
private:
    bool UseProgressBars         = true;
    AnchorMode CurrentAnchorMode = AnchorMode::Draggable;
    char AnchorOverlay[256] = {0};
    int32_t PositionX      = 100;
    int32_t PositionY       = 100;
    D3DCOLOR BackgroundColor = D3DCOLOR_ARGB(0, 0, 0, 0);
    int32_t MaxVisibleBars   = 8;
    bool ReverseSort         = false;

    //Font only used in text mode.
    char Font[256] = "Courier";
    int32_t FontSize = 8;
    int32_t CharacterWidth = 30;

    //Borders only used if progress bars enabled.
    int32_t BorderX          = 1;
    int32_t BorderY          = 1;

public:
    TimerPanelSettings()
    {}
    TimerPanelSettings(bool ProgressBars)
    {
        UseProgressBars = ProgressBars;
    }

    bool GetUseProgressBars()
    {
        return UseProgressBars;
    }

    AnchorMode GetAnchorMode()
    {
        return CurrentAnchorMode;
    }
    const char* GetAnchorOverlay()
    {
        return AnchorOverlay;
    }

    //PanelName must be set if mode is relative.  It may be NULL if mode is Fixed or Draggable.
    void SetAnchorMode(AnchorMode Mode, const char* PanelName)
    {
        CurrentAnchorMode = Mode;
        if (PanelName == NULL)
            memset(AnchorOverlay, 0, 256);
        else
            strcpy_s(AnchorOverlay, 256, PanelName);
    }

    const char* GetFontName()
    {
        return Font;
    }
    int32_t GetFontSize()
    {
        return FontSize;
    }
    void SetFont(const char* Name, int32_t Size)
    {
        strcpy_s(Font, 256, Name);
        FontSize = Size;
    }
    int32_t GetCharacterWidth()
    {
        return CharacterWidth;
    }
    void SetCharacterWidth(int32_t Width)
    {
        CharacterWidth = Width;
    }

    int32_t GetPositionX()
    {
        return PositionX;
    }
    int32_t GetPositionY()
    {
        return PositionY;
    }
    void SetPosition(int32_t X, int32_t Y)
    {
        PositionX = max(0, X);
        PositionY = max(0, Y);
    }

    int32_t GetBorderX()
    {
        return BorderX;
    }
    int32_t GetBorderY()
    {
        return BorderY;
    }
    void SetBorder(int32_t X, int32_t Y)
    {
        BorderX = X;
        BorderY = Y;
    }

    D3DCOLOR GetBackgroundColor()
    {
        return BackgroundColor;
    }
    void SetBackgroundColor(D3DCOLOR Color)
    {
        BackgroundColor = Color;
    }

    int32_t GetMaxVisibleBars()
    {
        return MaxVisibleBars;
    }
    void SetMaxVisibleBars(int32_t Count)
    {
        MaxVisibleBars = Count;
    }

    bool GetReverseSort()
    {
        return ReverseSort;
    }
    void SetReverseSort(bool Enabled)
    {
        ReverseSort = Enabled;
    }
};

class TimerSingle
{
public:
    virtual ~TimerSingle()
    { }
    virtual SingleTimerSettings* GetSettings() = 0;
    virtual void SetSettings(SingleTimerSettings Settings) = 0;
};
class TimerPanel
{
public:
    virtual ~TimerPanel()
    { }
    virtual TimerPanelSettings* GetSettings() = 0;
    virtual TimerSingle* CreateTimer(SingleTimerSettings Settings) = 0;
    virtual bool CallClickEvent(int32_t xpos, int32_t ypos) = 0;
    virtual void UpdatePanel() = 0;
};

class TimerGlobals
{
public:
    static std::map<IFontObject*, TimerPanel*> gPanelMap;
};

extern bool __stdcall gPanelClick(Ashita::MouseEvent eventId, void* object, int32_t xpos, int32_t ypos);

/* Put function definition somewhere in a code file.

std::map<IFontObject*, TimerPanel*> TimerGlobals::gPanelMap = std::map<IFontObject*, TimerPanel*>();
bool __stdcall gPanelClick(Ashita::MouseEvent eventId, void* object, int32_t xpos, int32_t ypos)
{
    if (eventId != Ashita::MouseEvent::ClickLeft)
        return false;

    bool retValue                                      = false;
    std::map<IFontObject*, TimerPanel*>::iterator iter = TimerGlobals::gPanelMap.find((IFontObject*)object);
    if (iter != TimerGlobals::gPanelMap.end())
    {
        if (iter->second->CallClickEvent(xpos, ypos))
            retValue = true;
    }

    return retValue;
}

*/
#endif