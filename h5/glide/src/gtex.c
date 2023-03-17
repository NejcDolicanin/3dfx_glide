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
** $Header: /cvsroot/glide/glide2x/h3/glide/src/gtex.c,v 1.2 2000/10/03 18:29:56 mercury Exp $
** $Log: gtex.c,v $
** Revision 1.2  2000/10/03 18:29:56  mercury
** 003-clean_up_h3-000, h3 tree cleanup.
**
** Revision 1.1.1.1  1999/12/07 21:49:28  joseph
** Initial checkin into SourceForge.
**
** 4     KoolSmoky napalm support. need to work on 2ppc support.
** 
** 3     3/17/99 6:16p Dow
** Phantom Menace and other fixes.
** 
** 49    3/08/99 12:20p Peter
** removed unnecessary nop's
** 
** 48    3/02/99 2:22p Peter
** 2d nop flushes (although 3d nop should too)
** 
** 47    2/19/99 10:15a Peter
** multi-base sourcing
** 
** 46    2/16/99 4:14p Peter
** made texture table declarations consistent
** 
** 45    2/10/99 2:24p Peter
** corrected alignment textures within an alignment allocation unit
** 
** 44    12/09/98 4:44p Atai
** fixed debug build
** 
** 42    10/20/98 4:39p Atai
** update tramOffset and tramSize
** 
** 41    8/12/98 4:25p Peter
** fixed sense of trilinear fixup
** 
** 40    6/24/98 10:55a Peter
** gary's trilinear hell bug
** 
** 39    5/18/98 3:20p Peter
** crybaby warning for grTexCombine about different lod based factors
** 
** 38    5/07/98 4:13p Peter
** lbe texture thing
** 
** 37    4/16/98 3:58p Peter
** 1x1 lod fix, sorry John
** 
** 36    4/14/98 6:41p Peter
** Merge w/ cvg glide cleanup
** 
** 35    3/28/98 11:24a Dow
** itwoç
 * 
 * 32    1/13/98 12:42p Atai
 * fixed grtexinfo, grVertexLayout, and draw triangle
 * 
 * 31    1/09/98 6:48p Atai
 * grTexInfo, GR_LOD_* and GR_ASPECT_*
 * 
 * 30    1/08/98 7:09p Peter
 * real hw stuff modulo makefile change
 * 
 * 29    1/08/98 4:58p Atai
 * tex table broadcast, grVertexLayout enable/disable, stq, and some
 * defines
 * 
 * 28    1/07/98 10:22a Peter
 * lod dithering env var
 * 
 * 27    1/05/98 6:06p Atai
 * glide extension stuff
 * 
 * 26    12/18/97 10:52a Atai
 * fixed grGet(GR_VIDEO_POS)
 * 
 * 25    12/17/97 4:45p Peter
 * groundwork for CrybabyGlide
 * 
 * 24    12/11/97 4:15p Peter
 * fixed assertions
 * 
 * 23    12/05/97 4:26p Peter
 * watcom warnings
 * 
 * 22    11/20/97 6:57p Dow
 * baseaddress for banshee
 * 
 * 21    11/18/97 4:36p Peter
 * chipfield stuff cleanup and w/ direct writes
 * 
 * 20    11/17/97 4:55p Peter
 * watcom warnings/chipfield stuff
 * 
 * 19    11/03/97 3:43p Peter
 * h3/cvg cataclysm
 * 
 * 18    9/15/97 7:31p Peter
 * more cmdfifo cleanup, fixed normal buffer clear, banner in the right
 * place, lfb's are on, Hmmmm.. probably more
 * 
 * 17    9/05/97 12:31p Peter
 * more reg write grouping
 * 
 * 16    7/08/97 2:48p Peter
 * 
 * 15    6/06/97 10:47a Peter
 * texture downloading, fixed 640x480 dimension, changed cvg dep to be the
 * same as sst1
 * 
 * 14    5/27/97 1:16p Peter
 * Basic cvg, w/o cmd fifo stuff. 
 * 
 * 13    5/21/97 6:05a Peter
 * 
 * 12    3/04/97 9:08p Dow
 * 
 * 11    3/03/97 10:58a Jdt
 * Fixe for 2-pass trilinear
 * 
 * 10    12/23/96 1:37p Dow
 * chagnes for multiplatform glide
**
*/

#include <3dfx.h>

#define FX_DLL_DEFINITION
#include <fxdll.h>
#include <glide.h>
#include "fxglide.h"

#if GLIDE_CHECK_TRILINEAR
/* Make sure that the trilinear blending bits are set in a
 * consistent manner across the tmu's.  This only really matters if
 * we have multiple tmu's, but this state is really setup across
 * multiple calls (grTexCombine, grTexMipMapMode, and
 * grTexSource).  
 * 
 * NB: This must be called after the shadows are updated because
 * _grTexCheckTriLinear() will use the shadow register values
 * to determine the current state of trilinearness. 
 *
 * FixMe: This should eventually get merged in w/ the texture
 * statemonster. When/If that ever happens.
 */
static void
_grTexCheckTriLinear(GrChipID_t tmu)
{
#define FN_NAME "_grTexCheckTriLinear"
  GR_BEGIN_NOFIFOCHECK(FN_NAME, 90);
  GDBG_INFO_MORE(gc->myLevel, "(0x%X)\n", tmu);

#if (GLIDE_NUM_TMU > 2)
#error "(GLIDE_NUM_TMU > 2): Write this code"
#endif

/* NB: The factor mask needs to include the factor bits as
 * well as the reverse bit so that we can differentiate
 * between the lodfrac and the (1 - lodfrac) case.
 */
#define SST_TC_FACTOR_MASK (SST_TC_MSELECT | SST_TC_REVERSE_BLEND)

#define SST_MIPMAP_LEVEL_MASK (SST_LOD_ODD | SST_LOD_TSPLIT)

  /* Is this tmu on? */
  if (!gc->tmuLodDisable[tmu]) {
    const struct tmu_config_t* tmu0 = gc->state.tmu_config;
    const struct tmu_config_t* tmu1 = gc->state.tmu_config + 1;
    const struct tmu_config_t* curTmu = gc->state.tmu_config + tmu;
    const struct tmu_config_t* otherTmu = gc->state.tmu_config + !tmu;

    /* This is the 'tricky' state where we have to manage the states
     * of teh tmu's together to get the correct effect. Within this
     * state there are two sub-states: two-pass trilinear and single
     * pass using both tmu's w/ split levels.  
     *
     * Case 1 - TMU set for lod blending and has both even/odd levels
     */
    if (((curTmu->textureMode & SST_TRILINEAR) == SST_TRILINEAR) &&
        (curTmu->evenOdd == GR_MIPMAPLEVELMASK_BOTH)) {
      
      /* Check the 'other' tmu to see if it is active, if not then we
       * are doing two pass trilinear so check that we have the
       * correct even/odd things set based on the factor where one
       * pass will use GR_COMBINE_FACTOR_LOD_FRACTION and the other
       * will use (1 - GR_COMBINE_FACTOR_LOD_FRACTION).
       */
      if (gc->tmuLodDisable[!tmu]) {
        /* NB: In this case the rgb/alpha factors need to match so
         * checking for only one of them is fine.
         */
        const FxU32 levelMask = (((curTmu->textureMode & SST_TC_FACTOR_MASK) == SST_TC_MLODFRAC)
                                 ? SST_LOD_ODD
                                 : 0);
        
        GDBG_INFO(gc->myLevel, FN_NAME": Two-pass trilinear fixup (0x%X) : tLOD : (0x%X : 0x%X)\n",
                  tmu, curTmu->tLOD, ((curTmu->tLOD & ~SST_MIPMAP_LEVEL_MASK) | levelMask));
        
        GR_SET_EXPECTED_SIZE(sizeof(FxU32), 1);
        GR_SET(eChipTMU0, SST_TMU(hw, 0), tLOD, 
               ((curTmu->tLOD & ~SST_MIPMAP_LEVEL_MASK) | levelMask));
        GR_CHECK_SIZE();
      } else {
        /* One pass trilinear
         *
         * Make sure that the tmu's have the levels split across the
         * two tmu's. There are basically three cases based on what
         * the user might have already set on the other tmu.
         */        
        if (((tmu0->textureMode & SST_TC_BLEND) == SST_TC_BLEND) &&
            ((tmu1->textureMode & SST_TC_REPLACE) == SST_TC_REPLACE)) {
          FxU32  evenOdd[GLIDE_NUM_TMU];
          FxU32* curEvenOdd = evenOdd + tmu;
          
          {
            FxU32 i;
            
            for(i = 0; i < GLIDE_NUM_TMU; i++) {
              evenOdd[i] = gc->state.tmu_config[i].tLOD & SST_LOD_ODD;
            }
          }
              
          /* 1 - The other tmu already has the even levels. */
          if ((otherTmu->evenOdd == GR_MIPMAPLEVELMASK_EVEN) &&
              (curTmu->evenOdd != GR_MIPMAPLEVELMASK_ODD)) {
            *curEvenOdd = SST_LOD_ODD;
            goto __tmuRegUpdate;
          }

          /* 2 - The other tmu already has the odd levels. */
          if ((otherTmu->evenOdd == GR_MIPMAPLEVELMASK_ODD) &&
              (curTmu->evenOdd != GR_MIPMAPLEVELMASK_EVEN)) {
            *curEvenOdd = 0;
            goto __tmuRegUpdate;
          }

          /* 3 - The other tmu already has both the levels. If the
           * downstream tmu's factor is lodFrac then the upstream tmu
           * needs to be (1 - lodFrac) and vice-versa.  
           */
          if (otherTmu->evenOdd == GR_MIPMAPLEVELMASK_BOTH) {
            evenOdd[0] = (((tmu0->textureMode & SST_TC_FACTOR_MASK) == SST_TC_MLODFRAC)
                          ? SST_LOD_ODD
                          : 0);
            evenOdd[1] = ~evenOdd[0] & SST_LOD_ODD;

            goto __tmuRegUpdate;
          }

          /* Do the register updates */
          if (0) {
        __tmuRegUpdate:
            GDBG_INFO(gc->myLevel, FN_NAME": Tri-linear fixup (0x%X : 0x%X) : (0x%X : 0x%X)\n",
                      tmu0->tLOD, tmu1->tLOD,
                      ((tmu0->tLOD & ~SST_MIPMAP_LEVEL_MASK) | evenOdd[0]),
                      ((tmu1->tLOD & ~SST_MIPMAP_LEVEL_MASK) | evenOdd[1]));

            GR_SET_EXPECTED_SIZE((sizeof(FxU32) << 1), 2);
            {
              GR_SET(eChipTMU0, SST_TMU(hw, 0), tLOD, 
                     ((tmu0->tLOD & ~SST_MIPMAP_LEVEL_MASK) | evenOdd[0]));
              GR_SET(eChipTMU1, SST_TMU(hw, 1), tLOD, 
                     ((tmu1->tLOD & ~SST_MIPMAP_LEVEL_MASK) | evenOdd[1]));
            }
            GR_CHECK_SIZE();
          }
        }
      }
    }
  }
  
  GR_END();
#undef FN_NAME
}
#endif /* GLIDE_CHECK_TRILINEAR */

void
_grTex2ppc(FxBool enable)
{
#define FN_NAME "_grTex2ppc"
#ifdef FX_GLIDE_NAPALM
  /*
  ** enable combineMode[29]
  ** control the band in renderMode[24:22]
  ** make tmu 1 state the same as tmu0
  */
  FxU32 renderMode;
  FxU32 combineMode;
  FxU32 tmu0cMode;
  FxU32 tmu1cMode;
  //FxU32 tmu0cMode_tmuShadow;
  //FxU32 tmu1cMode_tmuShadow;
  FxBool oldstate;

  GR_BEGIN_NOFIFOCHECK("_grTex2ppc",88);

  renderMode = gc->state.fbi_config.renderMode;
  combineMode = gc->state.fbi_config.combineMode;
  oldstate = (combineMode & SST_CM_ENABLE_TWO_PIXELS_PER_CLOCK) ? FXTRUE : FXFALSE;
    
  if (enable == oldstate) return;
  
  GDBG_INFO(gc->myLevel, FN_NAME ": changing 2ppc mode to: %s\n", enable ? "FXTRUE" : "FXFALSE");
  //gc->state.mode2ppc = enable;

  if(enable) {
    tmu0cMode = gc->state.tmu_config[0].combineMode;
    tmu1cMode = gc->state.tmu_config[1].combineMode;
    //tmu0cMode_tmuShadow = gc->state.tmuShadow[0].combineMode;
    //tmu1cMode_tmuShadow = gc->state.tmuShadow[1].combineMode;

    renderMode |= (_GlideRoot.environment.band2ppc << SST_RM_TWO_PIXELS_PER_CLOCK_BAND_SELECTION_SHIFT);
    combineMode |= SST_CM_ENABLE_TWO_PIXELS_PER_CLOCK;
    tmu0cMode |= SST_CM_ENABLE_TWO_PIXELS_PER_CLOCK;
    tmu1cMode |= SST_CM_ENABLE_TWO_PIXELS_PER_CLOCK;
    //tmu0cMode_tmuShadow |= SST_CM_ENABLE_TWO_PIXELS_PER_CLOCK;
    //tmu1cMode_tmuShadow |= SST_CM_ENABLE_TWO_PIXELS_PER_CLOCK;
    REG_GROUP_BEGIN(((FifoChipField)(0x02UL << GR_TMUMASK_TMU0)), textureMode, 7, 0x7F);
    {
      REG_GROUP_SET(SST_TMU(hw, GR_TMUMASK_TMU0), textureMode, gc->state.tmu_config[GR_TMUMASK_TMU0].textureMode);
      REG_GROUP_SET(SST_TMU(hw, GR_TMUMASK_TMU0), tLOD, gc->state.tmu_config[GR_TMUMASK_TMU0].tLOD);
      REG_GROUP_SET(SST_TMU(hw, GR_TMUMASK_TMU0), tDetail, gc->state.tmu_config[GR_TMUMASK_TMU0].tDetail);
      REG_GROUP_SET(SST_TMU(hw, GR_TMUMASK_TMU0), texBaseAddr, gc->state.tmu_config[GR_TMUMASK_TMU0].texBaseAddr);
      REG_GROUP_SET(SST_TMU(hw, GR_TMUMASK_TMU0), texBaseAddr1, gc->state.tmu_config[GR_TMUMASK_TMU0].texBaseAddr_1);
      REG_GROUP_SET(SST_TMU(hw, GR_TMUMASK_TMU0), texBaseAddr2, gc->state.tmu_config[GR_TMUMASK_TMU0].texBaseAddr_2);
      REG_GROUP_SET(SST_TMU(hw, GR_TMUMASK_TMU0), texBaseAddr38, gc->state.tmu_config[GR_TMUMASK_TMU0].texBaseAddr_3_8);
    }
    REG_GROUP_END();
  } else {
    /* Make sure that all TMU registers are restored if 2PPC mode is being switched off. */
    REG_GROUP_BEGIN(((FifoChipField)(0x02UL << GR_TMUMASK_TMU1)), textureMode, 7, 0x7F);
    {
      REG_GROUP_SET(SST_TMU(hw, GR_TMUMASK_TMU1), textureMode, gc->state.tmu_config[GR_TMUMASK_TMU1].textureMode);
      REG_GROUP_SET(SST_TMU(hw, GR_TMUMASK_TMU1), tLOD, gc->state.tmu_config[GR_TMUMASK_TMU1].tLOD);
      REG_GROUP_SET(SST_TMU(hw, GR_TMUMASK_TMU1), tDetail, gc->state.tmu_config[GR_TMUMASK_TMU1].tDetail);
      REG_GROUP_SET(SST_TMU(hw, GR_TMUMASK_TMU1), texBaseAddr, gc->state.tmu_config[GR_TMUMASK_TMU1].texBaseAddr);
      REG_GROUP_SET(SST_TMU(hw, GR_TMUMASK_TMU1), texBaseAddr1, gc->state.tmu_config[GR_TMUMASK_TMU1].texBaseAddr_1);
      REG_GROUP_SET(SST_TMU(hw, GR_TMUMASK_TMU1), texBaseAddr2, gc->state.tmu_config[GR_TMUMASK_TMU1].texBaseAddr_2);
      REG_GROUP_SET(SST_TMU(hw, GR_TMUMASK_TMU1), texBaseAddr38, gc->state.tmu_config[GR_TMUMASK_TMU1].texBaseAddr_3_8);
    }
    REG_GROUP_END();
    
    tmu0cMode = gc->state.tmu_config[0].combineMode;
    tmu1cMode = gc->state.tmu_config[1].combineMode;
    //tmu0cMode_tmuShadow = gc->state.tmuShadow[0].combineMode;
    //tmu1cMode_tmuShadow = gc->state.tmuShadow[1].combineMode;

    renderMode &= ~(SST_RM_TWO_PIXELS_PER_CLOCK_BAND_SELECTION);
    combineMode &= ~(SST_CM_ENABLE_TWO_PIXELS_PER_CLOCK);
    tmu0cMode &= ~(SST_CM_ENABLE_TWO_PIXELS_PER_CLOCK);
    tmu1cMode &= ~(SST_CM_ENABLE_TWO_PIXELS_PER_CLOCK);
    //tmu0cMode_tmuShadow &= ~(SST_CM_ENABLE_TWO_PIXELS_PER_CLOCK);
    //tmu1cMode_tmuShadow &= ~(SST_CM_ENABLE_TWO_PIXELS_PER_CLOCK);
    {
      /*
      ** Need to flush the tmu pipeline going from 2ppc to 1ppc
      ** by sending 12 nopCMD
      */
      FxU32 i;
      GR_SET_EXPECTED_SIZE(sizeof(FxU32)*12, 12);
      for (i = 0; i < 12; i++) {
        GR_SET(eChipTMU0 | eChipTMU1, hw, nopCMD,  0);
      }
      GR_CHECK_SIZE();
    }
  }

  /* set combine mode */
  REG_GROUP_BEGIN(BROADCAST_ID, renderMode, 2, 0x0401);
  {
    REG_GROUP_SET(hw, renderMode, renderMode);
    REG_GROUP_SET(hw, combineMode, combineMode);
#if DEBUG_2PPC
    GDBG_PRINTF("FBI  combineMode: %08lx\n",combineMode);
#endif
  }
  REG_GROUP_END();

  REG_GROUP_BEGIN(eChipTMU0, combineMode, 1, 0x1);
  {
    REG_GROUP_SET(SST_TMU(hw, 0), combineMode, tmu0cMode);
#if DEBUG_2PPC
    GDBG_PRINTF("TMU0 combineMode: %08lx  2ppc=%d\n",tmu0cMode,enable);
#endif    
  }
  REG_GROUP_END();
  REG_GROUP_BEGIN(eChipTMU1, combineMode, 1, 0x1);
  {
    REG_GROUP_SET(SST_TMU(hw, 1), combineMode, tmu1cMode);
#if DEBUG_2PPC
    GDBG_PRINTF("TMU1 combineMode: %08lx  2ppc=%d\n",tmu1cMode,enable);
#endif
  }
  REG_GROUP_END();

  gc->state.fbi_config.renderMode = renderMode;
  gc->state.fbi_config.combineMode = combineMode;
  gc->state.tmu_config[0].combineMode = tmu0cMode;
  gc->state.tmu_config[1].combineMode = tmu1cMode;
  //gc->state.tmuShadow[0].combineMode = tmu0cMode_tmuShadow;
  //gc->state.tmuShadow[1].combineMode = tmu1cMode_tmuShadow;

  GR_END();
  
#endif
#undef FN_NAME
} /* _grTex2ppc */

/*---------------------------------------------------------------------------
** grTexClampMode
*/
GR_ENTRY(grTexClampMode, void, 
          (GrChipID_t tmu, GrTextureClampMode_t s_clamp_mode, GrTextureClampMode_t t_clamp_mode))
{
#define FN_NAME "grTexClampMode"
  FxU32 
    texturemode,
    /*tLod,*/
    clampMode =(((s_clamp_mode == GR_TEXTURECLAMP_CLAMP) ? SST_TCLAMPS : 0) |
                ((t_clamp_mode == GR_TEXTURECLAMP_CLAMP) ? SST_TCLAMPT : 0));//,
    //mirrorMode =(((s_clamp_mode == GR_TEXTURECLAMP_MIRROR_EXT) ? SST_TMIRRORS : 0) |
    //             ((t_clamp_mode == GR_TEXTURECLAMP_MIRROR_EXT) ? SST_TMIRRORT : 0));
  //GrChipID_t tmus;

  GR_BEGIN_NOFIFOCHECK("grTexClampMode",88);
  GDBG_INFO_MORE(gc->myLevel,"(%d, %d,%d)\n",tmu,s_clamp_mode,t_clamp_mode);
  GR_CHECK_TMU(FN_NAME, tmu);

  /*
  ** Assuming Banshee family has max tmu size of 2
  */
  texturemode  = gc->state.tmu_config[tmu].textureMode;
  texturemode &= ~(SST_TCLAMPS | SST_TCLAMPT);
  texturemode |=  clampMode;

  //tLod = gc->state.tmu_config[tmu].tLOD;
  //tLod &= ~(SST_TMIRRORS | SST_TMIRRORT);        /* disable mirror */
  //if (mirrorMode) tLod |= mirrorMode;

  gc->state.tmu_config[tmu].textureMode = texturemode;
  //gc->state.tmu_config[tmu].tLOD        = tLod;

  /* Update real shadows and update hardware immediately if we can. */
  if(!gc->do2ppc/*gc->state.mode2ppc*/) {
    //if (IS_NAPALM(gc->bInfo->pciInfo.deviceID)) _grChipMask( SST_CHIP_MASK_ALL_CHIPS );
    REG_GROUP_BEGIN((0x02 << tmu), textureMode, 1, 0x1);
    {
      REG_GROUP_SET(SST_TMU(hw, tmu), textureMode, texturemode);
      //REG_GROUP_SET(tmuHw, tLOD, gc->state.tmu_config[tmu].tLOD);
    }
    REG_GROUP_END();
    //if (IS_NAPALM(gc->bInfo->pciInfo.deviceID)) _grChipMask(gc->chipmask);
    if(gc->state.tmu_config[tmu].texSubLodDither)
      g2LodBiasPerChip(tmu, gc->state.tmu_config[tmu].tLOD);
  }
  
  /*tmus = MultitextureAndTrilinear();
  if(tmus & tmu) g2LodBiasPerChip(tmu);*/
  
  GR_END();

#undef FN_NAME
} /* grTexClampMode */

/*---------------------------------------------------------------------------
** grTexCombine
*/
GR_ENTRY(grTexCombine, void, 
         (GrChipID_t tmu,
          GrCombineFunction_t rgb_function, GrCombineFactor_t rgb_factor,  
          GrCombineFunction_t alpha_function, GrCombineFactor_t alpha_factor, 
          FxBool rgb_invert, FxBool alpha_invert))
{
#define FN_NAME "grTexCombine"
  FxU32 texturemode;
  FxU32 tLod;
  FxU32 tmuMask;
  FxBool localColorP = FXFALSE;
  FxBool localAlphaP = FXFALSE;

  GR_BEGIN_NOFIFOCHECK("grTexCombine",88);
  GDBG_INFO_MORE(gc->myLevel,"(%d, %d,%d, %d,%d, %d,%d)\n",
                 tmu, rgb_function, rgb_factor, 
                 alpha_function, alpha_factor,
                 rgb_invert, alpha_invert);
  GR_CHECK_TMU(FN_NAME, tmu);
  GR_CHECK_W(FN_NAME,
               rgb_function < GR_COMBINE_FUNCTION_ZERO ||
               rgb_function > GR_COMBINE_FUNCTION_SCALE_MINUS_LOCAL_ADD_LOCAL_ALPHA,
               "unsupported texture color combine function");
  GR_CHECK_W(FN_NAME,
               alpha_function < GR_COMBINE_FUNCTION_ZERO ||
               alpha_function > GR_COMBINE_FUNCTION_SCALE_MINUS_LOCAL_ADD_LOCAL_ALPHA,
               "unsupported texture alpha combine function");
  GR_CHECK_W(FN_NAME,
               (rgb_factor & 0x7) < GR_COMBINE_FACTOR_ZERO ||
               (rgb_factor & 0x7) > GR_COMBINE_FACTOR_LOD_FRACTION ||
               rgb_factor > GR_COMBINE_FACTOR_ONE_MINUS_LOD_FRACTION,
               "unsupported texture color combine scale factor");
  GR_CHECK_W(FN_NAME,
               (alpha_factor & 0x7) < GR_COMBINE_FACTOR_ZERO ||
               (alpha_factor & 0x7) > GR_COMBINE_FACTOR_LOD_FRACTION ||
               alpha_factor > GR_COMBINE_FACTOR_ONE_MINUS_LOD_FRACTION,
               "unsupported texture alpha combine scale factor");

#if DEBUG_2PPC
  GDBG_PRINTF("grTexCombine(tmu=%d,rgbfunc=%x,rgbfact=%x,afunc=%x,afact=%x,ri=%d,ai=%d)\n",
    tmu,rgb_function,rgb_factor,alpha_function,alpha_factor,rgb_invert,alpha_invert);
#endif
    
#ifdef FX_GLIDE_NAPALM
  {
    /*
    ** turn off combine mode if necessary
    */
    FxU32 combineMode = gc->state.tmu_config[tmu].combineMode;
    FxU32 isCombineExt = (combineMode & SST_CM_USE_COMBINE_MODE);
    if (isCombineExt) {
      SstRegs* tmuHw = SST_TMU(hw, tmu);
      /* It appears that some of the combineMode bits still take effect */
      /* even if SST_CM_USE_COMBINE_MODE is cleared.  Thus, to make sure */
      /* that things work right with old style combine modes, we clear   */
      /* all of the combine mode bits to zero. */
      combineMode &= ~(
                       SST_CM_USE_COMBINE_MODE |
                       SST_CM_TC_OTHERSELECT   |
                       SST_CM_TC_LOCALSELECT   |
                       SST_CM_TC_MSELECT_7     |
                       SST_CM_TC_INVERT_OTHER  |
                       SST_CM_TC_INVERT_LOCAL  |
                       SST_CM_TC_INVERT_ADD_LOCAL |
                       SST_CM_TC_OUTSHIFT      |
                       SST_CM_TCA_OTHERSELECT  |
                       SST_CM_TCA_LOCALSELECT  |
                       SST_CM_TCA_INVERT_OTHER |
                       SST_CM_TCA_INVERT_LOCAL |
                       SST_CM_TCA_INVERT_ADD_LOCAL |
                       SST_CM_TCA_OUTSHIFT |
                       SST_CM_DISABLE_CHROMA_SUBSTITUTION
                      );
      GR_SET_EXPECTED_SIZE(sizeof(FxU32)*1, 1);
      GR_SET((0x02UL << tmu), SST_TMU(hw, tmu), combineMode, combineMode);
      gc->state.tmu_config[tmu].combineMode = combineMode;
    }
  }
#endif
  /* tmuMask tells grColorCombineFunction what to turn off an on if 
     the ccFunc requires texture mapping */
  texturemode = gc->state.tmu_config[tmu].textureMode;
  texturemode &= ~(SST_TCOMBINE | SST_TACOMBINE);
  tLod = gc->state.tmu_config[tmu].tLOD;
  tLod &= ~(SST_LOD_ODD);

  //gc->state.tcc_requires_constant_color[tmu] = FXFALSE;
  //gc->state.tac_requires_constant_color[tmu] = FXFALSE;

  //gc->state.combineExtsInUse &= ~(STATE_USING_TCC|STATE_USING_TAC);

  tmuMask = GR_TMUMASK_TMU0 << tmu;
  gc->state.tmuMask &= ~tmuMask;
  //gc->state.tmuColorPassthrough &= ~tmuMask;
  //gc->state.tmuAlphaPassthrough &= ~tmuMask;

  /* Check for obvious passthrough case */
  /*if((rgb_function == GR_COMBINE_FUNCTION_SCALE_OTHER) &&
     (rgb_factor == GR_COMBINE_FACTOR_ONE) &&
     (alpha_function == GR_COMBINE_FUNCTION_SCALE_OTHER) &&
     (alpha_factor == GR_COMBINE_FACTOR_ONE)) {
    gc->state.tmuColorPassthrough |= tmuMask;
    gc->state.tmuAlphaPassthrough |= tmuMask;
  }*/

  /* setup scale factor bits */
  texturemode |= (rgb_factor & 0x7) << SST_TC_MSELECT_SHIFT;
  if (!(rgb_factor & 0x8)) texturemode |= SST_TC_REVERSE_BLEND;

  if (((rgb_factor & 0x7) == GR_COMBINE_FACTOR_LOCAL) ||
       ((rgb_factor & 0x7) == GR_COMBINE_FACTOR_LOCAL_ALPHA))
    gc->state.tmuMask |= tmuMask;

  texturemode |= (alpha_factor & 0x7) << SST_TCA_MSELECT_SHIFT;
  if (!(alpha_factor & 0x8)) texturemode |= SST_TCA_REVERSE_BLEND;

  if (((alpha_factor & 0x7) == GR_COMBINE_FACTOR_LOCAL) ||
       ((alpha_factor & 0x7) == GR_COMBINE_FACTOR_LOCAL_ALPHA))
    gc->state.tmuMask |= tmuMask;

  /* setup invert output bits */
  if (rgb_invert) texturemode |= SST_TC_INVERT_OUTPUT;
  if (alpha_invert) texturemode |= SST_TCA_INVERT_OUTPUT;

  /* setup core color combine unit bits */
  switch (rgb_function) {
  case GR_COMBINE_FUNCTION_ZERO:
    texturemode |= SST_TC_ZERO_OTHER;
    localColorP = FXTRUE;
    break;

  case GR_COMBINE_FUNCTION_LOCAL:
    texturemode |= SST_TC_ZERO_OTHER | SST_TC_ADD_CLOCAL;
    
    gc->state.tmuMask |= tmuMask;
    localColorP = FXTRUE;
    break;

  case GR_COMBINE_FUNCTION_LOCAL_ALPHA:
    texturemode |= SST_TC_ZERO_OTHER | SST_TC_ADD_ALOCAL;
    gc->state.tmuMask |= tmuMask;
    localColorP = FXTRUE;
    break;

  case GR_COMBINE_FUNCTION_SCALE_OTHER:
    break;

  case GR_COMBINE_FUNCTION_SCALE_OTHER_ADD_LOCAL:
    texturemode |= SST_TC_ADD_CLOCAL;
    gc->state.tmuMask |= tmuMask;
    break;

  case GR_COMBINE_FUNCTION_SCALE_OTHER_ADD_LOCAL_ALPHA:
    texturemode |= SST_TC_ADD_ALOCAL;
    gc->state.tmuMask |= tmuMask;
    break;

  case GR_COMBINE_FUNCTION_SCALE_OTHER_MINUS_LOCAL:
    texturemode |= SST_TC_SUB_CLOCAL;
    gc->state.tmuMask |= tmuMask;
    break;

  case GR_COMBINE_FUNCTION_SCALE_OTHER_MINUS_LOCAL_ADD_LOCAL:
    texturemode |= SST_TC_SUB_CLOCAL | SST_TC_ADD_CLOCAL;
    gc->state.tmuMask |= tmuMask;
    break;

  case GR_COMBINE_FUNCTION_SCALE_OTHER_MINUS_LOCAL_ADD_LOCAL_ALPHA:
    texturemode |= SST_TC_SUB_CLOCAL | SST_TC_ADD_ALOCAL;
    gc->state.tmuMask |= tmuMask;
    break;

  case GR_COMBINE_FUNCTION_SCALE_MINUS_LOCAL_ADD_LOCAL:
    texturemode |= SST_TC_ZERO_OTHER | SST_TC_SUB_CLOCAL | SST_TC_ADD_CLOCAL;
    gc->state.tmuMask |= tmuMask;
    localColorP = ((rgb_factor & 0x07UL) != GR_COMBINE_FACTOR_OTHER_ALPHA);
    break;

  case GR_COMBINE_FUNCTION_SCALE_MINUS_LOCAL_ADD_LOCAL_ALPHA:
    texturemode |= SST_TC_ZERO_OTHER | SST_TC_SUB_CLOCAL | SST_TC_ADD_ALOCAL;
    gc->state.tmuMask |= tmuMask;
    localColorP = ((rgb_factor & 0x07UL) != GR_COMBINE_FACTOR_OTHER_ALPHA);
    break;
  }
  
  switch (alpha_function) {
  case GR_COMBINE_FUNCTION_ZERO:
    texturemode |= SST_TCA_ZERO_OTHER;
    localAlphaP = FXTRUE;
    break;

  case GR_COMBINE_FUNCTION_LOCAL:
    texturemode |= SST_TCA_ZERO_OTHER | SST_TCA_ADD_CLOCAL;
    gc->state.tmuMask |= tmuMask;
    localAlphaP = FXTRUE;
    break;

  case GR_COMBINE_FUNCTION_LOCAL_ALPHA:
    texturemode |= SST_TCA_ZERO_OTHER | SST_TCA_ADD_ALOCAL;
    gc->state.tmuMask |= tmuMask;
    localAlphaP = FXTRUE;
    break;

  case GR_COMBINE_FUNCTION_SCALE_OTHER:
    break;

  case GR_COMBINE_FUNCTION_SCALE_OTHER_ADD_LOCAL:
    texturemode |= SST_TCA_ADD_CLOCAL;
    gc->state.tmuMask |= tmuMask;
    break;

  case GR_COMBINE_FUNCTION_SCALE_OTHER_ADD_LOCAL_ALPHA:
    texturemode |= SST_TCA_ADD_ALOCAL;
    gc->state.tmuMask |= tmuMask;
    break;

  case GR_COMBINE_FUNCTION_SCALE_OTHER_MINUS_LOCAL:
    texturemode |= SST_TCA_SUB_CLOCAL;
    gc->state.tmuMask |= tmuMask;
    break;

  case GR_COMBINE_FUNCTION_SCALE_OTHER_MINUS_LOCAL_ADD_LOCAL:
    texturemode |= SST_TCA_SUB_CLOCAL | SST_TCA_ADD_CLOCAL;
    gc->state.tmuMask |= tmuMask;
    break;

  case GR_COMBINE_FUNCTION_SCALE_OTHER_MINUS_LOCAL_ADD_LOCAL_ALPHA:
    texturemode |= SST_TCA_SUB_CLOCAL | SST_TCA_ADD_ALOCAL;
    gc->state.tmuMask |= tmuMask;
    break;

  case GR_COMBINE_FUNCTION_SCALE_MINUS_LOCAL_ADD_LOCAL:
    texturemode |= SST_TCA_ZERO_OTHER | SST_TCA_SUB_CLOCAL | SST_TCA_ADD_CLOCAL;
    gc->state.tmuMask |= tmuMask;
    localAlphaP = ((alpha_factor & 0x07UL) != GR_COMBINE_FACTOR_OTHER_ALPHA);
    break;

  case GR_COMBINE_FUNCTION_SCALE_MINUS_LOCAL_ADD_LOCAL_ALPHA:
    texturemode |= SST_TCA_ZERO_OTHER | SST_TCA_SUB_CLOCAL | SST_TCA_ADD_ALOCAL;
    gc->state.tmuMask |= tmuMask;
    localAlphaP = ((alpha_factor & 0x07UL) != GR_COMBINE_FACTOR_OTHER_ALPHA);
    break;
  }
  
  //gc->state.tcc_requires_prev_texture[tmu] = !localColorP;
  //gc->state.tac_requires_prev_texture[tmu] = !localAlphaP;
  
  /* Hack to enable TWO-PASS Trilinear */
  if (texturemode & SST_TRILINEAR) {
    if ((texturemode & SST_TC_ZERO_OTHER) &&
        (texturemode & SST_TC_BLEND_LODFRAC) &&
        !(texturemode & SST_TC_REVERSE_BLEND)) {
      tLod |= SST_LOD_ODD;
    }
  }

  /* Hack to use single pass trilinear in UMA state */
  if((texturemode & SST_TRILINEAR) &&
     (_GlideRoot.environment.enUma == FXTRUE) &&
     (gc->state.tmu_config[tmu].evenOdd == GR_MIPMAPLEVELMASK_BOTH) &&
     (tmu == GR_TMU1)) {
    tLod |= SST_LOD_ODD;
  }
  
  tLod |= _gr_evenOdd_xlate_table[gc->state.tmu_config[tmu].evenOdd];

  /* Update shadows */
  GR_SET_EXPECTED_SIZE(sizeof(FxU32)*1, 1);
  GR_SET((0x02UL << tmu), SST_TMU(hw, tmu), textureMode, texturemode);
  gc->state.tmu_config[tmu].textureMode = texturemode;
  gc->state.tmu_config[tmu].tLOD        = tLod;

#if 1
  /* If the state of a tmu changes from active then make sure that it
   * is addressing some valid tiny texture so that it does not spend
   * time thrashing on a large texture access.
   *
   * NB: We don't update the shadow here so the other bits in the
   * register shadow should be unchanged.
   */
  {
    const FxI32 upstreamTmu = tmu + 1;

    if (upstreamTmu < gc->num_tmu) {
      if (localColorP && localAlphaP) {
        SstRegs* tmuHw = SST_TMU(hw, upstreamTmu);
        
        GR_SET_EXPECTED_SIZE(sizeof(FxU32), 1);
        GR_SET((0x02UL << upstreamTmu), tmuHw, tLOD, SST_TLOD_MINMAX_INT(GR_LOD_1, GR_LOD_1));
        GR_CHECK_SIZE();
        
        gc->tmuLodDisable[upstreamTmu] = FXTRUE;
        
        GDBG_INFO(gc->myLevel, FN_NAME": Disabling tmu(%ld) : (0x%X : 0x%X)\n", 
                  upstreamTmu, rgb_function, alpha_function);

        if(IS_NAPALM(gc->bInfo->pciInfo.deviceID) && (gc->do2ppc))
          _grTex2ppc(FXTRUE);
        
      } else if (gc->tmuLodDisable[upstreamTmu]) {
        SstRegs* tmuHw = SST_TMU(hw, upstreamTmu);
        
        GR_SET_EXPECTED_SIZE(sizeof(FxU32), 1);
        GR_SET((0x02UL << upstreamTmu), tmuHw, tLOD, gc->state.tmu_config[upstreamTmu].tLOD);
        GR_CHECK_SIZE();
        
        gc->tmuLodDisable[upstreamTmu] = FXFALSE;

        if(IS_NAPALM(gc->bInfo->pciInfo.deviceID) && (gc->do2ppc))
          _grTex2ppc(FXFALSE);
        
      }
    } else {
      if(IS_NAPALM(gc->bInfo->pciInfo.deviceID) && (gc->do2ppc))
          _grTex2ppc(FXFALSE);
    }
    /* If this tmu is inactive then also do the reset for the register
     * write, but not in the shadow so that the remaining bits are
     * retained.
     */
    if ((gc->state.tmuMask & tmuMask) == 0) {
      tLod = SST_TLOD_MINMAX_INT(GR_LOD_1, GR_LOD_1);
    }
  }
#endif
  
  /* update register */
  {
    SstRegs* tmuHw = SST_TMU(hw, tmu);
    
    REG_GROUP_BEGIN((0x02 << tmu), textureMode, 2, 0x3);
    {
      REG_GROUP_SET(tmuHw, textureMode , texturemode);
      REG_GROUP_SET(tmuHw, tLOD, tLod);
    }
    REG_GROUP_END();
  }

#if GLIDE_CHECK_TRILINEAR
  /* Make sure that the trilinear blending bits are set in a
   * consistent manner across the tmu's.  This only really matters if
   * we have multiple tmu's, but this state is really setup across
   * multiple calls (grTexCombine, grTexMipMapMode, and
   * grTexSource).  
   * 
   * NB: This must happen after the shadows are updated because
   * _grTexCheckTriLinear() will use the shadow register values
   * to determine the current state of trilinearness.
   */
  if (gc->num_tmu > 1) _grTexCheckTriLinear(tmu);
#endif /* GLIDE_CHECK_TRILINEAR */

  GR_END();
#undef FN_NAME
} /* grTexCombine */

/*
** _grTexDetailControl, NOTE: its up to caller to account for bytes
*/
GR_DDFUNC(_grTexDetailControl, void, (GrChipID_t tmu, FxU32 detail))
{
#define FN_NAME "_grTexDetailControl"
  GR_BEGIN_NOFIFOCHECK(FN_NAME,88);
  GDBG_INFO_MORE(gc->myLevel, "(%d, 0x%X)\n", tmu, detail);
  GR_CHECK_TMU(FN_NAME, tmu);

  gc->state.tmu_config[tmu].tDetail = detail;
  
  if(!gc->do2ppc/*gc->state.mode2ppc*/) {
    //if(IS_NAPALM(gc->bInfo->pciInfo.deviceID)) _grChipMask( SST_CHIP_MASK_ALL_CHIPS );
    GR_SET_EXPECTED_SIZE(sizeof(FxU32)*1, 1);
    GR_SET((0x02UL << tmu), SST_TMU(hw, tmu), tDetail, detail);
    //if(IS_NAPALM(gc->bInfo->pciInfo.deviceID)) _grChipMask( gc->chipmask );
  }

  GR_END();
#undef FN_NAME
} /* _grTexDetailControl */

/*---------------------------------------------------------------------------
** grTexFilterMode
*/

GR_ENTRY(grTexFilterMode, void, 
         (GrChipID_t tmu, GrTextureFilterMode_t minfilter, GrTextureFilterMode_t magfilter))
{
#define FN_NAME "grTexFilterMode"
  FxU32 texMode;

  GR_BEGIN_NOFIFOCHECK("grTexFilterMode",99);
  GDBG_INFO_MORE(gc->myLevel,"(%d,%d,%d)\n",tmu,minfilter,magfilter);
  GR_CHECK_TMU(FN_NAME, tmu);

  texMode  = gc->state.tmu_config[tmu].textureMode;
  texMode &= ~(SST_TMINFILTER | SST_TMAGFILTER);
  texMode |= (minfilter == GR_TEXTUREFILTER_BILINEAR ? SST_TMINFILTER : 0) |
             (magfilter == GR_TEXTUREFILTER_BILINEAR ? SST_TMAGFILTER : 0);

  gc->state.tmu_config[tmu].textureMode = texMode;
  if(gc->do2ppc/*gc->state.mode2ppc*/) gc->state.tmu_config[1-tmu].textureMode = texMode;
  
  /* Update real shadows and update hardware immediately if we can. */
  if(!gc->do2ppc/*gc->state.mode2ppc*/) {
    //if(IS_NAPALM(gc->bInfo->pciInfo.deviceID)) _grChipMask( SST_CHIP_MASK_ALL_CHIPS );
    REG_GROUP_BEGIN((0x02 << tmu), textureMode, 1, 0x1);
    {
      REG_GROUP_SET(SST_TMU(hw, tmu), textureMode, texMode);
    }
    REG_GROUP_END();
    //if(IS_NAPALM(gc->bInfo->pciInfo.deviceID)) _grChipMask( gc->chipmask );
  }

  GR_END();
#undef FN_NAME
} /* grTexFilterMode */

/*---------------------------------------------------------------------------
** grTexLodBiasValue
*/

GR_ENTRY(grTexLodBiasValue, void, 
         (GrChipID_t tmu, float fvalue))
{
#define FN_NAME "grTexLodBiasValue"
  FxU32 tLod;
  FxI32 lodBias;
  //GrChipID_t tmus;

  GR_BEGIN_NOFIFOCHECK("grTexLodBiasValue",88);
  GDBG_INFO_MORE(gc->myLevel,"(%d,%g)\n",tmu,fvalue);
  GR_CHECK_TMU(FN_NAME, tmu);
  
  tLod = gc->state.tmu_config[tmu].tLOD;
  tLod &= ~(SST_LODBIAS);
  lodBias = _grTexFloatLODToFixedLOD(fvalue);
  /* Sign extend it. */
  lodBias = ((lodBias << (32-6)) >> (32-6));
  lodBias += _GlideRoot.environment.lodBias;
  if(lodBias > 0x1f) lodBias = 0x1f;
  else if(lodBias < -0x20) lodBias = -0x20;
  /* Mask it back off. */
  lodBias &= 0x3f;
  tLod |= lodBias << SST_LODBIAS_SHIFT;
  
  gc->state.tmu_config[tmu].tLOD = tLod;
  
  /* Update real shadows and update hardware immediately if we can. */
  if(!gc->do2ppc/*gc->state.mode2ppc*/) {
    //if(IS_NAPALM(gc->bInfo->pciInfo.deviceID)) _grChipMask( SST_CHIP_MASK_ALL_CHIPS );
    REG_GROUP_BEGIN((0x02 << tmu), tLOD, 1, 0x1);
    {
      REG_GROUP_SET(SST_TMU(hw, tmu), tLOD, tLod);
    }
    REG_GROUP_END();
    //if(IS_NAPALM(gc->bInfo->pciInfo.deviceID)) _grChipMask( gc->chipmask );
    if(gc->state.tmu_config[tmu].texSubLodDither)
      g2LodBiasPerChip(tmu, gc->state.tmu_config[tmu].tLOD);
  }

  /*tmus = MultitextureAndTrilinear();
  if(tmus & tmu) g2LodBiasPerChip(tmu);*/
  
  GR_END();
#undef FN_NAME
} /* grTexLodBiasValue */

/*-------------------------------------------------------------------
  Function: grTexMipMapMode
  Date: 6/2
  Implementor(s): GaryMcT, Jdt
  Library: glide
  Description:
    Sets the mip map mode for the specified TMU
    "Ex" because glide's grTexMipMapMode is inadequate for 
    low level texture memory management
  Arguments:
    tmu       - tmu to update
    mmMode   - mipmap mode 
      One of:
        GR_MIPMAP_DISABLE
        GR_MIPMAP_NEAREST
        GR_MIPMAP_NEAREST_DITHER
    lodBlend - enable lodBlending
      FXTRUE  - enabled
      FXFALSE - disabled
  Return:
  none
  -------------------------------------------------------------------*/

GR_ENTRY(grTexMipMapMode, void, 
         (GrChipID_t tmu, GrMipMapMode_t mmMode, FxBool lodBlend))
{
#define FN_NAME "grTexMipMapMode"
  FxU32
    tLod,
    texMode;
  FxBool tBig = FXFALSE;
  //GrChipID_t tmus;

  GR_BEGIN_NOFIFOCHECK("grTexMipMapMode",88);
  GDBG_INFO_MORE(gc->myLevel,"(%d,%d,%d)\n",tmu,mmMode,lodBlend);
  GR_CHECK_TMU(FN_NAME, tmu);
  
  /*--------------------------------------------------------------
    Get Current tLod and texMode register values
    --------------------------------------------------------------*/
  tLod  = gc->state.tmu_config[tmu].tLOD;
  texMode = gc->state.tmu_config[tmu].textureMode;

  /*--------------------------------------------------------------
    Do we want big (>256) textures or not?
    --------------------------------------------------------------*/
/*#ifdef FX_GLIDE_NAPALM
  if ((IS_NAPALM(gc->bInfo->pciInfo.deviceID)) && (tLod & SST_TBIG)) {
    tBig = FXTRUE;
  }
#endif*/

  /*--------------------------------------------------------------
    Clear LODMIN, LODMAX and LODDITHER
    --------------------------------------------------------------*/
  tLod    &= ~(SST_LODMIN | SST_LODMAX | SST_LOD_ODD);
  texMode &= ~(SST_TLODDITHER | SST_TRILINEAR);

  /*--------------------------------------------------------------
    Encode Mipmap Mode Bits
    --------------------------------------------------------------*/
  switch (mmMode) {
  case GR_MIPMAP_DISABLE:
    /*----------------------------------------------------------
      To disable mipmapping set the min and max lods to the same
      value
      ----------------------------------------------------------*/
    tLod |= SST_TLOD_MINMAX_INT(gc->state.tmu_config[tmu].largeLod,
                                gc->state.tmu_config[tmu].largeLod);
    break;

  case GR_MIPMAP_NEAREST_DITHER:
    if (gc->state.allowLODdither) texMode |= SST_TLODDITHER;
    /* intentional fall-through to set lodmin and lodmax values */

  case GR_MIPMAP_NEAREST:
    /*----------------------------------------------------------
      Set LODMIN and LODMAX in the tLod register to the 
      actual min and max LODs of the current texture.
      ----------------------------------------------------------*/
    tLod |= SST_TLOD_MINMAX_INT(gc->state.tmu_config[tmu].largeLod,
                                gc->state.tmu_config[tmu].smallLod);
    if(_GlideRoot.environment.texSubLodDither && !lodBlend)
      gc->state.tmu_config[tmu].texSubLodDither = FXTRUE;
    break;

  default:
    GrErrorCallback("grTexMipMapMode:  invalid mode passed", FXFALSE);
    break;
  }
  
  gc->state.tmu_config[tmu].mmMode = mmMode;

  /* Force LOD dithering if the user asked for it.
   *
   * NB: There is a performance hit for this, but it does
   * look better.
   */
  texMode |= _GlideRoot.environment.texLodDither;

  /*--------------------------------------------------------------
    Fix trilinear and evenOdd bits -

    This is a bit of a hack to make two pass trilinear work with
    full textures.  The assumption here is that the only reason
    you would ever set up Multiply by LODFRAC w/o REVERSE BLEND
    is for the ODD pass of trilinear.  
    --------------------------------------------------------------*/
  if (lodBlend) {
    /* If we're doing trilinear for real then nuke the lod dithering
     * at the same time because it just looks bad.  
     */
    texMode = ((texMode & ~SST_TLODDITHER) | SST_TRILINEAR);

    if ((texMode & SST_TC_ZERO_OTHER) &&
        (texMode & SST_TC_BLEND_LODFRAC) &&
        !(texMode & SST_TC_REVERSE_BLEND)) {
        tLod |= SST_LOD_ODD;
    }
  }
  tLod |= _gr_evenOdd_xlate_table[gc->state.tmu_config[tmu].evenOdd];
  
  /*--------------------------------------------------------------
    Write State To Hardware and Update Glide Shadow State
    --------------------------------------------------------------*/
  gc->state.tmu_config[tmu].tLOD        = tLod;
  gc->state.tmu_config[tmu].textureMode = texMode;

  if(gc->do2ppc/*gc->state.mode2ppc*/) {
    gc->state.tmu_config[1-tmu].textureMode = texMode;
    gc->state.tmu_config[1-tmu].tLOD = tLod;
  }

  /* Update real shadows and update hardware immediately if we can. */
  if(!gc->do2ppc/*gc->state.mode2ppc*/) {
    //if(IS_NAPALM(gc->bInfo->pciInfo.deviceID)) _grChipMask( SST_CHIP_MASK_ALL_CHIPS );
    REG_GROUP_BEGIN((0x02 << tmu), textureMode, 2, 0x3);
    {
      REG_GROUP_SET(SST_TMU(hw, tmu), textureMode, gc->state.tmu_config[tmu].textureMode);
      REG_GROUP_SET(SST_TMU(hw, tmu), tLOD, gc->state.tmu_config[tmu].tLOD);
    }
    REG_GROUP_END();
    //if(IS_NAPALM(gc->bInfo->pciInfo.deviceID)) _grChipMask( gc->chipmask );
    if(gc->state.tmu_config[tmu].texSubLodDither)
      g2LodBiasPerChip(tmu, gc->state.tmu_config[tmu].tLOD);
  }

#if GLIDE_CHECK_TRILINEAR
  /* Make sure that the trilinear blending bits are set in a
   * consistent manner across the tmu's.  This only really matters if
   * we have multiple tmu's, but this state is really setup across
   * multiple calls (grTexCombine, grTexMipMapMode, and
   * grTexSource).  
   * 
   * NB: This must happen after the shadows are updated because
   * _grTexCheckTriLinear() will use the shadow register values
   * to determine the current state of trilinearness.
   */
  if (gc->num_tmu > 1) _grTexCheckTriLinear(tmu);
#endif /* GLIDE_CHECK_TRILINEAR */

  /*tmus = MultitextureAndTrilinear();
  if(tmus & tmu) g2LodBiasPerChip(tmu);*/

  GR_END();
#undef FN_NAME
} /* grTexMipMapMode */

/*-------------------------------------------------------------------
  Function: grTexMinAddress
  Date: 6/2
  Implementor(s): GaryMcT, Jdt
  Library: glide
  Description:
    Returns address of start of texture ram for a TMU
  Arguments:
    tmu
  Return:
    integer texture base address, this pointer is not to be dereferenced
    by the application, it is on to be used by grTexDownload(),
    and grTExDownloadLevel()
  -------------------------------------------------------------------*/
/*-------------------------------------------------------------------
  Function: grTexNCCTable
  Date: 6/3
  Implementor(s): jdt
  Library: glide
  Description:
    select one of the two NCC tables
  Arguments:
    tmu - which tmu
    table - which table to select
        One of:
            GR_TEXTABLE_NCC0
            GR_TEXTABLE_NCC1
            GR_TEXTABLE_PALETTE
  Return:
    none
  -------------------------------------------------------------------*/

#if defined(GLIDE3) && defined(GLIDE3_ALPHA)
GR_ENTRY(grTexNCCTable, void, 
         (GrNCCTable_t table))
{
#define FN_NAME "grTexNCCTable"
  FxU32 texMode, i;
  
  GR_BEGIN("grTexNCCTable",88,4, 1);
  GDBG_INFO_MORE(gc->myLevel,"\n");
  GR_CHECK_F(myName, table > GR_TEXTABLE_PALETTE, "invalid ncc table specified");

  /*------------------------------------------------------------------
    Update local state
    ------------------------------------------------------------------*/
  for (i = 0; i < GLIDE_NUM_TMU; i++)
    gc->state.tmu_config[i].nccTable = table;
  
  /*------------------------------------------------------------------
    Grab shadow texMode, update TexMode, update shadow/real register
    ------------------------------------------------------------------*/
  for (i = 0; i < GLIDE_NUM_TMU; i++)
    texMode  = gc->state.tmu_config[i].textureMode;
  texMode &= ~(SST_TNCCSELECT);
  if (table == GR_TEXTABLE_NCC1)
    texMode |= SST_TNCCSELECT;
  else 
    texMode &= ~(SST_TNCCSELECT);

  for (i = 0; i < GLIDE_NUM_TMU; i++) {
    GR_SET((0x02UL << i), SST_TMU(hw, i), textureMode, texMode);
    gc->state.tmu_config[i].textureMode = texMode;
  }

  GR_END();
#undef FN_NAME
} /* grTexNCCTable */
#else
GR_ENTRY(grTexNCCTable, void, 
         (GrChipID_t tmu, GrNCCTable_t table))
{
#define FN_NAME "grTexNCCTable"
  FxU32 texMode;
  
  GR_BEGIN("grTexNCCTable",88,4, 1);
  GDBG_INFO_MORE(gc->myLevel,"(%d)\n",tmu);
  GR_CHECK_TMU(FN_NAME, tmu);
  GR_CHECK_F(FN_NAME, table > GR_TEXTABLE_PALETTE, "invalid ncc table specified");

  /*------------------------------------------------------------------
    Update local state
    ------------------------------------------------------------------*/
  gc->state.tmu_config[tmu].nccTable = table;
  
  /*------------------------------------------------------------------
    Grab shadow texMode, update TexMode, update shadow/real register
    ------------------------------------------------------------------*/
  texMode  = gc->state.tmu_config[tmu].textureMode;
  texMode &= ~(SST_TNCCSELECT);
  if (table == GR_TEXTABLE_NCC1)
    texMode |= SST_TNCCSELECT;
  else 
    texMode &= ~(SST_TNCCSELECT);

  if(!gc->do2ppc/*gc->state.mode2ppc*/) {
    //if(IS_NAPALM(gc->bInfo->pciInfo.deviceID)) _grChipMask( SST_CHIP_MASK_ALL_CHIPS );
    GR_SET((0x02UL << tmu), SST_TMU(hw, tmu), textureMode, texMode);
    gc->state.tmu_config[tmu].textureMode = texMode;
    //if(IS_NAPALM(gc->bInfo->pciInfo.deviceID)) _grChipMask( gc->chipmask );
  }

  GR_END();
#undef FN_NAME
} /* grTexNCCTable */
#endif

/*-------------------------------------------------------------------
  Function: grTexSource
  Date: 6/2
  Implementor(s): GaryMcT, Jdt
  Library: glide
  Description:
    Sets up the current texture for texture mapping on the specified
    TMU.
  Arguments:
    tmu          - which tmu
    startAddress - texture start address
    evenOdd  - which set of mipmap levels have been downloaded for
                the selected texture
                One of:
                  GR_MIPMAPLEVELMASK_EVEN 
                  GR_MIPMAPLEVELMASK_ODD
                  GR_MIPMAPLEVELMASK_BOTH
    info         - pointer to GrTexInfo structure containing
                   texture dimensions
  Return:
    none
  -------------------------------------------------------------------*/

GR_ENTRY(grTexSource, void, 
         (GrChipID_t tmu, FxU32 startAddress, FxU32 evenOdd, GrTexInfo *info))
{
#define FN_NAME "grTexSource"
  FxU32 baseAddress, texMode, tLod;
  FxBool tBig = FXFALSE, doNOP = FXFALSE;
  //GrChipID_t tmus;

  GR_BEGIN_NOFIFOCHECK("grTexSource", 88);
  GDBG_INFO_MORE(gc->myLevel, 
                 "(%d, 0x%X, 0x%X, [%d:%d %d %d 0x%X])\n",
                 tmu, startAddress, evenOdd, 
                 info->smallLod, info->largeLod,
                 info->aspectRatio,
                 info->format,
                 info->data);

  GR_CHECK_TMU(FN_NAME, tmu);
  GR_CHECK_COMPATABILITY(FN_NAME, 
                         startAddress >= gc->tmu_state[tmu].total_mem, 
                         "invalid startAddress");

  GR_CHECK_COMPATABILITY(FN_NAME,
                         /*(gc->windowed ? FXFALSE :*/
                          (startAddress +
                           grTexTextureMemRequired(evenOdd, info)
                           >= gc->tmu_state[tmu].total_mem)/*)*/,
                         "insufficient texture ram at startAddress");
  GR_CHECK_F(FN_NAME, evenOdd > 0x3 || evenOdd == 0, "evenOdd mask invalid");

#ifdef FX_GLIDE_NAPALM

  /*--------------------------------------------------------------
    Do we want big (>256) textures or not?
    --------------------------------------------------------------*/
  /*if ((IS_NAPALM(gc->bInfo->pciInfo.deviceID))
      && (info->largeLod > GR_LOD_256)) {
    tBig = FXTRUE;
  }*/
#endif

  /*-------------------------------------------------------------
    Update Texture Unit State
    -------------------------------------------------------------*/
  gc->state.tmu_config[tmu].smallLod = info->smallLod;//_g2LodXlat(info->smallLod, tBig);
  gc->state.tmu_config[tmu].largeLod = info->largeLod;//_g2LodXlat(info->largeLod, tBig);
  gc->state.tmu_config[tmu].evenOdd  = evenOdd;
  
  /* Calculate Base Address
   * texBaseAddr gets the address of the largest mipmap level that the
   * hw is capable of handling (256 in the current sst1 core
   * derivatives) even if some of the larger levels don't exist.  Now,
   * we'll basically calculate the offset (from the beginning) of the
   * largest mipmap level, and subtract that from start_address to
   * calculate the baseAdress.
   *
   * Capiche?  
   */
  //{
    /*const struct GrTmuMemInfo*
      memInfo = gc->tmuMemInfo + tmu;

    if (memInfo->texTiled) {
      const FxI32 
        baseAddressAdj = _grTexCalcBaseAddressTiled(tmu, startAddress,
                                                    info->aspectRatio,
                                                    info->largeLod, 
                                                    info->format,                                                     
                                                    evenOdd);
      baseAddress = memInfo->tramOffset + baseAddressAdj;
      baseAddress = SST_TEXTURE_MUNGE_ADDRESS(baseAddress);

      baseAddress |= SST_TEXTURE_IS_TILED;
      baseAddress |= (memInfo->texStrideTiles << SST_TEXTURE_TILESTRIDE_SHIFT);
      
      GR_CHECK_F(FN_NAME, (gc->windowed && (startAddress != 0)),
                 "Surface extension only supports startAddress 0\n");
    }  else {*/
      baseAddress = (FxI32)_grTexCalcBaseAddress(startAddress,
                                                 info->largeLod,
                                                 info->aspectRatio,
                                                 info->format,
                                                 evenOdd);
      baseAddress += gc->tramOffset[tmu];
      baseAddress &= SST_TEXTURE_ADDRESS;
    /*}*/
  //}
  
  /*-------------------------------------------------------------
    Update Texture Mode
    -------------------------------------------------------------*/
#if 0
  {
    GrTextureFormat_t format = info->format;
    FxU32 oldTexMode;
    oldTexMode = texMode = gc->state.tmu_config[tmu].textureMode;
    texMode &= ~SST_TFORMAT;

#ifdef FX_GLIDE_NAPALM
    texMode &= ~SST_COMPRESSED_TEXTURES;
#endif

#if 0    
    if ((format == GR_TEXFMT_P_8) ||
        (format == GR_TEXFMT_YIQ_422) ||
        (format == GR_TEXFMT_AYIQ_8422) ||
        (format == GR_TEXFMT_AP_88)) {
      gc->state.palletizedTexture[tmu] = FXTRUE;
    } else {
      gc->state.palletizedTexture[tmu] = FXFALSE;
    }      
#endif
    
    /* Adjust texture format to match the current texture table type */
#ifdef FX_GLIDE_NAPALM
    /* Glide uses a different value than the HW's textureMode reg */
    switch(format) {
    case GR_TEXFMT_P_8:
      format = ((gc->state.tex_table == GR_TEXTABLE_PALETTE_6666_EXT)
                ? GR_TEXFMT_P_8_RGBA
                : GR_TEXFMT_P_8);
      break;
    case GR_TEXFMT_ARGB_8888:
      format = (SST_ARGB8888 >> SST_TFORMAT_SHIFT);
      break;
    case GR_TEXFMT_ARGB_CMP_FXT1:
      format = (SST_3DFX_COMPRESSED >> SST_TFORMAT_SHIFT);
      texMode |= SST_COMPRESSED_TEXTURES;
      break;
    case GR_TEXFMT_ARGB_CMP_DXT1:
      format = (SST_DXT1 >> SST_TFORMAT_SHIFT);
      texMode |= SST_COMPRESSED_TEXTURES;
      break;
    case GR_TEXFMT_ARGB_CMP_DXT2:
      format = (SST_DXT2 >> SST_TFORMAT_SHIFT);
      texMode |= SST_COMPRESSED_TEXTURES;
      break;
    case GR_TEXFMT_ARGB_CMP_DXT3:
      format = (SST_DXT3 >> SST_TFORMAT_SHIFT);
      texMode |= SST_COMPRESSED_TEXTURES;
      break;
    case GR_TEXFMT_ARGB_CMP_DXT4:
      format = (SST_DXT4 >> SST_TFORMAT_SHIFT);
      texMode |= SST_COMPRESSED_TEXTURES;
      break;
    case GR_TEXFMT_ARGB_CMP_DXT5:
      format = (SST_DXT5 >> SST_TFORMAT_SHIFT);
      texMode |= SST_COMPRESSED_TEXTURES;
      break;
    }
#else
    if (format == GR_TEXFMT_P_8) {
      format = ((gc->state.tex_table == GR_TEXTABLE_PALETTE_6666_EXT)
                ? GR_TEXFMT_P_8_RGBA
                : GR_TEXFMT_P_8);
    }
#endif

    /* There seems to be a minor hardware bug when switching from a
     * non-compressed texture to a compressed one.  We have to idle
     * the TMUs when this transition happens. */
    if((texMode ^ oldTexMode) & SST_COMPRESSED_TEXTURES & texMode) {
      doNOP = FXTRUE;
    }

    texMode |= (SST_TPERSP_ST | 
                SST_TCLAMPW |
                (format << SST_TFORMAT_SHIFT));
  }
#endif
  texMode = gc->state.tmu_config[tmu].textureMode;
  texMode &= ~SST_TFORMAT;
  texMode |= (info->format << SST_TFORMAT_SHIFT) | SST_TPERSP_ST | SST_TCLAMPW;
  /*-------------------------------------------------------------
    Compute TLOD (keep LODBIAS intact)
    -------------------------------------------------------------*/
  {
    const FxU32 
      smallLOD    = info->smallLod,//_g2LodXlat(info->smallLod, tBig),
      largeLOD    = info->largeLod,//_g2LodXlat(info->largeLod, tBig),
      aspectRatio = info->aspectRatio;
    
    tLod = gc->state.tmu_config[tmu].tLOD;
    tLod &= ~(SST_LODMIN | SST_LODMAX | SST_LOD_ASPECT |
      SST_LOD_TSPLIT | SST_LOD_ODD | SST_LOD_S_IS_WIDER);

/*#ifdef FX_GLIDE_NAPALM
    if (tBig) {
      tLod |= SST_TBIG;
    } else {
      tLod &= ~SST_TBIG;
    }
#endif*/

    if (gc->state.tmu_config[tmu].mmMode == GR_MIPMAP_DISABLE )
      tLod |= SST_TLOD_MINMAX_INT(largeLOD, largeLOD);
    else
      tLod |= SST_TLOD_MINMAX_INT(largeLOD, smallLOD);

    tLod |= _gr_evenOdd_xlate_table[evenOdd];
    tLod |= _gr_aspect_xlate_table[aspectRatio];
  }

  /* update shadows */
  gc->state.tmu_config[tmu].textureMode = texMode;
  gc->state.tmu_config[tmu].tLOD = tLod;
  gc->state.tmu_config[tmu].texBaseAddr = baseAddress;
  /* If in 2PPC, then update both "real" TMU register shadows. */
  if(gc->do2ppc/*gc->state.mode2ppc*/) {
    gc->state.tmu_config[1-tmu].textureMode = texMode;
    gc->state.tmu_config[1-tmu].tLOD = tLod;
    gc->state.tmu_config[1-tmu].texBaseAddr = baseAddress;
  }    

  /* Update real shadows and update hardware immediately if we can. */
  /*if(doNOP) {
      GR_SET_EXPECTED_SIZE(sizeof(FxU32)*1, 1);
      GR_SET(eChipTMU0 | eChipTMU1, hw, nopCMD,  0);
      GR_CHECK_SIZE();
    }    */
  if(!gc->do2ppc/*gc->state.mode2ppc*/) {
    //if(IS_NAPALM(gc->bInfo->pciInfo.deviceID)) _grChipMask( SST_CHIP_MASK_ALL_CHIPS );
    REG_GROUP_BEGIN((0x02UL << tmu), textureMode, 3, 0x0B);
    {
      REG_GROUP_SET(SST_TMU(hw, tmu), textureMode, texMode);
      REG_GROUP_SET(SST_TMU(hw, tmu), tLOD, tLod);
      REG_GROUP_SET(SST_TMU(hw, tmu), texBaseAddr, baseAddress);
    }
    REG_GROUP_END();
    //if(IS_NAPALM(gc->bInfo->pciInfo.deviceID)) _grChipMask( gc->chipmask );
    if(gc->state.tmu_config[tmu].texSubLodDither)
      g2LodBiasPerChip(tmu, gc->state.tmu_config[tmu].tLOD);
  }

  /* Synch flush data w/ new state */
  //gc->tmuMemInfo[tmu].postPacket[1] = ~baseAddress;
  //gc->tmuMemInfo[tmu].postPacket[5] = baseAddress;

#if GLIDE_CHECK_TRILINEAR
  /* Make sure that the trilinear blending bits are set in a
   * consistent manner across the tmu's.  This only really matters if
   * we have multiple tmu's, but this state is really setup across
   * multiple calls (grTexCombine, grTexMipMapMode, and
   * grTexSource).  
   * 
   * NB: This must happen after the shadows are updated because
   * _grTexCheckTriLinear() will use the shadow register values
   * to determine the current state of trilinearness.
   */
  if (gc->num_tmu > 1) _grTexCheckTriLinear(tmu);
#endif /* GLIDE_CHECK_TRILINEAR */

  /*
  ** Update s and t scale for clip coordinates
  */
  /*switch (info->aspectRatio) {
  case GR_ASPECT_8x1:
    gc->state.tmu_config[tmu].s_scale = 256.f;
    gc->state.tmu_config[tmu].t_scale = 32.f;
    break;
  case GR_ASPECT_4x1:
    gc->state.tmu_config[tmu].s_scale = 256.f;
    gc->state.tmu_config[tmu].t_scale = 64.f;
    break;
  case GR_ASPECT_2x1:
    gc->state.tmu_config[tmu].s_scale = 256.f;
    gc->state.tmu_config[tmu].t_scale = 128.f;
    break;
  case GR_ASPECT_1x1:
    gc->state.tmu_config[tmu].s_scale = 256.f;
    gc->state.tmu_config[tmu].t_scale = 256.f;
    break;
  case GR_ASPECT_1x2:
    gc->state.tmu_config[tmu].s_scale = 128.f;
    gc->state.tmu_config[tmu].t_scale = 256.f;
    break;
  case GR_ASPECT_1x4:
    gc->state.tmu_config[tmu].s_scale = 64.f;
    gc->state.tmu_config[tmu].t_scale = 256.f;
    break;
  case GR_ASPECT_1x8:
    gc->state.tmu_config[tmu].s_scale = 32.f;
    gc->state.tmu_config[tmu].t_scale = 256.f;
    break;
  }*/


  /*tmus = MultitextureAndTrilinear();
  if(tmus & tmu) g2LodBiasPerChip(tmu);*/

  GR_END();
#undef FN_NAME
} /* grTexSource */


/*-------------------------------------------------------------------
  Function: grTexMultibase
  Date: 11/4/96
  Implementor(s): gmt
  Library: Glide
  Description:
    Enable multiple base addresses for texturing.
  Arguments:
    tmu    - which tmu
    enable - flag which enables/disables multibase
  Return:
    none
  -------------------------------------------------------------------*/

GR_ENTRY(grTexMultibase, void, 
         (GrChipID_t tmu, FxBool enable))
{
#define FN_NAME "grTexMultibase"
  FxU32 tLod;
  //GrChipID_t tmus;
    
  GR_BEGIN_NOFIFOCHECK("grTexMultibase",88);
  GDBG_INFO_MORE(gc->myLevel,"(%d,%d)\n",tmu,enable);
  GR_CHECK_TMU(FN_NAME,tmu);
  
  tLod  = gc->state.tmu_config[tmu].tLOD;
  if (enable)
    tLod |= SST_TMULTIBASEADDR;
  else
    tLod &= ~SST_TMULTIBASEADDR;
  /*--------------------------------------------------------------
    Write State To Hardware and Update Glide Shadow State
    --------------------------------------------------------------*/
  gc->state.tmu_config[tmu].tLOD = tLod;

  /* Update real shadows and update hardware immediately if we can. */
  if(!gc->do2ppc/*gc->state.mode2ppc*/) {
    //if(IS_NAPALM(gc->bInfo->pciInfo.deviceID)) _grChipMask( SST_CHIP_MASK_ALL_CHIPS );
    REG_GROUP_BEGIN((0x02 << tmu), tLOD, 1, 0x1);
    {
      REG_GROUP_SET(SST_TMU(hw, tmu), tLOD, gc->state.tmu_config[tmu].tLOD);
    }
    REG_GROUP_END();
    //if(IS_NAPALM(gc->bInfo->pciInfo.deviceID)) _grChipMask( gc->chipmask );
    if(gc->state.tmu_config[tmu].texSubLodDither)
      g2LodBiasPerChip(tmu, gc->state.tmu_config[tmu].tLOD);
  }


  /*tmus = MultitextureAndTrilinear();
  if(tmus & tmu)  g2LodBiasPerChip(tmu);*/

  GR_END();
#undef FN_NAME
} /* grTexMultibase */

/*-------------------------------------------------------------------
  Function: grTexMultibaseAddress
  Date: 11/4/96
  Implementor(s): gmt
  Library: Glide
  Description:
    Set the base address for a particular set of mipmaps
  Arguments:
    tmu    - which tmu
    range  - range of lods that are based at this starting address
             One of:
             GR_TEXBASE_2048     \  These are not valid
             GR_TEXBASE_1024      \ for Banshee or     
             GR_TEXBASE_512       / Voodoo3. They are  
             GR_TEXBASE_256_TO_1 /  valid for Napalm.  
             GR_TEXBASE_256
             GR_TEXBASE_128
             GR_TEXBASE_64
             GR_TEXBASE_32_TO_1
    startAddress - start address that data was downloaded to 
                    hardware with using grTexDownload/Level
    info         - pointer to GrTexInfo structure containing
                   texture dimensions
  Return:
    none
  -------------------------------------------------------------------*/

GR_ENTRY(grTexMultibaseAddress, void, 
         (GrChipID_t tmu, GrTexBaseRange_t range, FxU32 startAddress,
          FxU32 evenOdd, GrTexInfo *info))
{
#define FN_NAME "grTexMultibaseAddress"
  GrTexBaseRange_t
    maxPossibleLodRange = GR_TEXBASE_256; /* But see below for Napalm */

  GR_BEGIN_NOFIFOCHECK("grTexMultibaseAddress", 88);
  GDBG_INFO_MORE(gc->myLevel,"(%d, 0x%X, 0x%X)\n", tmu, range, startAddress);

  GR_CHECK_TMU(FN_NAME, tmu);
  if (IS_NAPALM(gc->bInfo->pciInfo.deviceID)) {
    maxPossibleLodRange = GR_TEXBASE_2048;
  }
  GR_CHECK_F(FN_NAME, range > maxPossibleLodRange, "invalid range");
  GR_CHECK_F(FN_NAME, startAddress >= gc->tmu_state[tmu].total_mem,
             "invalid startAddress");
  GR_CHECK_F(FN_NAME, evenOdd > 0x3, "evenOdd mask invalid");
  GR_CHECK_F(FN_NAME, info == NULL, "invalid info pointer");

  /* Write relevant registers out to hardware and shadows */
  {
    GrLOD_t
      largeLevelLod;
    FxU32
      baseAddrRegIndex,
      *addrRegShadow,
      *addrRegShadowHW, *addrRegShadowHWAlt;

    switch (range) {
    case GR_TEXBASE_256:
      largeLevelLod = GR_LOD_256;
      baseAddrRegIndex = (offsetof(SstRegs, texBaseAddr) >> 2UL);
      addrRegShadow = &gc->state.tmu_config[tmu].texBaseAddr;
      addrRegShadowHW = &gc->state.tmu_config[tmu].texBaseAddr;
      addrRegShadowHWAlt = &gc->state.tmu_config[1-tmu].texBaseAddr;
      break;
      
    case GR_TEXBASE_128:
      largeLevelLod = GR_LOD_128;
      baseAddrRegIndex = (offsetof(SstRegs, texBaseAddr1) >> 2UL);
      addrRegShadow = &gc->state.tmu_config[tmu].texBaseAddr_1;
      addrRegShadowHW = &gc->state.tmu_config[tmu].texBaseAddr_1;
      addrRegShadowHWAlt = &gc->state.tmu_config[1-tmu].texBaseAddr_1;
      break;
      
    case GR_TEXBASE_64:
      largeLevelLod = GR_LOD_64;
      baseAddrRegIndex = (offsetof(SstRegs, texBaseAddr2) >> 2UL);
      addrRegShadow = &gc->state.tmu_config[tmu].texBaseAddr_2;
      addrRegShadowHW = &gc->state.tmu_config[tmu].texBaseAddr_2;
      addrRegShadowHWAlt = &gc->state.tmu_config[1-tmu].texBaseAddr_2;
      break;
      
    case GR_TEXBASE_32_TO_1:
      largeLevelLod = GR_LOD_32;
      baseAddrRegIndex = (offsetof(SstRegs, texBaseAddr38) >> 2UL);
      addrRegShadow = &gc->state.tmu_config[tmu].texBaseAddr_3_8;
      addrRegShadowHW = &gc->state.tmu_config[tmu].texBaseAddr_3_8;
      addrRegShadowHWAlt = &gc->state.tmu_config[1-tmu].texBaseAddr_3_8;
      break;
    }
    
    {
      //const struct GrTmuMemInfo*
      //  memInfo = gc->tmuMemInfo + tmu;
      const FifoChipField 
        tmuChip = (FifoChipField)(0x02UL << tmu);
      const FxU32
        baseAddress = (gc->tramOffset[tmu] +
                       _grTexCalcBaseAddress(startAddress,
                                             largeLevelLod,
                                             info->aspectRatio,
                                             info->format,
                                             evenOdd)) & SST_TEXTURE_ADDRESS;
      
      *addrRegShadow = baseAddress;
      
      /* Update real shadows and update hardware immediately if we can. */
      if(!gc->do2ppc/*gc->state.mode2ppc*/) {
        //if(IS_NAPALM(gc->bInfo->pciInfo.deviceID)) _grChipMask( SST_CHIP_MASK_ALL_CHIPS );
        GR_SET_EXPECTED_SIZE(sizeof(FxU32), 1);
        GR_SET_INDEX(tmuChip, SST_TMU(hw, tmu), baseAddrRegIndex, baseAddress);
        GR_CHECK_SIZE();
        //if(IS_NAPALM(gc->bInfo->pciInfo.deviceID)) _grChipMask( gc->chipmask );
      }
      
      /* Synch flush data w/ new state */
      /*if (addrRegShadow == &gc->state.tmu_config[tmu].texBaseAddr) {
        gc->tmuMemInfo[tmu].postPacket[1] = ~baseAddress;
        gc->tmuMemInfo[tmu].postPacket[5] = baseAddress;
      }*/
    }
  }
  
  GR_END();
#undef FN_NAME
} /* grTexMultibaseAddress */

#if defined(GLIDE3)
/*-------------------------------------------------------------------
  Function: grTexChromaModeExt
  Date: 17-Dec-97
  Implementor(s): atai
  Description:
  Draw triangles
  Arguments:
  Return:
  -------------------------------------------------------------------*/
void _grTexChromaModeExt(GrChipID_t tmu, GrChromakeyMode_t mode)
{
#define FN_NAME "_grTexChromaModeExt"

#undef FN_NAME
} /* grTexChromaModeExt */

/*-------------------------------------------------------------------
  Function: grTexChromaRangeExt
  Date: 17-Dec-97
  Implementor(s): atai
  Description:
  Draw triangles
  Arguments:
  Return:
  -------------------------------------------------------------------*/
void _grTexChromaRangeExt(GrChipID_t tmu, GrColor_t min, GrColor_t max)
{
#define FN_NAME "_grTexChromaRangeExt"

#undef FN_NAME
} /* grTexChromaRangeExt */
#endif /* defined(GLIDE3) */

/*
** _grTexForceLod
**
** Forces tLOD to a specific LOD level.  This is useful only for
** debugging purposes.  GMT: obsolete, please remove
** AYT: added it back for Averger.
*/
void 
_grTexForceLod(GrChipID_t tmu, int value)
{
#define FN_NAME "_grTexForceLod"
  GR_DCL_GC;
  GR_DCL_HW;
  FxU32 tLod = gc->state.tmu_config[tmu].tLOD;
  FxBool tBig = FXFALSE;

/*#ifdef FX_GLIDE_NAPALM
  if ((IS_NAPALM(gc->bInfo->pciInfo.deviceID)) && (tLod & SST_TBIG)) {
    tBig = FXTRUE;
  }
#endif*/
  
  //value = _g2LodXlat(value, tBig);

  tLod &= ~(SST_LODMIN | SST_LODMAX);
  tLod |= SST_TLOD_MINMAX_INT(value,value);
  gc->state.tmu_config[tmu].tLOD = tLod;

  if(!gc->do2ppc/*gc->state.mode2ppc*/) {
    //if(IS_NAPALM(gc->bInfo->pciInfo.deviceID)) _grChipMask( SST_CHIP_MASK_ALL_CHIPS );
    GR_SET_EXPECTED_SIZE(sizeof(FxU32), 1);
    GR_SET((0x02UL << tmu), SST_TMU(hw, tmu), tLOD, tLod);
    GR_CHECK_SIZE();
    //if(IS_NAPALM(gc->bInfo->pciInfo.deviceID)) _grChipMask( gc->chipmask );
  }
  
#undef FN_NAME
} /* _grTexForceLod */

#if 0 /* KoolSmoky - removed */
/*-------------------------------------------------------------------
  Function: _g2LodXlat
  Date: 1-Jul-99
  Implementor(s): LEW
  Description:
  Translate LOD value between software's sense to hardware's sense.

  Arguments:
  someLod: A GrLOD_t index to be translated (subtracted from
    GR_LOD_LOG2_2048 if we're using big textures, or from
    GR_LOD_LOG2_256 if we're not.)
  tBig: Boolean telling us whether big textures are to be used.  Use
    the SST_TBIG bit from the tLOD register for this.
  
  NB: SST_TBIG didn't exist before Napalm, so if we are on Banshee or
  Voodoo3 then we know for sure that tBig is false.
    
  Return:
  The translated LOD index.
  -------------------------------------------------------------------*/
GrLOD_t
_g2LodXlat(const GrLOD_t someLOD, const FxBool tBig)
{
#define FN_NAME "_g2LodXlat"
  if (tBig) {
    return someLOD - GR_LOD_2048;
  } else {
    return someLOD;
  }
#undef FN_NAME
} /* _g2LodXlat */
#endif

/*-------------------------------------------------------------------
  Function: 		_g2LodBiasPerChip

  Date: 			14-Nov-2000

  Implementor: 		Jonny Cochrane

  Description: 		Sub sample LOD Dithering. Called if TMU is in
                        texSubLodDither state.

                        2chip x2fsaa - no sli, 1 sample per chip
                        2chip x4fsaa - no sli, 2 samples per chip
                        4chip x2fsaa - 2 way sli, 1 sample per sli unit
                        4chip x4fsaa - no sli, 1 sample per chip
                        4chip x8fsaa - no sli, 2 samples per chip

  Arguments:		tmu: TMU to set. tLod: tLOD to modify.

  Return:		None
  -------------------------------------------------------------------*/
void FX_CSTYLE g2LodBiasPerChip(GrChipID_t tmu, FxU32 tLod)
{
#define FN_NAME "g2LodBiasPerChip"
  FxU32 newtLod ;
  FxI32 oldLodBias, newLodBias;
  unsigned int i;
  unsigned int idx;

  static FxI32 chipLodBias[2][2][4] =
    /* 4.2 format for tLod register
     * 2chip x2fsaa - no sli, 1 sample per chip
     * 2chip x4fsaa - no sli, 2 samples per chip
     * 4chip x2fsaa - 2 way sli, 1 sample per sli unit
     * 0.00, 0.50, 0.00, 0.50 - the last 2 are used only when 4chip
     * {0x00, 0x02, 0x00, 0x02},
     * 4chip x4fsaa - no sli, 1 sample per chip
     * 4chip x8fsaa - no sli, 2 samples per chip
     * 0.00, 0.25, 0.50, 0.75 - 4 chip
     * {0x00, 0x01, 0x02, 0x03}
     * if mipmap dithering is enabled
     * we will use different tLod registers.
     */
  {
    {
      {0x00<<SST_LODBIAS_SHIFT, 0x02<<SST_LODBIAS_SHIFT, 0x00<<SST_LODBIAS_SHIFT, 0x02<<SST_LODBIAS_SHIFT},
      {0x00<<SST_LODBIAS_SHIFT, 0x01<<SST_LODBIAS_SHIFT, 0x02<<SST_LODBIAS_SHIFT, 0x03<<SST_LODBIAS_SHIFT}
    },
    {
      {-0x01<<SST_LODBIAS_SHIFT, 0x02<<SST_LODBIAS_SHIFT, -0x01<<SST_LODBIAS_SHIFT, 0x02<<SST_LODBIAS_SHIFT},
      {-0x01<<SST_LODBIAS_SHIFT, 0x00<<SST_LODBIAS_SHIFT,  0x01<<SST_LODBIAS_SHIFT, 0x02<<SST_LODBIAS_SHIFT}
    }
  };
  
  GR_BEGIN_NOFIFOCHECK("g2LodBiasPerChip", 88);

  if( /*(_GlideRoot.environment.texSubLodDither != 1) ||*//* we won't get here if 0 */
      (gc->sliCount == gc->chipCount) /*||
      (gc->windowed)*/ )
    return;
  
  /* sign extend it. */
  oldLodBias = (((tLod & SST_LODBIAS) << (32-6-SST_LODBIAS_SHIFT)) >> (32-6-SST_LODBIAS_SHIFT));

  /* unmask lodbias */
  tLod &= ~(SST_LODBIAS);

  idx = ((gc->chipCount - gc->sliCount) > 2);

  for (i = 0; i < gc->chipCount; i++)
  {
    /* don't need to modify lodbias if 0 */
    if(chipLodBias[_GlideRoot.environment.texLodDither][idx][i] != 0) {
      newLodBias = oldLodBias + chipLodBias[_GlideRoot.environment.texLodDither][idx][i];
      
      if(newLodBias > (0x1f<<SST_LODBIAS_SHIFT)) newLodBias = (0x1f<<SST_LODBIAS_SHIFT);
      else if(newLodBias < (-0x20<<SST_LODBIAS_SHIFT)) newLodBias = (-0x20<<SST_LODBIAS_SHIFT);
      newLodBias &= SST_LODBIAS;
      newtLod = tLod | newLodBias;
      
      {
        SstRegs* tmuHw = SST_TMU(hw, tmu);
        _grChipMask( 0x01 << i );
        REG_GROUP_BEGIN((0x02 << tmu), tLOD, 1, 0x1);
        {
          REG_GROUP_SET(tmuHw, tLOD, tLod);
        }
        REG_GROUP_END();
      }
    }
  }
  
  /* Restore chip mask */
  _grChipMask( gc->chipmask );

  GR_END();
#undef FN_NAME
}

#if 0 /* KoolSmoky - removed */
/*-------------------------------------------------------------------
  Function: 		MultitextureAndTrilinear

  Date:                 4-Nov-2000

  Implementor: 		Jonny Cochrane

  Description: 		Determines if we are multitexturing and NOT
                        trilinear filtering. so that we can do sub
                        sample lod dithering which gives an almost
                        trilinear result.
                        mipmap dithering (lod dithering) is passed
                        through, see g2LodBiasPerChip
  					
  Arguments:		None  

  Return:               tmuMask of the TMUs that can do it
  -------------------------------------------------------------------*/
GrChipID_t FX_CSTYLE MultitextureAndTrilinear(void)
{

#define FN_NAME "MultitextureAndTrilinear"
  
  GR_DCL_GC;

  GrChipID_t retval = FXBIT(0x4);

   /* KoolSmoky - we can only do this if we are running in;
    *   2chip x2fsaa - no sli, 1 sample per chip
    *   2chip x4fsaa - no sli, 2 samples per chip
    *   4chip x2fsaa - 2 way sli, 1 sample per sli unit
    *   4chip x4fsaa - no sli, 1 sample per chip
    *   4chip x8fsaa - no sli, 2 samples per chip
    *   ...and with FX_GLIDE_LOD_SUBSAMPLE_DITHER=1
    */
  
  if( (_GlideRoot.environment.texSubLodDither != 1) ||
      (gc->sliCount == gc->chipCount) /*||
      (gc->windowed) ||
      ((gc->state.tmuMask & GR_TMUMASK_TMU0) == 0) ||
      ((gc->state.tmuMask & GR_TMUMASK_TMU1) == 0)*/ )
    return retval;

#if 0

#if 0 /* Jonny Cochrane's method */
  if(_GlideRoot.environment.texLodDither) return 0;
  
  if( (gc->state.tmu_config[0].evenOdd == 3) && //both even and odd
      (gc->state.tmu_config[1].evenOdd == 3) && //both even and odd
      (gc->state.tmu_config[0].textureMode & (SST_TMINFILTER | SST_TMAGFILTER)) && //and bilinear for mag and min filter
      (gc->state.tmu_config[1].textureMode & (SST_TMINFILTER | SST_TMAGFILTER)) )  //and bilinear for mag and min filter
    retval |= (GR_TMUMASK_TMU0 | GR_TMUMASK_TMU1);
  
#else /* Colourless's method if bilinear filtering is required */
  if( (gc->state.tmu_config[0].evenOdd == 3) && //both even and odd
      (gc->state.tmu_config[0].textureMode & (SST_TMINFILTER | SST_TMAGFILTER)) && //and bilinear for mag and min filter
      !(gc->state.tmu_config[0].textureMode & (SST_TRILINEAR | SST_TLODDITHER)) ) //and not mipmap dithering or trilinear
    retval |= GR_TMUMASK_TMU0;

  if( (gc->state.tmu_config[1].evenOdd == 3) && //both even and odd
      (gc->state.tmu_config[1].textureMode & (SST_TMINFILTER | SST_TMAGFILTER)) && //and bilinear for mag and min filter
      !(gc->state.tmu_config[1].textureMode & (SST_TRILINEAR | SST_TLODDITHER)) ) //and not mipmap dithering or trilinear
    retval |= GR_TMUMASK_TMU1;
#endif

#else

#if 0 /* Colourless's method */
  if( (gc->state.tmu_config[0].evenOdd == 3) && //both even and odd
      !(gc->state.tmu_config[0].textureMode & (SST_TRILINEAR | SST_TLODDITHER)) ) //and not trilinear and not dither mode
    retval |= GR_TMUMASK_TMU0;

  if( (gc->state.tmu_config[1].evenOdd == 3) && //both even and odd
      !(gc->state.tmu_config[1].textureMode & (SST_TRILINEAR | SST_TLODDITHER)) && //and not trilinear and not dither mode
      !(gc->state.tmu_config[0].textureMode & SST_TRILINEAR) )
    retval |= GR_TMUMASK_TMU1;
  
#else /* KoolSmoky - we will allow mip dithering. */
  if( !(gc->state.tmu_config[1].textureMode & SST_TRILINEAR) && 
      !(gc->state.tmu_config[0].textureMode & SST_TRILINEAR) ) { //not single or two pass trilinear
    if(gc->state.tmu_config[0].evenOdd == 3) //both even and odd
      retval |= GR_TMUMASK_TMU0;
    if(gc->state.tmu_config[1].evenOdd == 3) //both even and odd
      retval |= GR_TMUMASK_TMU1;
  }
#endif
  
#endif

  return retval;
}
#endif
