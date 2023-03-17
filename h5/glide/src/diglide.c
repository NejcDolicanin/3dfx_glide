/*
** THIS SOFTWARE IS SUBJECT TO COPYRIGHT PROTECTION AND IS OFFERED ONLY
** PURSUANT TO THE 3DFX GLIDE GENERAL PUBLIC LICENSE. THERE IS NO RIGHT
** TO USE THE GLIDE TRADEMARK WITHOUT PRIOR WRITTEN PERMISSION OF 3DFX
** INTERACTIVE, INC. A COPY OF THIS LICENSE MAY BE OBTAINED FROM THE 
** DISTRIBUTOR OR BY CONTACTING 3DFX INTERACTIVE INC(info@3dfx.com). 
** THIS PROGRAM IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
** EXPRESSED OR IMPLIED. SEE THE 3DFX GLIDE GENERAL PUBLIC LICENSE FOR A
** FULL TEXT OF THE NON-WARRANTY PROVISIONS.  
** 
** USE, DUPLICATION OR DISCLOSURE BY THE GOVERNMENT IS SUBJECT TO
** RESTRICTIONS AS SET FORTH IN SUBDIVISION (C)(1)(II) OF THE RIGHTS IN
** TECHNICAL DATA AND COMPUTER SOFTWARE CLAUSE AT DFARS 252.227-7013,
** AND/OR IN SIMILAR OR SUCCESSOR CLAUSES IN THE FAR, DOD OR NASA FAR
** SUPPLEMENT. UNPUBLISHED RIGHTS RESERVED UNDER THE COPYRIGHT LAWS OF
** THE UNITED STATES.  
** 
** COPYRIGHT 3DFX INTERACTIVE, INC. 1999, ALL RIGHTS RESERVED
**
** $Header: /cvsroot/glide/glide2x/h3/glide/src/diglide.c,v 1.1.1.1 1999/12/07 21:49:26 joseph Exp $
** $Log: diglide.c,v $
** Revision 1.1.1.1  1999/12/07 21:49:26  joseph
** Initial checkin into SourceForge.
**
** 
** 3     3/17/99 6:16p Dow
** Phantom Menace and other fixes.
** 
** 38    3/17/99 1:37p Atai
** use grHints to enable/disable uma hack
** 
** 37    4/14/98 6:41p Peter
** Merge w/ cvg glide cleanup
** 
** 36    3/28/98 11:24a Dow
** itwo�
** 
** 35    1/20/98 10:50a Atai
** validate state in grGlideGetState
 * 
 * 33    1/07/98 10:22a Peter
 * lod dithering env var
 * 
 * 32    1/06/98 3:53p Atai
 * remove grHint, modify grLfbWriteRegion and grGet
 * 
 * 31    12/17/97 4:05p Atai
 * added grChromaRange(), grGammaCorrecionRGB(), grRest(), and grGet()
 * functions
 * 
 * 30    12/09/97 12:20p Peter
 * mac glide port
 * 
 * 29    12/01/97 5:46p Peter
 * fixed variable names in swizzle
 * 
 * 28    12/01/97 5:17p Peter
 * 
 * 27    11/18/97 4:36p Peter
 * chipfield stuff cleanup and w/ direct writes
 * 
 * 26    11/14/97 5:02p Peter
 * more comdex stuff
 * 
 * 25    11/14/97 12:09a Peter
 * comdex thing and some other stuff
 * 
 * 24    11/12/97 2:27p Peter
 * 
 * 23    11/12/97 11:39a Dow
 * H3 Stuff
 * 
 * 22    11/12/97 9:21a Dow
 * Changed CVG_FIFO to USE_PACKET_FIFO
 * 
 * 21    11/04/97 4:00p Dow
 * Banshee Mods
 * 
 * 20    11/03/97 3:43p Peter
 * h3/cvg cataclysm
 * 
 * 19    10/16/97 3:40p Peter
 * packed rgb
 * 
 * 18    9/20/97 10:53a Peter
 * keep track of palette stats
 * 
 * 17    9/15/97 7:31p Peter
 * more cmdfifo cleanup, fixed normal buffer clear, banner in the right
 * place, lfb's are on, Hmmmm.. probably more
 * 
 * 16    9/10/97 10:13p Peter
 * fifo logic from GaryT, non-normalized fp first cut
 * 
 * 15    7/25/97 11:40a Peter
 * removed dHalf, change field name to match real use for cvg
 * 
 * 14    7/08/97 2:48p Peter
 * 
 * 13    6/30/97 3:20p Peter
 * error callback
 * 
 * 12    6/23/97 4:43p Peter
 * cleaned up #defines etc for a nicer tree
 * 
**
*/

#include <string.h>
#include <3dfx.h>
#include <glidesys.h>

#define FX_DLL_DEFINITION
#include <fxdll.h>
#include <glide.h>
#include "fxglide.h"

#include "rcver.h"
static char glideIdent[] = "@#%" VERSIONSTR ;

/* the root of all EVIL */
struct _GlideRoot_s GR_CDECL _GlideRoot;
/* This is global to speed up the function call wrappers */

/*---------------------------------------------------------------------------
**
*/
void
_grDisplayStats(void)
{
    int frames = _GlideRoot.stats.bufferSwaps;

    if (frames <= 0) frames = 1;
    gdbg_info(80,"GLIDE STATISTICS:\n");
    gdbg_info(80,"     triangles processed: %7d       tris drawn: %7d\n",
                   _GlideRoot.stats.trisProcessed,
                   _GlideRoot.stats.trisDrawn);
    gdbg_info(80,"            buffer swaps: %7d       tris/frame: %7d , %d\n",
                   _GlideRoot.stats.bufferSwaps,
                   _GlideRoot.stats.trisProcessed/frames,
                   _GlideRoot.stats.trisDrawn/frames);
    gdbg_info(80,"                  points: %7d       pnts/frame: %7d\n",
                   _GlideRoot.stats.pointsDrawn,
                   _GlideRoot.stats.pointsDrawn/frames);
    gdbg_info(80,"                   lines: %7d      lines/frame: %7d\n",
                   _GlideRoot.stats.linesDrawn,
                   _GlideRoot.stats.linesDrawn/frames);
    gdbg_info(80,"       texture downloads: %7d    texture bytes: %7d\n",
                   _GlideRoot.stats.texDownloads,  _GlideRoot.stats.texBytes);
    gdbg_info(80,"       palette downloads: %7d    palette bytes: %7d\n",
                   _GlideRoot.stats.palDownloads, _GlideRoot.stats.palBytes);
    gdbg_info(80,"           NCC downloads: %7d        NCC bytes: %7d\n",
              _GlideRoot.stats.nccDownloads, _GlideRoot.stats.nccBytes);

#if USE_PACKET_FIFO
    gdbg_info(80,"\tCommandFifo:\n");
    gdbg_info(80,"\t\tWraps: %ld\n", _GlideRoot.stats.fifoWraps);
    if (_GlideRoot.stats.fifoWraps > 0) {
      gdbg_info(80,"\t\tAvg Drain Depth: %g\n", 
                (double)_GlideRoot.stats.fifoWrapDepth / _GlideRoot.stats.fifoWraps);
    }
    gdbg_info(80,"\t\tStalls: %ld\n", _GlideRoot.stats.fifoStalls);
    if (_GlideRoot.stats.fifoStalls > 0) {
      gdbg_info(80,"\t\tAvg Stall Depth: %g\n", 
                (double)_GlideRoot.stats.fifoStallDepth / _GlideRoot.stats.fifoStalls);
    }
#endif /* CVG_FIFO */

#if GLIDE_FP_CLAMP
    gdbg_info(80, "\tTSU Value Clamping failures:    %7ld\n", _GlideRoot.stats.tsuValClamp);
#if HOOPTI_TRI_SETUP_COMPARE
    gdbg_info(80, "\tTSU Exponent Clamping failures: %7ld\n", _GlideRoot.stats.tsuExpClamp);
#endif /* HOOPTI_TRI_SETUP_COMPARE */
#endif /* GLIDE_FP_CLAMP */
}

#if !USE_PACKET_FIFO
/*
** fifoFree is kept in bytes, each fifo entry is 8 bytes, but since there
** are headers involved, we assume an average of 2 registers per 8 bytes
** or 4 bytes of registers stored in every fifo entry
*/
void
_grReCacheFifo(FxI32 n)
{
#if !(GLIDE_PLATFORM & GLIDE_HW_H3)
  GR_DCL_GC;
  gc->state.fifoFree = ((grSstStatus() >> SST_MEMFIFOLEVEL_SHIFT) & 0xffff)<<2;
#endif
}

FxI32 GR_CDECL
_grSpinFifo(FxI32 n)
{
  GR_DCL_GC;
  do {
    _grReCacheFifo(n);
  } while (gc->state.fifoFree < 0);
  return gc->state.fifoFree;
}
#endif /* !USE_PACKET_FIFO */

/*---------------------------------------------------------------------------
**
*/
void 
_grSwizzleColor(GrColor_t *color)
{
  GR_DCL_GC;
  FxU32 red, green, blue, alpha;
  
  switch(gc->state.color_format) {
  case GR_COLORFORMAT_ARGB:
    break;

  case GR_COLORFORMAT_ABGR:
    red     = *color & 0x00ff;
    blue    = (*color >> 16) & 0xff;
    *color &= 0xff00ff00;
    *color |= ((red << 16) | blue);
    break;

  case GR_COLORFORMAT_RGBA:
    blue   = (*color & 0x0000ff00) >> 8;
    green  = (*color & 0x00ff0000) >> 16;
    red    = (*color & 0xff000000) >> 24;
    alpha  = (*color & 0x000000ff);
    *color = (alpha << 24) | (red << 16) | (green << 8) | blue;
    break;

  case GR_COLORFORMAT_BGRA:
    blue   = (*color & 0xff000000) >> 24;
    green  = (*color & 0x00ff0000) >> 16;
    red    = (*color & 0x0000ff00) >> 8;
    alpha  = (*color & 0x000000ff);
    *color = (alpha << 24) | (red << 16) | (green << 8) | blue;
    break;

  default:
    GR_ASSERT(0);
    break;
  }
} /* _grSwizzleColor */

/*---------------------------------------------------------------------------
** grGlideGetVersion
** NOTE: allow this to be called before grGlideInit()
*/
GR_DIENTRY(grGlideGetVersion, void, (char version[80]))
{
  GDBG_INFO(87,"grGlideGetVersion(0x%x) => \"%s\"\n",version,glideIdent+3);
  GR_ASSERT(version != NULL);
  strcpy(version,glideIdent+3);
} /* grGlideGetVersion */

/*---------------------------------------------------------------------------
** grGlideGetState
*/
GR_DIENTRY(grGlideGetState, void, (GrState *state))
{
  GR_BEGIN_NOFIFOCHECK("grGlideGetState",87);
  GDBG_INFO_MORE(gc->myLevel,"(0x%x)\n",state);
  GR_ASSERT(state != NULL);
#if defined(GLIDE3) && defined(GLIDE3_ALPHA)
  _grValidateState();
#endif
  *state = gc->state;
  GR_END();
} /* grGlideGetState */

#ifndef GLIDE_ALPHA
/*---------------------------------------------------------------------------
** grHints
*/
GR_DIENTRY(grHints, void, (GrHint_t hintType, FxU32 hints))
{
  GR_BEGIN_NOFIFOCHECK_NORET("grHints",85);
  GDBG_INFO_MORE(gc->myLevel,"(%d,0x%x)\n",hintType,hints);

  switch (hintType) {
  case GR_HINT_STWHINT:
    if (gc->state.paramHints != hints) {
      gc->state.paramHints = hints;
      _grUpdateParamIndex();
    }
    break;

  case GR_HINT_FIFOCHECKHINT:
    /* swFifoLWM is kept internally in bytes, hints are in fifo entries */ 
    gc->state.checkFifo = hints;
    break;

  case GR_HINT_FPUPRECISION:
    hints ? double_precision_asm() : single_precision_asm();
    break;

  case GR_HINT_ALLOW_MIPMAP_DITHER:
    /* Regardless of the game hint, force the user selection */
    gc->state.allowLODdither = ((_GlideRoot.environment.texLodDither != 0) || 
                                hints);
    break;
  case GR_HINT_ENABLE_UMA:
    _GlideRoot.environment.enUma = (hints) ? FXTRUE : FXFALSE;
    break;
  default:
    GR_CHECK_F(myName, 1, "invalid hints type");
  }
  GR_END();
} /* grHints */
#endif

/*---------------------------------------------------------------------------
** grGlideInit
*/
GR_DIENTRY(grGlideInit, void, (void))
{
  GDBG_INIT();
  
  GDBG_INFO(80,"grGlideInit()\n");

  /* KoolSmoky - let's detect glide devices *before* GETENV is called
  ** need to know where the devices are first if we want multimonitor
  ** capabilities.
  */
  if ( !_grSstDetectResources() ) {
#ifdef GLIDE_INIT_HWC
    GrErrorCallback( hwcGetErrorString(), FXTRUE );
#endif
  }
//  _GlideInitEnvironment();                      /* the main init code */
  FXUNUSED(*glideIdent);

#if GDBG_INFO_ON
  gdbg_error_set_callback(_grErrorCallback);
#endif

#if USE_PACKET_FIFO && (WTF_P_COMDEX || WTF_P_COMDEX_RESET)
  /* Work around for apps that call some grXXX state call
   * before calling grSstWinOpen which inits the command fifo.  
   * See gsst.c:grSstWinOpen for more of this.
   */
  {
    const FxBool fifoSwapP = _grDummyFifo(FXTRUE);
    GR_ASSERT(fifoSwapP);
  }
#endif /* USE_PACKET_FIFO && (WTF_P_COMDEX || WTF_P_COMDEX_RESET) */

  grSstSelect(0);

#if defined(GLIDE3) && defined(GLIDE3_ALPHA)
  _grResetTriStats();
#else
  grResetTriStats();
#endif
  GDBG_INFO(281,"grGlideInit --done---------------------------------------\n");
} /* grGlideInit */


/*---------------------------------------------------------------------------
**  grGlideShamelessPlug - grGlideShamelessPlug
**
**  Returns:
**
**  Notes:
**
*/
#ifndef GLIDE3_ALPHA
GR_DIENTRY(grGlideShamelessPlug, void, (const FxBool mode))
{
  GDBG_INFO(80,"grGlideShamelessPlug(%d)\n",mode);
  _GlideRoot.environment.shamelessPlug = mode;
} /* grGlideShamelessPlug */
#endif


/*---------------------------------------------------------------------------
**  grResetTriStats - Set triangle counters to zero.
*/
#if defined(GLIDE3) && defined(GLIDE3_ALPHA)
void FX_CSTYLE
_grResetTriStats(void)
#else
GR_DIENTRY(grResetTriStats, void, (void))
#endif
{
  GDBG_INFO(80,"grResetTriStats()\n");
  _GlideRoot.stats.bufferSwaps = 0;
  _GlideRoot.stats.linesDrawn = 0;
  _GlideRoot.stats.trisProcessed = 0;
  _GlideRoot.stats.trisDrawn = 0;
  _GlideRoot.stats.texDownloads = 0;
  _GlideRoot.stats.texBytes = 0;
  _GlideRoot.stats.palDownloads = 0;
  _GlideRoot.stats.palBytes = 0;
} /* grResetTriStats */


/*---------------------------------------------------------------------------
**  grResetTriStats - Set triangle counters to zero.
*/
GR_DIENTRY(grTriStats, void, (FxU32 *trisProcessed, FxU32 *trisDrawn))
{
  GDBG_INFO(80,"grTriStats() => %d %d\n",
                _GlideRoot.stats.trisProcessed,
                _GlideRoot.stats.trisDrawn);
  *trisProcessed = _GlideRoot.stats.trisProcessed;
  *trisDrawn = _GlideRoot.stats.trisDrawn;
} /* grTriStats */

void GR_CDECL
_grFence(void)
{
  GDBG_INFO(120,"\t\t\t\t\t\t\tFENCE\n");
  P6FENCE;
}
