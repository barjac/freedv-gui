//==========================================================================
// Name:            defines.h
// Purpose:         Definitions used by plots derived from plot class.
// Created:         August 27, 2012
// Authors:         David Rowe, David Witten
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
#ifndef __FDMDV2_DEFINES__
#define __FDMDV2_DEFINES__

#include <cmath>

#include "wx/wx.h"
#include "logging/ulog.h"
#include "pipeline/modem_stats.h"

#define FDMDV_FCENTRE (1500)        // center of waterfall
#define FDMDV_SCALE \
  825 /* suggested scaling for 16 bit shorts                            */

// Minimum level for the Level gauge, negated.
#define LEVEL_GAUGE_MIN_DB 30

// Acceptable-range marker drawn as a thin coloured strip above the level
// meter's gauge, in % of the gauge's own 0-100 scale (amber below, green
// within, red above). Ported from upstream PR #1464 (log-scale meter) with
// PR #1472's colour-band idea folded in by Mooneer at these percentages.
// HIGH_PCT lowered from Mooneer's 85 back to PR #1472's own original 70
// (2026-09-19, Barry): "too lenient" at 85 (-4.5dBFS on this branch's -30
// to 0dB log scale) -- audio could sit uncomfortably close to true
// clipping before the meter's red zone ever warned about it. 70 (-9dBFS)
// matches the threshold from the original EMA-meter design's own live
// testing, before Mooneer's later log-scale port widened it.
#define LEVEL_METER_TARGET_LOW_PCT  30
#define LEVEL_METER_TARGET_HIGH_PCT 70

// Spectrogram and Waterfall

#define MIN_MAG_DB        -40.0     // min of spectrogram/waterfall magnitude axis
#define MAX_MAG_DB          0.0     // max of spectrogram/waterfall magnitude axis
#define STEP_MAG_DB         5.0     // magnitude axis step
#define BETA                0.95    // constant for time averaging spectrum data
#define MIN_F_HZ            0       // min freq on Waterfall and Spectrum
#define MAX_F_HZ            3000    // max freq on Waterfall and Spectrum
#define STEP_F_HZ           500     // major (e.g. text legend) freq step on Waterfall and Spectrum graticule
#define STEP_MINOR_F_HZ     100     // minor (ticks) freq step on Waterfall and Spectrum graticule
#define WATERFALL_SECS_Y    30      // number of seconds represented by y axis of waterfall
#define WATERFALL_SECS_STEP 5       // graticule y axis steps of waterfall
#define DT                  0.10    // time between real time graphing updates
#define FS                  8000    // FDMDV modem sample rate

// Scatter diagram 

#define SCATTER_MEM_SECS    10
// (symbols/frame)/(graphics update period) = symbols/s sent to scatter memory
// memory (symbols) = secs of memory * symbols/sec
#define SCATTER_MEM_SYMS_MAX    ((int)(SCATTER_MEM_SECS*((MODEM_STATS_NC_MAX+1)/DT)))
#define SCATTER_EYE_MEM_ROWS    ((int)(SCATTER_MEM_SECS/DT))

// Waveform plotting constants

#define WAVEFORM_PLOT_FS    400                            // sample rate (points/s) of waveform plotted to screen
#define WAVEFORM_PLOT_TIME  5                              // length or entire waveform on screen
#define WAVEFORM_PLOT_BUF   ((int)(DT*WAVEFORM_PLOT_FS))   // number of new samples we plot per DT

// SNR plot constants
#define NO_SNR_VAL (-10)
#define MAX_SNR_VAL (35)
#define SNR_PLOT_SECONDS (180)
#define SNR_PLOT_SECOND_SEGMENTS (6)
#define SNR_PLOT_DT (0.1)

// sample rate I/O & conversion constants

#define SAMPLE_RATE         48000                          // 48 kHz sampling rate rec. as we can trust accuracy of sound card
#define N8                  160                            // processing buffer size at 8 kHz
#define MEM8                (FDMDV_OS_TAPS/FDMDV_OS)
#define N48                 (N8*SAMPLE_RATE/FS)            // processing buffer size at 48 kHz
#define NUM_CHANNELS        2                              // I think most sound cards prefer stereo we will convert to mono
#define VOX_TONE_FREQ       1000.0                         // optional left channel vox tone freq
#define VOX_TONE_AMP        30000                          // optional left channel vox tone amp
#define FIFO_SIZE           640                            // default fifo size in ms
#define FRAME_DURATION_MS   20                             // default frame length of 20 mS = 0.02 seconds
#define MS_TO_SEC           1000
#define RECORD_FILE_SAMPLE_RATE (SAMPLE_RATE)

#define MAX_BITS_PER_CODEC_FRAME 64                            // 1600 bit/s mode
#define MAX_BYTES_PER_CODEC_FRAME (MAX_BITS_PER_CODEC_FRAME/8)
#define MAX_BITS_PER_FDMDV_FRAME 40                            // 2000 bit/s mode

// Squelch
#define SQ_DEFAULT_SNR       -2.0

// Level Gauge
#define FROM_RADIO_MAX       0.8
#define FROM_MIC_MAX         0.8

// Decay rate for the Level meter, applied once per GUI update (every DT sec).
// Target: -6 dB/sec (Barry, 2026-09-16: upstream's original -12dB/sec fell
// back too fast to comfortably watch).
//   20*log10(LEVEL_BETA) = -6 * DT
//   LEVEL_BETA = 10^(-6*DT/20) = 10^(-0.03) ≈ 0.933   (for DT = 0.10)
// => -0.60 dB per timer fire; the 30 dB gauge range fully decays in 5 s.
// Still used by the RX ("From Radio") side of the shared level gauge --
// see LEVEL_METER_TX_DECAY_TIME_CONSTANT_SEC below for the TX ("From Mic")
// side's own, differently-shaped decay.
#define LEVEL_DECAY_DB_PER_SEC 6.0
#define LEVEL_BETA (std::pow(10.0, -LEVEL_DECAY_DB_PER_SEC * DT / 20.0))

// TX ("From Mic") level meter: own independent refresh timer + decay curve,
// decoupled from the RX side and from the shared DT constant (2026-09-19,
// Barry: "the meter is still a bit jumpy... there is also quite a lag").
//
// Why not just lower DT: DT is a widely-shared foundational timing constant
// -- it sizes several unrelated buffers (WAVEFORM_PLOT_BUF,
// SCATTER_MEM_SYMS_MAX, SCATTER_EYE_MEM_ROWS, the g_avmag FIFO), drives the
// refresh rate of every other real-time plot (waveform/scatter/eye), and is
// used directly as a time-step unit in voicekeyer.cpp's own RX-pause/sync
// timing logic. Speeding it up to fix just this one meter would be a broad,
// higher-risk change touching all of that. A dedicated timer for the TX
// level meter alone avoids all of it.
//
// Second, more fundamental finding (2026-09-19, same day): giving the meter
// its own faster *timer* wasn't enough on its own -- Barry reported the
// meter still lagged noticeably and had started visibly flickering, and
// that its timing "appears to coincide with the encoder input not the mic
// audio." Traced to TxRxThread::txProcessing_(): the TX pipeline (including
// this meter's original tap, positioned before EQ/leveler/limiter but still
// *inside* the pipeline) only receives new data in bursts paced by RADE's
// own modem-frame batching requirement, not continuously -- so no GUI-side
// polling rate, however fast, can get fresher data than the pipeline
// itself produces, and polling faster than that bursty rate just adds a
// new artifact (jump on a real burst, decay on the empty ticks in between,
// repeating at the burst rate -- perceived as flickering).
//
// Real fix: the meter now taps the raw mic audio directly in
// MainFrame::OnTxInAudioData_() (the actual low-level sound-card callback),
// via a second, independent FIFO write alongside the existing one that
// feeds the real TX pipeline -- both writes use the same already-captured
// callback data, so this can't ever steal samples from (or otherwise
// affect) the real audio path. This is genuinely upstream of RNNoise/EQ/
// leveler/limiter *and* of the modem's own batching, at the sound card's
// own native, continuous callback rate -- as fast and true-to-the-mic as
// this meter can get. LEVEL_METER_TX_RAW_BUF_MAX is a generous per-tick
// cap (comfortably covers a 25ms window even at a 96kHz sound card, with
// headroom), not tied to any particular configured sample rate -- the
// actual read size is however many samples are really available each tick
// (see g_levelMeterTxRawFifo's usage in main.cpp), since this raw feed's
// rate no longer needs to match WAVEFORM_PLOT_FS at all.
//
// 25ms (4x the old shared 100ms DT-based rate) -- now genuinely meaningful
// given the data backing it is continuous rather than bursty.
//
// A same-day diagnostic detour (2026-09-19) briefly slowed this back to
// 0.1s while chasing what looked like a flicker -- that turned out to be a
// real, separate, pre-existing bug (see the ID_TIMER_DEMOD_IN comment in
// OnTimer(), main.cpp) rather than anything rate-dependent, so this is back
// to its real value.
#define LEVEL_METER_TX_REFRESH_PERIOD_SEC 0.025
#define LEVEL_METER_TX_REFRESH_TIMER_PERIOD ((int)(LEVEL_METER_TX_REFRESH_PERIOD_SEC*1000))
// Must comfortably exceed one tick's worth of raw mic samples at the sound
// card's actual rate, or the per-tick read in OnTimer() (main.cpp) caps
// below the real per-tick arrival rate, g_levelMeterTxRawFifo backs up
// faster than it drains, and once full every new write silently fails
// (GenericFIFO::write() returns -1 and drops the sample rather than
// overwriting) -- the meter then reads increasingly stale data forever
// instead of live speech. 16384 comfortably covers even a 100ms tick at
// 96kHz (9600) with headroom, so it's generously sized for 25ms too; kept
// at this more generous value (was 4096) since there's no cost to it.
#define LEVEL_METER_TX_RAW_BUF_MAX 16384

// Decay curve: the RX side's LEVEL_BETA decays the *linear* amplitude value
// by a constant multiplicative factor every tick -- mathematically a genuine
// exponential decay (the same behavior a real analog R/C meter circuit has
// in the voltage/amplitude domain), but because the gauge display is
// dB-scaled (logarithmic), an exponential decay in linear amplitude maps to
// a perfectly straight, constant-slope line in dB terms -- reported as
// "drops right down to the bottom" between words. A curve that visibly
// *decelerates* as it nears the gauge's floor needs the exponential decay
// applied directly to the displayed dB value instead (the opposite of what
// a literal R/C circuit does, but closer to the requested visual behavior):
//   displayedDb += (floorDb - displayedDb) * (1 - exp(-dt/tau))
// 1.5s starting value reuses the "plateau" time constant from the earlier,
// separate EMA-meter design's own live-tuned value (see
// project_level_meter_simple_fix.md/LEVEL_METER_TIME_CONSTANT_SEC) as an
// informed guess, not re-derived from scratch -- not yet live-tested here.
#define LEVEL_METER_TX_DECAY_TIME_CONSTANT_SEC 1.5

// TX Attenuation (0.1 dB increments)
#define TX_ATTENUATION_MIN (-300) /* -30 dB */
#define TX_ATTENUATION_MAX (0)
#define TX_ATTENUATION_DEFAULT TX_ATTENUATION_MAX
#define TX_ATTENUATION_SMALL_STEP (2) /* 0.2 dB */
#define TX_ATTENUATION_LARGE_STEP (10) /* 1 dB */

// SNR
#define SNRSLOW_BETA        0.5                           // time constant for slow SNR for display

// Text messaging Data
#define MAX_CALLSIGN         80

// Real-time memory block size
#define CODEC2_REAL_TIME_MEMORY_SIZE (512*1024)
   
enum
{
    ID_ROTATE_LEFT = wxID_HIGHEST + 1,
    ID_ROTATE_RIGHT,
    ID_RESIZE,
    ID_PAINT_BG
};

// Codec 2 LPC Post Filter defaults, from codec-dev/src/quantise.c

#define CODEC2_LPC_PF_GAMMA 0.5
#define CODEC2_LPC_PF_BETA  0.2

// PlugIns ...

#define PLUGIN_MAX_PARAMS 4

#endif  //__FDMDV2_DEFINES__
