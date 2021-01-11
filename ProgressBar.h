#ifndef __ASHITA_ProgressBar_H_INCLUDED__
#define __ASHITA_ProgressBar_H_INCLUDED__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <algorithm>
#include <chrono>
#include <list>
#include "C:\Program Files (x86)\Ashita 4\plugins\sdk\Ashita.h"
#include "TimerInterface.h"
using namespace std::chrono;

class AshitaProgressBar : public TimerSingle
{
private:
    SingleTimerSettings mSettings;
    uint8_t mIndex;
    IAshitaCore* m_AshitaCore;
    IFontObject* pOverlay  = NULL;
    IFontObject* pEmptyBar    = NULL;
    IFontObject* pProgressBar = NULL;
    IFontObject* pLeftText    = NULL;
    IFontObject* pRightText   = NULL;
    IFontObject* pIcon = NULL;
    char mIdentifier[256];
    bool mBlinkActive;
    int32_t mBlinkFrameCount;

public:
    AshitaProgressBar(IAshitaCore* AshitaCore, SingleTimerSettings Settings, const char* Identifier, uint8_t Index, IFontObject* pOverlay)
        : m_AshitaCore(AshitaCore)
        , mSettings(Settings)
        , mIndex(Index)
        , mBlinkFrameCount(0)
        , mBlinkActive(false)
    {
        strcpy_s(mIdentifier, 256, Identifier);
        char buffer[256];
        sprintf_s(buffer, 256, "%s_empty_%u", mIdentifier, mIndex);
        pEmptyBar = m_AshitaCore->GetFontManager()->Create(buffer);
        sprintf_s(buffer, 256, "%s_progress_%u", mIdentifier, mIndex);
        pProgressBar = m_AshitaCore->GetFontManager()->Create(buffer);
        sprintf_s(buffer, 256, "%s_overlay_%u", mIdentifier, mIndex);
        this->pOverlay = m_AshitaCore->GetFontManager()->Create(buffer);
        sprintf_s(buffer, 256, "%s_icon_%u", mIdentifier, mIndex);
        pIcon = m_AshitaCore->GetFontManager()->Create(buffer);
        sprintf_s(buffer, 256, "%s_lefttext_%u", mIdentifier, mIndex);
        pLeftText = m_AshitaCore->GetFontManager()->Create(buffer);
        sprintf_s(buffer, 256, "%s_righttext_%u", mIdentifier, mIndex);
        pRightText = m_AshitaCore->GetFontManager()->Create(buffer);
        Initialize();
    }
    ~AshitaProgressBar()
    {
        //Delete all the created font objects.
        char buffer[256];
        sprintf_s(buffer, 256, "%s_empty_%u", mIdentifier, mIndex);
        m_AshitaCore->GetFontManager()->Delete(buffer);
        sprintf_s(buffer, 256, "%s_progress_%u", mIdentifier, mIndex);
        m_AshitaCore->GetFontManager()->Delete(buffer);
        sprintf_s(buffer, 256, "%s_overlay_%u", mIdentifier, mIndex);
        m_AshitaCore->GetFontManager()->Delete(buffer);
        sprintf_s(buffer, 256, "%s_icon_%u", mIdentifier, mIndex);
        m_AshitaCore->GetFontManager()->Delete(buffer);
        sprintf_s(buffer, 256, "%s_lefttext_%u", mIdentifier, mIndex);
        m_AshitaCore->GetFontManager()->Delete(buffer);
        sprintf_s(buffer, 256, "%s_righttext_%u", mIdentifier, mIndex);
        m_AshitaCore->GetFontManager()->Delete(buffer);
    }


public:
    bool ClickCheck(int32_t posx, int32_t posy)
    {
        if (pOverlay->GetVisible() == false)
            return false;
        if (posx < mSettings.GetPositionX())
            return false;
        if (posy < mSettings.GetPositionY())
            return false;
        if (posx > (mSettings.GetPositionX() + mSettings.GetTotalWidth()))
            return false;
        if (posy > (mSettings.GetPositionY() + mSettings.GetTotalHeight()))
            return false;
        return true;
    }
    SingleTimerSettings* GetSettings()
    {
        return &mSettings;
    }
    void SetSettings(SingleTimerSettings Settings)
    {
        mSettings = Settings;
        mBlinkFrameCount = 0;
        mBlinkActive     = false;
        pOverlay->SetMouseCallback(NULL);
        Initialize();
    }
    void SetVisible(bool value)
    {
        pOverlay->SetVisible(value);
        pEmptyBar->SetVisible(value);
        pProgressBar->SetVisible(value);
        pLeftText->SetVisible(value);
        pRightText->SetVisible(value);
        if (mSettings.GetIconEnabled())
        {
            pIcon->SetVisible(value);
        }
        else
        {
            pIcon->SetVisible(false);
        }
    }
    void Update()
    {
        //Update position based on settings(ashitaprogressbarmanager will update the settings)
        pOverlay->SetPositionX(mSettings.GetPositionX());
        pOverlay->SetPositionY(mSettings.GetPositionY());

        //Update color, this will vary with time remaining according to settings.
        pProgressBar->GetBackground()->SetColor(mSettings.GetCurrentColor());

        //Mostly self explanatory, use remaining time to set text.
        int32_t remainingTime = mSettings.GetRemainingTimeMs();
        if (remainingTime == 0)
        {
            if (mSettings.GetShowMs())
                pRightText->SetText("0.0s");
            else
                pRightText->SetText("0.0s");
        }
        else if ((remainingTime < 60000) && (mSettings.GetShowMs()))
        {
            char durationBuffer[32];
            sprintf_s(durationBuffer, 32, "%d.%ds", remainingTime / 1000, (remainingTime % 1000) / 100);
            pRightText->SetText(durationBuffer);
        }
        else
        {
            char durationBuffer[32];
            sprintf_s(durationBuffer, 32, "%d:%02d", remainingTime / 60000, (remainingTime % 60000) / 1000);
            pRightText->SetText(durationBuffer);
        }

        //We want to right align, so move text left it's own width from the bar's total width.
        pRightText->SetPositionX(mSettings.GetTotalWidth() - pRightText->GetBackground()->GetWidth());

        //Special case for 0, so we don't end up with a negative percent from fillPercent calculation.
        if (remainingTime == 0)
        {
            if (mSettings.GetFillForward())
            {
                pProgressBar->GetBackground()->SetWidth(mSettings.GetBarWidth());
                pProgressBar->GetBackground()->SetVisible(true);
            }
            else
            {
                pProgressBar->GetBackground()->SetVisible(false);
            }
        }

        else
        {
            double fillPercent = static_cast<double>(mSettings.GetElapsedTimeMs()) / static_cast<double>(mSettings.GetDurationMs());
            if (!mSettings.GetFillForward())
                fillPercent = double(1.0) - fillPercent;

            int32_t barWidth = (int32_t)floor(fillPercent * (double)mSettings.GetBarWidth());
            pProgressBar->GetBackground()->SetWidth(barWidth);
            pProgressBar->GetBackground()->SetVisible(true);
        }

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
                pProgressBar->GetBackground()->SetColor(mSettings.GetBlinkColor());
                pProgressBar->GetBackground()->SetWidth(mSettings.GetBarWidth());
                pProgressBar->GetBackground()->SetVisible(true);
            }
            else
            {
                pProgressBar->GetBackground()->SetVisible(false);
            }
        }

        SetVisible(true);
    }

private:
    void Initialize()
    {
        pOverlay->SetAutoResize(false);
        pOverlay->GetBackground()->SetVisible(true);
        pOverlay->GetBackground()->SetWidth(mSettings.GetTotalWidth());
        pOverlay->GetBackground()->SetHeight(mSettings.GetTotalHeight());
        if (mSettings.GetOverlayEnabled())
        {
            pOverlay->GetBackground()->SetTextureFromFile(mSettings.GetOverlayFile());
        }
        else
        {
            pOverlay->GetBackground()->SetColor(D3DCOLOR_ARGB(0, 0, 0, 0));
        }
        pOverlay->SetPositionX(mSettings.GetPositionX());
        pOverlay->SetPositionY(mSettings.GetPositionY());
        pOverlay->SetVisible(false);

        pEmptyBar->SetParent(pOverlay);
        pEmptyBar->SetAutoResize(false);
        pEmptyBar->GetBackground()->SetColor(mSettings.GetBackgroundColor());
        pEmptyBar->GetBackground()->SetVisible(true);
        pEmptyBar->GetBackground()->SetWidth(mSettings.GetBarWidth() + (2 * mSettings.GetBarOutline()));
        pEmptyBar->GetBackground()->SetHeight(mSettings.GetBarHeight() + (2 * mSettings.GetBarOutline()));
        pEmptyBar->SetPositionX((mSettings.GetIconEnabled() && !mSettings.GetOverlapIcon()) ? mSettings.GetIconWidth() : 0);
        pEmptyBar->SetPositionY(mSettings.GetFontSize());
        pEmptyBar->SetVisible(false);

        pProgressBar->SetParent(pOverlay);
        pProgressBar->SetAutoResize(false);
        pProgressBar->GetBackground()->SetColor(mSettings.GetHighDurationColor());
        pProgressBar->GetBackground()->SetVisible(true);
        pProgressBar->GetBackground()->SetWidth(mSettings.GetBarWidth());
        pProgressBar->GetBackground()->SetHeight(mSettings.GetBarHeight());
        pProgressBar->SetPositionX(((mSettings.GetIconEnabled() && !mSettings.GetOverlapIcon()) ? mSettings.GetIconWidth() : 0) + mSettings.GetBarOutline());
        pProgressBar->SetPositionY(mSettings.GetFontSize() + mSettings.GetBarOutline());

        pLeftText->SetParent(pOverlay);
        pLeftText->SetPadding(0);
        pLeftText->SetFontFamily(mSettings.GetFontName());
        pLeftText->SetFontHeight(mSettings.GetFontSize());
        pLeftText->SetAutoResize(false);
        pLeftText->GetBackground()->SetColor(D3DCOLOR_ARGB(0, 0, 0, 0));
        pLeftText->GetBackground()->SetVisible(false);
        pLeftText->GetBackground()->SetWidth(mSettings.GetTotalWidth() - (mSettings.GetIconEnabled() ? mSettings.GetIconWidth() + 3 : 0));
        pLeftText->GetBackground()->SetHeight(mSettings.GetFontSize());
        pLeftText->SetColor(mSettings.GetFontColor());
        pLeftText->SetBold(false);
        pLeftText->SetText(mSettings.GetDisplayText());
        pLeftText->SetPositionX(mSettings.GetIconEnabled() ? mSettings.GetIconWidth() + 3 : 0);
        pLeftText->SetPositionY(0);
        pLeftText->SetVisible(false);

        pRightText->SetParent(pOverlay);
        pRightText->SetPadding(0);
        pRightText->SetFontFamily(mSettings.GetFontName());
        pRightText->SetFontHeight(mSettings.GetFontSize());
        pRightText->SetAutoResize(true);
        pRightText->GetBackground()->SetColor(D3DCOLOR_ARGB(0, 0, 0, 0));
        pRightText->GetBackground()->SetVisible(false);
        pRightText->GetBackground()->SetWidth(mSettings.GetTotalWidth());
        pRightText->GetBackground()->SetHeight(mSettings.GetFontSize());
        pRightText->SetColor(mSettings.GetFontColor());
        pRightText->SetBold(false);
        pRightText->SetText("");
        pRightText->SetPositionX(0);
        pRightText->SetPositionY(0);
        pRightText->SetVisible(false);

        if (mSettings.GetIconEnabled())
        {
            pIcon->SetParent(pOverlay);
            pIcon->SetAutoResize(false);
            pIcon->GetBackground()->SetTextureFromFile(mSettings.GetIconFile());
            pIcon->SetPositionX(0);
            pIcon->SetPositionY(0);
            pIcon->GetBackground()->SetWidth(mSettings.GetIconWidth());
            pIcon->GetBackground()->SetHeight(mSettings.GetIconHeight());
            pIcon->GetBackground()->SetVisible(true);
            pIcon->SetVisible(false);
        }
    }
};
class AshitaProgressBarManager : public TimerPanel
{
private:
    IAshitaCore* m_AshitaCore;
    IFontObject* pBackground = NULL;
    IFontObject* pOverlay    = NULL;
    std::list<AshitaProgressBar*> mBarList;
    TimerPanelSettings mSettings;
    bool mIndexInUse[256] = {0};

public:
    char mIdentifier[256] = {0};

    AshitaProgressBarManager(IAshitaCore* AshitaCore, TimerPanelSettings Settings, const char* Identifier)
        : m_AshitaCore(AshitaCore)
        , mSettings(Settings)
    {
        strcpy_s(mIdentifier, 256, Identifier);
        Initialize();
    }

    ~AshitaProgressBarManager()
    {
        //Remove all bars.  Destructor will clear font objects.
        for (std::list<AshitaProgressBar*>::iterator it = mBarList.begin(); it != mBarList.end(); it++)
        {
            delete *it;
        }

        //Delete font objects.
        m_AshitaCore->GetFontManager()->Delete(pBackground->GetAlias());
        m_AshitaCore->GetFontManager()->Delete(pOverlay->GetAlias());
        TimerGlobals::gPanelMap.erase(pOverlay);
    }

public:
    TimerPanelSettings* GetSettings()
    {
        return &mSettings;
    }
    TimerSingle* CreateTimer(SingleTimerSettings Settings)
    {
        AshitaProgressBar* newProgressBar = NULL;

        //If we have an existing bar with the same text, apply new settings instead of creating a new bar.
        for (std::list<AshitaProgressBar*>::iterator iter = mBarList.begin(); iter != mBarList.end(); iter++)
        {
            if (_stricmp((*iter)->GetSettings()->GetDisplayText(), Settings.GetDisplayText()) == 0)
            {
                (*iter)->SetSettings(Settings);
                newProgressBar = *iter;
                break;
            }
        }

        //If no existing bar was found, create a new one then recreate overlay to ensure it stays on top.
        if (newProgressBar == NULL)
        {
            newProgressBar = new AshitaProgressBar(m_AshitaCore, Settings, mIdentifier, GetFirstValidIndex(), pOverlay);
            mBarList.push_back(newProgressBar);
            RecreateOverlay();
        }

        return (TimerSingle*)newProgressBar;
    }
    bool CallClickEvent(int32_t xpos, int32_t ypos)
    {
        bool retValue = false;
        for (std::list<AshitaProgressBar*>::iterator iter = mBarList.begin(); iter != mBarList.end();)
        {
            if ((*iter)->ClickCheck(xpos, ypos))
            {
                if (GetKeyState(VK_CONTROL) & 0x8000)
                {
                    delete *iter;
                    iter    = mBarList.erase(iter);
                    retValue = true;
                    continue;
                }
                else if ((*iter)->GetSettings()->GetClickEnabled())
                {
                    m_AshitaCore->GetChatManager()->QueueCommand((*iter)->GetSettings()->GetClickMode(), (*iter)->GetSettings()->GetClickCommand());
                    retValue = true;
                }
            }
            iter++;
        }
        return retValue;
    }
    void UpdatePanel()
    {
        SortBars();
        int32_t Height = GetPanelHeight();
        int32_t Width = GetPanelWidth();
        pBackground->GetBackground()->SetHeight(Height);
        pBackground->GetBackground()->SetWidth(Width);
        pOverlay->GetBackground()->SetHeight(Height);
        pOverlay->GetBackground()->SetWidth(Width);
        UpdatePanelLocation(Width, Height);
        int barCount = UpdateBars();

        if (barCount == 0)
        {
            //If we have no bars, set our sizes to 0 and hide our panels.
            pBackground->SetVisible(false);
            pOverlay->SetVisible(false);
        }

        else
        {
            //Otherwise, show them.
            pBackground->SetVisible(true);
            pOverlay->SetVisible(true);
        }
    }

private:
    void Initialize()
    {
        //Initialize the background and overlays.
        char buffer[256] = {0};
        sprintf_s(buffer, 256, "%s_background", mIdentifier);
        pBackground = m_AshitaCore->GetFontManager()->Create(buffer);
        pBackground->SetAutoResize(false);
        pBackground->GetBackground()->SetColor(mSettings.GetBackgroundColor());
        pBackground->GetBackground()->SetVisible(true);
        pBackground->SetText("");
        pBackground->SetVisible(false);
        TimerGlobals::gPanelMap.insert(std::make_pair(pBackground, (TimerPanel*)this));
        pBackground->SetMouseCallback(gPanelClick);

        sprintf_s(buffer, 256, "%s_overlay", mIdentifier);
        pOverlay = m_AshitaCore->GetFontManager()->Create(buffer);
        pOverlay->SetAutoResize(false);
        pOverlay->GetBackground()->SetColor(D3DCOLOR_ARGB(0, 255, 255, 255));
        pOverlay->GetBackground()->SetVisible(true);
        pOverlay->SetText("");
        pOverlay->SetVisible(false);
        pOverlay->SetPositionX(mSettings.GetPositionX());
        pOverlay->SetPositionY(mSettings.GetPositionY());
    }
    void RecreateOverlay()
    {
        //We do this to ensure the overlay is always the last created font object and we can right click drag if it's a draggable mode.
        char buffer[256];
        sprintf_s(buffer, 256, "%s_overlay", mIdentifier);
        m_AshitaCore->GetFontManager()->Delete(buffer);
        pOverlay = m_AshitaCore->GetFontManager()->Create(buffer);
        pOverlay->SetAutoResize(false);
        pOverlay->GetBackground()->SetColor(D3DCOLOR_ARGB(0, 255, 255, 255));
        pOverlay->GetBackground()->SetVisible(true);
        pOverlay->SetText("");
        pOverlay->SetVisible(false);
        pOverlay->SetPositionX(mSettings.GetPositionX());
        pOverlay->SetPositionY(mSettings.GetPositionY());
    }
    void SortBars()
    {
        mBarList.sort([](AshitaProgressBar* a, AshitaProgressBar* b) {
            return (a->GetSettings()->GetRemainingTimeMs() < b->GetSettings()->GetRemainingTimeMs());
        });
    }
    void UpdatePanelLocation(int32_t Width, int32_t Height)
    {
        //If we aren't in fixed mode, default to leaving it alone.  If an anchor overlay is found, we'll update it here and it'll be corrected before it actually moves.
        if (mSettings.GetAnchorMode() != AnchorMode::Fixed)
        {
            mSettings.SetPosition(pOverlay->GetPositionX(), pOverlay->GetPositionY());
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
        pOverlay->SetPositionX(mSettings.GetPositionX());
        pOverlay->SetPositionY(mSettings.GetPositionY());
        pBackground->SetPositionX(mSettings.GetPositionX());
        pBackground->SetPositionY(mSettings.GetPositionY());
    }
    uint8_t GetFirstValidIndex()
    {
        for (int x = 0; x < 256; x++)
        {
            if (!mIndexInUse[x])
            {
                mIndexInUse[x] = true;
                return x;
            }
        }
        throw new std::exception("Was not able to reserve an index, as all 256 bars were active.");
    }
    int32_t GetPanelWidth()
    {
        int32_t barCount = 0;
        int widestBar    = 0;

        //Iterate bars to find widest bar, and size panel accordingly.
        for (std::list<AshitaProgressBar*>::iterator iter = mBarList.begin(); iter != mBarList.end(); iter++)
        {
            //Skip bar if it's done.
            if ((*iter)->GetSettings()->GetRemoveTimeMs() == 0)
                continue;

            //If this is wider than last bar, increase panel width accordingly.
            if ((*iter)->GetSettings()->GetTotalWidth() > widestBar)
                widestBar = (*iter)->GetSettings()->GetTotalWidth();

            //If we reached max bars, we don't care about any others yet.
            barCount++;
            if (barCount >= mSettings.GetMaxVisibleBars())
                break;
        }

        return widestBar + (mSettings.GetBorderX() * 2);
    }
    int32_t GetPanelHeight()
    {
        int32_t barCount = 0;
        int barOffset    = mSettings.GetBorderY();

        //Iterate bars, adding heights and spacing.
        for (std::list<AshitaProgressBar*>::iterator iter = mBarList.begin(); iter != mBarList.end(); iter++)
        {
            //Skip bar if it's done.
            if ((*iter)->GetSettings()->GetRemoveTimeMs() == 0)
                continue;

            //If we aren't on the first bar, add vertical spacing.
            if (barOffset != mSettings.GetBorderY())
                barOffset += (*iter)->GetSettings()->GetBarSpacing();

            //Add the bar's height.
            barOffset += (*iter)->GetSettings()->GetTotalHeight();

            //If we reached max bars, we don't care about any others yet.
            barCount++;
            if (barCount == mSettings.GetMaxVisibleBars())
                break;
        }
        return barOffset + mSettings.GetBorderY();
    }
    int32_t UpdateBars()
    {
        //Create variables to store some information on bars as we iterate.
        int32_t barCount = 0;
        int barOffset    = mSettings.GetBorderY();

        //Iterate bars, setting locations.
        for (std::list<AshitaProgressBar*>::iterator iter = mBarList.begin(); iter != mBarList.end();)
        {
            //Remove bar if it's ready to be removed.
            if ((*iter)->GetSettings()->GetIsFinished())
            {
                delete *iter;
                iter = mBarList.erase(iter);
                continue;
            }
            //Hide bar if we've already displayed our max.
            else if (barCount >= mSettings.GetMaxVisibleBars())
            {
                (*iter)->SetVisible(false);
                iter++;
                continue;
            }

            //If we aren't on the first bar, add vertical spacing.
            if (barOffset != mSettings.GetBorderY())
                barOffset += (*iter)->GetSettings()->GetBarSpacing();

            //Calculate Y position depending on if we're using reverse sort or not.
            float barPositionY = mSettings.GetPositionY() + barOffset;
            if (mSettings.GetReverseSort())
                barPositionY = (mSettings.GetPositionY() + pOverlay->GetBackground()->GetHeight()) - (barOffset + (*iter)->GetSettings()->GetTotalHeight());

            //Set the bar's position variables and call update, which will move it into place and update it's appearance.
            (*iter)->GetSettings()->SetPosition(mSettings.GetPositionX() + mSettings.GetBorderX(), barPositionY);
            (*iter)->Update();

            //Add the bar's information to our totals for displaying the next bar.
            barOffset += (*iter)->GetSettings()->GetTotalHeight();
            barCount++;
            iter++;
        }
        return barCount;
    }
};

#endif