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
** $Header: f:\\cvsroot/Glide3x/h5/glide3/src/gaa.c,v 1.6 2002/04/29 15:50:33 KoolSmoky Exp $
** $Log: 
**  5    3dfx      1.2.1.0.1.0 10/11/00 Brent           Forced check in to enforce
**       branching.
**  4    3dfx      1.2.1.0     06/20/00 Joseph Kain     Changes to support the
**       Napalm Glide open source release.  Changes include cleaned up offensive
**       comments and new legal headers.
**  3    3dfx      1.2         11/08/99 Larry  warner   Changes to enable building
**       with MSVC 6.
**  2    3dfx      1.1         11/02/99 Larry  warner   Clean up some debug code to
**       make MSVC 6 happy.
**  1    3dfx      1.0         09/11/99 StarTeam VTS Administrator 
** $
** 
** 25    7/14/99 9:39a Atai
** direct register write for glide3x
** test04 can do 4 sample aa (2 chips)
** 
** 24    5/12/99 10:20a Atai
** fixed PRS 5815
** 
** 23    4/16/99 2:48p Kcd
** Hack to allow defferent SETF macro to be used.
** 
** 22    2/18/99 4:38p Kcd
** Oops, broke the x86 build.  That's the strangest cast I've ever seen.
** 
** 21    2/18/99 3:24p Kcd
** Mac happiness.
** 
** 20    12/03/98 11:26p Dow
** Code 'cleanup' heheh
** 
** 19    11/04/98 5:59p Jeske
** add splash speckle mini-workaround
** 
** 18    10/12/98 9:51a Peter
** dynamic 3DNow!(tm)
** 
** 17    9/25/98 5:41p Atai
** fix aa point routine - saving the wrong culling flag
** 
** 16    9/23/98 2:34p Atai
** fixed grDrawVertexArrayContiguous line routine in aa clip coords mode
** 
** 15    9/18/98 6:43p Atai
** fix clip coords line and triangles
** 
** 14    8/18/98 4:30p Atai
** fixed aa triangle in clip coords
** 
** 13    7/01/98 8:40a Jdt
** removed gc arg from trisetup funcs
** 
** 12    8/03/98 6:39a Jdt
** moved stats from global into gc
** 
** 11    7/18/98 12:26a Jdt
** Changes to reflect new shadow register structure
** 
** 10    7/16/98 8:17a Jdt
** fxcmd.h
** 
** 9     6/25/98 1:19p Atai
** aaVpDrawArrayEdgeSense
** 
** 8     6/09/98 11:59a Atai
** 1. update glide header
** 2. fix cull mode
** 3. fix tri stats
** 
** 7     5/29/98 2:09p Atai
** remove polygon code
** 
** 6     5/15/98 4:02p Atai
** fogCoord and texchroma extension for Banshee
** 
** 4     3/21/98 11:31a Atai
** added GR_TRIANGLE_STRIP_CONTINUE and GR_TRIANGLE_FAN_CONTINUE
** 
** 3     1/26/98 11:30a Atai
** update to new glide.h
** 
** 2     1/22/98 10:35a Atai
** 1. introduce GLIDE_VERSION, g3\glide.h, g3\glideutl.h, g2\glide.h,
** g2\glideutl.h
** 2. fixed grChromaRange, grSstOrigin, and grGetProcAddress
 * 
 * 1     1/16/98 4:29p Atai
 * create glide 3 src
 * 
 * 61    1/15/98 2:46p Atai
 * fixed grDrawPoint and grDrawLine in aa mode
 * 
 * 60    12/17/97 4:45p Peter
 * groundwork for CrybabyGlide
 * 
 * 59    12/12/97 12:43p Atai
 * move i and dateElem into the set up loop
 * 
 * 57    12/08/97 10:40a Atai
 * modify draw vertex primitive routines to do grDrawVertexArrayLinear()
 * 
 * 56    12/05/97 4:26p Peter
 * watcom warnings
 * 
 * 55    11/24/97 4:40p Peter
 * aa prims vs packing
 * 
 * 54    11/21/97 6:05p Atai
 * use one datalist (tsuDataList) in glide3
 * 
 * 53    11/21/97 3:20p Peter
 * direct writes tsu registers
 * 
 * 52    11/18/97 6:11p Peter
 * fixed glide3 effage
 * 
 * 51    11/18/97 4:36p Peter
 * chipfield stuff cleanup and w/ direct writes
 * 
 * 50    11/18/97 3:26p Atai
 * update vData 
 * optimize state monster
 * 
 * 49    11/17/97 4:55p Peter
 * watcom warnings/chipfield stuff
 * 
 * 48    11/06/97 6:10p Atai
 * update GrState size
 * rename grDrawArray to grDrawVertexArray
 * update _grDrawPoint and _grDrawVertexList
 * 
 * 47    11/04/97 6:35p Atai
 * 1. sync with data structure changes
 * 2. break up aa triangle routine
 * 
 * 46    11/04/97 5:04p Peter
 * cataclysm part deux
 * 
 * 45    11/04/97 4:57p Atai
 * use byte offset
 * 
 * 44    11/03/97 3:43p Peter
 * h3/cvg cataclysm
 * 
 * 43    11/03/97 3:19p Atai
 * optimization
 * 
 * 42    10/29/97 2:45p Peter
 * C version of Taco's packing code
 * 
 * 41    10/29/97 2:24p Atai
 * re-work aa draw routines to increase sbench number
 * 
 * 40    10/21/97 8:38p Atai
 * added lines routine for grDrawArray
 * 
 * 39    10/21/97 3:22p Peter
 * hand pack rgb
 * 
 * 38    10/19/97 12:51p Peter
 * no tsu happiness
 * 
 * 37    10/17/97 3:15p Peter
 * removed unused addr field from datalist
 * 
 * 36    10/17/97 10:15a Peter
 * packed rgb state cleanup
 * 
 * 35    10/16/97 5:33p Peter
 * argb != rgba
 * 
 * 34    10/16/97 3:40p Peter
 * packed rgb
 * 
 * 33    10/16/97 1:50p Atai
 * fix drawarray bugs
 * 
 * 32    10/14/97 7:33p Atai
 * fix compiler error
 * 
 * 31    10/14/97 5:40p Atai
 * added grculltest
 * 
 * 30    10/14/97 4:36p Atai
 * added aa points, strip line and trianlges for drawarray
 * 
 * 29    10/09/97 8:02p Dow
 * State Monster 1st Cut
 * 
 * 28    10/08/97 5:19p Peter
 * optinally clamp only texture params
 * 
 * 27    10/08/97 11:32a Peter
 * pre-computed packet headers for packet 3
 * 
 * 26    9/20/97 4:42p Peter
 * tri_setf fixup/big fifo
 * 
 * 25    9/16/97 2:49p Peter
 * fixed watcom unhappiness w/ static initializers
 * 
 * 24    9/15/97 7:31p Peter
 * more cmdfifo cleanup, fixed normal buffer clear, banner in the right
 * place, lfb's are on, Hmmmm.. probably more
 * 
 * 23    9/10/97 10:13p Peter
 * fifo logic from GaryT, non-normalized fp first cut
 * 
 * 22    8/30/97 5:58p Tarolli
 * cleanups
 * 
 * 21    7/29/97 4:31p Atai
 * replace old edge sense routine
 * 
 * 19    7/28/97 2:42p Peter
 * aa points?
 * 
 * 18    7/26/97 3:04p Peter
 * cleanup
 * 
 * 17    7/25/97 11:40a Peter
 * removed dHalf, change field name to match real use for cvg
 * 
 * 16    6/30/97 3:21p Peter
 * more aa through cmd fifo
 * 
 * 15    6/20/97 1:17p Peter
 * changes for new CVG_TRI macros
**
*/

#if SET_BSWAP
#define SLOW_SETF 1
#endif

#include <3dfx.h>
#define FX_DLL_DEFINITION
#include <fxdll.h>
#include <glide.h>

#include "fxglide.h"
#include "fxcmd.h"

#if macintosh && defined(__MWERKS__)
#pragma auto_inline off
#endif

static void aaDrawArrayEdgeSense(float *a, float *b, float *c);

/*
** -----------------------------------------------------------------------
**
** Antialiasing rendering functions
**
** -----------------------------------------------------------------------
*/

typedef enum {
  aaEdgeSenseTop,
  aaEdgeSenseLeft,
  aaEdgeSenseBottom,
  aaEdgeSenseRight
} aaEdgeSense;

/*-------------------------------------------------------------------
  Function: aaVpDrawArrayEdgeSense
  Date: 11-Dec-97
  Implementor(s): atai
  Description:
  Pass the line a, b. Use c to calculate edge sense. (clip space)
  Create new vertices from a, b.
  Arguments:
  
  Return:
  -------------------------------------------------------------------*/
static void
aaVpDrawArrayEdgeSense(float *a, float *b, float *c, float oowa, float oowb)
{
#define FN_NAME "aaVpDrawArrayEdgeSense"
  GR_DCL_GC;

#if GLIDE_HW_TRI_SETUP && GLIDE_PACKET3_TRI_SETUP
  float dx, dy, v2x, v2y;
  float *fa, *fb, v1a, v2a;
  float fax, fay, fbx, fby;
  FxU32 ia;
  aaEdgeSense sense;

  if (FARRAY(a, 4) >= FARRAY(b, 4)) {
    fa = b; fb = a;
  }
  else {
    fa = a; fb = b;
  }
  dx = FARRAY(fa, 0) - FARRAY(fb, 0);
  dy = FARRAY(fa, 4) - FARRAY(fb, 4);

  if (dx == 0.f) {
    sense = (FARRAY(c, 0) > FARRAY(fa, 0)) ? aaEdgeSenseLeft : aaEdgeSenseRight;
  }
  else if (dy == 0.0f) {
    sense = (FARRAY(c, 4) > FARRAY(fa, 4)) ? aaEdgeSenseBottom : aaEdgeSenseTop;
  }
  else {
    float dcax, dcay, dcbx, dcby, cp;
    dcax = FARRAY(fa, 0) - FARRAY(c, 0);
    dcay = FARRAY(fa, 4) - FARRAY(c, 4);
    dcbx = FARRAY(fb, 0) - FARRAY(c, 0);
    dcby = FARRAY(fb, 4) - FARRAY(c, 4);
    cp = dcax * dcby - dcay * dcbx;

    if (dx > 0.0f) {
      if (dx >= -dy)    /* X-major line */
        sense = (cp > 0.0f) ? aaEdgeSenseTop : aaEdgeSenseBottom;
      else              /* Y-major line */
        sense = (cp > 0.0f) ? aaEdgeSenseRight : aaEdgeSenseLeft;
    }
    else {
      if (dx <= dy)     /* X-major line */
        sense = (cp < 0.0f) ? aaEdgeSenseTop : aaEdgeSenseBottom;
      else              /* Y-major line */
        sense = (cp < 0.0f) ? aaEdgeSenseLeft : aaEdgeSenseRight;
    }
  }

  /* v1(a), v2(b), a, b */
  GR_SET_EXPECTED_SIZE(gc->state.vData.vSize << 2, 1);
  TRI_STRIP_BEGIN(kSetupStrip, 4, gc->state.vData.vSize, SSTCP_PKT3_BDDDDD);
  fax = FARRAY(a, 0)*oowa*gc->state.Viewport.hwidth+gc->state.Viewport.ox;
  fay = FARRAY(a, 4)*oowa*gc->state.Viewport.hheight+gc->state.Viewport.oy;
  fbx = FARRAY(b, 0)*oowb*gc->state.Viewport.hwidth+gc->state.Viewport.ox;
  fby = FARRAY(b, 4)*oowb*gc->state.Viewport.hheight+gc->state.Viewport.oy;

  switch (sense) {
  case aaEdgeSenseTop:
    TRI_SETF(fax);
    v2x = fbx;
    TRI_SETF(fay + _GlideRoot.pool.f1);
    v2y = fby + _GlideRoot.pool.f1;
    break;

  case aaEdgeSenseLeft:
    TRI_SETF(fax - _GlideRoot.pool.f1);
    v2x = fbx - _GlideRoot.pool.f1;
    TRI_SETF(fay);
    v2y = fby;
    break;

  case aaEdgeSenseBottom:
    TRI_SETF(fax);
    v2x = fbx;
    TRI_SETF(fay - _GlideRoot.pool.f1);
    v2y = fby - _GlideRoot.pool.f1;
    break;

  case aaEdgeSenseRight:
    TRI_SETF(fax + _GlideRoot.pool.f1);
    v2x = fbx + _GlideRoot.pool.f1;
    TRI_SETF(fay);
    v2y = fby;
    break;
  }

  if (gc->state.vData.colorType == GR_FLOAT) {
    ia = gc->state.vData.aInfo.offset;
    v1a = v2a = 0.f;
  }
  else {
    ia = gc->state.vData.pargbInfo.offset;
    *((FxU32 *)&v1a)=*((FxU32 *)((int)a + ia))&0x00ffffff;
    *((FxU32 *)&v2a)=*((FxU32 *)((int)b + ia))&0x00ffffff;
  }

  {

    AA_TRI_VP_SETFS(a,oowa);

    TRI_SETF(v2x);
    TRI_SETF(v2y);

    AA_TRI_VP_SETFS(b,oowb);

    TRI_SETF(fax);
    TRI_SETF(fay);
    TRI_VP_SETFS(a,oowa);

    TRI_SETF(fbx);
    TRI_SETF(fby);
    TRI_VP_SETFS(b,oowb);

  }
  gc->stats.othertrisDrawn+=2;
  TRI_END;
  GR_CHECK_SIZE();
#endif
#undef FN_NAME
}

/*---------------------------------------------------------------------------
** grAADrawTriangle
**
**  NOTE:  This doesn't quite work yet
*/
GR_ENTRY(grAADrawTriangle,
         void,
         (const void *a, const void *b, const void *c,
          FxBool ab_antialias, FxBool bc_antialias, FxBool ca_antialias)) 
{
#define FN_NAME "grAADrawTriangle"
  FxU32
    fbzMode,                    /* What we write to fbzMode */
    fbzModeOld;                 /* Squirrel away current fbzMode */

  GR_BEGIN_NOFIFOCHECK(FN_NAME, 96);
  GDBG_INFO_MORE(gc->myLevel,"(0x%x,0x%x,0x%x,%d,%d,%d)\n",
                 a,b,c,ab_antialias,bc_antialias,ca_antialias);
  
  GR_FLUSH_STATE();

  fbzModeOld = gc->state.shadow.fbzMode;
  
  {
    /*
    ** culling
    */
    float dxAB, dxBC, dyAB, dyBC;
    FxI32 j;
    dxAB = FARRAY(a, 0) - FARRAY(b, 0);
    dxBC = FARRAY(b, 0) - FARRAY(c, 0);
    
    dyAB = FARRAY(a, 4) - FARRAY(b, 4);
    dyBC = FARRAY(b, 4) - FARRAY(c, 4);
    
    *(float *)&j = dxAB * dyBC - dxBC * dyAB;
    if ((j & 0x7FFFFFFF) == 0) 
      return;
    if ((gc->state.cull_mode != GR_CULL_DISABLE) && (((FxI32)(j ^ (gc->state.cull_mode << 31UL))) >= 0))
      return;
  }
#if __POWERPC__
        {
                const void *verts[3];
                verts[0] = a;
                verts[1] = b;
                verts[2] = c;
                (*gc->archDispatchProcs.drawTrianglesProc)(GR_VTX_PTR_ARRAY, 3, verts);
        }
#else
  grDrawTriangle(a, b, c);
#endif
  /* Disable depth buffer writes for edge triangles */
  fbzMode = fbzModeOld;
  fbzMode &= ~(SST_ZAWRMASK);

  GR_SET_EXPECTED_SIZE(4 * 2, 1 * 2);
  /* speckle mini-workaround, another one below... - jeske */
  GR_SET(BROADCAST_ID, hw, nopCMD, 0); 
  GR_SET(BROADCAST_ID, hw, fbzMode, fbzMode);
  GR_CHECK_SIZE();

  if (gc->state.grCoordinateSpaceArgs.coordinate_space_mode == GR_WINDOW_COORDS) {
    if (ab_antialias)
      aaDrawArrayEdgeSense((float *)a, (float *)b, (float *)c);
    
    if (bc_antialias)
      aaDrawArrayEdgeSense((float *)b, (float *)c, (float *)a);
    
    if (ca_antialias)
      aaDrawArrayEdgeSense((float *)c, (float *)a, (float *)b);
  }
  else {
    float oowa, oowb, oowc;
    if (ab_antialias) {
      oowa = 1.0f / FARRAY(a, gc->state.vData.wInfo.offset);        
      oowb = 1.0f / FARRAY(b, gc->state.vData.wInfo.offset);        
      aaVpDrawArrayEdgeSense((float *)a, (float *)b, (float *)c, oowa, oowb);
    }
      
    if (bc_antialias) {
      if (!ab_antialias)
        oowb = 1.0f / FARRAY(b, gc->state.vData.wInfo.offset);        
      oowc = 1.0f / FARRAY(c, gc->state.vData.wInfo.offset);
      aaVpDrawArrayEdgeSense((float *)b, (float *)c, (float *)a, oowb, oowc);
    }
      
    if (ca_antialias) {
      if (!ab_antialias)
        oowa = 1.0f / FARRAY(a, gc->state.vData.wInfo.offset);        
      if (!bc_antialias)
        oowc = 1.0f / FARRAY(c, gc->state.vData.wInfo.offset);
      aaVpDrawArrayEdgeSense((float *)c, (float *)a, (float *)b, oowc, oowa);
    }
  }

  /* Restore the old fbzMode */
  GR_SET_EXPECTED_SIZE(4 * 2, 1 * 2);
  /* speckle mini-workaround, another one below... - jeske */
  GR_SET(BROADCAST_ID, hw, nopCMD, 0); 
  GR_SET(BROADCAST_ID, hw, fbzMode, fbzModeOld);
  GR_END();
#if 0
  FxU32
    fbzMode,                    /* What we write to fbzMode */
    fbzModeOld;                 /* Squirrel away current fbzMode */

  GR_BEGIN_NOFIFOCHECK(FN_NAME, 96);
  GDBG_INFO_MORE(gc->myLevel,"(0x%x,0x%x,0x%x,%d,%d,%d)\n",
                 a,b,c,ab_antialias,bc_antialias,ca_antialias);
  
  GR_FLUSH_STATE();

  fbzModeOld = gc->state.shadow.fbzMode;
  
  /* backfaced or zero area */
  if (TRISETUP(a, b, c ) <= 0) return;
   
  /* Disable depth buffer writes for edge triangles */
  fbzMode = fbzModeOld;
  fbzMode &= ~(SST_ZAWRMASK);

  GR_SET_EXPECTED_SIZE(4, 1);
  GR_SET(BROADCAST_ID, hw, fbzMode, fbzMode);
  GR_CHECK_SIZE();

  if (ab_antialias)
    aaDrawArrayEdgeSense((float *)a, (float *)b, (float *)c);
  
  if (bc_antialias)
    aaDrawArrayEdgeSense((float *)b, (float *)c, (float *)a);
  
  if (ca_antialias)
    aaDrawArrayEdgeSense((float *)c, (float *)a, (float *)b);

  /* Restore the old fbzMode */
  GR_SET_EXPECTED_SIZE(4, 1);
  GR_SET(BROADCAST_ID, hw, fbzMode, fbzModeOld);
  GR_END();
#endif
#undef FN_NAME
} /* grAADrawTriangle */

#define OK_TO_SKIP_XY
#define ZEROALPHA(_ss)   *((FxU32 *)&_ss)=*((FxU32 *)&_ss)&0x00ffffff

/*-------------------------------------------------------------------
  Function: _grAADrawPoints
  Date: 13-Oct-97
  Implementor(s): atai
  Description:
  Draw anti-alias points
  Arguments:
  
  Return:
  -------------------------------------------------------------------*/
void FX_CSTYLE
_grAADrawPoints(FxI32 mode, FxI32 count, void *pointers)
{
#define FN_NAME "_grAADrawPoints"

  /*
   * the aa point routine is similar to grAADrawPoint routine
   * except the data set up is from the pointer array and 
   * its data layout
   */
#if GLIDE_HW_TRI_SETUP && GLIDE_PACKET3_TRI_SETUP
  GR_DCL_GC;
  float *e, ptX, ptY;
  FxU32 ia;
  FxU32 i;
  FxU32 vsize;
  FxI32 stride = mode;
  FxU32 tmp_cullStripHdr;

  GDBG_INFO(94,"_grAADrawPoints(0x%x, %d, 0x%x)\n", mode, count, pointers);

  GDBG_INFO_MORE(gc->myLevel, "(count = %d, pointers = 0x%x)\n",
                 count, pointers);

  if (stride == 0)
    stride = gc->state.vData.vStride;

  GR_FLUSH_STATE();

  if (gc->state.vData.colorType == GR_FLOAT)
    ia = gc->state.vData.aInfo.offset;
  else
    ia = gc->state.vData.pargbInfo.offset;

  vsize = gc->state.vData.vSize * 6;

  /* turn off culling so triangles unconditionally draw */
  tmp_cullStripHdr = gc->cmdTransportInfo.cullStripHdr;
  gc->cmdTransportInfo.cullStripHdr &= GR_CULL_MASK;

  if (gc->state.grCoordinateSpaceArgs.coordinate_space_mode == GR_WINDOW_COORDS) {
    while (count--) {
      FxU32 dataElem = 0;

      /* We draw this as a 4 triangle fan centered around E. */
      GR_SET_EXPECTED_SIZE(vsize, 1);
      TRI_STRIP_BEGIN(kSetupFan, 
                      0x06UL, gc->state.vData.vSize, 
                      SSTCP_PKT3_BDDDDD);

      e = pointers;
      if (mode)
        e = *(float **)e;
      (float *)pointers += stride;
      
      ptX = FARRAY(e, gc->state.vData.vertexInfo.offset);
      ptY = FARRAY(e, gc->state.vData.vertexInfo.offset+4);

      /* Send down the original point center */
      TRI_SETF(ptX);
      TRI_SETF(ptY);

      dataElem = 0;
      i = gc->tsuDataList[dataElem];
      while (i != GR_DLIST_END) {
        TRI_SETF(FARRAY(e, i));
        dataElem++;
        i = gc->tsuDataList[dataElem];
      }
      
      /* Send the 'surrounding' vertices w/ alpha == 0.0f.  To avoid
       * the vertex copying from the previous version we carry around
       * a table of offsets that we add into the original point to get
       * the new points.
       */
      {
        int v;
        static const float xAdjust[] = {  1.0f, -1.0f, -1.0f, 1.0f,  1.0f };
        static const float yAdjust[] = { -1.0f, -1.0f,  1.0f, 1.0f, -1.0f };
      
        ASSERT(ArraySize(xAdjust) == ArraySize(yAdjust));
        
        if (gc->state.vData.colorType == GR_FLOAT) {
          
          for(v = 0; v < ArraySize(xAdjust); v++) {            
            TRI_SETF(ptX + xAdjust[v]);
            TRI_SETF(ptY + yAdjust[v]);
            
            dataElem = 0;
            
            i = gc->tsuDataList[dataElem];
            
            while (i != GR_DLIST_END) {
              if (i == ia) {
                TRI_SETF(0.0f);
              }
              else {
                TRI_SETF(FARRAY(e, i));
              }
              dataElem++;
              i = gc->tsuDataList[dataElem];
            }
          }
        }
        else {
          for(v = 0; v < ArraySize(xAdjust); v++) {            
            TRI_SETF(ptX + xAdjust[v]);
            TRI_SETF(ptY + yAdjust[v]);
            
            dataElem = 0;
            
            i = gc->tsuDataList[dataElem];
            
            while (i != GR_DLIST_END) {
              FxU32 argb;
              
              if (i == ia) {
                argb = *((FxU32 *)((int)e + i)) & 0x00ffffff;
                TRI_SETF(*((float *)&argb));
              }
              else {
                TRI_SETF(FARRAY(e, i));
              }
              dataElem++;
              i = gc->tsuDataList[dataElem];
            }
          }
        }
        
      }
      TRI_END;
      GR_CHECK_SIZE();
    }
  }
  else {
    /*
     * first cut of clip space coordinate code.
     */
    
    float oow;

    while (count--) {
      FxU32 dataElem = 0;

      /* We draw this as a 4 triangle fan centered around E. */
      GR_SET_EXPECTED_SIZE(vsize, 1);
      TRI_STRIP_BEGIN(kSetupFan, 
                      0x06UL, gc->state.vData.vSize, 
                      SSTCP_PKT3_BDDDDD);

      e = pointers;
      if (mode)
        e = *(float **)e;
      oow = 1.0f / FARRAY(e, gc->state.vData.wInfo.offset);        
      (float *)pointers += stride;
      
      ptX = FARRAY(e, gc->state.vData.vertexInfo.offset)
        *oow*gc->state.Viewport.hwidth+gc->state.Viewport.ox;
      ptY = FARRAY(e, gc->state.vData.vertexInfo.offset+4)
        *oow*gc->state.Viewport.hheight+gc->state.Viewport.oy;

      /* Send down the original point center */
      TRI_SETF(ptX);
      TRI_SETF(ptY);

      TRI_VP_SETFS(e,oow);
      
      /* Send the 'surrounding' vertices w/ alpha == 0.0f.  To avoid
       * the vertex copying from the previous version we carry around
       * a table of offsets that we add into the original point to get
       * the new points.
       */
      {
        int v;
        static const float xAdjust[] = {  1.0f, -1.0f, -1.0f, 1.0f,  1.0f };
        static const float yAdjust[] = { -1.0f, -1.0f,  1.0f, 1.0f, -1.0f };
      
        ASSERT(ArraySize(xAdjust) == ArraySize(yAdjust));
        
        if (gc->state.vData.colorType == GR_FLOAT) {
          
          for(v = 0; v < ArraySize(xAdjust); v++) {            
            TRI_SETF(ptX + xAdjust[v]);
            TRI_SETF(ptY + yAdjust[v]);
            AA_TRI_VP_SETFS(e, oow);
          }
        }
        else {
          for(v = 0; v < ArraySize(xAdjust); v++) {            
            TRI_SETF(ptX + xAdjust[v]);
            TRI_SETF(ptY + yAdjust[v]);
            AA_TRI_VP_SETFS(e, oow);
          }
        }
        
      }
      TRI_END;
      GR_CHECK_SIZE();
    }
  }

  gc->cmdTransportInfo.cullStripHdr = tmp_cullStripHdr;
  gc->stats.pointsDrawn+=count;
  gc->stats.othertrisDrawn+=(count<<2);

#endif

#undef FN_NAME
} /* _grAADrawPoints */

#if macintosh && defined(__MWERKS__) && !DEBUG
#pragma optimization_level                              1
#endif

/*-------------------------------------------------------------------
  Function: _grAADrawLineStrip
  Date: 13-Oct-97
  Implementor(s): atai
  Description:
  Draw anti-alias strip line
  Arguments:
  
  Return:
  -------------------------------------------------------------------*/
void FX_CSTYLE
_grAADrawLineStrip(FxI32 mode, FxI32 ltype, FxI32 count, void *pointers)
{
#define FN_NAME "_grAADrawLineStrip"
#if GLIDE_HW_TRI_SETUP && GLIDE_PACKET3_TRI_SETUP
  /*
   * the aa line strip routine is similar to grAADrawLine routine
   * except the data set up is from the pointer array and 
   * its data layout
   */
  GR_DCL_GC;
  float           adx, ady;         /* |dX| and |dY| */
  float **lPtrs = (float **) pointers;
  float *v1, *v2;
  float v1a, v2a;
  FxU32 ia, vNum = 0;
  FxU32 vsize;
  FxU32 sCount;
  FxI32 stride = mode;
  FxU32 tmp_cullStripHdr;

  GDBG_INFO(95,"_grAADrawLineStrip(count = %d, pointers = 0x%x)\n",
            count, pointers);

  if (gc->state.vData.colorType == GR_FLOAT) {
    ia = gc->state.vData.aInfo.offset;
    v1a = 0.0f;
    v2a = 0.0f;
  }
  else
    ia = gc->state.vData.pargbInfo.offset;

  GR_FLUSH_STATE();

  if (stride == 0)
    stride = gc->state.vData.vStride;
  if (ltype == GR_LINES)
    sCount = count >> 1; /* line list */
  else
    sCount = count-1;    /* strip line */

  vsize = 6 * gc->state.vData.vSize;

  tmp_cullStripHdr = gc->cmdTransportInfo.cullStripHdr;
  gc->cmdTransportInfo.cullStripHdr &= GR_CULL_MASK;

  if (gc->state.grCoordinateSpaceArgs.coordinate_space_mode == GR_WINDOW_COORDS) {
    while (sCount--) {
      v1 = (float *)pointers;
      v2 = (float *)pointers + stride;
      if (mode) {
        v1 = *(float **)v1;
        v2 = *(float **)v2;
      }
      (float *)pointers += stride;
      if (ltype == GR_LINES)
        (float *)pointers += stride;
      
      /* draw from low Y to high Y */
      if (FARRAY(v2, gc->state.vData.vertexInfo.offset+4) < FARRAY(v1, gc->state.vData.vertexInfo.offset+4)) {
        float* tv = v1; v1 = v2; v2 = tv;
      }
      
      /* compute deltas and absolute deltas */
      adx = FARRAY(v2, gc->state.vData.vertexInfo.offset) - FARRAY(v1, gc->state.vData.vertexInfo.offset);
      ady = FARRAY(v2, gc->state.vData.vertexInfo.offset+4) - FARRAY(v1, gc->state.vData.vertexInfo.offset+4);
      if (adx < 0)
        adx = -adx;
      if (ady < 0)
        ady = -ady;
      
      if (gc->state.vData.colorType != GR_FLOAT) {
        *((FxU32 *)&v1a)=*((FxU32 *)((int)v1 + ia))&0x00ffffff;
        *((FxU32 *)&v2a)=*((FxU32 *)((int)v2 + ia))&0x00ffffff;
      }
      
      if (adx >= ady) {           /* X major line */
        FxU32 i, dataElem;
        
        GR_SET_EXPECTED_SIZE(vsize, 1);
        TRI_STRIP_BEGIN(kSetupStrip, 6, gc->state.vData.vSize, SSTCP_PKT3_BDDDDD);

        /* point 1 (b) = (v2)*/
        TRI_SETF(FARRAY(v2, gc->state.vData.vertexInfo.offset));
        TRI_SETF(FARRAY(v2, gc->state.vData.vertexInfo.offset+4) - _GlideRoot.pool.f1);
        dataElem = 0;
        i = gc->tsuDataList[dataElem];
        while (i != GR_DLIST_END) { 
          if (i == ia) { TRI_SETF(v2a); } else { TRI_SETF(FARRAY(v2, i)); } 
          dataElem++; i = gc->tsuDataList[dataElem];
        }
        
        /* point 2 (a) = (v1)*/
        TRI_SETF(FARRAY(v1, gc->state.vData.vertexInfo.offset));
        TRI_SETF(FARRAY(v1, gc->state.vData.vertexInfo.offset+4) - _GlideRoot.pool.f1);
        dataElem = 0;
        i = gc->tsuDataList[dataElem];
        while (i != GR_DLIST_END) { 
          if (i == ia) { TRI_SETF(v1a); } else { TRI_SETF(FARRAY(v1, i)); } 
          dataElem++; i = gc->tsuDataList[dataElem];
        }
        
        /* point 3 (v2) = (v2)*/
        TRI_SETF(FARRAY(v2, gc->state.vData.vertexInfo.offset));
        TRI_SETF(FARRAY(v2, gc->state.vData.vertexInfo.offset+4));
        dataElem = 0;
        i = gc->tsuDataList[dataElem];
        while (i != GR_DLIST_END) { 
          TRI_SETF(FARRAY(v2, i));
          dataElem++; i = gc->tsuDataList[dataElem];
        }
        
        /* point 4 (v1) = (v1)*/
        TRI_SETF(FARRAY(v1, gc->state.vData.vertexInfo.offset));
        TRI_SETF(FARRAY(v1, gc->state.vData.vertexInfo.offset+4));
        dataElem = 0;
        i = gc->tsuDataList[dataElem];
        while (i != GR_DLIST_END) { 
          TRI_SETF(FARRAY(v1, i));
          dataElem++; i = gc->tsuDataList[dataElem];
        }
        
        /* point 5 (f) = (v2)*/
        TRI_SETF(FARRAY(v2, gc->state.vData.vertexInfo.offset));
        TRI_SETF(FARRAY(v2, gc->state.vData.vertexInfo.offset+4) + _GlideRoot.pool.f1);
        dataElem = 0;
        i = gc->tsuDataList[dataElem];
        while (i != GR_DLIST_END) { 
          if (i == ia) { TRI_SETF(v2a); } else { TRI_SETF(FARRAY(v2, i)); } 
          dataElem++; i = gc->tsuDataList[dataElem];
        }
        
        /* point 6 (a) = (v1)*/
        TRI_SETF(FARRAY(v1, gc->state.vData.vertexInfo.offset));
        TRI_SETF(FARRAY(v1, gc->state.vData.vertexInfo.offset+4) + _GlideRoot.pool.f1);
        dataElem = 0;
        i = gc->tsuDataList[dataElem];
        while (i != GR_DLIST_END) { 
          if (i == ia) { TRI_SETF(v1a); } else { TRI_SETF(FARRAY(v1, i)); } 
          dataElem++; i = gc->tsuDataList[dataElem];
        }
        
        TRI_END;
        GR_CHECK_SIZE();
      } else {                      /* Y major line */
        FxU32 i, dataElem;
        
        GR_SET_EXPECTED_SIZE(vsize, 1);
        TRI_STRIP_BEGIN(kSetupStrip, 6, gc->state.vData.vSize, SSTCP_PKT3_BDDDDD);

        /* point 1 (b) = (v2)*/
        TRI_SETF(FARRAY(v2, gc->state.vData.vertexInfo.offset) + _GlideRoot.pool.f1);
        TRI_SETF(FARRAY(v2, gc->state.vData.vertexInfo.offset+4));
        dataElem = 0;
        i = gc->tsuDataList[dataElem];
        while (i != GR_DLIST_END) { 
          if (i == ia) { TRI_SETF(v2a); } else { TRI_SETF(FARRAY(v2, i)); } 
          dataElem++; i = gc->tsuDataList[dataElem];
        }
        
        /* point 2 (a) = (v1)*/
        TRI_SETF(FARRAY(v1, gc->state.vData.vertexInfo.offset) + _GlideRoot.pool.f1);
        TRI_SETF(FARRAY(v1, gc->state.vData.vertexInfo.offset+4));
        dataElem = 0;
        i = gc->tsuDataList[dataElem];
        while (i != GR_DLIST_END) { 
          if (i == ia) { TRI_SETF(v1a); } else { TRI_SETF(FARRAY(v1, i)); } 
          dataElem++; i = gc->tsuDataList[dataElem];
        }
        
        /* point 3 (v2) = (v2)*/
        TRI_SETF(FARRAY(v2, gc->state.vData.vertexInfo.offset));
        TRI_SETF(FARRAY(v2, gc->state.vData.vertexInfo.offset+4));
        dataElem = 0;
        i = gc->tsuDataList[dataElem];
        while (i != GR_DLIST_END) { 
          TRI_SETF(FARRAY(v2, i));
          dataElem++; i = gc->tsuDataList[dataElem];
        }
        
        /* point 4 (v1) = (v1)*/
        TRI_SETF(FARRAY(v1, gc->state.vData.vertexInfo.offset));
        TRI_SETF(FARRAY(v1, gc->state.vData.vertexInfo.offset+4));
        dataElem = 0;
        i = gc->tsuDataList[dataElem];
        while (i != GR_DLIST_END) { 
          TRI_SETF(FARRAY(v1, i));
          dataElem++; i = gc->tsuDataList[dataElem];
        }

        /* point 5 (f) = (v2)*/
        TRI_SETF(FARRAY(v2, gc->state.vData.vertexInfo.offset) - _GlideRoot.pool.f1);
        TRI_SETF(FARRAY(v2, gc->state.vData.vertexInfo.offset+4));
        dataElem = 0;
        i = gc->tsuDataList[dataElem];
        while (i != GR_DLIST_END) { 
          if (i == ia) { TRI_SETF(v2a); } else { TRI_SETF(FARRAY(v2, i)); } 
          dataElem++; i = gc->tsuDataList[dataElem];
        }
        
        /* point 6 (a) = (v1)*/
        TRI_SETF(FARRAY(v1, gc->state.vData.vertexInfo.offset) - _GlideRoot.pool.f1);
        TRI_SETF(FARRAY(v1, gc->state.vData.vertexInfo.offset+4));
        dataElem = 0;
        i = gc->tsuDataList[dataElem];
        while (i != GR_DLIST_END) { 
          if (i == ia) { TRI_SETF(v1a); } else { TRI_SETF(FARRAY(v1, i)); } 
          dataElem++; i = gc->tsuDataList[dataElem];
        }
        
        TRI_END;
        GR_CHECK_SIZE();
      }
      gc->stats.linesDrawn++;
      gc->stats.othertrisDrawn+=4;
    }
  }
  else {
    float oowa, oowb, owa, owb, tmp1, tmp2, fax, fay, fbx, fby;

    if (ltype == GR_LINE_STRIP) {
      v1 = (float *)pointers;
      if (mode) {
        v1 = *(float **)v1;
      }
      oowb = 1.0f / FARRAY(v1, gc->state.vData.wInfo.offset);        
    }
    while (sCount--) {
      if (ltype == GR_LINES) {
        v1 = (float *)pointers;
        v2 = (float *)pointers + stride;
        if (mode) {
          v1 = *(float **)v1;
          v2 = *(float **)v2;
        }
        (float *)pointers += stride;
        if (ltype == GR_LINES)
          (float *)pointers += stride;
        owa = oowa = 1.0f / FARRAY(v1, gc->state.vData.wInfo.offset);
        owb = oowb = 1.0f / FARRAY(v2, gc->state.vData.wInfo.offset);
      }
      else {
        owa = oowa = oowb;
        v1 = (float *)pointers;
        v2 = (float *)pointers + stride;
        if (mode) {
          v1 = *(float **)v1;
          v2 = *(float **)v2;
        }
        (float *)pointers += stride;
        owb = oowb = 1.0f / FARRAY(v2, gc->state.vData.wInfo.offset);
      }
      
      fay = tmp1 = FARRAY(v1, gc->state.vData.vertexInfo.offset+4)
        *oowa*gc->state.Viewport.hheight+gc->state.Viewport.oy;
      fby = tmp2 = FARRAY(v2, gc->state.vData.vertexInfo.offset+4)
        *oowb*gc->state.Viewport.hheight+gc->state.Viewport.oy;        

      /* draw from low Y to high Y */
      if (tmp2 < tmp1) {
        float* tv = v1; v1 = v2; v2 = tv;
        owa = oowb; owb = oowa;
        fay = tmp2;
        fby = tmp1;     
      }
      fax = FARRAY(v1, gc->state.vData.vertexInfo.offset)
        *owa*gc->state.Viewport.hwidth+gc->state.Viewport.ox;
      fbx = FARRAY(v2, gc->state.vData.vertexInfo.offset)
        *owb*gc->state.Viewport.hwidth+gc->state.Viewport.ox;
        
      /* compute deltas and absolute deltas */
      adx = fbx - fax;
      ady = fby - fay;
      if (adx < 0)
        adx = -adx;
      if (ady < 0)
        ady = -ady;
      /*
      if (gc->state.vData.colorType != GR_FLOAT) {
        *((FxU32 *)&v1a)=*((FxU32 *)((int)v1 + ia))&0x00ffffff;
        *((FxU32 *)&v2a)=*((FxU32 *)((int)v2 + ia))&0x00ffffff;
      }
      */
      
      if (adx >= ady) {           /* X major line */
        
        GR_SET_EXPECTED_SIZE(vsize, 1);
        TRI_STRIP_BEGIN(kSetupStrip, 6, gc->state.vData.vSize, SSTCP_PKT3_BDDDDD);

        /* point 1 (b) = (v2)*/
        TRI_SETF(fbx);
        TRI_SETF(fby - _GlideRoot.pool.f1);
        AA_TRI_VP_SETFS(v2, owb);
        
        /* point 2 (a) = (v1)*/
        TRI_SETF(fax);
        TRI_SETF(fay - _GlideRoot.pool.f1);
        AA_TRI_VP_SETFS(v1, owa);
        
        /* point 3 (v2) = (v2)*/
        TRI_SETF(fbx);
        TRI_SETF(fby);
        TRI_VP_SETFS(v2, owb);
        
        /* point 4 (v1) = (v1)*/
        TRI_SETF(fax);
        TRI_SETF(fay);
        TRI_VP_SETFS(v1, owa);

        /* point 5 (f) = (v2)*/
        TRI_SETF(fbx);
        TRI_SETF(fby + _GlideRoot.pool.f1);
        AA_TRI_VP_SETFS(v2, owb);

        /* point 6 (a) = (v1)*/
        TRI_SETF(fax);
        TRI_SETF(fay + _GlideRoot.pool.f1);
        AA_TRI_VP_SETFS(v1, owa);
        
        TRI_END;
        GR_CHECK_SIZE();
      } else {                      /* Y major line */
        
        GR_SET_EXPECTED_SIZE(vsize, 1);
        TRI_STRIP_BEGIN(kSetupStrip, 6, gc->state.vData.vSize, SSTCP_PKT3_BDDDDD);

        /* point 1 (b) = (v2)*/
        TRI_SETF(fbx + _GlideRoot.pool.f1);
        TRI_SETF(fby);
        AA_TRI_VP_SETFS(v2, owb);
        
        /* point 2 (a) = (v1)*/
        TRI_SETF(fax + _GlideRoot.pool.f1);
        TRI_SETF(fay);
        AA_TRI_VP_SETFS(v1, owa);
        
        /* point 3 (v2) = (v2)*/
        TRI_SETF(fbx);
        TRI_SETF(fby);
        TRI_VP_SETFS(v2, owb);
        
        /* point 4 (v1) = (v1)*/
        TRI_SETF(fax);
        TRI_SETF(fay);
        TRI_VP_SETFS(v1, owa);

        /* point 5 (f) = (v2)*/
        TRI_SETF(fbx - _GlideRoot.pool.f1);
        TRI_SETF(fby);
        AA_TRI_VP_SETFS(v2, owb);

        /* point 6 (a) = (v1)*/
        TRI_SETF(fax - _GlideRoot.pool.f1);
        TRI_SETF(fay);
        AA_TRI_VP_SETFS(v1, owa);

        TRI_END;
        GR_CHECK_SIZE();
      }
      gc->stats.linesDrawn++;
      gc->stats.othertrisDrawn+=4;
    }
  }

  gc->cmdTransportInfo.cullStripHdr = tmp_cullStripHdr;

#endif
#undef FN_NAME
} /* _grAADrawLineStrip */

#if macintosh && defined(__MWERKS__) && !DEBUG
#pragma optimization_level                              4
#endif

/*-------------------------------------------------------------------
  Function: aaDrawArrayEdgeSense
  Date: 13-Oct-97
  Implementor(s): atai
  Description:
  Pass the line a, b. Use c to calculate edge sense.
  Create new vertices from a, b.
  Arguments:
  
  Return:
  -------------------------------------------------------------------*/
static void
aaDrawArrayEdgeSense(float *a, float *b, float *c)
{
#define FN_NAME "aaDrawArrayEdgeSense"
  GR_DCL_GC;

#if GLIDE_HW_TRI_SETUP && GLIDE_PACKET3_TRI_SETUP
  float dx, dy, v2x, v2y;
  float *fa, *fb, v1a, v2a;
  FxU32 ia;
  aaEdgeSense sense;

  if (FARRAY(a, gc->state.vData.vertexInfo.offset+4) >= FARRAY(b, gc->state.vData.vertexInfo.offset+4)) {
    fa = b; fb = a;
  }
  else {
    fa = a; fb = b;
  }
  dx = FARRAY(fa, gc->state.vData.vertexInfo.offset) - FARRAY(fb, gc->state.vData.vertexInfo.offset);
  dy = FARRAY(fa, gc->state.vData.vertexInfo.offset+4) - FARRAY(fb, gc->state.vData.vertexInfo.offset+4);

  if (dx == 0.f) {
    sense = (FARRAY(c, gc->state.vData.vertexInfo.offset) > FARRAY(fa, gc->state.vData.vertexInfo.offset)) ? aaEdgeSenseLeft : aaEdgeSenseRight;
  }
  else if (dy == 0.0f) {
    sense = (FARRAY(c, gc->state.vData.vertexInfo.offset+4) > FARRAY(fa, gc->state.vData.vertexInfo.offset+4)) ? aaEdgeSenseBottom : aaEdgeSenseTop;
  }
  else {
    float dcax, dcay, dcbx, dcby, cp;
    dcax = FARRAY(fa, gc->state.vData.vertexInfo.offset) - FARRAY(c, gc->state.vData.vertexInfo.offset);
    dcay = FARRAY(fa, gc->state.vData.vertexInfo.offset+4) - FARRAY(c, gc->state.vData.vertexInfo.offset+4);
    dcbx = FARRAY(fb, gc->state.vData.vertexInfo.offset) - FARRAY(c, gc->state.vData.vertexInfo.offset);
    dcby = FARRAY(fb, gc->state.vData.vertexInfo.offset+4) - FARRAY(c, gc->state.vData.vertexInfo.offset+4);
    cp = dcax * dcby - dcay * dcbx;

    if (dx > 0.0f) {
      if (dx >= -dy)    /* X-major line */
        sense = (cp > 0.0f) ? aaEdgeSenseTop : aaEdgeSenseBottom;
      else              /* Y-major line */
        sense = (cp > 0.0f) ? aaEdgeSenseRight : aaEdgeSenseLeft;
    }
    else {
      if (dx <= dy)     /* X-major line */
        sense = (cp < 0.0f) ? aaEdgeSenseTop : aaEdgeSenseBottom;
      else              /* Y-major line */
        sense = (cp < 0.0f) ? aaEdgeSenseLeft : aaEdgeSenseRight;
    }
  }

  /* v1(a), v2(b), a, b */
  GR_SET_EXPECTED_SIZE(gc->state.vData.vSize << 2, 1);
  TRI_STRIP_BEGIN(kSetupStrip, 4, gc->state.vData.vSize, SSTCP_PKT3_BDDDDD);
  switch (sense) {
  case aaEdgeSenseTop:
    TRI_SETF(FARRAY(a, gc->state.vData.vertexInfo.offset));
    v2x = FARRAY(b, gc->state.vData.vertexInfo.offset);
    TRI_SETF(FARRAY(a, gc->state.vData.vertexInfo.offset+4) + _GlideRoot.pool.f1);
    v2y = FARRAY(b, gc->state.vData.vertexInfo.offset+4) + _GlideRoot.pool.f1;
    break;

  case aaEdgeSenseLeft:
    TRI_SETF(FARRAY(a, gc->state.vData.vertexInfo.offset) - _GlideRoot.pool.f1);
    v2x = FARRAY(b, gc->state.vData.vertexInfo.offset) - _GlideRoot.pool.f1;
    TRI_SETF(FARRAY(a, gc->state.vData.vertexInfo.offset+4));
    v2y = FARRAY(b, gc->state.vData.vertexInfo.offset+4);
    break;

  case aaEdgeSenseBottom:
    TRI_SETF(FARRAY(a, gc->state.vData.vertexInfo.offset));
    v2x = FARRAY(b, gc->state.vData.vertexInfo.offset);
    TRI_SETF(FARRAY(a, gc->state.vData.vertexInfo.offset+4) - _GlideRoot.pool.f1);
    v2y = FARRAY(b, gc->state.vData.vertexInfo.offset+4) - _GlideRoot.pool.f1;
    break;

  case aaEdgeSenseRight:
    TRI_SETF(FARRAY(a, gc->state.vData.vertexInfo.offset) + _GlideRoot.pool.f1);
    v2x = FARRAY(b, gc->state.vData.vertexInfo.offset) + _GlideRoot.pool.f1;
    TRI_SETF(FARRAY(a, gc->state.vData.vertexInfo.offset+4));
    v2y = FARRAY(b, gc->state.vData.vertexInfo.offset+4);
    break;
  }

  if (gc->state.vData.colorType == GR_FLOAT) {
    ia = gc->state.vData.aInfo.offset;
    v1a = v2a = 0.f;
  }
  else {
    ia = gc->state.vData.pargbInfo.offset;
    *((FxU32 *)&v1a)=*((FxU32 *)((int)a + ia))&0x00ffffff;
    *((FxU32 *)&v2a)=*((FxU32 *)((int)b + ia))&0x00ffffff;
  }

  {
    FxU32 i, dataElem;
    dataElem = 0;
    i = gc->tsuDataList[dataElem];
    while (i != GR_DLIST_END) {
      if (i == ia) {
        TRI_SETF(v1a);
      }
      else {
        TRI_SETF(FARRAY(a, i));
      }
      dataElem++;
      i = gc->tsuDataList[dataElem];
    }
    TRI_SETF(v2x);
    dataElem = 0;
    TRI_SETF(v2y);
    i = gc->tsuDataList[dataElem];
    while (i != GR_DLIST_END) {
      if (i == ia) {
        TRI_SETF(v2a);
      }
      else {
        TRI_SETF(FARRAY(b, i));
      }
      dataElem++;
      i = gc->tsuDataList[dataElem];
    }
    dataElem = 0;
    TRI_SETF(FARRAY(a, gc->state.vData.vertexInfo.offset));
    TRI_SETF(FARRAY(a, gc->state.vData.vertexInfo.offset+4));
    i = gc->tsuDataList[dataElem];
    while (i != GR_DLIST_END) {
      TRI_SETF(FARRAY(a, i));
      dataElem++;
      i = gc->tsuDataList[dataElem];
    }
    dataElem = 0;
    TRI_SETF(FARRAY(b, gc->state.vData.vertexInfo.offset));
    TRI_SETF(FARRAY(b, gc->state.vData.vertexInfo.offset+4));
    i = gc->tsuDataList[dataElem];
    while (i != GR_DLIST_END) {
      TRI_SETF(FARRAY(b, i));
      dataElem++;
      i = gc->tsuDataList[dataElem];
    }
  }
  gc->stats.othertrisDrawn+=2;
  TRI_END;
  GR_CHECK_SIZE();

#endif

#undef FN_NAME
} /* aaCalcEdgeSense */

/*-------------------------------------------------------------------
  Function: _grAADrawTriangles
  Date: 13-Oct-97
  Implementor(s): atai
  Description:
  Draw anti-alias triangles
  Arguments:
  
  Return:
  -------------------------------------------------------------------*/
void FX_CSTYLE
_grAADrawTriangles(FxI32 mode, FxI32 ttype, FxI32 count, void *pointers)
{
#define FN_NAME "_grAADrawTriangles"

  GR_DCL_GC;

  float **lPtr = (float **)pointers;
  FxI32 tCount = 3;
  FxU32 fbzModeOld;                 /* Squirrel away current fbzMode */
  FxI32 stride = mode;
  FxI32 xindex = (gc->state.vData.vertexInfo.offset >> 2);
  FxI32 yindex = xindex + 1;

  GDBG_INFO(96, "_grAADrawTriangles (count = %d, pointers = 0x%x)\n",
            count, pointers);

  GR_FLUSH_STATE();

  if (ttype == GR_TRIANGLES)
    (*gc->archDispatchProcs.drawTrianglesProc)(mode, count, pointers);

  fbzModeOld = gc->state.shadow.fbzMode;
  gc->state.shadow.fbzMode &= ~(SST_ZAWRMASK);
  /* gc->state.invalid |= fbzModeBIT; */
  GR_FLUSH_STATE();

  if (stride == 0)
    stride = gc->state.vData.vStride;

  /* backfaced or zero area */
  while (tCount <= count) {
    float *a, *b, *c;

    GR_BEGIN_NOFIFOCHECK("_grAADrawTri",96);
      
    a = pointers;
    b = (float *)pointers + stride;
    c = b + stride;
    if (mode) {
      a = *(float **)a;
      b = *(float **)b;
      c = *(float **)c;
    }
    (float *)pointers += stride*3;

    /* move culling test to here */
    {
      float *fa = a;
      float *fb = b;
      float *fc = c;
      float dxAB, dxBC, dyAB, dyBC;
      /*
      **  Sort the vertices.
      **  Whenever the radial order is reversed (from counter-clockwise to
      **  clockwise), we need to change the area of the triangle.  Note
      **  that we know the first two elements are X & Y by looking at the
      **  grVertex structure.  
      */
      {
        int ay = *(int *)&a[(gc->state.vData.vertexInfo.offset>>2)+1];
        int by = *(int *)&b[(gc->state.vData.vertexInfo.offset>>2)+1];
        int cy = *(int *)&c[(gc->state.vData.vertexInfo.offset>>2)+1];
        int culltest = gc->state.cull_mode;
        
        if (ay < 0) ay ^= 0x7FFFFFFF;
        if (by < 0) by ^= 0x7FFFFFFF;
        if (cy < 0) cy ^= 0x7FFFFFFF;
        
        if (ay < by) {
          if (by > cy) {    /* acb */
            if (ay < cy) {
              fa = a;
              fb = c;
              fc = b;
              culltest ^= 1;
            } else {                /* cab */
              fa = c;
              fb = a;
              fc = b;
            }
            /* else it's already sorted */
          }
        } else {
          if (by < cy) {    /* bac */
            if (ay < cy) {
              fa = b;
              fb = a;
              fc = c;
              culltest ^= 1;
            } else {                /* bca */
              fa = b;
              fb = c;
              fc = a;
            }
          } else {          /* cba */
            fa = c;
            fb = b;
            fc = a;
            culltest ^= 1;
          }
        }
        
        /* Compute Area */
        dxAB = fa[xindex] - fb[xindex];
        dxBC = fb[xindex] - fc[xindex];
        
        dyAB = fa[yindex] - fb[yindex];
        dyBC = fb[yindex] - fc[yindex];
        
        /* Stash the area in the float pool for easy access */
        gc->pool.ftemp1 = dxAB * dyBC - dxBC * dyAB;
        
#define FloatVal(__f) (((__f) < 786432.875) ? (__f) : ((__f) - 786432.875))
        {
          const FxI32 j = *(FxI32*)&gc->pool.ftemp1;
          
          /* Zero-area triangles are BAD!! */
          if ((j & 0x7FFFFFFF) == 0) {
            GDBG_INFO(291, FN_NAME": Culling (%g %g) (%g %g) (%g %g) : (%g : 0x%X : 0x%X)\n",
                      FloatVal(fa[xindex]), FloatVal(fa[yindex]), 
                      FloatVal(fb[xindex]), FloatVal(fb[yindex]), 
                      FloatVal(fc[xindex]), FloatVal(fc[yindex]), 
                      gc->pool.ftemp1, gc->state.cull_mode, culltest);

            goto done;
          }
      
          /* Backface culling, use sign bit as test */
          if ((gc->state.cull_mode != GR_CULL_DISABLE) &&
              ((j ^ (culltest << 31)) >= 0)) {
        
            GDBG_INFO(291, FN_NAME": Culling (%g %g) (%g %g) (%g %g) : (%g : 0x%X : 0x%X)\n",
                      FloatVal(fa[xindex]), FloatVal(fa[yindex]), 
                      FloatVal(fb[xindex]), FloatVal(fb[yindex]), 
                      FloatVal(fc[xindex]), FloatVal(fc[yindex]), 
                      gc->pool.ftemp1, gc->state.cull_mode, culltest);
        
            goto done;
          }
        }
      }
    } /* end culling test */
      
    aaDrawArrayEdgeSense(a, b, c);
      
    aaDrawArrayEdgeSense(b, c, a);
      
    aaDrawArrayEdgeSense(c, a, b);
      
  done:
    lPtr += 3;
    tCount+=3;
    gc->stats.trisProcessed++;
  }

  gc->state.shadow.fbzMode = fbzModeOld;
  gc->state.invalid |= fbzModeBIT;
  GR_FLUSH_STATE();

#undef FN_NAME
} /* _grAADrawTriangles */

/*-------------------------------------------------------------------
  Function: _grAAVpDrawTriangles
  Date: 11-Dec-97
  Implementor(s): atai
  Description:
  Draw anti-alias triangles (clip coords)
  Arguments:
  
  Return:
  -------------------------------------------------------------------*/
void FX_CSTYLE
_grAAVpDrawTriangles(FxI32 mode, FxI32 ttype, FxI32 count, void *pointers)
{
#define FN_NAME "_grAAVpDrawTriangles"

  GR_DCL_GC;

  float **lPtr = (float **)pointers;
  FxI32 tCount = 3;
  FxU32 fbzModeOld;                 /* Squirrel away current fbzMode */
  FxI32 stride = mode;
  FxI32 xindex = (gc->state.vData.vertexInfo.offset >> 2);
  FxI32 yindex = xindex + 1;

  GDBG_INFO(96, "_grAADrawTriangles (count = %d, pointers = 0x%x)\n",
            count, pointers);

  GR_FLUSH_STATE();

  if (ttype == GR_TRIANGLES)
    (*gc->archDispatchProcs.drawTrianglesProc)(mode, count, pointers);

  fbzModeOld = gc->state.shadow.fbzMode;
  gc->state.shadow.fbzMode &= ~(SST_ZAWRMASK);
  /* gc->state.invalid |= fbzModeBIT; */
  GR_FLUSH_STATE();

  if (stride == 0)
    stride = gc->state.vData.vStride;

  /* backfaced or zero area */
  while (tCount <= count) {
    float *a, *b, *c;
    float oowa, oowb, oowc;

    GR_BEGIN_NOFIFOCHECK("_grAADrawTri",96);
      
    a = pointers;
    b = (float *)pointers + 1;
    c = (float *)pointers + 2;
    if (mode) {
      a = *(float **)a;
      b = *(float **)b;
      c = *(float **)c;
    }
    (float *)pointers += stride*3;
    oowa = 1.0f / FARRAY(a, gc->state.vData.wInfo.offset);        
    oowb = 1.0f / FARRAY(b, gc->state.vData.wInfo.offset);        
    oowc = 1.0f / FARRAY(c, gc->state.vData.wInfo.offset);        


    /* move culling test to here */
    {
      float *fa = a;
      float *fb = b;
      float *fc = c;
      float dxAB, dxBC, dyAB, dyBC;
      /*
      **  Sort the vertices.
      **  Whenever the radial order is reversed (from counter-clockwise to
      **  clockwise), we need to change the area of the triangle.  Note
      **  that we know the first two elements are X & Y by looking at the
      **  grVertex structure.  
      */
      {
        float fay = a[yindex]*oowa*gc->state.Viewport.hheight*gc->state.Viewport.oy;
        float fby = b[yindex]*oowb*gc->state.Viewport.hheight*gc->state.Viewport.oy;
        float fcy = c[yindex]*oowc*gc->state.Viewport.hheight*gc->state.Viewport.oy;
        int ay = *(int *)&fay;
        int by = *(int *)&fby;
        int cy = *(int *)&fcy;
        int culltest = gc->state.cull_mode;
        
        if (ay < 0) ay ^= 0x7FFFFFFF;
        if (by < 0) by ^= 0x7FFFFFFF;
        if (cy < 0) cy ^= 0x7FFFFFFF;
        
        if (ay < by) {
          if (by > cy) {    /* acb */
            if (ay < cy) {
              fa = a;
              fb = c;
              fc = b;
              
              culltest ^= 1;
            } else {                /* cab */
              fa = c;
              fb = a;
              fc = b;
            }
            /* else it's already sorted */
          }
        } else {
          if (by < cy) {    /* bac */
            if (ay < cy) {
              fa = b;
              fb = a;
              fc = c;
              culltest ^= 1;
            } else {                /* bca */
              fa = b;
              fb = c;
              fc = a;
            }
          } else {          /* cba */
            fa = c;
            fb = b;
            fc = a;
            culltest ^= 1;
          }
        }
        
        /* Compute Area */
        dxAB = fa[xindex] - fb[xindex];
        dxBC = fb[xindex] - fc[xindex];
        
        dyAB = fa[yindex] - fb[yindex];
        dyBC = fb[yindex] - fc[yindex];
        
        /* Stash the area in the float pool for easy access */
        gc->pool.ftemp1 = dxAB * dyBC - dxBC * dyAB;
        
#define FloatVal(__f) (((__f) < 786432.875) ? (__f) : ((__f) - 786432.875))
        {
          const FxI32 j = *(FxI32*)&gc->pool.ftemp1;
          
          /* Zero-area triangles are BAD!! */
          if ((j & 0x7FFFFFFF) == 0) {
            GDBG_INFO(291, FN_NAME": Culling (%g %g) (%g %g) (%g %g) : (%g : 0x%X : 0x%X)\n",
                      FloatVal(fa[xindex]), FloatVal(fa[yindex]), 
                      FloatVal(fb[xindex]), FloatVal(fb[yindex]), 
                      FloatVal(fc[xindex]), FloatVal(fc[yindex]), 
                      gc->pool.ftemp1, gc->state.cull_mode, culltest);

            goto done;
          }
      
          /* Backface culling, use sign bit as test */
          if ((gc->state.cull_mode != GR_CULL_DISABLE) &&
              ((j ^ (culltest << 31)) >= 0)) {
        
            GDBG_INFO(291, FN_NAME": Culling (%g %g) (%g %g) (%g %g) : (%g : 0x%X : 0x%X)\n",
                      FloatVal(fa[xindex]), FloatVal(fa[yindex]), 
                      FloatVal(fb[xindex]), FloatVal(fb[yindex]), 
                      FloatVal(fc[xindex]), FloatVal(fc[yindex]), 
                      gc->pool.ftemp1, gc->state.cull_mode, culltest);
        
            goto done;
          }
        }
      }
    } /* end culling test */
      
    aaVpDrawArrayEdgeSense(a, b, c, oowa, oowb);
      
    aaVpDrawArrayEdgeSense(b, c, a, oowb, oowc);
      
    aaVpDrawArrayEdgeSense(c, a, b, oowc, oowa);
      
  done:
    lPtr += 3;
    tCount+=3;
    gc->stats.trisProcessed++;
  }

  gc->state.shadow.fbzMode = fbzModeOld;
  gc->state.invalid |= fbzModeBIT;
  GR_FLUSH_STATE();

#undef FN_NAME
} /* _grAAVpDrawTriangles */

/*-------------------------------------------------------------------
  Function: _grAADrawVertexList
  Date: 14-Oct-97
  Implementor(s): atai
  Description:
  Sends an aa triangle strip/fan to CVG.
  Arguments:
  
  Return:
  -------------------------------------------------------------------*/
void FX_CSTYLE
_grAADrawVertexList(FxU32 type, FxI32 mode, FxI32 count, void *pointers)
{
#define FN_NAME "_grAADrawVertexList"

  GR_DCL_GC;
  FxU32 sCount = count;
  float *v[3];
  FxBool flip = FXFALSE;
  FxU32 fbzModeOld;                 /* Squirrel away current fbzMode */
  FxI32 stride = mode;

  if (sCount <= 2) return;

  GR_FLUSH_STATE();
  (*gc->archDispatchProcs.drawVertexList)(SSTCP_PKT3_BDDDDD, type, mode, count, pointers);

  fbzModeOld = gc->state.shadow.fbzMode;
  gc->state.shadow.fbzMode &= ~(SST_ZAWRMASK);
  /* gc->state.invalid |= fbzModeBIT; */
  GR_FLUSH_STATE();
  if (stride == 0)
    stride = gc->state.vData.vStride;

  sCount-=2;
  if (type == kSetupFan) {
    v[0] = (mode == 0) ? pointers : *(float **)pointers;
    while (sCount--) {
      (float *)pointers += stride;
      if (mode) {
        v[1] = *(float **)pointers;
        v[2] = *((float **)pointers+1);
      } else {
        v[1] = pointers;
        v[2] = (float *)pointers+stride;
      }
      if (gc->state.grCoordinateSpaceArgs.coordinate_space_mode == GR_WINDOW_COORDS)
        _grAADrawTriangles(1, type, 3, v);
      else
        _grAAVpDrawTriangles(1, type, 3, v);    
    }
  }
  else if (type == kSetupStrip){
    while (sCount--) {
      if (flip) {
        if (mode) {
          v[0] = *((float **)pointers+1);
          v[1] = *(float **)pointers;
          v[2] = *((float **)pointers+2);
        } else {
          v[0] = (float *)pointers+stride;
          v[1] = pointers;
          v[2] = (float *)pointers+(stride<<1);
        }
      }
      else { 
        if (mode) {
          v[0] = *(float **)pointers;
          v[1] = *((float **)pointers+1);
          v[2] = *((float **)pointers+2);
        } else {
          v[0] = pointers;
          v[1] = (float *)pointers+stride;
          v[2] = (float *)pointers+(stride<<1);
        }
      }
      if (gc->state.grCoordinateSpaceArgs.coordinate_space_mode == GR_WINDOW_COORDS)
        _grAADrawTriangles(1, type, 3, v);
      else
        _grAAVpDrawTriangles(1, type, 3, v);    
      (float *)pointers += stride;
      flip = ~flip;
    }
    flip = ~flip;
  }

  gc->state.shadow.fbzMode = fbzModeOld;
  gc->state.invalid |= fbzModeBIT;
  GR_FLUSH_STATE();

#undef FN_NAME
} /* _grAADrawVertexList */


