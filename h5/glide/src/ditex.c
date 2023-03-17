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
** $Header: /cvsroot/glide/glide2x/h3/glide/src/ditex.c,v 1.1.1.1 1999/12/07 21:49:26 joseph Exp $
** $Log: ditex.c,v $
** Revision 1.1.1.1  1999/12/07 21:49:26  joseph
** Initial checkin into SourceForge.
**
** 
** 3     3/17/99 6:16p Dow
** Phantom Menace and other fixes.
** 
** 24    2/16/99 4:14p Peter
** sanitized source/download
** 
** 23    2/10/99 2:24p Peter
** corrected alignment textures within an alignment allocation unit
** 
** 22    12/03/98 11:27p Dow
** Code 'cleanup' he�
** 
** 21    10/20/98 7:12p Peter
** static build hapiiness
** 
** 20    7/19/98 4:34p Mikec
** Relaxed 2MB span constraint on downloadmipmap.
** 
** 19    5/18/98 2:41p Dow
** 16-byte boundaries
** 
** 18    4/14/98 6:41p Peter
** Merge w/ cvg glide cleanup
 * 
 * 16    1/13/98 12:42p Atai
 * fixed grtexinfo, grVertexLayout, and draw triangle
 * 
 * 15    1/09/98 6:48p Atai
 * grTexInfo, GR_LOD_* and GR_ASPECT_*
 * 
 * 13    12/09/97 12:20p Peter
 * mac glide port
 * 
 * 12    12/02/97 9:49a Dow
 * Got rid of Texelfx rev 0 warning
 * 
 * 11    11/20/97 6:58p Dow
 * Marked _grTexTextureMemRequired for movement
 * 
 * 10    8/18/97 3:52p Peter
 * pre-hw arrival fixes/cleanup
 * 
 * 9     6/02/97 4:09p Peter
 * Compile w/ gcc for Dural
 * 
 * 8     5/27/97 1:16p Peter
 * Basic cvg, w/o cmd fifo stuff. 
 * 
 * 7     5/21/97 6:04a Peter
 * 
 * 6     3/15/97 8:09p Jdt
 * Remove grTexDownloadTable from this file because I added SST-1 only
 * code to it
 * 
 * 5     3/09/97 10:31a Dow
 * Added GR_DIENTRY for di glide functions
 * 
 * 4     2/12/97 2:09p Hanson
 * Hopefully removed the rest of my effage. 
 * 
 * 3     2/12/97 12:34p Dow
 * Fixed Hanson effage
 * 
 * 2     1/18/97 11:41p Dow
 * Fixed Gary's "Last C Bug" analog
 * Fixed usage of _grMipMapOffset_Tsplit
 * 
 * 1     12/23/96 1:39p Dow
 * Changes for multiplatform
**
*/

#include <3dfx.h>

#define FX_DLL_DEFINITION
#include <fxdll.h>
#include <glide.h>
#include "fxglide.h"

const FxU32 _gr_aspect_index_table[] =
{
   3,
   2,
   1,
   0,
   1,
   2,
   3,
};

/* size in texels  */
const FxU32 _grMipMapHostSize[4][9] = 
{
  {                             /* 1:1 aspect ratio */
    65536,                      /* 0 : 256x256  */
    16384,                      /* 1 : 128x128  */
    4096,                       /* 2 :  64x64   */
    1024,                       /* 3 :  32x32   */
    256,                        /* 4 :  16x16   */
    64,                         /* 5 :   8x8    */
    16,                         /* 6 :   4x4    */
    4,                          /* 7 :   2x2    */
    1,                          /* 8 :   1x1    */
  },
  {                             /* 2:1 aspect ratio */
    32768,                      /* 0 : 256x128  */
    8192,                       /* 1 : 128x64   */
    2048,                       /* 2 :  64x32   */
    512,                        /* 3 :  32x16   */
    128,                        /* 4 :  16x8    */
    32,                         /* 5 :   8x4    */
    8,                          /* 6 :   4x2    */
    2,                          /* 7 :   2x1    */
    1,                          /* 8 :   1x1    */
  },
  {                             /* 4:1 aspect ratio */
    16384,                      /* 0 : 256x64   */
    4096,                       /* 1 : 128x32   */
    1024,                       /* 2 :  64x16   */
    256,                        /* 3 :  32x8    */
    64,                         /* 4 :  16x4    */
    16,                         /* 5 :   8x2    */
    4,                          /* 6 :   4x1    */
    2,                          /* 7 :   2x1    */
    1,                          /* 8 :   1x1    */
  },
  {                             /* 8:1 aspect ratio */
    8192,                       /* 0 : 256x32   */
    2048,                       /* 1 : 128x16   */
    512,                        /* 2 :  64x8    */
    128,                        /* 3 :  32x4    */
    32,                         /* 4 :  16x2    */
    8,                          /* 5 :   8x1    */
    4,                          /* 6 :   4x1    */
    2,                          /* 7 :   2x1    */
    1,                          /* 8 :   1x1    */
  }
};

const int _grMipMapHostWH[G3_ASPECT_TRANSLATE(GR_ASPECT_1x8)+1][G3_LOD_TRANSLATE(GR_LOD_1)+1][2] =
{
   { 
     { 256 , 32 }, 
     { 128 , 16 }, 
     { 64  , 8 }, 
     { 32  , 4 }, 
     { 16  , 2 }, 
     { 8   , 1 }, 
     { 4   , 1 }, 
     { 2   , 1 }, 
     { 1   , 1 }
   },
   { 
     { 256 , 64 }, 
     { 128 , 32 }, 
     { 64  , 16 }, 
     { 32  , 8 }, 
     { 16  , 4 }, 
     { 8   , 2 },
     { 4   , 1 }, 
     { 2   , 1 }, 
     { 1   , 1 }
   } ,
   { 
     { 256 , 128 }, 
     { 128 , 64 }, 
     { 64  , 32 }, 
     { 32  , 16 },
     { 16  , 8 },
     { 8   , 4 }, 
     { 4   , 2 }, 
     { 2   , 1 }, 
     { 1   , 1 }
   },
   { 
     { 256 , 256 }, 
     { 128 , 128 }, 
     { 64  , 64 },
     { 32  , 32 },
     { 16  , 16 },
     { 8   , 8 }, 
     { 4   , 4 }, 
     { 2   , 2 }, 
     { 1   , 1 }
   },
   { 
     { 128, 256 },
     { 64,  128 },
     { 32,  64  },
     { 16,  32  },
     { 8,   16  }, 
     { 4,   8   }, 
     { 2,   4   }, 
     { 1,   2   }, 
     { 1,   1 }
   },
   {
     { 64,  256 },
     { 32,  128 },
     { 16,  64  },
     { 8,   32  },
     { 4,   16  },
     { 2,   8   }, 
     { 1,   4   }, 
     { 1,   2   }, 
     { 1,   1 }
   },
   { 
     { 32,  256 },
     { 16,  128 },
     { 8,   64  },
     { 4,   32  },
     { 2,   16  },
     { 1,   8   }, 
     { 1,   4   }, 
     { 1,   2   },
     { 1,   1 }
   }
};

/* translates GR_ASPECT_* to bits for the TLOD register */
const FxU32 _gr_aspect_xlate_table[] =
{
   (3<< SST_LOD_ASPECT_SHIFT) | SST_LOD_S_IS_WIDER,
   (2<< SST_LOD_ASPECT_SHIFT) | SST_LOD_S_IS_WIDER,
   (1<< SST_LOD_ASPECT_SHIFT) | SST_LOD_S_IS_WIDER,
    0<< SST_LOD_ASPECT_SHIFT,
    1<< SST_LOD_ASPECT_SHIFT,
    2<< SST_LOD_ASPECT_SHIFT,
    3<< SST_LOD_ASPECT_SHIFT
};

const FxU32 _gr_evenOdd_xlate_table[] = 
{
  0xFFFFFFFF,                      /* invalid */
  SST_LOD_TSPLIT,                  /* even */
  SST_LOD_TSPLIT | SST_LOD_ODD,    /* odd  */
  0,                               /* both */
};

/* the size of each mipmap level in texels, 4 is the minimum no matter what */
/* index is [aspect_ratio][lod]                                             */
static const FxU32 _grMipMapSize[4][16] = {
  {   /* 8:1 aspect ratio */
    0x02000,        /* 0 : 256x32   */
    0x00800,        /* 1 : 128x16   */
    0x00200,        /* 2 :  64x8    */
    0x00080,        /* 3 :  32x4    */
    0x00020,        /* 4 :  16x2    */
    0x00010,        /* 5 :   8x1    */
    0x00008,        /* 6 :   4x1    */
    0x00004,        /* 7 :   2x1    */
    0x00004,        /* 8 :   1x1    */
  },
  {   /* 4:1 aspect ratio */
    0x04000,        /* 0 : 256x64   */
    0x01000,        /* 1 : 128x32   */
    0x00400,        /* 2 :  64x16   */
    0x00100,        /* 3 :  32x8    */
    0x00040,        /* 4 :  16x4    */
    0x00010,        /* 5 :   8x2    */
    0x00008,        /* 6 :   4x1    */
    0x00004,        /* 7 :   2x1    */
    0x00004,        /* 8 :   1x1    */
  },
  {   /* 2:1 aspect ratio */
    0x08000,        /* 0 : 256x128  */
    0x02000,        /* 1 : 128x64   */
    0x00800,        /* 2 :  64x32   */
    0x00200,        /* 3 :  32x16   */
    0x00080,        /* 4 :  16x8    */
    0x00020,        /* 5 :   8x4    */
    0x00008,        /* 6 :   4x2    */
    0x00004,        /* 7 :   2x1    */
    0x00004,        /* 8 :   1x1    */
  },
  {   /* 1:1 aspect ratio */
    0x10000,        /* 0 : 256x256  */
    0x04000,        /* 1 : 128x128  */
    0x01000,        /* 2 :  64x64   */
    0x00400,        /* 3 :  32x32   */
    0x00100,        /* 4 :  16x16   */
    0x00040,        /* 5 :   8x8    */
    0x00010,        /* 6 :   4x4    */
    0x00004,        /* 7 :   2x2    */
    0x00004,        /* 8 :   1x1    */
  },
};


/* the offset from mipmap level 0 of each mipmap level in texels            */
/* index is [aspect_ratio][lod]                                             */
static FxU32 _grMipMapOffset[4][16];
static FxU32 _grMipMapOffset_Tsplit[4][16];

/* initialize the MipMap Offset arrays */
void
_grMipMapInit(void)
{
  int ar,lod;

  for(ar = 0; ar < 4; ar++)  {          /* for each aspect ratio          */
    _grMipMapOffset[ar][0] = 0;         /* start off with offset=0        */
    for(lod = 1; lod <= 9; lod++) {     /* for each lod, add in prev size */
      _grMipMapOffset[ar][lod] = _grMipMapOffset[ar][lod - 1] + _grMipMapHostSize[3 - ar][lod - 1];
    }

    _grMipMapOffset_Tsplit[ar][0] = 0;  /* start off with offset=0 */
    _grMipMapOffset_Tsplit[ar][1] = 0;  /* start off with offset=0 */
    for (lod = 2; lod <= 9; lod++) {    /* for each lod, add in prev size */
      _grMipMapOffset_Tsplit[ar][lod] = _grMipMapOffset_Tsplit[ar][lod - 2] + _grMipMapHostSize[3 - ar][lod - 2];
    }
  }
} /* _grMipMapInit */

/*---------------------------------------------------------------------------
**  This is not DI anymore.  Perhaps all of these size routines need
**  to be in gtex.c now.
*/
FxU32
_grTexTextureMemRequired(GrLOD_t small_lod, GrLOD_t large_lod, 
                         GrAspectRatio_t aspect, GrTextureFormat_t format,
                         FxU32 evenOdd,
                         FxBool roundP)
{
#define FN_NAME "_grTexTextureMemRequired"
  FxU32 memrequired;

  GR_CHECK_W(FN_NAME, 
             small_lod < large_lod, 
             "small_lod bigger than large_lod");
  GR_CHECK_F(FN_NAME, 
             (evenOdd > GR_MIPMAPLEVELMASK_BOTH) || (evenOdd == 0), 
             "invalid evenOdd mask");

  /* mirror aspect ratios because it does not matter for
   * the size computation which dimension is shrunk.
   */
  if ( aspect > GR_ASPECT_1x1 )
    aspect =  GR_ASPECT_1x8 - aspect;

  if ( evenOdd == GR_MIPMAPLEVELMASK_BOTH ) {
    memrequired  = _grMipMapOffset[aspect][small_lod + 1];
    memrequired -= _grMipMapOffset[aspect][large_lod];
  } else {
    memrequired = 0;
    /* construct XOR mask   */
    evenOdd = (evenOdd == GR_MIPMAPLEVELMASK_EVEN);

    /* Sum all the mipmap levels that match the xor mask for the lod */
    while (large_lod <= small_lod) {
      if ((large_lod ^ evenOdd) & 1) {
        memrequired += _grMipMapSize[aspect][large_lod];
      }
      large_lod++;
    }
  }

  /* Convert from texels to bytes */
  if ( format >= GR_TEXFMT_16BIT )
    memrequired <<= 1;

  if (roundP) {
    memrequired += SST_TEXTURE_ALIGN_MASK;
    memrequired &= ~SST_TEXTURE_ALIGN_MASK;
  }

  return memrequired;
#undef FN_NAME
} /* _grTexTextureMemRequired */

#if 0 /* KoolSmoky - removed */
FxU16
_grTexFloatLODToFixedLOD( float value )
{
  float num_quarters;
  int   new_value;

  num_quarters = ( value + .125F ) / .25F;
  new_value    = ( int ) num_quarters;

  new_value   &= 0x003F;

  return new_value;
} /* _grTexFloatLODToFixedLOD */
#endif

/*---------------------------------------------------------------------------
** _grTexCalcBaseAddress
*/
FxU32
_grTexCalcBaseAddress( FxU32 start, GrLOD_t large_lod,
                       GrAspectRatio_t aspect, GrTextureFormat_t format,
                       FxU32 odd_even_mask)
{
  FxU32 
    sum_of_lod_sizes;

  if ( aspect > G3_ASPECT_TRANSLATE(GR_ASPECT_1x1) )         /* mirror aspect ratios         */
    aspect =  G3_ASPECT_TRANSLATE(GR_ASPECT_1x8) - aspect;

  if ( odd_even_mask == GR_MIPMAPLEVELMASK_BOTH ) {
    sum_of_lod_sizes = _grMipMapOffset[aspect][large_lod];
  } else {
    if (((odd_even_mask == GR_MIPMAPLEVELMASK_EVEN) && (large_lod & 1)) ||
        ((odd_even_mask == GR_MIPMAPLEVELMASK_ODD) && !(large_lod & 1)))
      large_lod += 1;
    sum_of_lod_sizes = _grMipMapOffset_Tsplit[aspect][large_lod];
  }
    
  if ( format >= GR_TEXFMT_16BIT )
    sum_of_lod_sizes <<= 1;

  /* Clamp the size down. The hw is still going to use the complete
   * size computation, but it always starts from an aligned baseAddr
   * so we need to take this into account before doing the subtraction
   * otherwise we'll be on the wrong 'side' of the minimum aligned
   * allocation unit.  
   */
  sum_of_lod_sizes &= ~SST_TEXTURE_ALIGN_MASK;

  return ( start - sum_of_lod_sizes );
} /* _grTexCalcBaseAddress */

/*---------------------------------------------------------------------------
** grTexCalcMemRequired
*/
GR_DIENTRY(grTexCalcMemRequired, FxU32,
           ( GrLOD_t small_lod, GrLOD_t large_lod, 
            GrAspectRatio_t aspect, GrTextureFormat_t format ))
{
  FxU32 memrequired;

  memrequired = _grTexTextureMemRequired(small_lod, large_lod, 
                                         aspect, format,
                                         GR_MIPMAPLEVELMASK_BOTH,
                                         FXTRUE);
  GDBG_INFO(88,"grTexCalcMemRequired(%d,%d,%d,%d) => 0x%x(%d)\n",
                small_lod,large_lod,aspect,format,memrequired,memrequired);

  return memrequired;
} /* grTexCalcMemRequired */


/*---------------------------------------------------------------------------
** grTexDetailControl
*/
GR_DIENTRY(grTexDetailControl, void,
           ( GrChipID_t tmu, int lod_bias, FxU8 detail_scale,
            float detail_max ))
{
#define FN_NAME "grTexDetailControl"
  FxU32 tDetail;
  FxU32 dmax    = ( FxU32 ) ( detail_max * _GlideRoot.pool.f255 );
  FxU32 dscale  = detail_scale;
  
  GR_BEGIN_NOFIFOCHECK("grTexDetailControl",88);
  GDBG_INFO_MORE(gc->myLevel,"(%d,%d,%g)\n",tmu,detail_scale,detail_max);
  GR_CHECK_TMU( FN_NAME, tmu );
  GR_CHECK_F( myName, lod_bias < -32 || lod_bias > 31, "lod_bias out of range" );
  GR_CHECK_F( myName, detail_scale > 7, "detail_scale out of range" );
  GR_CHECK_F( myName, detail_max < 0.0 || detail_max > 1.0, "detail_max out of range" );

  tDetail  = ( ( lod_bias << SST_DETAIL_BIAS_SHIFT ) & SST_DETAIL_BIAS );
  tDetail |= ( ( dmax << SST_DETAIL_MAX_SHIFT ) & SST_DETAIL_MAX );
  tDetail |= ( ( dscale << SST_DETAIL_SCALE_SHIFT ) & SST_DETAIL_SCALE );
  
  /* MULTIPLAT */
  _grTexDetailControl( tmu, tDetail );
  GR_END();
#undef FN_NAME
} /* grTexDetailControl */

GR_DIENTRY(grTexMinAddress, FxU32, ( GrChipID_t tmu ))
{
#define FN_NAME "grTexMinAddress"
  GR_BEGIN_NOFIFOCHECK_RET("grTexMinAddress",88);
  GDBG_INFO_MORE(gc->myLevel,"(%d)\n",tmu);
  GR_CHECK_TMU(FN_NAME, tmu);
  FXUNUSED( tmu );
  GR_RETURN(0);
#undef FN_NAME
} /* grTexMinAddress */


/*-------------------------------------------------------------------
  Function: grTexMaxAddress
  Date: 6/2
  Implementor(s): GaryT
  Library: glide
  Description:
    Returns address of maximum extent of texture ram for a given TMU
  Arguments:
    tmu
  Return:
    the largest valid texture start Address
  -------------------------------------------------------------------*/
GR_DIENTRY(grTexMaxAddress, FxU32, ( GrChipID_t tmu ))
{
#define FN_NAME "grTexMaxAddress"
  GR_BEGIN_NOFIFOCHECK_RET("grTexMaxAddress",88);
  GDBG_INFO_MORE(gc->myLevel,"(%d)\n",tmu);
  GR_CHECK_TMU(FN_NAME, tmu );
  GR_RETURN(gc->tmu_state[tmu].total_mem - SST_TEXTURE_ALIGN);
#undef FN_NAME
} /* grTexMaxAddress */


/*-------------------------------------------------------------------
  Function: grTexTextureMemRequired
  Date: 6/2
  Implementor(s): GaryMcT, Jdt
  Library: glide
  Description:
    Returns the tmu memory required to store the specified mipmap    
    ( Gary and I don't like the name of this function, but are 
      a little backed into a corner because of the existence 
      of grTexMemRequired() which does not imply any distinction
      between texture memory and system ram )
  Arguments:
    evenOdd  - which set of mipmap levels are to be stored
                One of:
                  GR_MIPMAPLEVELMASK_EVEN 
                  GR_MIPMAPLEVELMASK_ODD
                  GR_MIPMAPLEVELMASK_BOTH
    info      - pointer to GrTexInfo structure defining dimensions
                of texture
  Return:
    offset to be added to current texture base address to calculate next 
    valid texture memory download location
  -------------------------------------------------------------------*/
GR_DIENTRY(grTexTextureMemRequired, FxU32,
           ( FxU32 evenOdd, GrTexInfo *info))
{
  FxU32 memrequired;

  GR_CHECK_F( "grTexTextureMemRequired", !info, "invalid info pointer" );
  memrequired = _grTexTextureMemRequired(info->smallLod,
                                         info->largeLod,
                                         info->aspectRatio,
                                         info->format,
                                         evenOdd,
                                         FXTRUE);
                        
  GDBG_INFO(88,"grTexTextureMemRequired(%d,0x%x) => 0x%x(%d)\n",
                evenOdd,info,memrequired,memrequired);

  return memrequired;
} /* grTexTextureMemRequired */


/*-------------------------------------------------------------------
  Function: grTexDownloadMipMap
  Date: 6/2
  Implementor(s): GaryMcT, Jdt
  Library: glide
  Description:
    Downloads a texture mipmap to the specified tmu at the specified
    base address.
  Arguments:
    tmu          - which tmu
    startAddress - starting address for texture download,
    evenOdd  - which set of mipmap levels have been downloaded for
                the selected texture
                One of:
                  GR_MIPMAPLEVELMASK_EVEN
                  GR_MIPMAPLEVELMASK_ODD
                  GR_MIPMAPLEVELMASK_BOTH
    info       - pointer to GrTexInfo structure defining dimension of
                 texture to be downloaded and containing texture data
  Return:
    none
  -------------------------------------------------------------------*/
GR_DIENTRY(grTexDownloadMipMap, void, 
           ( GrChipID_t tmu, FxU32 startAddress, FxU32
            evenOdd, GrTexInfo  *info ))
{
  GR_DCL_GC;
  GrLOD_t lod;
  char *src_base;

  FXUNUSED(gc);

  GDBG_INFO(89,"grTexDownloadMipMap(%d,0x%x,%d,0x%x\n",tmu,startAddress,evenOdd,info);
  GR_CHECK_TMU( "grTexDownloadMipMap", tmu );
  GR_CHECK_COMPATABILITY("grTexDownloadMipMap", 
                         startAddress + grTexTextureMemRequired( evenOdd, info ) > gc->tmu_state[tmu].total_mem,
                         "insufficient texture ram at startAddress" );
  GR_CHECK_F( "grTexDownloadMipMap", evenOdd > 0x3, "evenOdd mask invalid" );
  GR_CHECK_F( "grTexDownloadMipMap", !info, "info invalid" );

  src_base = (char *)info->data;
  
  /*---------------------------------------------------------------
    Download one mipmap level at a time
    ---------------------------------------------------------------*/
  for( lod = info->largeLod; lod <= info->smallLod; lod++ ) {
    grTexDownloadMipMapLevel( tmu, 
                              startAddress, 
                              lod,
                              info->largeLod,
                              info->aspectRatio,
                              info->format,
                              evenOdd,
                              src_base );
               
    src_base += _grMipMapHostSize[_gr_aspect_index_table[info->aspectRatio]][lod] << (info->format>=GR_TEXFMT_16BIT);
  }
} /* grTexDownloadMipMap */


/*-------------------------------------------------------------------
  Function: grTexDownloadTablePartial
  Date: 6/3
  Implementor(s): GaryT
  Library: glide
  Description:
    download part of a look up table data to a tmu
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
GR_DIENTRY(grTexDownloadTablePartial, void,
           ( GrChipID_t tmu, GrTexTable_t type, 
            void *data, int start, int end ))
{
#define FN_NAME "grTexDownloadTablePartial"
  GR_BEGIN_NOFIFOCHECK("grTexDownloadTablePartial",89);
  GDBG_INFO_MORE(gc->myLevel,"(%d,%d,0x%x, %d,%d)\n",tmu,type,data,start,end);
  GR_CHECK_TMU(FN_NAME,tmu);
  GR_CHECK_F(myName, type > 0x2, "invalid table specified");
  GR_CHECK_F(myName, !data, "invalid data pointer");

  if ( type == GR_TEXTABLE_PALETTE )     /* Need Palette Download Code */
    _grTexDownloadPalette( tmu, (GuTexPalette *)data, start, end );
  else {                                 /* Type is an ncc table */
    _grTexDownloadNccTable( tmu, type, (GuNccTable*)data, start, end );
  }
  GR_END();
#undef FN_NAME
} /* grTexDownloadTable */

/*---------------------------------------------------------------------------
** grTexDownloadMipMapLevel
*/
GR_DIENTRY(grTexDownloadMipMapLevel, void, 
           ( GrChipID_t tmu, FxU32 startAddress, GrLOD_t thisLod,
            GrLOD_t largeLod, GrAspectRatio_t aspectRatio,
            GrTextureFormat_t format, FxU32 evenOdd, void *data )) 
{
  GR_BEGIN_NOFIFOCHECK("grTexDownloadMipMapLevel",89);
  GDBG_INFO_MORE(gc->myLevel,"(%d,0x%x, %d,%d,%d, %d,%d 0x%x)\n",
                  tmu,startAddress,thisLod,largeLod,aspectRatio,
                  format,evenOdd,data);
  grTexDownloadMipMapLevelPartial( tmu, startAddress,
                                   thisLod, largeLod, 
                                   aspectRatio, format,
                                   evenOdd, data,
                                   0, _grMipMapHostWH[aspectRatio][thisLod][1] - 1 );
  GR_END();
} /* grTexDownloadMipmapLevel */

FxU16 rle_line[256];
FxU16 *rle_line_end;

#if (GLIDE_PLATFORM & (GLIDE_OS_WIN32 | GLIDE_OS_MACOS | defined(__linux__)))
void rle_decode_line_asm(FxU16 *tlut,FxU8 *src,FxU16 *dest)
{
   /* don't do anything just shut up the compiler */
}
#endif /* (GLIDE_PLATFORM & (GLIDE_OS_WIN32 | GLIDE_OS_MACOS)) */
