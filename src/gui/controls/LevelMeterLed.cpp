//==========================================================================
// Name:            LevelMeterLed.cpp
// Purpose:         A compact, custom-drawn LED-style level meter.
// Authors:         Claude Code (for Barry Jones, G4MKT)
//
// License:
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License version 2.1,
//  as published by the Free Software Foundation.  This program is
//  distributed in the hope that it will be useful, but WITHOUT ANY
//  WARRANTY; without even the implied warranty of MERCHANTABILITY or
//  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
//  License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, see <http://www.gnu.org/licenses/>.
//
//==========================================================================
#include <cmath>
#include <algorithm>

#include <wx/dcbuffer.h>

#include "LevelMeterLed.h"
// No PR #1445 tint theming on this branch -- GetParent()->GetBackgroundColour()
// is the same portable fallback the old m_levelTargetMarker strip used.

LevelMeterLed::LevelMeterLed(
    wxWindow* parent, wxWindowID id,
    float minDb, float maxDb, float segmentDb,
    float amberStartDb, float redStartDb,
    int segmentWidthPx, int segmentHeightPx,
    const wxPoint& pos)
    : wxWindow()
    , minDb_(minDb)
    , maxDb_(maxDb)
    , segmentDb_(segmentDb)
    , amberStartDb_(amberStartDb)
    , redStartDb_(redStartDb)
    , segmentWidthPx_(segmentWidthPx)
    , segmentHeightPx_(segmentHeightPx)
    , currentDb_(minDb)
    , litSegments_(0)
{
    numSegments_ = (int)std::lround((maxDb_ - minDb_) / segmentDb_);

    Create(parent, id, pos, wxSize(segmentWidthPx_ * numSegments_, segmentHeightPx_));
    SetBackgroundStyle(wxBG_STYLE_PAINT); // we draw every pixel ourselves
    Bind(wxEVT_PAINT, &LevelMeterLed::OnPaint, this);
}

wxSize LevelMeterLed::DoGetBestSize() const
{
    return wxSize(segmentWidthPx_ * numSegments_, segmentHeightPx_);
}

void LevelMeterLed::Reset()
{
    SetLevelDb(minDb_);
}

void LevelMeterLed::SetLevelDb(float db)
{
    if (db < minDb_) db = minDb_;
    if (db > maxDb_) db = maxDb_;
    currentDb_ = db;

    // Segment i (lower bound minDb_ + i*segmentDb_) lights as soon as the
    // level rises *above* that lower bound -- ceil (not floor) so that
    // exactly at the absolute floor (currentDb_ == minDb_) zero segments
    // are lit (important for Reset()), while still lighting segment i
    // immediately on crossing its own threshold, e.g. amber's first
    // segment lights right as the level exceeds amberStartDb_, not only
    // once it reaches the *next* segment's threshold.
    int newLit = (int)std::ceil((currentDb_ - minDb_) / segmentDb_);
    newLit = std::max(0, std::min(numSegments_, newLit));

    if (newLit != litSegments_)
    {
        litSegments_ = newLit;
        Refresh(false); // false: we repaint every pixel ourselves, no need to erase first
    }
}

void LevelMeterLed::OnPaint(wxPaintEvent&)
{
    wxAutoBufferedPaintDC dc(this);

    dc.SetBackground(wxBrush(GetParent()->GetBackgroundColour()));
    dc.Clear();

    static const wxColour GREEN(0, 200, 0);
    static const wxColour AMBER(230, 160, 0);
    static const wxColour RED(220, 30, 30);
    const float DIM_FACTOR = 0.22f; // unlit segments show a dim version of their eventual colour

    dc.SetPen(*wxTRANSPARENT_PEN); // segments butt together, no per-segment borders

    for (int i = 0; i < numSegments_; i++)
    {
        float lowerDb = minDb_ + i * segmentDb_;
        wxColour zoneColour = (lowerDb >= redStartDb_) ? RED : (lowerDb >= amberStartDb_) ? AMBER : GREEN;

        bool lit = i < litSegments_;
        wxColour fillColour = lit
            ? zoneColour
            : wxColour(
                (unsigned char)(zoneColour.Red() * DIM_FACTOR),
                (unsigned char)(zoneColour.Green() * DIM_FACTOR),
                (unsigned char)(zoneColour.Blue() * DIM_FACTOR));

        dc.SetBrush(wxBrush(fillColour));
        dc.DrawRectangle(i * segmentWidthPx_, 0, segmentWidthPx_, segmentHeightPx_);
    }
}
