#ifndef __ASHITA_TextTimer_H_INCLUDED__
#define __ASHITA_TextTimer_H_INCLUDED__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "TimerInterface.h"

class AshitaTextTimer : public TimerSingle
{
private:
    IAshitaCore* m_AshitaCore;
    SingleTimerSettings mSettings;
    bool mBlinkActive;
    int32_t mBlinkFrameCount;

public:
    AshitaTextTimer(IAshitaCore* AshitaCore, SingleTimerSettings Settings)
        : m_AshitaCore(AshitaCore)
        , mSettings(Settings)
        , mBlinkFrameCount(0)
        , mBlinkActive(false)
    {}

    ~AshitaTextTimer()
    {}

public:
    bool ClickEvent()
    {
        if (!mSettings.GetClickEnabled())
            return false;

        m_AshitaCore->GetChatManager()->QueueCommand(mSettings.GetClickMode(), mSettings.GetClickCommand());
        return true;
    }
    SingleTimerSettings* GetSettings()
    {
        return &mSettings;
    }
    void SetSettings(SingleTimerSettings Settings)
    {
        mSettings        = Settings;
        mBlinkFrameCount = 0;
        mBlinkActive     = false;
    }
    std::string GetString(int Width)
    {
        std::string Base = GetColor(mSettings.GetCurrentColor());
        if (mSettings.GetIsBlinking())
        {
            mBlinkFrameCount++;
            if (mBlinkFrameCount >= mSettings.GetBlinkFrameCount())
            {
                mBlinkActive     = !mBlinkActive;
                mBlinkFrameCount = 0;
            }

            if (mBlinkActive)
            {
                Base = GetColor(mSettings.GetBlinkColor());
            }
        }

        std::string LeftText(mSettings.GetDisplayText());
        std::string RightText;

        //Mostly self explanatory, use remaining time to set text.
        int32_t remainingTime = mSettings.GetRemainingTimeMs();
        if (remainingTime == 0)
        {
            if (mSettings.GetShowMs())
                RightText = "0.0s";
            else
                RightText = "0.0s";
        }
        else if ((remainingTime < 60000) && (mSettings.GetShowMs()))
        {
            char durationBuffer[32];
            sprintf_s(durationBuffer, 32, "%d.%ds", remainingTime / 1000, (remainingTime % 1000) / 100);
            RightText = durationBuffer;
        }
        else
        {
            char durationBuffer[32];
            sprintf_s(durationBuffer, 32, "%d:%02d", remainingTime / 60000, (remainingTime % 60000) / 1000);
            RightText = durationBuffer;
        }

        int32_t LeftWidth = Width - (RightText.length() + 1);
        if (LeftWidth > LeftText.length())
        {
            LeftText = LeftText.append(LeftWidth - LeftText.length(), ' ');
        }
        else if (LeftText.length() > LeftWidth)
        {
            LeftText = LeftText.substr(0, LeftWidth);
        }

        return (Base + LeftText + std::string(" ") + RightText);
    }

private:
    std::string GetColor(D3DCOLOR Input)
    {
        char buffer[256];
        sprintf_s(buffer, 256, "|c%8X|", Input);
        return std::string(buffer);
    }
};
class AshitaTextTimerManager : public TimerPanel
{
private:
    IAshitaCore* m_AshitaCore;
    IFontObject* pFontObject = NULL;
    std::list<AshitaTextTimer*> mTimerList;
    TimerPanelSettings mSettings;
    int mBarCount;
    std::map<int, AshitaTextTimer*> mPositionMap;

public:
    char mIdentifier[256] = {0};

    AshitaTextTimerManager(IAshitaCore* AshitaCore, TimerPanelSettings Settings, const char* Identifier)
        : m_AshitaCore(AshitaCore)
        , mSettings(Settings)
    {
        strcpy_s(mIdentifier, 256, Identifier);
        Initialize();
    }

    ~AshitaTextTimerManager()
    {
        //Remove all bars.
        std::list<AshitaTextTimer*>::iterator it = mTimerList.begin();
        while (it != mTimerList.end())
        {
            delete (*it);
            mTimerList.erase(it++);
        }

        //Delete font objects.
        m_AshitaCore->GetFontManager()->Delete(pFontObject->GetAlias());
        TimerGlobals::gPanelMap.erase(pFontObject);
    }

public:
    TimerPanelSettings* GetSettings()
    {
        return &mSettings;
    }
    TimerSingle* CreateTimer(SingleTimerSettings Settings)
    {
        AshitaTextTimer* newTimer = NULL;

        //If we have an existing bar with the same text, apply new settings instead of creating a new bar.
        for (std::list<AshitaTextTimer*>::iterator iter = mTimerList.begin(); iter != mTimerList.end(); iter++)
        {
            if (_stricmp((*iter)->GetSettings()->GetDisplayText(), Settings.GetDisplayText()) == 0)
            {
                (*iter)->SetSettings(Settings);
                newTimer = *iter;
                break;
            }
        }

        //If no existing bar was found, create a new one then recreate overlay to ensure it stays on top.
        if (newTimer == NULL)
        {
            newTimer = new AshitaTextTimer(m_AshitaCore, Settings);
            mTimerList.push_back(newTimer);
        }

        return (TimerSingle*)newTimer;
    }
    bool CallClickEvent(int32_t xpos, int32_t ypos)
    {
        //Divide out panel height by number of lines to determine which line was clicked.
        int PixelsPerLine = pFontObject->GetBackground()->GetHeight() / mBarCount;
        int RelativeClick = ypos - pFontObject->GetBackground()->GetPositionY();
        int Offset        = RelativeClick / PixelsPerLine;
        if (GetKeyState(VK_CONTROL) & 0x8000)
        {
            for (std::list<AshitaTextTimer*>::iterator iter = mTimerList.begin(); iter != mTimerList.end();)
            {
                if (*iter == mPositionMap[Offset])
                {
                    delete *iter;
                    iter = mTimerList.erase(iter);
                    return true;
                }
                else
                    iter++;
            }
            return false;
        }
        else
        {
            return mPositionMap[Offset]->ClickEvent();
        }
    }
    void UpdatePanel()
    {
        SortBars();
        UpdateTimers();
        UpdatePanelLocation();
    }

private:
    void Initialize()
    {
        //Initialize the background and overlays.
        char buffer[256] = {0};
        sprintf_s(buffer, 256, "%s_overlay", mIdentifier);
        pFontObject = m_AshitaCore->GetFontManager()->Create(buffer);
        pFontObject->SetFontFamily(mSettings.GetFontName());
        pFontObject->SetFontHeight(mSettings.GetFontSize());
        pFontObject->SetPadding(0);
        pFontObject->SetAutoResize(true);
        pFontObject->GetBackground()->SetColor(mSettings.GetBackgroundColor());
        pFontObject->GetBackground()->SetVisible(true);
        pFontObject->SetPositionX(mSettings.GetPositionX());
        pFontObject->SetPositionY(mSettings.GetPositionY());
        pFontObject->SetVisible(true);
        TimerGlobals::gPanelMap.insert(std::make_pair(pFontObject, (TimerPanel*)this));
        pFontObject->SetMouseCallback(gPanelClick);
    }
    void SortBars()
    {
        mTimerList.sort([](AshitaTextTimer* a, AshitaTextTimer* b) {
            return (a->GetSettings()->GetRemainingTimeMs() < b->GetSettings()->GetRemainingTimeMs());
        });
    }
    void UpdatePanelLocation()
    {
        int32_t Height = pFontObject->GetBackground()->GetHeight();
        int32_t Width = pFontObject->GetBackground()->GetWidth();
        //If we aren't in fixed mode, default to leaving it alone.  If an anchor overlay is found, we'll update it here and it'll be corrected before it actually moves.
        if (mSettings.GetAnchorMode() != AnchorMode::Fixed)
        {
            mSettings.SetPosition(pFontObject->GetPositionX(), pFontObject->GetPositionY());
        }

        if (mSettings.GetAnchorMode() == AnchorMode::Left)
        {
            IFontObject* Object = m_AshitaCore->GetFontManager()->Get(mSettings.GetAnchorOverlay());
            if (Object)
            {
                mSettings.SetPosition(Object->GetPositionX() - Width, Object->GetPositionY());
            }
        }

        else if (mSettings.GetAnchorMode() == AnchorMode::Top)
        {
            IFontObject* Object = m_AshitaCore->GetFontManager()->Get(mSettings.GetAnchorOverlay());
            if (Object)
            {
                mSettings.SetPosition(Object->GetPositionX(), Object->GetPositionY() - Height);
            }
        }

        else if (mSettings.GetAnchorMode() == AnchorMode::Right)
        {
            IFontObject* Object = m_AshitaCore->GetFontManager()->Get(mSettings.GetAnchorOverlay());
            if (Object)
            {
                mSettings.SetPosition(Object->GetPositionX() + Object->GetBackground()->GetWidth(), Object->GetPositionY());
            }
        }

        else if (mSettings.GetAnchorMode() == AnchorMode::Bottom)
        {
            IFontObject* Object = m_AshitaCore->GetFontManager()->Get(mSettings.GetAnchorOverlay());
            if (Object)
            {
                mSettings.SetPosition(Object->GetPositionX(), Object->GetPositionY() + Object->GetBackground()->GetHeight());
            }
        }

        //Move the overlays.
        pFontObject->SetPositionX(mSettings.GetPositionX());
        pFontObject->SetPositionY(mSettings.GetPositionY());
    }
    void UpdateTimers()
    {
        //Create variables to store some information on bars as we iterate.
        int32_t barCount = 0;
        std::string TextBuffer;

        std::list<AshitaTextTimer*> insertOrder;

        //Iterate bars, setting locations.
        for (std::list<AshitaTextTimer*>::iterator iter = mTimerList.begin(); iter != mTimerList.end();)
        {
            //Remove bar if it's ready to be removed.
            if ((*iter)->GetSettings()->GetIsFinished())
            {
                delete *iter;
                iter = mTimerList.erase(iter);
                continue;
            }
            //Hide bar if we've already displayed our max.
            else if (barCount >= mSettings.GetMaxVisibleBars())
            {
                iter++;
                continue;
            }

            if (!mSettings.GetReverseSort())
            {
                if (barCount != 0)
                    TextBuffer = TextBuffer.append("\n");

                TextBuffer = TextBuffer.append((*iter)->GetString(mSettings.GetCharacterWidth()));
            }
            else
            {
                if (TextBuffer.size() < 1)
                {
                    TextBuffer = (*iter)->GetString(mSettings.GetCharacterWidth());                
                }
                else
                {
                    TextBuffer = TextBuffer.insert(0, (*iter)->GetString(mSettings.GetCharacterWidth()) + "\n");
                }
            }
            insertOrder.push_back(*iter);            

            barCount++;
            iter++;
        }

        mPositionMap.clear();
        if (!mSettings.GetReverseSort())
        {
            for (int x = 0; x < barCount; x++)
            {
                mPositionMap[x] = *(insertOrder.begin());
                insertOrder.pop_front();
            }
        }
        else
        {
            for (int x = 0; x < barCount; x++)
            {
                mPositionMap[(barCount - x) - 1] = *(insertOrder.begin());
                insertOrder.pop_front();
            }
        }

        pFontObject->SetText(TextBuffer.c_str());
        mBarCount = barCount;
    }
};
#endif