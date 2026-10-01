//==========================================================================
// Name:            LevelMeterLed.h
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
#ifndef __FREEDV_LEVEL_METER_LED__
#define __FREEDV_LEVEL_METER_LED__

#include <wx/wx.h>

// Replaces the old continuous wxGauge-based Level meter (2026-10-01, Barry:
// "It seems rather jittery on the decay, steps are not fast enough to look
// smooth to the human eye... maybe an led style bar graph would be
// better"). A wxGauge has no smoothing of its own and only integer-dB
// resolution across a narrow pixel width, so a slow linear dB/s decay
// necessarily looks steppy on it -- an LED-style meter doesn't have this
// problem at all, since discrete steps are the correct look for it, not an
// artifact to hide.
//
// Rectangular segments (width/height independently settable -- Barry's own
// spec, 2026-10-01: segment width sized to fill the same width the old
// gauge occupied, but only 8px tall, closer to the old amber/green/red
// target-marker strip's look than to the old gauge's own 15px height), no
// gaps between them, coloured in three zones (green/amber/red). Segment
// width in dB is fixed (segmentDb); segment count is derived from the
// min/max range so the calibration (where amber/red start) is set purely
// by minDb/maxDb/segmentDb/amberStartDb/redStartDb, not by an arbitrary
// segment count chosen first.
class LevelMeterLed : public wxWindow
{
    public:
        LevelMeterLed(
            wxWindow* parent, wxWindowID id,
            float minDb, float maxDb, float segmentDb,
            float amberStartDb, float redStartDb,
            int segmentWidthPx, int segmentHeightPx,
            const wxPoint& pos = wxDefaultPosition);
        virtual ~LevelMeterLed() = default;

        // Sets the displayed level. Internally clamped to [minDb, maxDb] --
        // callers don't need to clamp before calling, unlike the old
        // wxGauge-based code's own std::max() call at each use site.
        // Only triggers a repaint if the number of lit segments actually
        // changed (same flicker-avoidance principle as the old gauge code's
        // own "only call SetValue when the displayed integer changes"
        // check, but encapsulated here instead of duplicated at each call
        // site).
        void SetLevelDb(float db);

        // Equivalent to SetLevelDb(minDb) -- all segments off.
        void Reset();

        virtual wxSize DoGetBestSize() const override;

    private:
        void OnPaint(wxPaintEvent& event);

        float minDb_;
        float maxDb_;
        float segmentDb_;
        float amberStartDb_;
        float redStartDb_;
        int segmentWidthPx_;
        int segmentHeightPx_;
        int numSegments_;
        float currentDb_;
        int litSegments_;
};

#endif // __FREEDV_LEVEL_METER_LED__
