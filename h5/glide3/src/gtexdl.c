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
** $Header: f:\\cvsroot/Glide3x/h5/glide3/src/gtexdl.c,v 1.6 2002/05/04 02:07:43 KoolSmoky Exp $
** $Log:
**  12   ve3d      1.9         05/02/02 KoolSmoky       added DXT-ness
**  11   3dfx      1.8.1.0.1.0 10/11/00 Brent           Forced check in to enforce
**       branching.
**  10   3dfx      1.8.1.0     06/20/00 Joseph Kain     Changes to support the
**       Napalm Glide open source release.  Changes include cleaned up offensive
**       comments and new legal headers.
**  9    3dfx      1.8         04/04/00 Kenneth Dyke    Fixed addressing for large
**       tiled textures.
**  8    3dfx      1.7         01/31/00 Adam Briggs     changed the IS_NAPALM macro
**       to cooperate with the display driver version of the same
**  7    3dfx      1.6         01/31/00 Adam Briggs     Changed all device ID magic
**       numbers to use those defined in fxhal.h & added IS_NAPALM macro to test
**       against device ID range
**  6    3dfx      1.5         01/19/00 Kenneth Dyke    Fixed texture memory sanity
**       check bug.
**  5    3dfx      1.4         10/27/99 Stephane Huaulme keeping the Mac compiler
**       happy...
**  4    3dfx      1.3         10/26/99 Larry  warner   Turn off mipmap level size
**       debug check
**       if we're in windowed mode, to enable the
**       napalm mustpass.bat conform test to run.
**  3    3dfx      1.2         10/21/99 Larry  warner   Added 4-bit and 32-bit
**       download code for tiled memory.
**  2    3dfx      1.1         09/22/99 Larry  warner   Created download procedures
**       for FXT1 format.
**  1    3dfx      1.0         09/11/99 StarTeam VTS Administrator 
** $
** 
** 58    8/25/99 8:45p Larryw
** Don't bother with 16-byte alignment magic for FXT1.
** 
** 57    8/19/99 7:55p Larryw
** Backed out my last change.
** 
** 56    8/18/99 3:20p Larryw
** FXT1 refinements.
** 
** 55    8/05/99 5:03p Larryw
** FXT1 format works now.
** 
** 54    7/29/99 7:07p Larryw
** Pave the way for FXT1 (but not quite there yet).
** 
** 53    7/22/99 8:14p Larryw
** Texture format byte-depth improvements
** 
** 52    7/14/99 6:23p Larryw
** Remove obsolete G3_LOD_TRANSLATE() macro
** Define _grMipMapOffset[][] at compile time
** Fix 2k texture address-finding
** 
** 51    7/07/99 6:52p Larryw
** * 2k texture support
** * Reversed order of LOD tables
** * Added 512,1024, and 2048-sized entries in tables
** * Nullified G3_LOD_TRANSLATE()
** * Created _g3LodXlat() for where tLOD register is read/written
** * Misc cosmetic changes.
** 
** 50    6/14/99 5:16p Larryw
** Added 32-bit texture format support.
** 
** 49    6/03/99 5:23p Russp
** Add in some ifdef'd code to allow variable texture alignment
** 
** 48    4/16/99 9:01a Dow
** Fixed bogus 11th arg to GDBG_INFO_MORE
** 
** 47    4/04/99 8:51p Atai
** Partial check-in for alt-tab issue. set FX_GLIDE_ALT_TAB=1 to build
** glide3x with hwcQueryContext built into GR_BEGIN_NOFIFOCHECK. It works
** with DEBUG glide only. In the non-debug glide, we can still see the
** desktop corruption.
** 
** 46    3/24/99 6:17p Peter
** reduce nop flush for chain downloads
** 
** 45    3/05/99 10:34p Peter
** removed extraneous nops
** 
** 44    3/02/99 3:20p Atai
** added 2d nop before texture download to maintain cache coherency
** 
** 43    3/02/99 2:08p Peter
** 2d nop flushes pixels (although 3d nop should too)
** 
** 42    2/24/99 4:51p Peter
** the rest of tiled texturing
** 
** 41    2/19/99 10:25a Atai
** check invalid textable with GR_TEXTABLE_PALETTE_6666_EXT
** 
** 40    2/18/99 6:00p Peter
** sub alignment linear textures
** 
** 39    2/10/99 3:08p Peter
** i swear I will never cut and paste code again, for as long as I live
** 
** 38    2/10/99 2:34p Peter
** corrected alignment textures within an alignment unit
** 
** 37    2/04/99 5:17p Peter
** fixed my eff-ed alignment for small lod levels (1x1)
** 
** 36    2/02/99 4:36p Peter
** download through lfb rather than texture port
** 
** 35    1/25/99 6:35p Peter
** tiled texture cleanup
** 
** 34    1/14/99 7:48p Peter
** cleanedup bytes per texel stuff
** 
** 33    12/14/98 6:19p Dow
** Fixed for current surface extension spec
** 
** 32    12/03/98 11:26p Dow
** 
** 31    11/30/98 6:57p Peter
** windowed texture effage
** 
** 30    11/15/98 3:21a Atai
** first attempt to make 2 tmus work in H4 glide3x full screen mode, just
** in time check-in for comdex demo. warning: the code is not completed
** yet.
** 
** 29    10/13/98 5:27p Peter
** 6666 format hack
** 
** 28    10/12/98 9:51a Peter
** dynamic 3DNow!(tm)
** 
** 27    10/09/98 2:44p Atai
** fixed 6666 palette
** 
** 26    9/24/98 11:17a Dow
** AMD 3DNow! (tm) mods
** 
** 25    9/11/98 2:09p Peter
** removed debugging code
** 
** 24    9/11/98 1:44p Peter
** texture mask fixes
** 
** 23    9/09/98 12:56p Atai
** relaxed 2mb texture address constraint for uma devices
** 
** 22    9/04/98 3:32p Peter
** reversed the sense of the active bytes in small downloads
** 
** 21    8/27/98 10:05p Peter
** fixed state effage
** 
** 20    8/27/98 9:54p Peter
** flush old texels when replacing small lod levels
** 
** 19    8/27/98 1:55p Peter
** download problem w/ widths smaller than dword
** 
** 18    8/03/98 6:41a Jdt
** moved stats into GC from global location
** 
** 17    7/18/98 3:43p Jdt
** Fix texture download state saving bug.  Remove convert and download
** RLE.
** 
** 16    7/18/98 12:31a Jdt
** Changes to reflect new shadow register structure.
** Added shadowing of palette/ncc table downloads.
** 
** 15    7/17/98 2:07p Jdt
** Remove pointless SST_SEQ_8_DOWWNLD bit
** 
** 14    7/16/98 8:19a Jdt
** fxcmd.h
** 
** 13    6/08/98 12:47p Atai
** added return value for grTexDownloadMipmapLevelPartial
** 
** 12    6/05/98 7:51p Atai
** return FxBool for grTexDownloadMipMapLevelPartial
** 
** 11    5/29/98 11:45a Atai
** 1.added _EXT for extension #defines. 
** 2. change GR_TEXBASE_* values
** 3. Remove GR_TEXCHROMA_ENABLE_SUBSTITUTE_RGB
** 
** 10    5/08/98 1:40p Peter
** merged Anthony's palette of Taco changes
** 
** 9     5/05/98 3:01p Peter
** packet chipfield vs direct register chipfield
** 
** 8     4/30/98 5:01p Peter
** first pass glide3 merge
** 
** 7     4/30/98 10:34a Peter
** merged w/ cvg again
** 
** 6     4/29/98 2:32p Peter
** fixed texture palette broadcast
** 
** 5     4/22/98 4:57p Peter
** glide2x merge
** 
** 4     2/02/98 12:21p Atai
** fixed palette broadcasts in  _grTexDownloadNccTable and
** _grTexDownloadPalette 
** 
** 3     1/22/98 10:35a Atai
** 1. introduce GLIDE_VERSION, g3\glide.h, g3\glideutl.h, g2\glide.h,
** g2\glideutl.h
** 2. fixed grChromaRange, grSstOrigin, and grGetProcAddress
** 
** 2     1/18/98 12:03p Atai
** sync to rev 17 spec
 * 
 * 1     1/16/98 4:29p Atai
 * create glide 3 src
 * 
 * 54    1/09/98 6:48p Atai
 * grTexInfo, GR_LOD_* and GR_ASPECT_*
 * 
 * 53    1/08/98 7:09p Peter
 * real hw stuff modulo makefile change
 * 
 * 52    1/08/98 4:58p Atai
 * tex table broadcast, grVertexLayout enable/disable, stq, and some
 * defines
 * 
 * 51    1/08/98 11:06a Dow
 * Set palette downloads to broadcast.
 * 
 * 50    12/11/97 4:15p Peter
 * fixed assertions
 * 
 * 49    12/09/97 12:20p Peter
 * mac glide port
 * 
 * 48    12/05/97 4:26p Peter
 * watcom warnings
 * 
 * 47    12/02/97 9:48a Dow
 * Got rid of Texelfx rev 0 warning
 * 
 * 46    11/21/97 1:03p Peter
 * small texture download problem
 * 
 * 45    11/20/97 6:57p Dow
 * Texture Downloads for Banshee
 * 
 * 44    11/18/97 4:36p Peter
 * chipfield stuff cleanup and w/ direct writes
 * 
 * 43    11/17/97 4:55p Peter
 * watcom warnings/chipfield stuff
 * 
 * 42    11/14/97 12:09a Peter
 * comdex thing and some other stuff
 * 
 * 41    11/12/97 11:16a Peter
 * cleaned up assertions
 * 
 * 40    11/05/97 1:50p Peter
 * fixed partial palette downloads
 * 
 * 39    11/03/97 3:43p Peter
 * h3/cvg cataclysm
 * 
 * 38    10/08/97 11:33a Peter
 * reg group for palette download
 * 
 * 37    9/15/97 7:31p Peter
 * more cmdfifo cleanup, fixed normal buffer clear, banner in the right
 * place, lfb's are on, Hmmmm.. probably more
 * 
 * 36    9/04/97 3:32p Peter
 * starting grouping serial reg writes
 * 
 * 35    8/18/97 3:52p Peter
 * pre-hw arrival fixes/cleanup
 * 
 * 34    7/26/97 5:18p Peter
 * fixed macro effage
 * 
 * 33    7/08/97 2:49p Peter
 * started playing w/ h3 sim
 * 
 * 32    7/02/97 12:28p Peter
 * removed spurious NOP, tex dl
 * 
 * 31    6/30/97 3:22p Peter
 * cmd fifo sanity
 * 
**
*/

#include <3dfx.h>
#define FX_DLL_DEFINITION
#include <fxdll.h>
#include <glide.h>

#include "fxglide.h"
#include "fxcmd.h"

/*---------------------------------------------------------------------------
** _grTexDownloadNccTable
**
** Downloads an ncctable to the specified _physical_ TMU(s).  This
** function is called internally by Glide and should not be executed
** by an application.
*/
GR_DDFUNC(_grTexDownloadNccTable,
          void,
          (GrChipID_t tmu, FxU32 which, const GuNccTable *table, int start, int end))
{
#define FN_NAME "_grTexDownloadNccTable"
  GR_BEGIN_NOFIFOCHECK(FN_NAME,89);
  GDBG_INFO_MORE(gc->myLevel,"(%d,%d, 0x%x, %d,%d)\n",tmu,which,table,start,end);
  GR_ASSERT(start==0);
  GR_ASSERT(end==11);

  /* check for null pointer */
  if (table == NULL) return;

  gc->stats.palDownloads++;
  gc->stats.palBytes += (end-start+1)<<2;

  if (gc->tmu_state[tmu].ncc_table[which] != table) {
    SstRegs* texHW;
    int i;
#ifdef GLIDE_POINTCAST_PALETTE
    texHW = SST_TMU(hw,tmu);
#else
    texHW = SST_CHIP(hw, 0x06UL);
#endif

    if (which == 0) {
#ifdef GLIDE_POINTCAST_PALETTE
      REG_GROUP_BEGIN((0x02UL << tmu), nccTable0, 12, 0x0FFF);
#else
      REG_GROUP_BEGIN(0x06UL, nccTable0, 12, 0x0FFF);
#endif
      for (i = 0; i < 12; i++) {
        gc->state.shadow.tmuState[tmu].nccTable0[i] = table->packed_data[i];
        REG_GROUP_SET(texHW, nccTable0[i], table->packed_data[i]);
      }
      REG_GROUP_END();
    } else {
#ifdef GLIDE_POINTCAST_PALETTE
      REG_GROUP_BEGIN((0x02UL << tmu), nccTable1, 12, 0x0FFF);
#else
      REG_GROUP_BEGIN(0x06UL, nccTable1, 12, 0x0FFF);
#endif
      for (i = 0; i < 12; i++) {
        gc->state.shadow.tmuState[tmu].nccTable1[i] = table->packed_data[i];
        REG_GROUP_SET(texHW, nccTable1[i], table->packed_data[i]);
      }
      REG_GROUP_END();
    }

    gc->tmu_state[tmu].ncc_table[which] = table;
  }

  GR_END();
#undef FN_NAME
} /* _grTexDownloadNccTable */

/*-------------------------------------------------------------------
  Function: _grTexDownloadPalette
  Date: 6/9
  Implementor(s): jdt
  Library: Glide
  Description:
    Private function to download a palette to the specified tmu
  Arguments:
    tmu - which tmu to download the palette to
    pal - the pallete data
    start - beginning index to download
    end   - ending index to download
  Return:
    none
  -------------------------------------------------------------------*/
GR_DDFUNC(_grTexDownloadPalette,
          void,
          (GrChipID_t tmu, GrTexTable_t type, GuTexPalette *pal, int start, int end))
{
#define FN_NAME "_grTexDownloadPalette"
  GR_BEGIN_NOFIFOCHECK(FN_NAME, 89);
  GDBG_INFO_MORE(gc->myLevel,"(%d,0x%x, %d,%d)\n",tmu,pal,start,end);

  GR_CHECK_F(FN_NAME, pal == NULL, "pal invalid");
  GR_CHECK_F(FN_NAME, start < 0, "invalid start index");
  GR_CHECK_F(FN_NAME, end > 255, "invalid end index");
    
  /* NOTE:
  **
  **  This code broadcasts the palette because in the future, we will
  **  only support one global texture palette no matter how many TMUs
  **  there are.  This is fallout from the fact that future hardware
  **  has a unified memory architecture.
  **  
  **  Source licensees (meaning arcade or LBE vendors that) require the
  **  one palette/tmu mode should define GLIDE_POINTCAST_PALETTE on
  **  the command line for this file.  Understand, however, that this
  **  will not work on future hardware.
  */

#ifdef GLIDE_POINTCAST_PALETTE
  /*
  **  FURTHER NOTE:  
  **  There is a sublety (nice way of saying BUG) here.
  **  If TMU0 is specified, then the palette will be broadcast to all
  **  TMUS.  So, if the user downloads TMU1's palette, then TMU0's
  **  palette, TMU0's palette will be on *both* TMUs.  This is a
  **  pretty strong indicator that no one is using separate palettes
  **  on different TMUs.
  */
  hw = SST_TMU(hw,tmu);
#else
  hw = SST_CHIP(hw,0xE);
#endif

  gc->stats.palDownloads++;
  gc->stats.palBytes += ((end - start + 1) << 2);

  /* We divide the writes into 3 chunks trying to group things into
   * complete 8 word grouped packets to fit the nccTable palette
   * format: stuff before the 8 word alignment, aligned writes, and
   * stuff after the 8 word alignment to the end. The slop regions
   * are one packet apiece.  
   */
  {
#ifdef GLIDE_POINTCAST_PALETTE
    const FifoChipField chipId = (FifoChipField)(0x02UL << tmu);
#else
    const FifoChipField chipId = (FifoChipField)0x06UL;
#endif
    const int endSlop = (end & ~0x07);
    const int startSlop = MIN(((start + 8) & ~0x07) - 1, end);
    int i = start;

    if (type == GR_TEXTABLE_PALETTE) {
      /* Is the start of the palette range unaligned or is the end of
       * the range less than a completely aligned range?  
       */
      if (((start & 0x07) != 0) || (end < ((start + 8) & ~0x07))) {
        const FxI32 slopCount = startSlop - start + 1;
        GR_ASSERT((slopCount > 0) && (slopCount <= 8));
        
        REG_GROUP_BEGIN(chipId, nccTable0[4 + (start & 0x07)], 
                        slopCount, (0xFF >> (8 - slopCount)));
        while(i < start + slopCount) {
          FxU32 entry;
          
          entry = (0x80000000 | ((i & 0xFE) << 23) | pal->data[i] & 0xFFFFFF);
          
          gc->state.shadow.paletteRow[i>>3].data[i&7] = entry;
          REG_GROUP_SET(hw, nccTable0[4 + (i & 0x07)], entry );
          
          i++;
        }
        REG_GROUP_END();
      }

      /* Do all of the aligned palette ranges. */
      while(i < endSlop) {
        const int endIndex = i + 8;
        
        REG_GROUP_BEGIN(chipId, nccTable0[4], 8, 0xFF);
        while(i < endIndex) {
          FxU32 entry;
          
          entry = (0x80000000 | ((i & 0xFE) << 23) | pal->data[i] & 0xFFFFFF);
          
          gc->state.shadow.paletteRow[i>>3].data[i&7] = entry;
          REG_GROUP_SET(hw, nccTable0[4 + (i & 0x07)], entry );
          
          i++;
        }
        REG_GROUP_END();
      }
  
      /* Do we have any more slop at the end of the range? */
      if (i <= end) {
        const FxU32 slopCount = end - endSlop + 1;
        
        REG_GROUP_BEGIN(chipId, nccTable0[4], 
                        slopCount, (0xFF >> (8 - slopCount)));
        while(i <= end) {
          FxU32 entry;
          
          entry = (0x80000000 | ((i & 0xFE) << 23) | pal->data[i] & 0xFFFFFF);
          
          gc->state.shadow.paletteRow[i>>3].data[i&7] = entry;
          REG_GROUP_SET(hw, nccTable0[4 + (i & 0x07)], entry );
          
          i++;
        }
        REG_GROUP_END();
      }
    } else {
      /* Is the start of the palette range unaligned or is the end of
       * the range less than a completely aligned range?  
       */
      if (((start & 0x07) != 0) || (end < ((start + 8) & ~0x07))) {
        const FxI32 slopCount = startSlop - start + 1;
        GR_ASSERT((slopCount > 0) && (slopCount <= 8));
        
        REG_GROUP_BEGIN(chipId, nccTable0[4 + (start & 0x07)], 
                        slopCount, (0xFF >> (8 - slopCount)));
        while(i < start + slopCount) {
          FxU32
            p1, p2, p3, p4,
            entry;
          
          p1 = p2 = pal->data[i];
          p1 &= 0xfc000000;          p2 &= 0x00fc0000;
          p1 >>= 8;                  p2 >>= 6;
          p3 = p4 = pal->data[i];
          p3 &= 0x0000fc00;          p4 &= 0x000000fc;
          p3 >>= 4;                  p4 >>= 2;
          p1 |= p2;                  p3 |= p4;              p1 |= p3;

          entry = (0x80000000UL | ((i & 0xFEUL) << 23) | p1);
          gc->state.shadow.paletteRow[i>>3].data[i&7] = entry;
          REG_GROUP_SET(hw, nccTable0[4 + (i & 0x07)], entry);

          i++;
        }
        REG_GROUP_END();
      }

      /* Do all of the aligned palette ranges. */
      while(i < endSlop) {
        const int endIndex = i + 8;
        
        REG_GROUP_BEGIN(chipId, nccTable0[4], 8, 0xFF);
        while(i < endIndex) {
          FxU32 p1, p2, p3, p4;
          p1 = p2 = pal->data[i];
          p1 &= 0xfc000000;          p2 &= 0x00fc0000;
          p1 >>= 8;                  p2 >>= 6;
          p3 = p4 = pal->data[i];
          p3 &= 0x0000fc00;          p4 &= 0x000000fc;
          p3 >>= 4;                  p4 >>= 2;
          p1 |= p2;                  p3 |= p4;              p1 |= p3;
          REG_GROUP_SET(hw, nccTable0[4 + (i & 0x07)],
                        (0x80000000 | ((i & 0xFE) << 23) | p1));
          i++;
        }
        REG_GROUP_END();
      }
  
      /* Do we have any more slop at the end of the range? */
      if (i <= end) {
        const FxU32 slopCount = end - endSlop + 1;
        
        REG_GROUP_BEGIN(chipId, nccTable0[4], 
                        slopCount, (0xFF >> (8 - slopCount)));
        while(i <= end) {
          FxU32 
            p1, p2, p3, p4,
            entry;

          p1 = p2 = pal->data[i];
          p1 &= 0xfc000000;          p2 &= 0x00fc0000;
          p1 >>= 8;                  p2 >>= 6;
          p3 = p4 = pal->data[i];
          p3 &= 0x0000fc00;          p4 &= 0x000000fc;
          p3 >>= 4;                  p4 >>= 2;
          p1 |= p2;                  p3 |= p4;              p1 |= p3;

          entry = (0x80000000UL | ((i & 0xFE) << 23) | p1);
          gc->state.shadow.paletteRow[i>>3].data[i&7] = entry;
          REG_GROUP_SET(hw, nccTable0[4 + (i & 0x07)], entry);
          
          i++;
        }
        REG_GROUP_END();
      }
    }
  }

  /* NB: If we're changing table types and the currently selected texture is
   * a palettized texture then we need to change the texture format
   * behind the user's back to match the table type.
   */
  if (type != gc->state.tex_table) {
    FxI32
      i;

    for(i = 0; i < gc->num_tmu; i++) {
      const FxU32
        texFmt      = (gc->state.shadow.tmuState[i].textureMode & SST_TFORMAT),
        newTexMode  = (gc->state.shadow.tmuState[i].textureMode ^
                       ((GR_TEXFMT_P_8 ^ GR_TEXFMT_P_8_RGBA) << SST_TFORMAT_SHIFT));
      
      if ((texFmt == (GR_TEXFMT_P_8 << SST_TFORMAT_SHIFT)) ||
          (texFmt == (GR_TEXFMT_P_8_RGBA << SST_TFORMAT_SHIFT))) {
        
        GR_SET_EXPECTED_SIZE(sizeof(FxU32), 1);
        GR_SET((0x02UL << i), SST_TMU(hw, i), textureMode, newTexMode);
        GR_CHECK_SIZE();

        gc->state.shadow.tmuState[i].textureMode = newTexMode;
      }
    }
  }
    
  GR_END();
#undef FN_NAME
} /* _grTexDownloadPalette */

/*-------------------------------------------------------------------
  Function: grTexDownloadTable
  Date: 6/3
  Implementor(s): jdt, GaryMcT
  Library: glide
  Description:
    download look up table data to a tmu
  Arguments:
    tmu - which tmu
    type - what type of table to download
        One of:
            GR_TEXTABLE_NCC0
            GR_TEXTABLE_NCC1
            GR_TEXTABLE_PALETTE
    void *data - pointer to table data
  Return:
    none
  -------------------------------------------------------------------*/
GR_ENTRY(grTexDownloadTable,
         void,
         (GrTexTable_t type,  void *data))
{
  GR_BEGIN_NOFIFOCHECK("grTexDownloadTable",89);
  GDBG_INFO_MORE(gc->myLevel,"(%d,0x%x)\n",type,data);
  GR_CHECK_F(myName, type > GR_TEXTABLE_PALETTE_6666_EXT, "invalid table specified");
  GR_CHECK_F(myName, !data, "invalid data pointer");

  switch(type) {
  case GR_TEXTABLE_PALETTE:
  case GR_TEXTABLE_PALETTE_6666_EXT:
    _grTexDownloadPalette(GR_TMU0, type, (GuTexPalette *)data, 0, 255);
    break;
  default: /* Type is an ncc table */
    _grTexDownloadNccTable(GR_TMU0, type, (GuNccTable*)data, 0, 11);
    break;
  }

  /* NB: Set the current palette type after we do the download because
   * the palette download code may need to know that there is a table
   * type change and do something hoopti.  
   */
  gc->state.tex_table = type;

  GR_END();
} /* grTexDownloadTable */

/*-------------------------------------------------------------------
  Function: grTexDownloadMipMapLevelPartialTiled
  Date: 10-Dec-98
  Implementor(s): dow
  Description:
    Downloads a mipmap level to the specified tmu at the given
    texture start address--taking into account that the memory is
    tiled and the stride of that tiled memory may be much greater than
    the width of any given mipmap level.
  
  Arguments:
    tmu           - which tmu
    startAddress - starting address for texture download,
                     this should be some value between grTexMinAddress()
                     and grTexMaxAddress()
    thisLod      - lod constant that describes the mipmap level
                    to be downloaded
    largeLod     - largest level of detail in complete mipmap to 
                   be downloaded at startAddress of which level to
                   be downloaded is a part
    aspectRatio  - aspect ratio of this mipmap
    format        - format of mipmap image data
    evenOdd      - which set of mipmap levels have been downloaded for
                    the selected texture
                    One of:
                      GR_MIPMAPLEVELMASK_EVEN 
                      GR_MIPMAPLEVELMASK_ODD
                      GR_MIPMAPLEVELMASK_BOTH
    data          - pointer to mipmap data
  Return:
    FXTRUE;
  -------------------------------------------------------------------*/
FxBool
_grTexDownloadMipMapLevelPartialTiled(GrChipID_t tmu, 
                                      FxU32 startAddress,
                                      GrLOD_t thisLod, GrLOD_t largeLod,
                                      GrAspectRatio_t aspectRatio, 
                                      GrTextureFormat_t format, 
                                      FxU32 evenOdd, void *data, 
                                      int t, int maxT)
{
#define FN_NAME "_grTexDownloadMipMapLevelPartialTiled"
  GR_BEGIN_NOFIFOCHECK_RET(FN_NAME"\n", 89);

  GR_CHECK_TMU(FN_NAME, tmu);
  GR_ASSERT(gc->tmuMemInfo[tmu].texTiled);

  {
    const struct GrTmuMemInfo*
      memInfo = gc->tmuMemInfo + tmu;
    const FxU32 
      texelSize = _grBitsPerTexel[format],
      texStrideBytes = memInfo->texStrideBytes;
#if 0
    FxU32 maxS = WIDTH_BY_ASPECT_LOD(aspectRatio, thisLod);
#else
    FxU32 maxS;
#endif
    FxU32 
      texOffset = 0x00UL;

    if (thisLod < largeLod) {
      texOffset = _grTexCalcMipmapLevelOffsetTiled(tmu,
                                                   thisLod, largeLod,
                                                   aspectRatio,
                                                   format,
                                                   evenOdd,
                                                   NULL,
                                                   NULL);
    }    
    texOffset += memInfo->tramLfbAddr;

    GR_CHECK_F(FN_NAME, texelSize == 0, "invalid texture format");

    switch(format) {
    case GR_TEXFMT_ARGB_CMP_FXT1:
    //case GR_TEXFMT_ARGB_CMP_DXT1:
      maxS = WIDTH_BY_ASPECT_LOD_FXT1(aspectRatio, thisLod);
      break;
    case GR_TEXFMT_ARGB_CMP_DXT1:
    case GR_TEXFMT_ARGB_CMP_DXT2:
    case GR_TEXFMT_ARGB_CMP_DXT3:
    case GR_TEXFMT_ARGB_CMP_DXT4:
    case GR_TEXFMT_ARGB_CMP_DXT5:
      maxS = WIDTH_BY_ASPECT_LOD_DXT(aspectRatio, thisLod);
      break;
    default:
      maxS = WIDTH_BY_ASPECT_LOD(aspectRatio, thisLod);
      break;
    }

    switch(texelSize) {
    case 4:  /* 4-bit textures */
      {
#if 0
        const FxU32
              *src32 = (const FxU32*)data;
        
        /* Minimum maxS is 8 for FXT1 DXT1 format. */
        FxU32 fxt1MaxS = MAX(maxS, 8);
        switch(fxt1MaxS) {
        case 8:
          texOffset += (t * texStrideBytes);
          for (; t <= maxT; t++) {
            LINEAR_WRITE_BEGIN(1, SSTCP_PKT5_LFB, texOffset, 0x0UL, 0x0UL);
            LINEAR_WRITE_SET(texOffset, *src32);
            LINEAR_WRITE_END();
            src32++;
            texOffset += texStrideBytes;
          }
          break;
        default:
          for (; t <= maxT; t++) {
            FxU32
              texAddress = texOffset + t * texStrideBytes,
              s;

            LINEAR_WRITE_BEGIN((fxt1MaxS >> 3), SSTCP_PKT5_LFB, texAddress,
                               0x0UL, 0x0UL);
            for (s = 0; s < fxt1MaxS; s+=8)  {
              LINEAR_WRITE_SET(texAddress, *src32);
              texAddress +=4;
              src32++;
            }
            LINEAR_WRITE_END();
          }
          break;
        }
#else /* KoolSmoky - we don't need to see the minimum maxS anymore */
        switch(maxS) {
        case 4:
          {
            const FxU16
              *src16 = (const FxU16*)data;
            
            src16++;
            for(; t <= maxT; t+=4) {
              FxU32
                texAddress = texOffset + t * texStrideBytes, s;
              LINEAR_WRITE_BEGIN(2, SSTCP_PKT5_LFB, texAddress, 0x00UL, 0x00UL);
              for (s = 0; s < 2; s++)  {
                LINEAR_WRITE_SET(texAddress, (FxU32)(*src16));
                texAddress++;
                src16 += 4;
              }
              LINEAR_WRITE_END();
              
            }
          }
          break;
        case 8:
          {
            const FxU32
              *src32 = (const FxU32*)data;
            
            texOffset += (t * texStrideBytes);
            for (; t <= maxT; t++) {
              LINEAR_WRITE_BEGIN(1, SSTCP_PKT5_LFB, texOffset, 0x0UL, 0x0UL);
              LINEAR_WRITE_SET(texOffset, *src32);
              LINEAR_WRITE_END();
              src32++;
              texOffset += texStrideBytes;
            }
          }
          break;
        default:
          {
            const FxU32
              *src32 = (const FxU32*)data;
            
            for (; t <= maxT; t++) {
              FxU32
                texAddress = texOffset + t * texStrideBytes,
                s;
              
              LINEAR_WRITE_BEGIN((maxS >> 3), SSTCP_PKT5_LFB, texAddress,
                                 0x0UL, 0x0UL);
              for (s = 0; s < maxS; s+=8)  {
                LINEAR_WRITE_SET(texAddress, *src32);
                texAddress +=4;
                src32++;
              }
              LINEAR_WRITE_END();
            }
          }
          break;
        }
#endif
      }
      break;
    case 8:  /* 8-bit textures */
      {
        const FxU8
          *src8 = (const FxU8*)data;

        switch(maxS) {
        case 1:
          texOffset += (t * texStrideBytes);
          for (; t <= maxT; t++) {
            LINEAR_WRITE_BEGIN(1, SSTCP_PKT5_LFB, texOffset, 0x0UL, 0x0UL);
            LINEAR_WRITE_SET(((FxU32*)texOffset), *src8);
            LINEAR_WRITE_END();
            
            src8 += 1;
            texOffset += texStrideBytes;
          }
          break;
          
        case 2:
          texOffset += (t * texStrideBytes);
          for (; t <= maxT; t++) {
            LINEAR_WRITE_BEGIN(1, SSTCP_PKT5_LFB, texOffset, 0x0UL, 0x0UL);
            LINEAR_WRITE_SET(((FxU32*)texOffset), *(const FxU16*)src8);
            LINEAR_WRITE_END();
            
            src8 += 2;
            texOffset += texStrideBytes;
          }
          break;
#if 0
        case 4:
          texOffset += (t * texStrideBytes);
          for (; t <= maxT; t++) {
            LINEAR_WRITE_BEGIN(1, SSTCP_PKT5_LFB, texOffset, 0x0UL, 0x0UL);
            LINEAR_WRITE_SET(((FxU32*)texOffset), *(const FxU32*)src8);
            LINEAR_WRITE_END();
            
            src8 += 4;
            texOffset += texStrideBytes;
          }
          break;
#endif
        default:
          for (; t <= maxT; t++) {
            FxU32
              texAddress = (texOffset + t * texStrideBytes),
              s;
            
            LINEAR_WRITE_BEGIN((maxS >> 2), SSTCP_PKT5_LFB, texAddress, 0x0UL,
                               0x0UL);
            
            for (s = 0; s < maxS; s += 4) {
              LINEAR_WRITE_SET(((FxU32*)texAddress), *(const FxU32*)src8);
              
              src8 += 4;
              texAddress += 4;
            }
            
            LINEAR_WRITE_END();
          }
          break;
        }
      }
      break;
    case 16:  /* 16-bit textures */
      {
        const FxU16
          *src16 = (const FxU16*)data;
        
        switch(maxS) {
        case 1:
          texOffset += (t * texStrideBytes);
          for(; t <= maxT; t++) {
            LINEAR_WRITE_BEGIN(1, SSTCP_PKT5_LFB, texOffset, 0x00UL, 0x00UL);
            LINEAR_WRITE_SET_16(texOffset, *src16);
            LINEAR_WRITE_END();
            
            src16 += 1;
            texOffset += texStrideBytes;
          }
          break;
          
        case 2:
          texOffset += (t * texStrideBytes);
          for(; t <= maxT; t++) {
            LINEAR_WRITE_BEGIN(1, SSTCP_PKT5_LFB, texOffset, 0x00UL, 0x00UL);
            LINEAR_WRITE_SET_16(texOffset, *(const FxU32*)src16);
            LINEAR_WRITE_END();
            
            src16 += 2;
            texOffset += texStrideBytes;
          }
          break;
          
        default:
          for (; t <= maxT; t++) {
            FxU32
              texAddress = texOffset + t * texStrideBytes,
              s;
            
            LINEAR_WRITE_BEGIN((maxS >> 1), SSTCP_PKT5_LFB, texAddress, 0x0UL,
                               0x0UL);
            
            for (s = 0; s < maxS; s += 4) {
              LINEAR_WRITE_SET_16(texAddress + 0, *(const FxU32*)(src16 + 0));
              LINEAR_WRITE_SET_16(texAddress + 4, *(const FxU32*)(src16 + 2));
              
              src16 += 4;
              texAddress += 8;
            }
            
            LINEAR_WRITE_END();
          }
          break;
        }
      }
      break;
    case 32:  /* 32-bit textures */
      {
        const FxU32
          *src32 = (const FxU32*)data;
        
        switch(maxS) {
        case 1:
          texOffset += (t * texStrideBytes);
          for(; t <= maxT; t++) {
            LINEAR_WRITE_BEGIN(1, SSTCP_PKT5_LFB, texOffset, 0x00UL, 0x00UL);
            LINEAR_WRITE_SET(texOffset, *src32);
            LINEAR_WRITE_END();
            src32 += 1;
            texOffset += texStrideBytes;
          }
          break;
        default:
          for (; t <= maxT; t++) {
            FxU32
              texAddress = texOffset + t * texStrideBytes,
              s;
            
            LINEAR_WRITE_BEGIN(maxS, SSTCP_PKT5_LFB, texAddress, 0x0UL, 0x0UL);
            for(s = 0; s < maxS; s += 1) {
              LINEAR_WRITE_SET(texAddress, *src32);
              src32 += 1;
              texAddress += 4;
            }
            LINEAR_WRITE_END();
          }
          break;
        }
      }
      break;
    default:
      /* Undefined texture format */
      GR_ASSERT(texelSize);
      break;
    }

  }

  return FXTRUE;
#undef FN_NAME
} /* _grTexDownloadMipMapLevelPartialTiled */

/*-------------------------------------------------------------------
  Function: grTexDownloadMipMapLevelPartial
  Date: 6/2
  Implementor(s): GaryMcT, Jdt
  Library: glide
  Description:
    Downloads a mipmap level to the specified tmu at the given
    texture start address
  Arguments:
    tmu           - which tmu
    startAddress - starting address for texture download,
                     this should be some value between grTexMinAddress()
                     and grTexMaxAddress()
    thisLod      - lod constant that describes the mipmap level
                    to be downloaded
    largeLod     - largest level of detail in complete mipmap to 
                   be downloaded at startAddress of which level to
                   be downloaded is a part
    aspectRatio  - aspect ratio of this mipmap
    format        - format of mipmap image data
    evenOdd      - which set of mipmap levels have been downloaded for
                    the selected texture
                    One of:
                      GR_MIPMAPLEVELMASK_EVEN 
                      GR_MIPMAPLEVELMASK_ODD
                      GR_MIPMAPLEVELMASK_BOTH
    data          - pointer to mipmap data
  Return:
    none
  -------------------------------------------------------------------*/

GR_ENTRY(grTexDownloadMipMapLevelPartial,
         FxBool,
         (GrChipID_t tmu, FxU32 startAddress,
          GrLOD_t thisLod, GrLOD_t largeLod, GrAspectRatio_t aspectRatio,
          GrTextureFormat_t format, FxU32 evenOdd, 
          void *data, 
          int t, int max_t))
{
#define FN_NAME "grTexDownloadMipMapLevelPartial"
  GR_BEGIN_NOFIFOCHECK_RET(FN_NAME, 89);
  GDBG_INFO_MORE(gc->myLevel,"(%d,0x%x, %d,%d,%d, %d,%d 0x%x, %d,%d)\n",
                 tmu, startAddress,
                 thisLod, largeLod, aspectRatio,
                 format, evenOdd,
                 data,
                 t, max_t);

  /* Sanity checking */
  {

    GR_CHECK_TMU(FN_NAME, tmu);
    GR_CHECK_F(FN_NAME, 
               (gc->windowed ? FXFALSE :
                (startAddress + _grTexTextureMemRequired(thisLod, largeLod,
                                                         aspectRatio, 
                                                         format, evenOdd,
                                                         FXTRUE)
                 > gc->tmu_state[tmu].total_mem)),
               "insufficient texture ram at startAddress");
    GR_CHECK_F(FN_NAME, startAddress & SST_TEXTURE_ALIGN_MASK,
               "unaligned startAddress");
    GR_CHECK_F(FN_NAME, _grBitsPerTexel[format] == 0,
               "invalid texture format");
    if (!IS_NAPALM(gc->bInfo->pciInfo.deviceID)) {
      /*
       * Sanity checks for Banshee and Voodoo3
       */
      GR_CHECK_F(FN_NAME, thisLod > GR_LOD_LOG2_256,
                 "thisLod invalid");
      GR_CHECK_F(FN_NAME, largeLod > GR_LOD_LOG2_256,
                 "largeLod invalid");
      GR_CHECK_F(FN_NAME, (_grBitsPerTexel[format] == 4),
                 "this hardware doesn't support 4-bit textures");
      GR_CHECK_F(FN_NAME, (_grBitsPerTexel[format] == 32),
                 "this hardware doesn't support 32-bit textures");
    } else {
      /*
       * Sanity checks for Napalm
       */
      GR_CHECK_F(FN_NAME, thisLod > GR_LOD_LOG2_2048,
                 "thisLod invalid");
      GR_CHECK_F(FN_NAME, largeLod > GR_LOD_LOG2_2048,
                 "largeLod invalid");
    }
    
    GR_CHECK_F(FN_NAME, thisLod > largeLod,
               "thisLod may not be larger than largeLod");
    GR_CHECK_F(FN_NAME, 
               ((aspectRatio > GR_ASPECT_LOG2_8x1) || 
                (aspectRatio < GR_ASPECT_LOG2_1x8)),
               "aspectRatio invalid");
    GR_CHECK_F(FN_NAME, evenOdd > 0x3 || evenOdd == 0,
               "evenOdd mask invalid");
    GR_CHECK_F(FN_NAME, !data,
               "invalid data pointer");
#ifdef FX_GLIDE_NAPALM
    switch(format) {
    case GR_TEXFMT_ARGB_CMP_FXT1:
    case GR_TEXFMT_ARGB_CMP_DXT1:
      GR_CHECK_F(FN_NAME, max_t >=
                 _grMipMapHostWHCmp4Bit[G3_ASPECT_TRANSLATE(aspectRatio)]
                 [thisLod][1], "invalid end row for fxt1, dxt1");
      break;
    case GR_TEXFMT_ARGB_CMP_DXT2:
    case GR_TEXFMT_ARGB_CMP_DXT3:
    case GR_TEXFMT_ARGB_CMP_DXT4:
    case GR_TEXFMT_ARGB_CMP_DXT5:
      GR_CHECK_F(FN_NAME, max_t >=
                 _grMipMapHostWHDXT[G3_ASPECT_TRANSLATE(aspectRatio)]
                 [thisLod][1], "invalid end row for dxt2,3,4,5");
      break;
    default:
      GR_CHECK_F(FN_NAME, max_t >=
                 _grMipMapHostWH[G3_ASPECT_TRANSLATE(aspectRatio)][thisLod][1],
                 "invalid end row");
      break;
    }
#else
    GR_CHECK_F(FN_NAME, max_t >=
               _grMipMapHostWH[G3_ASPECT_TRANSLATE(aspectRatio)][thisLod][1],
               "invalid end row");
#endif /* FX_GLIDE_NAPALM */
#ifdef GLIDE_TEST_TEXTURE_ALIGNMENT
    /* always check texture alignment */
    INTERNAL_CHECK(FN_NAME, startAddress & SST_TEXTURE_ALIGN_MASK,
               "unaligned startAddress", FXTRUE);
#endif
  }

  /* Skip this level entirely if not in odd/even mask */
  if (!(evenOdd & (thisLod & 0x1
                   ? GR_MIPMAPLEVELMASK_ODD : GR_MIPMAPLEVELMASK_EVEN)))
    goto all_done;
    
  {
    struct GrTmuMemInfo*
      memInfo = gc->tmuMemInfo + tmu;

    /* Part1 of the texel cache coherency stuff for avenger. According
     * to the docs, the 3d nopCmd should do this all, but it does not
     * work as advertised. But wait, there's more after the download...
     *
     * Force a pixel flush which should force all of the
     * texture downloads to flush from internal fifos etc.  
     */
    GR_TEX_FLUSH_PRE(memInfo);

    if (memInfo->texTiled) {
      _grTexDownloadMipMapLevelPartialTiled(tmu, 
                                            startAddress, 
                                            thisLod, largeLod, aspectRatio,
                                            format, 
                                            evenOdd, data, 
                                            t, max_t);
    } else {
      FxU32
        baseAddress;

      /* Compute physical start address for the download. */
      {
        FxU32
          texOffset = 0x00UL;
        
        /* We need to do some magic to pack the small levels and have a
         * properly aligned baseAddr. If the current level is not going
         * to start on an alignment boundary when working backwards in
         * the chain we need to offset it into the block so that it is
         * addressable with an aligned baseAddr.  
         */
        {
          GrLOD_t minLod = thisLod;
          /*
           * We can safely skip this part if we're in FXT1-land
           * because the minimum level size is 16 bytes (8x4x1/2)
           * which matches the alignment restriction.
           */
          /* KoolSmoky - same for DXT2,3,4,5 were the minimum level
           * size is 16 bytes (4x4x1) which also matches the alignment
           * restriction.
           * same for DXT1 ((4x4x1/2)x2) minimum level size is 16 bytes
           */
          if((format < GR_TEXFMT_ARGB_CMP_DXT1) && (format != GR_TEXFMT_ARGB_CMP_FXT1)) {
            const FxU32
              aspectIndex = ((aspectRatio < GR_ASPECT_LOG2_1x1) 
                             ? -aspectRatio 
                             : aspectRatio),
              lodIndex = ((thisLod == GR_LOD_LOG2_256)
                          ? GR_LOD_LOG2_256 : thisLod + 1),
              formatMult = _grBitsPerTexel[format];
            FxU32
              levelSize = (_grMipMapHostSize[aspectIndex][lodIndex]
                           * formatMult)>>3; /* Cvt from bits to bytes */
            
            GR_CHECK_F(FN_NAME, formatMult == 0, "invalid texture format");
            
            if (levelSize < SST_TEXTURE_ALIGN) {
              GrLOD_t
                maxLod = lodIndex;
              
              /* Find the smallest level that naturally starts on an
               * alignment boundary. If this is larger than the current
               * mipmap chain's large lod then this we have to compute the
               * offset within this alignment unit.
               *
               * NB: This could be a table lookup, but I'm writing the
               * obvious code right now so that there aren't any
               * mystic #'s being pulled out of the recesses of my
               * colon.
               */
              while(maxLod < GR_LOD_LOG2_256) {
                levelSize = (_grMipMapHostSize[aspectIndex][maxLod]
                             * formatMult)>>3; // bits to bytes convertion
                if (levelSize >= SST_TEXTURE_ALIGN) break;
                // check on the Even/Odd mask to see if the mip-map affects this TMU
                if((maxLod & 1) ? (evenOdd & GR_MIPMAPLEVELMASK_ODD) : (evenOdd & GR_MIPMAPLEVELMASK_EVEN))
                  texOffset += levelSize;
                maxLod++;
              }
              
              /* maxLod is the index of the smallest level of this aspect
               * ratio that takes up at least a full alignment unit.  We
               * reset the small lod to this so that we can compute the
               * offset for the 'large' levels in the mipmap chain.  
               */
              GR_ASSERT(maxLod != GR_LOD_LOG2_256);
              minLod = maxLod - 1;
            }
          }
          
          if (minLod < largeLod) {
            texOffset += _grTexTextureMemRequired(minLod + 1, largeLod,
                                                  aspectRatio,
                                                  format,
                                                  evenOdd,
                                                  FXFALSE);
          }
        }
        
        /* Compute physical start address for the download. 
         *
         * NB: This is going directly to the 2d lfb space rather than
         * through the texture port so we have to add in the actual hw
         * offset that the texture 'surface' starts at.
         */
        baseAddress = (memInfo->tramOffset + 
                       startAddress +
                       texOffset);
      }
      
      /* Do the download */
      {
        const FxU32
          bitsPerTexel = _grBitsPerTexel[format];
        FxU32
          width, formatSel, widthSel, max_s;

        switch(format) {
        case GR_TEXFMT_ARGB_CMP_FXT1:
        //case GR_TEXFMT_ARGB_CMP_DXT1:
          /* Minimum size for 4-bit compressed format mipmaps is 8x4. FXT1,DXT1 */
          width = _grMipMapHostWHCmp4Bit[G3_ASPECT_TRANSLATE(aspectRatio)][thisLod][0];
          widthSel = (width >> 2);
          //widthSel = (width > 8)
          //  ? 4  /* Use _grTexDownload_Default_4_WideS() */
          //  : 3; /* Use _grTexDownload_Default_4_8()     */
          break;
        case GR_TEXFMT_ARGB_CMP_DXT1:
          width = _grMipMapHostWHCmp4Bit[G3_ASPECT_TRANSLATE(aspectRatio)][thisLod][0];
          /* hack for DXT1 8x4 mipmaps. download only 4x4. */
          widthSel = (_grMipMapHostWHDXT[G3_ASPECT_TRANSLATE(aspectRatio)][thisLod][0] >> 2);
          break;
        case GR_TEXFMT_ARGB_CMP_DXT2:
        case GR_TEXFMT_ARGB_CMP_DXT3:
        case GR_TEXFMT_ARGB_CMP_DXT4:
        case GR_TEXFMT_ARGB_CMP_DXT5:
          /* Minimum size for 8-bit compressed format mipmaps is 4x4. DXT2,3,4,5 */
          width = _grMipMapHostWHDXT[G3_ASPECT_TRANSLATE(aspectRatio)][thisLod][0];
          widthSel = (width >> 1); /* For 8-bit format */
          break;
        default:
          width =_grMipMapHostWH[G3_ASPECT_TRANSLATE(aspectRatio)][thisLod][0];
          widthSel = (width >> 1); /* For 8,16,32-bit formats */
          break;
        }

        /*
         * Interpretations:
         * formatSel: Chooses among 4, 8, 16, and 32-bit download procedures.
         *            We want formatSel == log2(bitsPerTexel >> 2).
         * max_s:     The width, measured in 32-bit units.
         */
        switch(bitsPerTexel) {
        case  4:
          formatSel = 0;
          max_s = width >> 3;
          break;
        case  8:
          formatSel = 1;
          max_s = width >> 2;
          break;
        case 16:
          formatSel = 2;
          max_s = width >> 1;
          break;
        case 32:
          formatSel = 3;
          max_s = width;
          break;
        default:
          /* Undefined format, but let's try 16-bit dimensions just in case. */
          formatSel = 2;
          max_s = width >> 1;
          break;
        }

        if (max_s <= 0) max_s = 1;
        if (widthSel > 3) widthSel = 4;
    
        gc->stats.texBytes += max_s * (max_t - t + 1) * 4;

        (*((*gc->archDispatchProcs.texDownloadProcs)[formatSel][widthSel]))
          (gc, baseAddress, max_s, t, max_t, data);
      }
    }

    /* If this is a small lod level in a texture replacing texels that
     * are already loaded then it may be necessary to flush the old
     * texels from memory before any other rendering operation using
     * this texture is issued. Unconditionally flush these old texels
     * just in case rather than being too clever.
     *
     * The reason that we need to flush here even though we're not
     * going through the texture port is because it is perfectly
     * legal to source once and download over and over again. (See
     * chd for a funny SpecOps story).
     *
     * NB: The documented nop does not currently work on banshee which
     * is why we do the ~texBaseAddr crap along w/ the 2d nop.
     */
    GR_TEX_FLUSH_POST(memInfo);
  }

 all_done:
  gc->stats.texDownloads++;

  return FXTRUE;
#undef FN_NAME
} /* grTexDownloadMipmapLevelPartial */
