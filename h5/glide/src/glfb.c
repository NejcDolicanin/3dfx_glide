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
** $Header: /cvsroot/glide/glide2x/h3/glide/src/glfb.c,v 1.2 2000/10/03 18:29:55 mercury Exp $
** $Log: glfb.c,v $
** Revision 1.2  2000/10/03 18:29:55  mercury
** 003-clean_up_h3-000, h3 tree cleanup.
**
** Revision 1.1.1.1  1999/12/07 21:49:27  joseph
** Initial checkin into SourceForge.
**
** 5     KoolSmoky addin stuff from glide3
** 
** 4     3/17/99 6:16p Dow
** Phantom Menace and other fixes.
** 
** 78    3/12/99 5:52p Hanson
** Fixed Descent2 bugs.  No idle on lfb lock needed to be disabled for
** banshee/avenger since it does not have SST1 style cmd fifo.  Updated
** ConvertAndDownloadRLE for Banshee/Avenger.
** 
** 77    3/10/99 6:39p Peter
** need to bump-n-grind in lfb lock too, fixed nested lock error condition
** 
** 76    3/08/99 4:13p Mikec
** Made sure when we return raw lfb in a write lock, origin is not lower
** left.
** 
** 75    2/17/99 5:05p Atai
** guard the lfb write ptr hack for window32 only
** 
** 74    9/10/98 12:39p Mikec
** In grLfbLock special case where we return read ptr for writeMode of
** 565, I took out ZA16 because it screwed up zbuffer writes on croc. It's
** good to err on safe side.
** 
** 73    9/01/98 9:55a Dow
** Fixed stride bugs for 32-bit formats
** 
** 72    8/29/98 12:11p Mikec
** Changed grLfbLock to return the current buffer lfbptr (read ptr)  if it
** is writeOnly and 565 and not pixelpipe. 
** 
** 71    8/27/98 8:40p Mikec
** Fixed lfb buffer write problem (stride wrongly shifted by 1). 
** 
** 70    7/06/98 7:04p Jdt
** Buffer flushing logic
** 
** 69    6/10/98 9:49a Peter
** lfb buffer addressing
** 
** 68    4/29/98 1:03p Peter
** read region/non 16bit write happiness
** 
** 67    4/14/98 6:40p Peter
** Fixed grLfbLock/merge w/ cvg glide cleanup
** 
** 66    4/08/98 12:25p Dow
** LFB read fix
** 
** 65    4/07/98 10:40p Dow
** LFB Fixes:  Round 1
** 
** 64    3/28/98 11:24a Dow
** itwo�
** 
** 62    2/17/98 12:51p Dow
** LINEAR_WRITE_EDGE mods
** 
** 61    2/08/98 3:08p Dow
** FIFO Works
** 
** 60    2/01/98 7:52p Peter
** grLfbWriteRegion byte count problems
 * 
 * 58    1/16/98 4:18p Atai
 * fixed lfb and grLoadGammaTable
 * 
 * 57    1/06/98 3:53p Atai
 * remove grHint, modify grLfbWriteRegion and grGet
 * 
 * 56    12/17/97 4:45p Peter
 * groundwork for CrybabyGlide
 * 
 * 55    12/15/97 6:04p Atai
 * disable obsolete glide2 api for glide3
 * 
 * 54    12/15/97 5:54p Peter
 * swizzle reads too
 * 
 * 53    12/11/97 4:15p Peter
 * mac lfb write region
 * 
 * 52    12/01/97 5:18p Peter
 * 
 * 51    11/25/97 12:09p Peter
 * nested calls to grLfbLock vs init code locking on v2
 * 
 * 50    11/18/97 4:36p Peter
 * chipfield stuff cleanup and w/ direct writes
 * 
 * 49    11/17/97 4:55p Peter
 * watcom warnings/chipfield stuff
 * 
 * 48    11/06/97 3:38p Dow
 * More banshee stuff
 * 
 * 47    11/04/97 5:04p Peter
 * cataclysm part deux
 * 
 * 46    11/03/97 3:43p Peter
 * h3/cvg cataclysm
 * 
 * 45    10/27/97 11:10a Peter
 * starting cleanup
 * 
 * 44    10/09/97 8:02p Dow
 * State Monster 1st Cut
 * 
 * 43    10/08/97 5:18p Peter
 * fixed grLfbLock wrt writemode_any
 * 
 * 42    10/08/97 11:33a Peter
 * hmmmm....
 * 
 * 41    9/30/97 1:03p Peter
 * more debugging code
 * 
 * 40    9/25/97 1:35p Peter
 * 
 * 39    9/24/97 4:09p Peter
 * lfb/idle fixes
 * 
 * 38    9/24/97 1:31p Peter
 * assert if grXX call inside lock/unlock
 * 
 * 37    9/20/97 10:54a Peter
 * naked lfb writes
 * 
 * 36    9/15/97 7:31p Peter
 * more cmdfifo cleanup, fixed normal buffer clear, banner in the right
 * place, lfb's are on, Hmmmm.. probably more
 * 
 * 35    9/10/97 10:13p Peter
 * fifo logic from GaryT, non-normalized fp first cut
 * 
 * 34    9/05/97 5:29p Peter
 * changes for direct hw
 * 
 * 33    9/04/97 3:32p Peter
 * starting grouping serial reg writes
 * 
 * 32    8/18/97 3:52p Peter
 * pre-hw arrival fixes/cleanup
 * 
 * 31    7/30/97 2:42p Peter
 * more cleanup
 * 
 * 30    7/28/97 2:41p Peter
 * turned sli code back on for cvg, but waiting for hal
 * 
 * 29    7/26/97 3:04p Peter
 * gratuitous plug is translucent
 * 
 * 28    7/02/97 12:28p Peter
 * removed spurious NOP, tex dl
 * 
 * 27    6/26/97 10:14a Peter
 * cmd fifo for lfb
 * 
 * 26    6/06/97 10:47a Peter
 * texture downloading, fixed 640x480 dimension, changed cvg dep to be the
 * same as sst1
 * 
**
*/
#include <3dfx.h>
#define FX_DLL_DEFINITION
#include <fxdll.h>
#include <glide.h>
#include "fxglide.h"

/*---------------------------------------------------------------------------
** grLfbConstantAlpha
*/
GR_ENTRY(grLfbConstantAlpha, void, (GrAlpha_t alpha))
{
  GR_BEGIN_NOFIFOCHECK("grLfbConstantAlpha",82);
  GDBG_INFO_MORE(gc->myLevel,"(0x%x)\n",alpha);

  gc->state.lfb_constant_alpha = alpha;
  GR_END();
} /* grLfbConstantAlpha */

/*---------------------------------------------------------------------------
** grLfbConstantDepth
*/
GR_ENTRY(grLfbConstantDepth, void, (FxU16 depth))
{
  GR_BEGIN_NOFIFOCHECK("grLfbConstantDepth",82);
  GDBG_INFO_MORE(gc->myLevel,"(0x%x)\n",depth);

  gc->state.lfb_constant_depth = depth;
  GR_END();
} /* grLfbConstantDepth */

/*-------------------------------------------------------------------
  Function: grLfbLock
  Date: 2/19
  Implementor(s): jdt
  Library: Glide
  Description:
  Secures a pointer to a requested frame buffer and guarantees ordered
  access to that buffer.

  Arguments:
  type - type of lock, one of:
  GR_LOCK_WRITE_ONLY
  GR_LOCK_READ_ONLY
  buffer - which buffer to lock, one of:
  GR_BUFFER_FRONTBUFFER
  GR_BUFFER_BACKBUFFER
  GR_BUFFER_AUXBUFFER
  writeMode - desired destination color format
  origin - desired lfb origin
  pixelPipeline - flag whether to process through pixpipe
  info   - pointer to info structure
  Return:
  FXTRUE  - lock is successful
  FXFALSE - lock fails
  -------------------------------------------------------------------*/
#if LFB_DISABLE_SLAVE_FIFO
static FxU32 slaveBaseSize;
#endif

GR_ENTRY(grLfbLock, FxBool,(GrLock_t type, GrBuffer_t buffer, 
                            GrLfbWriteMode_t writeMode, GrOriginLocation_t origin, 
                            FxBool pixelPipeline, GrLfbInfo_t *info)) 
{
#define FN_NAME "grLfbLock"
  FxBool
    rv = FXTRUE;
  const FxBool 
    idleLockP = ((type & GR_LFB_NOIDLE) == 0);
  FxU32
    lfbMode,
    zaColor,
    fbzMode,
    renderMode,
    _3dlfb = FXFALSE;
  GrLfbWriteMode_t fbMode = 0;
  GrLfbWriteMode_t depthMode = 0;

  GR_BEGIN_NOFIFOCHECK_RET("grLfbLock", 82);
  GDBG_INFO_MORE(gc->myLevel,"(%d,%d,%d,%d,%d,0x%x)\n", type, buffer, 
                 writeMode, origin, pixelPipeline, info);

  /* there is only one revision extant */
  GR_CHECK_F(myName, info->size != sizeof(GrLfbInfo_t), "uninitialized info structure passed.");
  GR_CHECK_F(myName, info == NULL, "Null info structure passed.");

  type = type & ~(GR_LFB_NOIDLE);
  
  /* Pray that no one has made any glide calls that touch the hardware... */
#ifdef FX_GLIDE_NAPALM
  if((gc->sliCount > 1) &&
     (type == GR_LFB_READ_ONLY)) {
    hwcSLIReadDisable(gc->bInfo);
  }
#endif
  
  /* Load now flushed state */
  lfbMode = gc->state.fbi_config.lfbMode;
  fbzMode = gc->state.fbi_config.fbzMode;
  zaColor = gc->state.fbi_config.zaColor;

  type = type & ~(GR_LFB_NOIDLE);
  if (gc->lockPtrs[type] != (FxU32)-1) {
    GDBG_INFO(83, "Read lock failure due to existing lock");
    rv = FXFALSE;
  }

  if (rv) {
    switch(type) {
    case GR_LFB_READ_ONLY:
      lfbMode &= ~(SST_LFB_READBUFSELECT | 
                   SST_LFB_YORIGIN);
      
      switch(buffer) {
      case GR_BUFFER_FRONTBUFFER:
      case GR_BUFFER_BACKBUFFER:
        lfbMode |= SST_LFB_READCOLORBUFFER;
        break;
        
      case GR_BUFFER_AUXBUFFER:
        lfbMode |= SST_LFB_READDEPTHABUFFER;
        break;
        
      default:
        GR_CHECK_F(myName, 
                   1, 
                   "illegal buffer parameter passed");
        rv = FXFALSE;
        break;
      }

      /* New: Since we now have multiple read formats, we set the
              writeFormat in the passed in lfbInfo struct to match
              what the actual framebuffer format is. */
      {
        FxU32 fbMode = 0;
        FxU32 depthMode = 0;
        switch(gc->grPixelFormat) {
          case GR_PIXFMT_ARGB_1555:
          case GR_PIXFMT_AA_2_ARGB_1555:
          case GR_PIXFMT_AA_4_ARGB_1555:
          case GR_PIXFMT_AA_8_ARGB_1555:	/* 8xaa */
            fbMode = GR_LFBWRITEMODE_1555;
            depthMode = GR_LFBWRITEMODE_ZA16;
            break;
          case GR_PIXFMT_ARGB_8888:
          case GR_PIXFMT_AA_2_ARGB_8888:
          case GR_PIXFMT_AA_4_ARGB_8888:
          case GR_PIXFMT_AA_8_ARGB_8888:	/* 8xaa */
            fbMode = GR_LFBWRITEMODE_8888;
            depthMode = GR_LFBWRITEMODE_Z32;
            break;
          case GR_PIXFMT_RGB_565:
          case GR_PIXFMT_AA_2_RGB_565:
          case GR_PIXFMT_AA_4_RGB_565:
          case GR_PIXFMT_AA_8_RGB_565:		/* 8xaa */
          default:
            fbMode = GR_LFBWRITEMODE_565;
            depthMode = GR_LFBWRITEMODE_ZA16;
            break;
        }  
        info->writeMode = ((buffer == GR_BUFFER_AUXBUFFER)
                           ? depthMode
                           : fbMode);
      }
      
      lfbMode |= (origin ? SST_LFB_YORIGIN : 0);
      break;
      
    case GR_LFB_WRITE_ONLY:
      /* Set up the constant depth register because it may have
       * been trashed by a call to grDepthBiasLevel
       * (depthbiaslevel and constant depth use the same register) 
       */
      zaColor = (((FxU32) gc->state.lfb_constant_depth) << SST_ZACOLOR_DEPTH_SHIFT);
      zaColor |= (gc->state.lfb_constant_alpha << SST_ZACOLOR_ALPHA_SHIFT);

      /* disable depth biasing */
      fbzMode &= ~(SST_ENZBIAS);
          
      lfbMode &= ~(SST_LFB_YORIGIN        |
                   SST_LFB_FORMAT         |
                   SST_LFB_ENPIXPIPE);
          
      switch(writeMode) {
      case GR_LFBWRITEMODE_RESERVED1:
      case GR_LFBWRITEMODE_RESERVED2:
      case GR_LFBWRITEMODE_RESERVED3:
      case GR_LFBWRITEMODE_RESERVED4:
      case GR_LFBWRITEMODE_RESERVED5:
      case GR_LFBWRITEMODE_RESERVED6:
      case GR_LFBWRITEMODE_RESERVED7:
        rv = FXFALSE;
      }

      /* Default to hw */
      switch(gc->grPixelFormat) {
        case GR_PIXFMT_ARGB_1555:
        case GR_PIXFMT_AA_2_ARGB_1555:
        case GR_PIXFMT_AA_4_ARGB_1555:
        case GR_PIXFMT_AA_8_ARGB_1555: 	/* 8xaa */
          fbMode = GR_LFBWRITEMODE_1555;
          depthMode = GR_LFBWRITEMODE_ZA16;
          break;
        case GR_PIXFMT_ARGB_8888:
        case GR_PIXFMT_AA_2_ARGB_8888:
        case GR_PIXFMT_AA_4_ARGB_8888:
        case GR_PIXFMT_AA_8_ARGB_8888:	/* 8xaa */
          fbMode = GR_LFBWRITEMODE_8888;
          depthMode = GR_LFBWRITEMODE_Z32;
          break; 
        case GR_PIXFMT_RGB_565:
        case GR_PIXFMT_AA_2_RGB_565:
        case GR_PIXFMT_AA_4_RGB_565:
        case GR_PIXFMT_AA_8_RGB_565: 	/* 8xaa */
        default:
          fbMode = GR_LFBWRITEMODE_565;
          depthMode = GR_LFBWRITEMODE_ZA16;
          break;
      }
      if (writeMode == GR_LFBWRITEMODE_ANY) {
        writeMode = ((buffer == GR_BUFFER_AUXBUFFER)
                     ? depthMode
                     : fbMode);
      }

#if (GLIDE_PLATFORM & GLIDE_OS_MACOS) && SET_BSWAP
      /* I'd do this in the switch() up above if writeMode wasn't being munged */
      switch(writeMode) {
      case GR_LFBWRITEMODE_565:
      case GR_LFBWRITEMODE_555:
      case GR_LFBWRITEMODE_1555:
      case GR_LFBWRITEMODE_ZA16:
      lfbMode |= SST_LFB_WRITE_SWAP16;
      }
#endif /* (GLIDE_PLATFORM & GLIDE_OS_MACOS) */
      
      switch(buffer) {
      case GR_BUFFER_FRONTBUFFER:
      case GR_BUFFER_BACKBUFFER:
        rv = ((writeMode != GR_LFBWRITEMODE_ZA16) &&
              (writeMode != GR_LFBWRITEMODE_Z32));
        break;

      case GR_BUFFER_AUXBUFFER:
        rv = ((writeMode != GR_LFBWRITEMODE_ZA16) &&
              (writeMode != GR_LFBWRITEMODE_Z32));
        break;

      default:
        GR_CHECK_F(myName, 1, "illegal buffer parameter passed");
        break;
      }

      lfbMode |= (writeMode << SST_LFB_FORMAT_SHIFT);
      lfbMode |= (origin ? SST_LFB_YORIGIN : 0);

      if (pixelPipeline) {
        lfbMode |= SST_LFB_ENPIXPIPE;

        fbzMode &= ~SST_YORIGIN;
        fbzMode |= (origin ? SST_YORIGIN : 0);
      }
      info->writeMode = writeMode;
      break;

    default:
      rv = FXFALSE;
      GDBG_INFO(gc->myLevel,  "Lock failed because of invalid lock type.");
      break;
    }
  }

  if (rv) {
    const FxU32
      lockCount = gc->cmdTransportInfo.lfbLockCount;

    gc->lockPtrs[type] = buffer;
    gc->cmdTransportInfo.lfbLockCount = 0x00UL;
    
    /* Setup the hw w/ the settings computed above. */
    switch(type) {
    case GR_LFB_READ_ONLY:
      /* I'm not sure why this is actually doing anything on UMA
       * architectures since reads are always done via direct LFB
       * accesses anyway, and nothing in this register affects that. -- KCD */
      GR_SET_EXPECTED_SIZE(sizeof(FxU32), 1);
      GR_SET(BROADCAST_ID, hw, lfbMode, lfbMode);
      GR_CHECK_SIZE();
      break;
      
    case GR_LFB_WRITE_ONLY:
      REG_GROUP_BEGIN(BROADCAST_ID, fbzMode, 3, 0x103);
      {          
        REG_GROUP_SET(hw, fbzMode, fbzMode);
        REG_GROUP_SET(hw, lfbMode, lfbMode);
        REG_GROUP_SET(hw, zaColor, zaColor);
      }
      REG_GROUP_END();
      break;
    }
    gc->state.fbi_config.lfbMode = lfbMode;

    /* Get the current lfb buffer */
    {
      /* FixMe: Is this true if we're triple buffering? */
      FxU32 colBufferIndex = 0;
      
      switch(buffer) {
      case GR_BUFFER_FRONTBUFFER:
        colBufferIndex = gc->frontBuffer;
        break;
        
      case GR_BUFFER_BACKBUFFER:
        colBufferIndex = gc->backBuffer;
        break;
        
      case GR_BUFFER_AUXBUFFER:
        colBufferIndex = gc->grColBuf;
        break;

      default:
        GR_CHECK_F(myName, 1, "illegal buffer parameter passed");
        rv = FXFALSE;
        break;
      }

      if (rv) {
#ifdef	__linux__
        if (!colBufferIndex) {
          info->strideInBytes = driInfo.stride;
        } else {
          info->strideInBytes     = gc->bInfo->buffInfo.bufLfbStride;
      }
#else	/* defined(__linux__) */
        /*
         * This is the default for 3D LFBs,
         * which are always 2048 pixels wide.
         */
        info->strideInBytes     = 0x1000;
#endif	/* defined(__linux__) */
        info->origin            = origin;

        switch(type) {
        case GR_LFB_READ_ONLY:
          {
#if defined(__linux__)
            if (colBufferIndex == 0) {
              info->strideInBytes = driInfo.stride;
            } else {
              info->strideInBytes     = gc->bInfo->buffInfo.bufLfbStride;
            }
#else	/* defined(__linux__) */
            info->strideInBytes     = gc->bInfo->buffInfo.bufLfbStride;
#endif	/* defined(__linux__) */
#if __POWERPC__
            if(IS_NAPALM(gc->bInfo->pciInfo.deviceID)) {
              if(gc->grPixelSize == 2) {
                info->lfbPtr = (void *)((FxU32)info->lfbPtr + gc->bInfo->pciInfo.swizzleOffset[3]);                
              } else {
                info->lfbPtr = (void *)((FxU32)info->lfbPtr + gc->bInfo->pciInfo.swizzleOffset[1]);  
              }
            }
#endif
            info->lfbPtr = (void *)gc->lfbBuffers[colBufferIndex];
          }
#if (GLIDE_PLATFORM & GLIDE_OS_WIN32)
        case GR_LFB_WRITE_ONLY:
          {
            /* Next, If it is writeOnly and 565 (or matches the FB format exactly) 
               and not pixelpipe, we just return the current buffer lfbPtr as the
               write ptr. This fixes those games that use the lfb write pointer to do
               lfb reads. --mikec */
            /* Note: It also appears that OpenGL depends on this, since they always
             * take both a read and write lock, but only save off the stride value 
             * from the latter one.  So if we return different strides OpenGL's lfb
             * accesses will be whacked. -- KCD */
            if((writeMode == fbMode) &&
               (!pixelPipeline) && 
               /* Origin must be upper left since we will return raw lfb */
               (origin != GR_ORIGIN_LOWER_LEFT)){
              info->lfbPtr = (void *)gc->lfbBuffers[colBufferIndex];
              info->strideInBytes     = gc->bInfo->buffInfo.bufLfbStride;
              GR_SET(BROADCAST_ID, hw, colBufferAddr, gc->buffers0[colBufferIndex]);
            } else {
              GR_SET(BROADCAST_ID, hw, colBufferAddr, gc->buffers0[colBufferIndex]);
              /* tbext */
              if (colBufferIndex < (FxU32)gc->grColBuf) {
                GR_SET_EXPECTED_SIZE(sizeof(FxU32), 1);
                GR_SET(BROADCAST_ID, hw, colBufferAddr, gc->buffers0[colBufferIndex]);
                GR_CHECK_SIZE();
              }
              
              /* Make sure dither rotation is disabled for 3D LFBs. */
              _3dlfb = FXTRUE;
              
#if defined(__linux__)
              /*
               * For Linux, we just return the correct address and
               * stride.
               */
              info->strideInBytes   = gc->bInfo->buffInfo.bufLfbStride;
              info->lfbPtr          = (void *)gc->lfbBuffers[colBufferIndex];
#else	/* defined(__linux__) */
              info->lfbPtr          = (void *)gc->lfb_ptr;
#endif	/* defined(__linux__) */
              
#ifndef __linux__
              switch (writeMode) {
              case GR_LFBWRITEMODE_565_DEPTH:
              case GR_LFBWRITEMODE_555_DEPTH:
              case GR_LFBWRITEMODE_1555_DEPTH:
              case GR_LFBWRITEMODE_888:
              case GR_LFBWRITEMODE_8888:
              case GR_LFBWRITEMODE_Z32:
                info->strideInBytes <<= 1;
                break;
              }
#endif	/* defined(__linux__) */
            }
          }
          break;

#endif /* (GLIDE_PLATFORM & GLIDE_OS_WIN32) */
        default:
          {
            /* %%KCD - Make sure we don't program the colBufferAddr to point to
             * the AUX buffer if the user is trying to do 3D LFB writes to
             * the AUX buffer! */
            if (colBufferIndex < (FxU32)gc->grColBuf) {
              GR_SET_EXPECTED_SIZE(sizeof(FxU32), 1);
              GR_SET(BROADCAST_ID, hw, colBufferAddr, gc->buffers0[colBufferIndex]);
              GR_CHECK_SIZE();
            }
            
            _3dlfb = FXTRUE;
            
            info->lfbPtr          = (void *)gc->lfb_ptr;
            
            switch (writeMode) {
            case GR_LFBWRITEMODE_565_DEPTH:
            case GR_LFBWRITEMODE_555_DEPTH:
            case GR_LFBWRITEMODE_1555_DEPTH:
            case GR_LFBWRITEMODE_888:
            case GR_LFBWRITEMODE_8888:
            case GR_LFBWRITEMODE_Z32:
              info->strideInBytes <<= 1;
              break;
            }
          }
          break;
        }

#ifdef FX_GLIDE_NAPALM
        /* SLI Effage.  It seems that in order for flipped LFBs to work right,
         * we have to have renderMode set up the same way rendering would want it,
         * otherwise bad stuff happens. */
        if(IS_NAPALM(gc->bInfo->pciInfo.deviceID)) {
          renderMode = gc->state.fbi_config.renderMode;
          fbzMode = gc->state.fbi_config.fbzMode;

          /* See above. */
          if(_3dlfb) {
            gc->state.fbi_config.renderMode &= ~SST_RM_DITHER_ROTATION;
          }

          /* This will screw with some shadow registers, so put them back. */
          grSstOrigin(origin);
          
          gc->state.fbi_config.renderMode = renderMode;
          gc->state.fbi_config.fbzMode = fbzMode;
        }
#endif

        /* NoIdle flag is ignored on all hardware except SST1 */
        if (idleLockP) {
          /* This is required to flush the write buffers before the
           * actual LFB accesses.
           */
          P6FENCE;

          /* If we're not using hole counting then we need to make sure that
           * any queued commands are actually flushed before checking the fifo
           * ptr's location.
           */
          if (!gc->cmdTransportInfo.autoBump)
            GR_BUMP_N_GRIND;
       
          /* Must idle to get things in the right order. */
          grSstIdle();
        }
        /* Pray that no one makes any glide calls that touch the hardware... */
#ifdef FX_GLIDE_NAPALM
        if((gc->sliCount > 1) &&
           (type == GR_LFB_READ_ONLY)) {
          hwcSLIReadEnable(gc->bInfo);
        }
#endif

#if LFB_DISABLE_SLAVE_FIFO
        /* Disable slave command FIFO */
        if(gc->chipCount > 1) {
          FxU32 depth;
          do {
            depth = GR_SLAVE_CAGP_GET(0, depth);
          } while(depth != 0);
        }
        slaveBaseSize = GR_SLAVE_CAGP_GET(0, baseSize);
        GR_SLAVE_CAGP_SET(0, baseSize, 0);
#endif
        
        /* Increment lock count */
        gc->cmdTransportInfo.lfbLockCount = lockCount + 1;
      }
    }
  }


  GR_RETURN(rv);
#undef FN_NAME
} /* grLfbLock */
    
/*-------------------------------------------------------------------
  Function: grLfbUnlock
  Date: 2/21
  Implementor(s): jdt
  Library: Glide
  Description:
  Unlock a previously locked buffer
  Arguments:
  type - type of lock (read only/write only)
  buffer - which buffer to unlock 
  Return:
  FXTRUE  - success
  FXFALSE - failure
  -------------------------------------------------------------------*/

GR_ENTRY(grLfbUnlock, FxBool, (GrLock_t type, GrBuffer_t buffer))
{
#define FN_NAME "grLfbUnlock"
  FxBool rval = FXFALSE;
  
  GR_BEGIN_NOFIFOCHECK_RET("grLfbUnLock", 83);
  GDBG_INFO_MORE(gc->myLevel,"(%d, %d)\n", type, buffer);
  
  type = type & ~(GR_LFB_NOIDLE);
  
  GR_CHECK_F(myName, 
             type != GR_LFB_WRITE_ONLY &&
             type != GR_LFB_READ_ONLY,
             "Bad type");
  
  GR_CHECK_F(myName, 
             buffer != GR_BUFFER_FRONTBUFFER &&
             buffer != GR_BUFFER_BACKBUFFER  &&
             buffer != GR_BUFFER_AUXBUFFER,
             "Bad buffer");

  rval = (gc->lockPtrs[type] == (FxU32)buffer);
  if (rval) {
    const FxU32
      lockCount = gc->cmdTransportInfo.lfbLockCount;

    /* Clear the current lfb lock state */
    gc->cmdTransportInfo.lfbLockCount = 0;
    gc->lockPtrs[type] = (FxU32)-1;

#ifdef FX_GLIDE_NAPALM
    if((gc->sliCount > 1) &&
       (type == GR_LFB_READ_ONLY)) {
      hwcSLIReadDisable(gc->bInfo);
    }
#endif

#if LFB_DISABLE_SLAVE_FIFO    
    /* Enable slave command FIFO */
    if(gc->chipCount > 1) {
    GR_SLAVE_CAGP_SET(0, baseSize, slaveBaseSize);
    }
#endif

    GR_SET_EXPECTED_SIZE(sizeof(FxU32), 1);
    GR_SET(BROADCAST_ID, hw, colBufferAddr, gc->buffers0[gc->curBuffer]);
    GR_CHECK_SIZE();

    if (type == GR_LFB_WRITE_ONLY) {
      GR_SET_EXPECTED_SIZE(sizeof(FxU32) << 1, 2);
      {
        /* Restore depth bias level */
        GR_SET(BROADCAST_ID, hw, zaColor, gc->state.fbi_config.zaColor);
        
        /* turn back on depth biasing */
        GR_SET(BROADCAST_ID, hw, fbzMode, gc->state.fbi_config.fbzMode);
      }
      GR_CHECK_SIZE();
    }

    gc->cmdTransportInfo.lfbLockCount = lockCount - 1;

#ifdef FX_GLIDE_NAPALM
    if((gc->sliCount > 1) &&
       (type == GR_LFB_READ_ONLY)) {
      if(gc->cmdTransportInfo.lfbLockCount != 0) {
        grSstIdle();
        hwcSLIReadEnable(gc->bInfo);
      } else {
        hwcSLIReadDisable(gc->bInfo);
      }        
    }
#endif  
  }

  GR_RETURN(rval);
#undef FN_NAME
} /* grLfbUnlock */


/*---------------------------------------------------------------------------
** grLfbWriteColorFormat
**
*/

GR_STATE_ENTRY(grLfbWriteColorFormat, void, (GrColorFormat_t colorFormat))
{
#define FN_NAME "grLfbWriteColorFormat"
  FxU32 lfbMode;

  GR_BEGIN_NOFIFOCHECK("grLfbWriteColorFormat",82);
  GDBG_INFO_MORE(gc->myLevel,"(%d)\n",colorFormat);

  GR_CHECK_F(myName,
             colorFormat < 0 || colorFormat > 0x3,
             "invalid color format");

#if defined(GLIDE3) && defined(GLIDE3_ALPHA)
  _grValidateState();
#endif

  lfbMode = gc->state.fbi_config.lfbMode;
  lfbMode &= ~SST_LFB_RGBALANES;
  lfbMode |= (colorFormat << SST_LFB_RGBALANES_SHIFT);

  {
    const FxBool sliP = gc->scanline_interleaved;

    GR_SET_EXPECTED_SIZE(sizeof(FxU32) + MaskSelect(sliP, sizeof(FxU32)),
                         1 + sliP);
    {
      GR_SET(BROADCAST_ID, hw, lfbMode, lfbMode);
      gc->state.fbi_config.lfbMode = lfbMode;
      
      if (sliP) GR_SET(BROADCAST_ID, hw, nopCMD, 0x0);
    }
    GR_CHECK_SIZE();
  }

  GR_END();
#undef FN_NAME
} /* grLfbWriteColorFormat */


/*---------------------------------------------------------------------------
**  grLfbWriteColorSwizzle  - set up SST for byte swizzling and word swapping
**
**  Registers/Bits Affected:
**  lfbMode:  bit(11), bit(12)
**
**  WARNING:  GMT: SST_LFB_WRITE_SWAP16 changes pixel addressing!!!
*/
GR_STATE_ENTRY(grLfbWriteColorSwizzle, void, (FxBool swizzleBytes, FxBool swapWords))
{
#define FN_NAME "grLfbWriteColorSwizzle"
  FxU32 lfbMode;

  GR_BEGIN_NOFIFOCHECK("grLfbWriteColorSwizzle",82);
  GDBG_INFO_MORE(gc->myLevel,"(%d,%d)\n",swizzleBytes,swapWords);

#if defined(GLIDE3) && defined(GLIDE3_ALPHA)
  _grValidateState();
#endif

  /* Clear out the bits we'll set back if appropriate */
  lfbMode = gc->state.fbi_config.lfbMode;
  lfbMode &= ~(SST_LFB_WRITE_SWAP16 | SST_LFB_WRITE_BYTESWAP);

  if (swizzleBytes)
    lfbMode |= SST_LFB_WRITE_BYTESWAP;

  if (swapWords)
    lfbMode |= SST_LFB_WRITE_SWAP16;

  {
    const FxBool sliP = gc->scanline_interleaved;

    GR_SET_EXPECTED_SIZE(sizeof(FxU32) + MaskSelect(sliP, sizeof(FxU32)),
                         1 + sliP);
    {
      GR_SET(BROADCAST_ID, hw, lfbMode, lfbMode);
      gc->state.fbi_config.lfbMode = lfbMode;
      
      if (sliP) GR_SET(BROADCAST_ID, hw, nopCMD, 0x0);
    }
    GR_CHECK_SIZE();
  }

  GR_END();
#undef FN_NAME
} /* grLfbWriteColorSwizzle */

FxBool
_grLfbWriteRegion(FxBool pixPipelineP,
                  GrBuffer_t dst_buffer, FxU32 dst_x, FxU32 dst_y, 
                  GrLfbSrcFmt_t src_format, 
                  FxU32 src_width, FxU32 src_height, 
                  FxI32 src_stride, const void *src_data)
{
#define FN_NAME "_grLfbWriteRegion"
  FxBool           rv = FXTRUE;
  GrLfbInfo_t      info;
  GrLfbWriteMode_t writeMode;
  
  GR_BEGIN_NOFIFOCHECK_RET(FN_NAME, 82);
  GDBG_INFO_MORE(gc->myLevel,
                 "(0x%x, %s, %d, %d, %d, %d, %d, %d, 0x%x)\n", 
                 dst_buffer,
                 (pixPipelineP ? "Enable" : "Disable"),
                 dst_x, dst_y, 
                 src_format, src_width, src_height,
                 src_stride, src_data);

  writeMode = ((src_format == GR_LFB_SRC_FMT_RLE16)
               ? GR_LFBWRITEMODE_565
               : src_format);

  info.size = sizeof(info);
  
  if (grLfbLock(GR_LFB_WRITE_ONLY, 
                 dst_buffer, 
                 writeMode,
                 GR_ORIGIN_UPPER_LEFT,
                 pixPipelineP,
                 &info)) {
    FxU32 *srcData;             /* Tracking Source Pointer */
    FxU32 *dstData;             /* Tracking Destination Pointer */
    FxU32 *end;                 /* Demarks End of each Scanline */
    FxI32 srcJump;              /* bytes to next scanline */
    FxU32 dstJump;              /* bytes to next scanline */
    FxU32 length;               /* bytes to copy in scanline */
    FxU32 scanline;             /* scanline number */
    int   aligned;              /* word aligned? */
    
    
    srcData = (FxU32 *) src_data;
    dstData = (FxU32 *) (((char*)info.lfbPtr)+ (dst_y*info.strideInBytes));
    scanline = src_height;
    
    switch(src_format) {
    /* 16-bit aligned */
    case GR_LFB_SRC_FMT_565:
    case GR_LFB_SRC_FMT_555:
    case GR_LFB_SRC_FMT_1555:
    case GR_LFB_SRC_FMT_ZA16:
      dstData = (FxU32*)(((FxU16*)dstData) + dst_x);
      length  = src_width * 2;
      aligned = !((int)dstData&0x2);
      srcJump = src_stride - length;
      dstJump = info.strideInBytes - length;
      if (aligned) {
        while(scanline--) {
          end = (FxU32*)((char*)srcData + length - 2);
          while(srcData < end) {
            SET_LFB(dstData[0], srcData[0]);
            dstData++;
            srcData++;
          }
                    
          if (((int)length) & 0x2) {
            SET_LFB_16((*(FxU16*)&(dstData[0])),
                  (*(FxU16*)&(srcData[0])));
            dstData = (FxU32*)(((FxU16*)dstData) + 1);
            srcData = (FxU32*)(((FxU16*)srcData) + 1);
          }
                    
          dstData = (FxU32*)(((char*)dstData)+dstJump);
          srcData = (FxU32*)(((char*)srcData)+srcJump);
        }
      } else {
        while(scanline--) {
          end = (FxU32*)((char*)srcData + length - 2);
          
          SET_LFB_16((*(FxU16*)&(dstData[0])),
                (*(FxU16*)&(srcData[0])));
          dstData = (FxU32*)(((FxU16*)dstData) + 1);
          srcData = (FxU32*)(((FxU16*)srcData) + 1);
                    
          while(srcData < end) {
            SET_LFB(dstData[0], srcData[0]);
            dstData++;
            srcData++;
          }
                    
          if (!(length & 0x2)) {
            SET_LFB_16((*(FxU16*)&(dstData[0])),
                  (*(FxU16*)&(srcData[0])));
            dstData = (FxU32*)(((FxU16*)dstData) + 1);
            srcData = (FxU32*)(((FxU16*)srcData) + 1);
          }
                    
          dstData = (FxU32*)(((char*)dstData)+dstJump);
          srcData = (FxU32*)(((char*)srcData)+srcJump);
        }
      }
      break;

    /* 32-bit aligned */
    case GR_LFB_SRC_FMT_888:
    case GR_LFB_SRC_FMT_8888:
    case GR_LFB_SRC_FMT_565_DEPTH:
    case GR_LFB_SRC_FMT_555_DEPTH:
    case GR_LFB_SRC_FMT_1555_DEPTH:
      dstData = ((FxU32*)dstData) + dst_x;
      length  = src_width * 4;
      srcJump = src_stride - length;
      dstJump = info.strideInBytes - length;
      while(scanline--) {
        end = (FxU32*)((char*)srcData + length);
        while(srcData < end) {
          SET_LFB(dstData[0], srcData[0]);
          dstData++;
          srcData++;
        }
        dstData = (FxU32*)(((char*)dstData)+dstJump);
        srcData = (FxU32*)(((char*)srcData)+srcJump);
      }
      break;

    case GR_LFB_SRC_FMT_RLE16:
      /* needs to be implemented */
      rv = FXFALSE;
      break;
    }
    grLfbUnlock(GR_LFB_WRITE_ONLY, dst_buffer);
  } else {
    rv = FXFALSE;
  }

  GR_RETURN(rv);
#undef FN_NAME
} /* _grLfbWriteRegion */

/*-------------------------------------------------------------------
  Function: grLfbWriteRegion
  Date: 3/5
  Implementor(s): jdt
  Library: Glide
  Description:
  Write a pixel rectangle to the frame buffer as efficiently as possible
  Arguments:
  dst_buffer  - buffer to which to copy data
  dst_x,dst_y - destination image start coordinates
  src_format  - data format of source image
  src_width, src_height 
              - dimensions of source image
  src_stride  - stride of source image in bytes, not meaningful
                for RLE images
  src_data    - pointer to source data memory           
  Return:
  FXTRUE  succeed
  FXFALSE fail
  -------------------------------------------------------------------*/
#if defined(GLIDE3) && defined(GLIDE3_ALPHA)
GR_ENTRY(grLfbWriteRegion, FxBool, (GrBuffer_t dst_buffer, 
                                    FxU32 dst_x, FxU32 dst_y, 
                                    GrLfbSrcFmt_t src_format, 
                                    FxU32 src_width, FxU32 src_height, 
                                    FxBool pixelPipeline,
                                    FxI32 src_stride, void *src_data))
#else
GR_ENTRY(grLfbWriteRegion, FxBool, (GrBuffer_t dst_buffer, 
                                    FxU32 dst_x, FxU32 dst_y, 
                                    GrLfbSrcFmt_t src_format, 
                                    FxU32 src_width, FxU32 src_height, 
                                    FxI32 src_stride, void *src_data))
#endif
{
  FxBool           rv = FXTRUE;
  
  GR_BEGIN_NOFIFOCHECK_RET("grLfbWriteRegion",82);
  GDBG_INFO_MORE(gc->myLevel,
                 "(0x%x,%d,%d,%d,%d,%d,%d,0x%x)\n", 
                 dst_buffer, dst_x, dst_y, 
                 src_format, src_width, src_height,
                 src_stride, src_data);
  
#if defined(GLIDE3) && defined(GLIDE3_ALPHA)
  if ((_GlideRoot.hwConfig.SSTs[_GlideRoot.current_sst].type == GR_SSTTYPE_SST96) && (pixelPipeline == FXTRUE))
    rv = FXFALSE;
  else
    rv = _grLfbWriteRegion(pixelPipeline, dst_buffer, dst_x, dst_y,
                           src_format, src_width, src_height,
                           src_stride, src_data);
#else
  rv = _grLfbWriteRegion(FXFALSE, dst_buffer, dst_x, dst_y,
                         src_format, src_width, src_height,
                         src_stride, src_data);
#endif

  GR_RETURN(rv);
} /* grLfbWriteRegion */

/*-------------------------------------------------------------------
  Function: grLfbReadRegion
  Date: 3/12
  Implementor(s): jdt
  Library: Glide
  Description:
  Grab a rectangle from the frame buffer into user supplied memory
  Arguments:
  src_buffer - buffer to read from 
  src_x      - x coordinate of upper left corner rectangle to read
  src_y      - y coordinate of upper left corner of rectangle to read
  src_width  - width of rectangle to read
  src_height - height of rectangle to read
  dst_stride - distance between scanlines in destination buffer
  dst_data   - pointer to user memory in which to place image
  Return:
  FXTRUE - success
  FXFALSE - failure
  -------------------------------------------------------------------*/
#if !__POWERPC__
GR_ENTRY(grLfbReadRegion, FxBool, (GrBuffer_t src_buffer, 
                                   FxU32 src_x, FxU32 src_y, 
                                   FxU32 src_width, FxU32 src_height, 
                                   FxU32 dst_stride, void *dst_data))
{
#define FN_NAME "grLfbReadRegion"
  FxU32 bpp;
   FxBool rv;
   GrLfbInfo_t info;

   GR_BEGIN_NOFIFOCHECK_RET("grLfbReadRegion",82);
   GDBG_INFO_MORE(gc->myLevel,
                  "(0x%x,%d,%d,%d,%d,%d,0x%x)\n",
                  src_buffer, src_x, src_y,
                  src_width, src_height, dst_stride, dst_data);

   bpp=gc->bInfo->h3pixelSize;
   info.size = sizeof(info);
   if (!src_width)
   {
      rv=FXTRUE;
      goto done;
   }
   rv=FXFALSE;

   if (grLfbLock(GR_LFB_READ_ONLY,
                 src_buffer, 
                 GR_LFBWRITEMODE_ANY,
                 GR_ORIGIN_UPPER_LEFT,
                 FXFALSE,
                 &info) )
   {
      FxU32 *src,*dst;
      FxI32 length,scanline;
      FxU32 src_adjust,dst_adjust,tmp;

      src=(FxU32 *) (((char*)info.lfbPtr)+
                     (src_y*info.strideInBytes) + (src_x * bpp));
      dst=dst_data;
      scanline=src_height;

      /* set length - alignment fix*/
      tmp=(((FxU32)src)&2);
      length=src_width * bpp - tmp;
      src_adjust=info.strideInBytes - tmp;
      dst_adjust=dst_stride - tmp;

      /* should be endian and pixel size safe */
         /* it would be nice to test if quad blocks were faster */
         /* like mmx loads and stores */
      while(src_height--)
      {
         /* adjust starting alignment */
         if (((FxU32)src)&3)
            *((FxU16 *)dst)++=*((FxU16 *)src)++;

         /* read in dwords of pixels */
         if(length)
         {
            FxU32 byte_index=0;
            FxU32 aligned=length&(~3);

            /* copies aligned dwords */
            do
            {
               *((FxU32 *)(((FxU32)dst) + byte_index))=*((FxU32 *)(((FxU32)src) + byte_index));
            }while((byte_index+=4)<aligned);

            /* handle backend misalignment */
            if (byte_index!=(FxU32)length)
               *((FxU16 *)(((FxU32)dst) + byte_index))=*((FxU16 *)(((FxU32)src) + byte_index));
         }
         /* adjust for next line */
         ((FxU8 *)src)+=src_adjust;
         ((FxU8 *)dst)+=dst_adjust;
      }
      rv=FXTRUE;
      /* unlock buffer */
      grLfbUnlock(GR_LFB_READ_ONLY,src_buffer);
   }
done:
   GR_RETURN(rv);
#undef FN_NAME
}/* grLfbReadRegion */
#else
/* if __POWERPC__ */
GR_ENTRY(grLfbReadRegion, FxBool, (GrBuffer_t src_buffer, 
                                   FxU32 src_x, FxU32 src_y, 
                                   FxU32 src_width, FxU32 src_height, 
                                   FxU32 dst_stride, void *dst_data))
{
#define FN_NAME "grLfbReadRegion"
  FxBool rv = FXTRUE;
  GrLfbInfo_t info;
  
  GR_BEGIN_NOFIFOCHECK_RET("grLfbReadRegion",82);
  GDBG_INFO_MORE(gc->myLevel,
                 "(0x%x,%d,%d,%d,%d,%d,0x%x)\n",
                 src_buffer, src_x, src_y,
                 src_width, src_height, dst_stride, dst_data);
  
  info.size = sizeof(info);
  if (grLfbLock(GR_LFB_READ_ONLY,
                src_buffer, 
                GR_LFBWRITEMODE_ANY,
                GR_ORIGIN_UPPER_LEFT,
                FXFALSE,
                &info) ) {
    FxU32 *srcData;             /* Tracking Source Pointer */
    FxU32 *dstData;             /* Tracking Destination Pointer */
    FxU32 *end;                 /* Demarks End of each Scanline */
    FxU32 srcJump;              /* bytes to next scanline */
    FxU32 dstJump;              /* bytes to next scanline */
    FxU32 length;               /* bytes to copy in scanline */
    FxU32 scanline;             /* scanline number */
    int   aligned;              /* word aligned? */
    FxU32 odd;                  /* is src_y odd? (for sli) */
    
    dstData = (FxU32 *) dst_data;
    srcData = (FxU32 *) (((char*)info.lfbPtr)+
                         (src_y*info.strideInBytes) +
                         (src_x<<1));
    scanline = src_height;
    length   = src_width * 2;
    dstJump  = dst_stride - length;
    srcJump  = info.strideInBytes - length;
    aligned  = !((int)srcData&0x2);
    odd      = (src_y+src_height) & 0x1;
    
#if __POWERPC__
  if(!(IS_NAPALM(gc->bInfo->pciInfo.deviceID))) {
#endif  
    if (aligned) {
      while(scanline--) {
        end = (FxU32*)((char*)srcData + length - 2);

        while(srcData < end) {
          *dstData++ = GET_LFB(*srcData++);
        }

        if (((int)length) & 0x2) {
          (*(FxU16*)dstData) = (FxU16)GET_LFB_16(*srcData);
          dstData = (FxU32*)(((FxU16*)dstData) + 1);
          srcData = (FxU32*)(((FxU16*)srcData) + 1);
        }
                
        dstData = (FxU32*)(((char*)dstData)+dstJump);
        srcData = (FxU32*)(((char*)srcData)+srcJump);
      }
    } else {
      while(scanline--) {
        end = (FxU32*)((char*)srcData + length - 2);

        (*(FxU16*)dstData) = (FxU16)GET_LFB_16(*srcData);
        dstData = (FxU32*)(((FxU16*)dstData) + 1);
        srcData = (FxU32*)(((FxU16*)srcData) + 1);

        while(srcData < end) {
          *dstData++ = GET_LFB(*srcData++);
        }

        if (!(((int)length) & 0x2)) {
          (*(FxU16*)dstData) = (FxU16)GET_LFB_16(*srcData);
          dstData = (FxU32*)(((FxU16*)dstData) + 1);
          srcData = (FxU32*)(((FxU16*)srcData) + 1);
        }
                
        dstData = (FxU32*)(((char*)dstData)+dstJump);
        srcData = (FxU32*)(((char*)srcData)+srcJump);
      }
    }
#if __POWERPC__
  } else {
    if (aligned) {
      while(scanline--) {
        end = (FxU32*)((char*)srcData + length - 2);

        while(srcData < end) {
          *dstData++ = *srcData++;
        }

        if (((int)length) & 0x2) {
          (*(FxU16*)dstData) = (*(FxU16 *)srcData);
          dstData = (FxU32*)(((FxU16*)dstData) + 1);
          srcData = (FxU32*)(((FxU16*)srcData) + 1);
        }
                
        dstData = (FxU32*)(((char*)dstData)+dstJump);
        srcData = (FxU32*)(((char*)srcData)+srcJump);
      }
    } else {
      while(scanline--) {
        end = (FxU32*)((char*)srcData + length - 2);

        (*(FxU16*)dstData) = (*(FxU16 *)srcData);
        dstData = (FxU32*)(((FxU16*)dstData) + 1);
        srcData = (FxU32*)(((FxU16*)srcData) + 1);

        while(srcData < end) {
          *dstData++ = *srcData++;
        }

        if (!(((int)length) & 0x2)) {
          (*(FxU16*)dstData) = (*(FxU16 *)srcData);
          dstData = (FxU32*)(((FxU16*)dstData) + 1);
          srcData = (FxU32*)(((FxU16*)srcData) + 1);
        }
                
        dstData = (FxU32*)(((char*)dstData)+dstJump);
        srcData = (FxU32*)(((char*)srcData)+srcJump);
      }
    }
  }
#endif  
    grLfbUnlock(GR_LFB_READ_ONLY, src_buffer);
  } else {
    rv = FXFALSE;
  }
  GR_RETURN(rv);
#undef FN_NAME
}/* grLfbReadRegion */
#endif /* if __POWERPC__ */

