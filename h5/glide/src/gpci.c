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
** $Header: /cvsroot/glide/glide2x/h3/glide/src/gpci.c,v 1.2 2000/10/03 18:29:56 mercury Exp $
** $Log: gpci.c,v $
** Revision 1.2  2000/10/03 18:29:56  mercury
** 003-clean_up_h3-000, h3 tree cleanup.
**
** Revision 1.1.1.1  1999/12/07 21:49:28  joseph
** Initial checkin into SourceForge.
**
** 5     8/29/02 KoolSmoky fixed help screen not showing in test.
**
** 4     KoolSmoky stole bits from glide3
** 
** 3     3/17/99 6:16p Dow
** Phantom Menace and other fixes.
** 
** 166   3/17/99 5:08p Peter
** removed whacky stuff now that the command fifo threshold stuff appears
** to make all happy (including the k7)
** 
** 165   3/17/99 1:37p Atai
** use grHints to enable/disable uma hack
** 
** 164   3/16/99 11:51a Atai
** Back door (set FX_GLIDE_ENABLE_UMA=1) to enable unified texture memory.
** TMUn memory size will the whole texture memory space. The offset for
** each TMU points to the start address of the memory pool.
** 
** 163   3/15/99 10:51p Dow
** Vile Hack
** 
** 162   3/13/99 9:48p Dow
** optimizations for B&G
** 
** 161   3/12/99 2:31p Dow
** Removed 3DNow for K7 (temp workaround)
** 
** 160   3/08/99 6:11p Atai
** report Voodoo3 fbi/tmu rev number as Banshee for EA games
** 
** 159   3/06/99 10:59a Atai
** fixed my ·F check-in.
** 
** 158   3/05/99 2:50p Atai
** fbi/tmu rev mods
** 
** 157   3/04/99 3:15p Atai
** mods for direct write
** 
** 156   12/09/98 2:07p Peter
** More Norbert's stuff for the other 3DNow!(tm) partners
** 
** 155   12/03/98 11:27p Dow
** Code 'cleanup' heç
** 
** 154   12/03/98 10:34p Dow
** Added GLIDE_FGETENV for floats and removed registry code
** 
** 153   12/03/98 12:37p Dow
** Fixed DOS build
** 
** 152   12/02/98 2:53p Dow
** NT/9X Registry reading fix
** 
** 151   11/19/98 9:53p Jeske
** make sure we look for Voodoo3/avenger also...
** 
** 150   11/18/98 7:59p Dow
** grxclk
** 
** 149   11/18/98 7:44p Atai
** use env var FX_GLIDE_NUM_TMU
** 
** 148   11/17/98 7:04p Atai
** added env var "FX_GLIDE_DISABLE_TMU1"
** 
** 147   11/10/98 6:44p Atai
** number of tmu and texture memory allocation
** 
** 146   11/09/98 3:32p Mikec
** 
** 143   11/05/98 1:55p Atai
** initialize 2nd tmu configs
** 
** 142   10/21/98 4:20p Atai
** gamma stuff
** 
** 141   10/21/98 10:41a Atai
** 
** 140   10/20/98 5:34p Atai
** added #ifdefs for hwc
** 
** 139   10/19/98 2:11p Peter
** ctrisetup happiness
** 
** 138   10/09/98 6:57p Peter
** dynamic 3DNow!(tm) mods
** 
** 137   9/24/98 7:40p Dow
** GL Driver Stuff
** 
** 136   9/24/98 12:01p Dow
** Turned on extra unmentionable games
** 
** 135   9/21/98 4:00p Dow
** Added to the unmentionable
** 
** 134   9/04/98 11:36a Peter
** re-open fix for nt (thanks to taco/rob/nt bob)
** 
** 133   8/30/98 11:15a Atai
** added tigerwood 99 to the game list
** 
** 132   8/27/98 6:35p Atai
** getenv FX_GLIDE_TMU_MEMSIZE
** 
** 131   8/27/98 1:58p Peter
** fill in hwConfig union"
** 
** 130   8/26/98 10:08p Atai
** return the correct reg path
** 
** 129   8/20/98 10:08a Dow
** Fix for registry GETENV stuff
** 
** 128   8/14/98 10:25a Dow
** Fixed hwConfig union effage
** 
** 127   7/24/98 2:03p Dow
** AGP Stuff
** 
** 126   7/23/98 1:17a Dow
** Bump & Grind
** 
** 125   7/18/98 7:24p Mikec
** Made win32 calls invisible to DOS compilation.
** 
** 123   7/18/98 5:13p Mikec
** EAhack done.
** 
** 122   7/14/98 2:48p Mikec
** Added Glide environment variable FX_GLIDE_EMUL_RUSH allow Banshee glide
** to report itself as Rush to the application. Set it to 1 to enable Rush
** reporting. By default Glide still reports itself as Voodoo Graphics. 
** 
** 121   7/13/98 5:32p Dow
** GETENV from registry
** 
** 120   7/09/98 11:49a Jdt
** Fix fencing for dos build
** 
** 119   7/09/98 10:19a Dow
** Registry getenv
** 
** 118   7/06/98 7:05p Jdt
** initenvironment simplified
** 
** 117   7/06/98 11:06a Mikec
** Added fbiRev offset to distinguish Banshee from Voodoo.
** Banshee check:
** If (hwconfig.SSTs[0].sstBoard.VoodooConfig.fbiRev > 0x1000)
** 
** 116   6/24/98 11:16a Dow
** Fixed DLLMAin messages
** 
** 115   6/09/98 5:04p Dow
** %$#@!
** 
** 114   6/09/98 2:39p Mikec
** 
** 113   6/04/98 6:53p Dow
** Resolutions to 1600x1200
** 
** 112   6/03/98 5:23p Dow
** Fixed DOS effage
** 
** 111   6/03/98 1:39p Dow
** dll main
** 
** 110   5/31/98 9:03a Dow
** 800x600 resolution
** 
** 109   5/22/98 2:37p Peter
** complete the lie that is glide2x on Banshee
** 
** 108   5/21/98 4:47p Dow
** Direct Register Writes Work
** 
** 107   5/18/98 3:20p Peter
** pts more resistant to changing rounding modes
** 
** 106   5/15/98 2:21p Dow
** Changed from Voodoo Rush to Voodoo
** 
** 105   5/12/98 2:42p Dow
** 
** 104   4/14/98 6:41p Peter
** Merge w/ cvg glide cleanup
** 
** 103   4/07/98 10:40p Dow
** LFB Fixes:  Round 1
** 
** 102   4/05/98 2:18p Dow
** DOS Glide Stuff
** 
** 101   4/03/98 2:11p Dow
** 
** 100   3/28/98 11:24a Dow
** itwoç
** 
** 99    3/11/98 8:28p Dow
** WinGlide
** 
** 97    2/08/98 3:08p Dow
** FIFO Works
** 
** 96    2/02/98 4:31p Dow
** IO w/o HAL now possible
** 
** 95    1/20/98 11:03a Peter
** env var to force triple buffering
 * 
 * 93    1/16/98 5:41p Peter
 * fixed sense of lod_dither
 * 
 * 92    1/14/98 10:22a Peter
 * no more hacks
 * 
 * 91    1/08/98 7:09p Peter
 * real hw stuff modulo makefile change
 * 
 * 90    1/07/98 11:18a Atai
 * remove GrMipMapInfo and GrGC.mm_table in glide3
 * 
 * 89    1/07/98 10:22a Peter
 * lod dithering env var
 * 
 * 88    12/17/97 10:08a Peter
 * fast system comdex twiddling
 * 
 * 87    12/09/97 4:20p Peter
 * 0x100 fbiRev ofset for v2
 * 
 * 86    12/09/97 12:20p Peter
 * mac glide port
 * 
 * 85    12/05/97 4:26p Peter
 * watcom warnings
 * 
 * 84    12/03/97 2:36p Peter
 * upped comdex reset defaults
 * 
 * 83    12/02/97 9:48a Dow
 * Removed some spurious code I inadvertantly added.
 * 
 * 82    11/21/97 6:24p Dow
 * Banshee Lying about being Rush stuf
 * 
 * 81    11/21/97 11:19a Dow
 * Made Banshee report Voodoo2
 * 
 * 80    11/20/97 6:39p Peter
 * 
 * 79    11/19/97 2:49p Peter
 * env vars in registry for win32
 * 
 * 78    11/17/97 4:55p Peter
 * watcom warnings/chipfield stuff
 * 
 * 77    11/15/97 7:43p Peter
 * more comdex silliness
 * 
 * 76    11/14/97 11:10p Peter
 * open vs hw init confusion
 * 
 * 75    11/14/97 5:02p Peter
 * more comdex stuff
 * 
 * 74    11/14/97 12:09a Peter
 * comdex thing and some other stuff
 * 
 * 73    11/12/97 9:54p Peter
 * fixed all the effage from new config
 * 
 * 72    11/12/97 9:37p Dow
 * Textures on Banshee half work
 * 
 * 71    11/12/97 9:22a Dow
 * h3 mods
 * 
 * 70    11/08/97 3:34p Peter
 * fixed stupid gdbg_info crasher
 * 
 * 69    11/04/97 4:00p Dow
 * Banshee Mods
 * 
 * 68    11/03/97 3:43p Peter
 * h3/cvg cataclysm
 * 
 * 67    11/01/97 12:11p Pgj
 * glide.dll ---> glide2x.dll
 * 
 * 66    10/31/97 8:53a Peter
 * last lying change, really
 * 
 * 65    10/30/97 3:42p Peter
 * protected the last bit of nonsense
 * 
 * 64    10/30/97 3:37p Peter
 * spoof sst1 stuff
 * 
 * 63    10/29/97 2:45p Peter
 * C version of Taco's packing code
 * 
 * 62    10/23/97 5:28p Peter
 * sli fifo thing
 * 
 * 61    9/15/97 7:31p Peter
 * more cmdfifo cleanup, fixed normal buffer clear, banner in the right
 * place, lfb's are on, Hmmmm.. probably more
 * 
 * 60    9/10/97 10:13p Peter
 * fifo logic from GaryT, non-normalized fp first cut
 * 
 * 59    9/05/97 5:29p Peter
 * changes for direct hw
 * 
 * 58    9/01/97 3:18p Peter
 * correct integer rounding for pts
 * 
 * 57    8/30/97 5:59p Tarolli
 * init and hal fixups
 * 
 * 56    8/30/97 1:19p Peter
 * first cut at using blit to clear, more to come to do inner rects
 * 
 * 55    8/18/97 3:52p Peter
 * pre-hw arrival fixes/cleanup
 * 
 * 54    7/30/97 2:42p Peter
 * shared and sanitized
 * 
 * 53    7/28/97 2:41p Peter
 * turned sli code back on for cvg, but waiting for hal
 * 
 * 52    7/25/97 11:40a Peter
 * removed dHalf, change field name to match real use for cvg
 * 
 * 51    7/08/97 2:47p Peter
 * fixed merge stupidity from last checkin
 * 
 * 50    7/02/97 12:28p Peter
 * removed spurious NOP, tex dl
 * 
 * 49    6/24/97 4:02p Peter
 * proper cmd fifo placement
 * 
 * 48    6/23/97 4:46p Peter
 * fixed my effage
 * 47    6/23/97 4:43p Peter
 * cleaned up #defines etc for a nicer tree
**
*/

#include <stdio.h>
#include <string.h>

#include <3dfx.h>
#include <glidesys.h>

#define FX_DLL_DEFINITION
#include <fxdll.h>
#include <glide.h>
#include "fxglide.h"

#if (GLIDE_PLATFORM & GLIDE_OS_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

/* Some macros to prevent RSI */
#define GC      _GlideRoot.GCs[ctx]
#define SST     _GlideRoot.hwConfig.SSTs[ctx]

#define REGSTR_PATH_3DFXSW              "Software\\3Dfx Interactive\\Voodoo2"
#define REGSTR_PATH_GLIDE               REGSTR_PATH_3DFXSW"\\Glide"

#if !defined(GLIDE_INIT_HAL)
#ifdef GETENV 
#undef GETENV
#endif
//#if defined(HWC_EXT_INIT)
#define GETENV(a, b) hwcGetenvEx(a, b)
//#else
//#define GETENV(a, b) hwcGetenv(a)
//#endif
#endif

/* Windows */
#define SEPARATOR '\\'
/* UNIX */
#define SEPARATOR2 '/'

#define kRevisionOffset  0x1000

/* null function procs that will get called when !gc->contextP. */
static FxI32 FX_CALL
_trisetup_null(const void* a, const void* b, const void* c)
{
  /* Not really accurate, but pretend its been culled for some reason */
  return 0;
}

static void FX_CALL
_grTexDownload_null(struct GrGC_s* gc, const FxU32 tmuBaseAddr,
                    const FxI32 maxS, const FxI32 minT, const FxI32 maxT,
                    void* texData)
{
  /* Do Nothing */
}

#if GLIDE_DISPATCH_SETUP
/* Collection of all of the known procs for a given system */
static GrTriSetupProc _triSetupProcs[][NUM_TRI_PROC_LISTS][2] = 
{
  /* Default Procs */
  {
    { _trisetup_Default_Default, _trisetup_Default_cull },
#if GLIDE_PACKED_RGB
    { _trisetup_Default_rgb,     _trisetup_Default_cull_rgb },
    { _trisetup_Default_argb,    _trisetup_Default_cull_argb },
#endif /* GLIDE_PACKED_RGB */
  },
#if GL_AMD3D
  /* 3DNow!(tm) Procs */
  {
    { _trisetup_3DNow_Default, _trisetup_3DNow_cull },
#if GLIDE_PACKED_RGB
    { _trisetup_3DNow_rgb,     _trisetup_3DNow_cull_rgb },
    { _trisetup_3DNow_argb,    _trisetup_3DNow_cull_argb },
#endif /* GLIDE_PACKED_RGB */
  },
#endif /* GL_AMD3D */
  /* null arch procs */
  {
    { _trisetup_null, _trisetup_null },
#if GLIDE_PACKED_RGB
    { _trisetup_null, _trisetup_null },
    { _trisetup_null, _trisetup_null },
#endif /* GLIDE_PACKED_RGB */
  },
};
#endif /* GLIDE_DISPATCH_SETUP */

static GrTexDownloadProc _texDownloadProcs[][2][4] = 
{
  /* Default Procs */
  { 
    {
      _grTexDownload_Default_8_1, 
      _grTexDownload_Default_8_2,
      _grTexDownload_Default_8_4,
      _grTexDownload_Default_8_WideS
    }, 
    {
      _grTexDownload_Default_16_1,
      _grTexDownload_Default_16_2,
      _grTexDownload_Default_16_WideS,
      _grTexDownload_Default_16_WideS
    }
  },
#if GL_AMD3D
  { 
    { 
      _grTexDownload_Default_8_1, 
      _grTexDownload_Default_8_2, 
      _grTexDownload_Default_8_4, 
      _grTexDownload_3DNow_MMX, 
    },
    {
      _grTexDownload_Default_16_1,
      _grTexDownload_Default_16_2,
      _grTexDownload_3DNow_MMX,
      _grTexDownload_3DNow_MMX,
    },
  },
#endif /* GL_AMD3D */
  /* null arch procs */
  {
    {
      _grTexDownload_null,
      _grTexDownload_null,
      _grTexDownload_null,
      _grTexDownload_null,
    },
    {
      _grTexDownload_null,
      _grTexDownload_null,
      _grTexDownload_null,
      _grTexDownload_null,
    },
  }
};

BOOL WINAPI 
DllMain(HANDLE hInst, ULONG  ul_reason_for_call, LPVOID lpReserved) 
{
  switch( ul_reason_for_call ) {
  case DLL_PROCESS_DETACH:
    GDBG_INFO(80, "DllMain: DLL_PROCESS_DETACH\n");
    grGlideShutdown();
#ifdef GLIDE_INIT_HWC
    {
      GR_DCL_GC;
      hwcUnmapMemory();
      hwcClearContextData();
    }
#endif
    break;
  case DLL_PROCESS_ATTACH:
    GDBG_INFO(80, "DllMain: DLL_PROCESS_ATTACH\n");
    break;
  case DLL_THREAD_ATTACH:
    GDBG_INFO(80, "DllMain: DLL_THREAD_ATTACH\n");
    break;
  case DLL_THREAD_DETACH:
    GDBG_INFO(80, "DllMain: DLL_THREAD_DETACH\n");
    break;
  default:
    GDBG_INFO(80, "DllMain: Unhandled message.\n");
    break;
  }
  
  return TRUE;

} /* DllMain */

/*
 * parseFilename
 *
 *      Return the file name portion of a filename/path.
 */
char *
_parseFilename(char *name)
{
    int i;

    if (name == NULL) 
      return NULL;
    for(i = strlen(name); i >= 0; i--)
        if ((name[i] == SEPARATOR) ||
            (name[i] == SEPARATOR2))
            return (name + i + 1);
    return name;
}  /* End of parseFilename*/


/*-------------------------------------------------------------------
  Function: _grSstDetectResources
  Date: --
  Implementor(s): Dow, Gmt, Jdt
  Library: Glide
  Description:
  Discover devices on the PCI bus.
  Discover configuration of detected devices.
  Initialize all Glide GC's 

  Recognized devices depend upon compile time flags

  This code should NOT initialize the hardware 
  any more than is necessary for discovery of 
  configuration

  Arguments: none
  Return: 
  FXTRUE  - at least one device was detected
  FXFALSE - no devices were detected.
  -------------------------------------------------------------------*/
FxBool 
_grSstDetectResources(void)
{
  static FxBool calledP = FXFALSE;
  FxBool rv = FXFALSE;

  FxU32 ctx;

  GDBG_INFO(280, "_grSstDetectResources()\n");

  if (!calledP) {
#if GLIDE_INIT_HAL
  FxU32 count = HAL_MAX_BOARDS;
  FxU32 chipCount = 1;

  /* The first time through the init code we need to map 
   * all of the boards. Future calls can just grab this
   * info out of the halInfo that we have here.
   */
    FxU32     device;
    HalInfo* halInfo = fxHalInit(0);
    if (halInfo == NULL) goto __errExit;
    
    {
      /*
      ** hack alert!!
      ** by default, there is no device around so we hack the device 
      ** number in here (copy code from diag).
      */
      char* envChipNum = GETENV("FX_GLIDE_NUM_CHIPS");
      FxBool useMultiFunctionDevices=FXFALSE;
      FxU32 busNumber, deviceNumber, functionNumber;
      FxU32 counter;

      if (envChipNum)
        chipCount = atoi(envChipNum);

      switch(chipCount) {
      case 4:
        busNumber      = 3;
        deviceNumber   = 2;
        functionNumber = 0;
        halInfo->boardInfo[2].pciBusNumber = busNumber;
        halInfo->boardInfo[2].pciDeviceNumber = deviceNumber;
        halInfo->boardInfo[2].pciFunctionNumber = functionNumber; 
        halInfo->boardInfo[2].deviceNumber = busNumber * 32 + deviceNumber;

        busNumber      = 3;
        deviceNumber   = 2;
        functionNumber = 1;
        halInfo->boardInfo[3].pciBusNumber = busNumber;
        halInfo->boardInfo[3].pciDeviceNumber = deviceNumber;
        halInfo->boardInfo[3].pciFunctionNumber = functionNumber; 
        halInfo->boardInfo[3].deviceNumber = busNumber * 32 + deviceNumber;

      case 2:
        busNumber      = 2;
        deviceNumber   = 1;
        functionNumber = 1;
        halInfo->boardInfo[1].pciBusNumber = busNumber;
        halInfo->boardInfo[1].pciDeviceNumber = deviceNumber;
        halInfo->boardInfo[1].pciFunctionNumber = functionNumber; 
        halInfo->boardInfo[1].deviceNumber = busNumber * 32 + deviceNumber;

        useMultiFunctionDevices=FXTRUE;
      case 1:
      default:
        busNumber      = 2;
        deviceNumber   = 1;
        functionNumber = 0;
        halInfo->boardInfo[0].pciBusNumber = busNumber;
        halInfo->boardInfo[0].pciDeviceNumber = deviceNumber;
        halInfo->boardInfo[0].pciFunctionNumber = functionNumber; 
        halInfo->boardInfo[0].deviceNumber = busNumber * 32 + deviceNumber;
        break;
      }
      /* Initialize the csim boards/chips */
      for(counter=0; counter<chipCount; counter++) {
        halInfo->boardInfo[counter].sstCSIM = csimInit(counter);      
        if(useMultiFunctionDevices)
          csimMakeMultiFunctionDevice(halInfo->boardInfo[counter].sstCSIM);
        CSIM_PRIVATE(halInfo->boardInfo[counter].sstCSIM)->environment.chipCount = chipCount;
        CSIM_PRIVATE(halInfo->boardInfo[counter].sstCSIM)->environment.sliBandHeight 
          = _GlideRoot.environment.sliBandHeight;
        
      }
    }
    
#if (GLIDE_PLATFORM & GLIDE_SST_SIM)
    /* The simulator can support any number of boards through
     * successive calls to fxHalMapBoard.
     */
    {
      const char* envBoardNum = GETENV("FX_SIM_BOARDS");
      FxU32 temp;
        
      count = (((envBoardNum != NULL) && (sscanf(envBoardNum, "%ld", &temp) == 1)) 
               ? temp
               : 1);
      if (count > HAL_MAX_BOARDS) {
        GDBG_INFO(0, "Error: FX_SIM_BOARDS(%ld) > %d. Using %d.\n",
                  count, HAL_MAX_BOARDS, HAL_MAX_BOARDS);
        count = HAL_MAX_BOARDS;
      }
      count *= chipCount;
    }
#endif /* (GLIDE_PLATFORM & GLIDE_SST_SIM) */


    for(ctx = device = 0; device < count; device++) {
      SstRegs* devRegs;
      devRegs = fxHalMapBoard(device);
    }

    for(ctx = device = 0; device < count; device++) {
      /* See RSI-prevention macros for usage of [ctx] */
      const FxDeviceInfo* curDev = NULL;
      FxBool regInitP = FXFALSE;
      SstRegs* devRegs;

      devRegs = fxHalMapBoard(device);
      if ((device % chipCount) == 0)
        GC.is_master = FXTRUE;
      else
        GC.is_master = FXFALSE;

      GC.chipCount  = chipCount;
      GC.sliBandHeight = _GlideRoot.environment.sliBandHeight;

      curDev = halInfo->boardInfo + device;

      GC.halInfo = halInfo;

      if (devRegs != NULL) {
        FxU32 tmuMem = 0x00;

        SST.type = GR_SSTTYPE_Banshee;

        if (!fxHalInitRegisters(curDev->virtAddr[0])) goto __errRegFailure;
          
#if USE_PACKET_FIFO && GLIDE_DEBUG && HAL_CSIM
        if ((halInfo->csim == 1) && (GETENV("FX_FIFO_DIRECT_EXEC") != NULL)) {
          //            halInfo->csim = -1;
          GDBG_INFO(80, "Turning on direct fifo execution\n");
        }
#endif /* USE_PACKET_FIFO && GLIDE_DEBUG && HAL_CSIM */

#ifdef DIRECT_IO
        GC.sstRegs = (SstRegs *) (curDev->physAddr[0] + 0x200000);
        GC.ioRegs = (SstIORegs *) (curDev->physAddr[0]);
        GC.cRegs = (SstCRegs *) (curDev->physAddr[0] + 0x80000);
        GC.rawLfb = (FxU32 *) curDev->physAddr[1];
#endif /* DIRECT_IO */

        /* This device is ready to go. */
        regInitP = FXTRUE;

        /* Set up pointers to the various address spaces within the hw */
        GC.base_ptr  = (FxU32*)SST_BASE_ADDRESS(curDev->physAddr[0]);
        GC.reg_ptr   = (FxU32*)devRegs;
        GC.lfb_ptr   = (FxU32*)SST_LFB_ADDRESS(curDev->physAddr[0]);
        GC.tex_ptr   = (FxU32*)SST_TEX_ADDRESS(curDev->physAddr[0]);
        
        GC.sstRegs = devRegs;
        GC.ioRegs = (SstIORegs *) SST_IO_ADDRESS(devRegs);
        GC.cRegs = (SstCRegs *) SST_CMDAGP_ADDRESS(devRegs);
        GC.gRegs = (SstGRegs *) SST_GUI_ADDRESS(devRegs);
#define SST_RAW_LFB_ADDRESS(sst)    (SST_RAW_LFB_OFFSET+SST_BASE_ADDRESS(sst))
        GC.rawLfb = (FxU32 *) SST_RAW_LFB_ADDRESS(devRegs);
        /* GC.rawLfb = (FxU32 *) SST_LFB_ADDRESS(devRegs); */
#ifdef FX_GLIDE_NAPALM
        /* force the deviceID to 7 for code development */
        {
          static hwcBoardInfo tmpbInfo;
          GC.bInfo = &tmpbInfo;
          GC.bInfo->pciInfo.deviceID = SST_DEVICE_ID_AP_OEM ;
        }
#endif

        /* Video parameters */
        GC.grSstRez     = GR_RESOLUTION_NONE;
        GC.grSstRefresh = curDev->fbiVideoRefresh;

        /* Chip configuration */
        GC.num_tmu              = curDev->numberTmus;
        GC.fbuf_size            = curDev->fbiMemSize;
        
        _GlideRoot.hwConfig.num_sst++;
        SST.sstBoard.SST96Config.fbRam    = curDev->fbiMemSize;
        SST.sstBoard.SST96Config.nTexelfx = curDev->numberTmus;

        {
          const FxU32 curTmuMemSize = 0x2;
              
          SST.sstBoard.SST96Config.tmuConfig.tmuRev = (2 + kRevisionOffset);
              
          SST.sstBoard.SST96Config.tmuConfig.tmuRam = curTmuMemSize;
          tmuMem += curTmuMemSize;

          /* Clear the tmu state */
          memset(&GC.tmu_state[0], 0, sizeof(GC.tmu_state[0]));       
          GC.tmu_state[0].total_mem = (curTmuMemSize << 20);
              
          GC.tmu_state[0].ncc_mmids[0] = 
            GC.tmu_state[0].ncc_mmids[1] = GR_NULL_MIPMAP_HANDLE;
        }
          
        rv = FXTRUE;
        ctx++;
      }

      __errRegFailure:
      /* Either this is not the hw we're expecting, or we could not
       * init/map the board for some reason. Either way try to cleanup.  
       */
      if (!regInitP && (devRegs != NULL)) {
        fxHalShutdown(devRegs);
      }
    }

    /* Done setting up. Don't do the silly mapping thing again. */
#else  /* GLIDE_INIT_HWC */
    /* There's a left brace before the #if */
    hwcBoardInfo
      *bInfo;
    hwcInfo
      *hInfo;                   /* Info about all the relavent boards */
    int tmu;
    char* envChipNum;
    FxU32 chipCount = 1;

#ifndef __linux__
    if ((hInfo = hwcInit(0x121a, 0x3)) == NULL)
      goto __errExit; 
#else	/* defined(__linux__) */
    if ((hInfo = hwcInit(0x121a, 0x9)) == NULL)
      goto __errExit; 
#endif	/* defined(__linux__) */

    /* Iterate through boards found */
    for (ctx = 0; ctx < hInfo->nBoards; ctx++) {
      
      bInfo = &hInfo->boardInfo[ctx];

      GC.bInfo = bInfo;

      /*
      ** hack alert!!
      ** the chipCount should come from the minivdd
      ** KoolSmoky - chipCount should be after hwcInit(0x....) because
      ** it calls GETENV.
      */
      envChipNum = GETENV("FX_GLIDE_NUM_CHIPS", GC.bInfo->RegPath);
      
      if (envChipNum)
        chipCount = atoi(envChipNum);

      if (bInfo->pciInfo.deviceID == SST_DEVICE_ID_H3)
        SST.type = GR_SSTTYPE_Banshee;
#ifdef FX_GLIDE_NAPALM
      else if (IS_NAPALM(bInfo->pciInfo.deviceID))
        SST.type = GR_SSTTYPE_Voodoo4;
#endif
      else 
        SST.type = GR_SSTTYPE_Voodoo3;

      if (!hwcMapBoard(bInfo, HWC_BASE_ADDR_MASK)) {
        GrErrorCallback(hwcGetErrorString(), FXTRUE);
      }

      if (!hwcInitRegisters(bInfo)) {
        GrErrorCallback(hwcGetErrorString(), FXTRUE);
      }

      /* NB: We cannot fail to map this board after this point */
      //GC.hwInitP = FXTRUE;

      _GlideRoot.hwConfig.num_sst++;

      GC.base_ptr = (FxU32 *) bInfo->regInfo.sstBase;
      GC.sstRegs = (SstRegs *) bInfo->regInfo.sstBase;
      GC.ioRegs = (SstIORegs *) bInfo->regInfo.ioMemBase;
      GC.cRegs = (SstCRegs *) bInfo->regInfo.cmdAGPBase;
      GC.lfb_ptr = (FxU32 *) bInfo->regInfo.lfbBase;
      GC.rawLfb = (FxU32 *) bInfo->regInfo.rawLfbBase;

      GC.chipCount  = bInfo->pciInfo.numChips;
      /* KCD: This can't be done here, since the environment variables
              may not have been set up yet! */
      /* GC.sliBandHeight = _GlideRoot.environment.sliBandHeight; */

#ifdef FX_GLIDE_NAPALM
      /* AJB- Point at slave chip regs
      */ 
      {
        FxU32 chip ;

        if (GC.chipCount)
          for (chip = 0 ;
               chip < GC.chipCount - 1 ;
               chip++)
          {
            GC.slaveSstRegs[chip] = (SstRegs  *)bInfo->regInfo.slaveSstBase[chip] ;
            GC.slaveCRegs[chip] =   (SstCRegs *)bInfo->regInfo.slaveCmdBase[chip] ;
          }
      }
#endif

      /* Video Parameters */
      GC.grSstRez = GR_RESOLUTION_NONE;
      GC.grSstRefresh = 0L;

      /* This is a uma device */
      switch (hInfo->boardInfo[ctx].pciInfo.deviceID) {
      case SST_DEVICE_ID_H3: /* Banshee */
        GC.num_tmu = 1;
        GC.fbuf_size = (bInfo->h3Mem - 2);
        break;
      case SST_DEVICE_ID_H4: /* Avenger low speed */
      case SST_DEVICE_ID_H4_OEM: /* Avenger high speed */
        /*
        ** For 8M board, we may only use one tmu for higher resolution.
        ** Need to re-visit the issue. 11/5/98
        */ 
        GC.num_tmu = 2;
        GC.fbuf_size = (bInfo->h3Mem - 4);
        break;
      default:
        /* KoolSmoky - This is NAPALM.
        */
        if ((hInfo->boardInfo[ctx].pciInfo.deviceID >= SST_DEVICE_ID_L_AP) &&
            (hInfo->boardInfo[ctx].pciInfo.deviceID <= SST_DEVICE_ID_H_AP)) {
          GC.num_tmu = 2;
          GC.fbuf_size = (bInfo->h3Mem - 4);
        } else {
          GC.num_tmu = 1;
          GC.fbuf_size = (bInfo->h3Mem - 2);
        }
        break;
      }
      
      if (bInfo->h3Mem == 4) {
        GC.num_tmu = 1;
        GC.fbuf_size = (bInfo->h3Mem - 2);
      }
      
      if (GETENV("FX_GLIDE_NUM_TMU", GC.bInfo->RegPath)) {
        int num_tmu = atoi(GETENV("FX_GLIDE_NUM_TMU", GC.bInfo->RegPath));
        switch (num_tmu) {
        case 1:
          GC.num_tmu = 1;
          GC.fbuf_size = (bInfo->h3Mem - 2);
          break;
        case 2:
          GC.num_tmu = 2;
          GC.fbuf_size = (bInfo->h3Mem - 4);
          break;
        }         
      }
      
      //GC.state.grEnableArgs.combine_ext_mode = GR_MODE_DISABLE;

      /* KoolSmoky - UMA for the TMUs */
      _GlideRoot.environment.enUma = FXFALSE;
      if( GETENV("FX_GLIDE_TEXTURE_UMA", GC.bInfo->RegPath) ) {
        if( atoi(GETENV("FX_GLIDE_TEXTURE_UMA", GC.bInfo->RegPath)) == 1 ) {
          _GlideRoot.environment.enUma = FXTRUE;
        } else {
          _GlideRoot.environment.enUma = FXFALSE;
        }
      }

      {
        int tmu;
        
        /* Default to the minimum texture memory that we will
         * advertise for any screen resolution.  
         */
        if (_GlideRoot.environment.emulRush) {
          SST.type = GR_SSTTYPE_SST96;
          SST.sstBoard.SST96Config.fbRam =  GC.fbuf_size;
          
          SST.sstBoard.SST96Config.nTexelfx = GC.num_tmu;
          SST.sstBoard.SST96Config.tmuConfig.tmuRev = (2 + kRevisionOffset);
          SST.sstBoard.SST96Config.tmuConfig.tmuRam = 0x2;
        } else {
          /* Banshee's ID is 0x1000. Always check that it's at least
           * 0x1000 for Banshee card.  
           */
          SST.type = GR_SSTTYPE_VOODOO;
          SST.sstBoard.VoodooConfig.fbRam = GC.fbuf_size;
          SST.sstBoard.VoodooConfig.fbiRev = 2 + 0x1000;
          SST.sstBoard.VoodooConfig.sliDetect = GC.sliCount;//FXFALSE;
          
          SST.sstBoard.VoodooConfig.nTexelfx = GC.num_tmu;
          for (tmu = 0; tmu < GC.num_tmu; tmu++) {
            SST.sstBoard.VoodooConfig.tmuConfig[tmu].tmuRev = (2 + kRevisionOffset);
            SST.sstBoard.VoodooConfig.tmuConfig[tmu].tmuRam = 0x2;
          }
        }

        if (hInfo->boardInfo[ctx].pciInfo.deviceID > SST_DEVICE_ID_H3) {
          if (_GlideRoot.environment.emulRush) {
            SST.sstBoard.SST96Config.tmuConfig.tmuRev |= 0x10000;
          } else {
            SST.sstBoard.VoodooConfig.fbiRev |= 0x10000;
            SST.sstBoard.VoodooConfig.tmuConfig[0].tmuRev |= 0x10000;
            SST.sstBoard.VoodooConfig.tmuConfig[1].tmuRev |= 0x10000;
          }
        }
        
      }
      
      /* Clear the tmu state */
      for (tmu = 0; tmu < GC.num_tmu; tmu++) {
        memset(&GC.tmu_state[0], 0, sizeof(GC.tmu_state[0]));       
        GC.tmu_state[0].total_mem = (0x2 << 20);
        GC.tmu_state[0].ncc_mmids[0] = GC.tmu_state[0].ncc_mmids[1] = GR_NULL_MIPMAP_HANDLE;
      }

    } /* iterate through boards found */
#endif
  } 

  /* Did we previously find boards? */
  rv = (_GlideRoot.hwConfig.num_sst != 0);
  calledP = FXTRUE;

  goto __errExit; /* Keep warnings happy */
 __errExit:
  return rv;
} /* _grSstDetectResources */


static void
displayBoardInfo(int i, GrHwConfiguration *hwc)
{
  if ((hwc->SSTs[i].type == GR_SSTTYPE_VOODOO) ||
      (hwc->SSTs[i].type == GR_SSTTYPE_Voodoo2)) {
    int tmuNum;

    GDBG_INFO(80,"SST board %d: 3Dfx Voodoo%s\n", 
              i, ((hwc->SSTs[i].type == GR_SSTTYPE_VOODOO) ? " Graphics" : "^2"));
    if (hwc->SSTs[i].sstBoard.VoodooConfig.sliDetect) {
      GDBG_INFO(80,"\tScanline Interleaved\n");
    }

    GDBG_INFO(80,"\tPixelfx rev 0x%lX with %d MB Frame Buffer\n",
              hwc->SSTs[i].sstBoard.VoodooConfig.fbiRev,
              hwc->SSTs[i].sstBoard.VoodooConfig.fbRam);
    GDBG_INFO(80,"\t%d Texelfx chips:\n",
              hwc->SSTs[i].sstBoard.VoodooConfig.nTexelfx);
    for (tmuNum = 0;
         tmuNum < hwc->SSTs[i].sstBoard.VoodooConfig.nTexelfx;
         tmuNum++) {
      GDBG_INFO(80,"\t\tTexelfx %d: Rev 0x%lX, %d MB Texture\n", tmuNum,
                hwc->SSTs[i].sstBoard.VoodooConfig.tmuConfig[tmuNum].tmuRev,
                hwc->SSTs[i].sstBoard.VoodooConfig.tmuConfig[tmuNum].tmuRam);
    }
  } else if (hwc->SSTs[i].type == GR_SSTTYPE_SST96) {
    GDBG_INFO(80,"SST board %d: 3Dfx Voodoo Rush\n", i);
    GDBG_INFO(80,"\tFBI Jr. with %d MB Frame Buffer\n",
              hwc->SSTs[i].sstBoard.SST96Config.fbRam);
    GDBG_INFO(80,"\tTexelfx chips:  1\n");
  } else {
    GDBG_INFO(80,"error: SSTs %d: unknown type\n",i);
  }
} /* displayBoardInfo */


#if defined( __WATCOMC__ )
FxU32 p6FenceVar;
#endif

void
_GlideInitEnvironment(int which)
{
#define FN_NAME "_GlideInitEnvironment"
  /* int i; */
  FxU32 ditherMode;
  const char* envStr;
  FxU32 ctx = which;
  double pi = 3.1415926535;
  const char* envStr2;

#if (GLIDE_PLATFORM & GLIDE_OS_WIN32)
  OSVERSIONINFO ovi;
#endif

  if (_GlideRoot.initialized) /* only execute once */
    return;
  
  GDBG_INIT();                          /* init the GDEBUG libraray */
  GDBG_INFO(80,"%s()\n", FN_NAME);
  GDBG_INFO(0,"GLIDE DEBUG LIBRARY\n"); /* unconditional display */

  
#if (GLIDE_PLATFORM & GLIDE_OS_WIN32)

  ovi.dwOSVersionInfoSize = sizeof ( ovi );
  GetVersionEx ( &ovi );
  if (ovi.dwPlatformId == VER_PLATFORM_WIN32_NT)
    _GlideRoot.OSWin95 = 0;
  else
    _GlideRoot.OSWin95 = 1;
  GDBG_INFO(80, "%s:  OS = %s\n", FN_NAME, _GlideRoot.OSWin95 ? "W9X" : "WNT");

#if defined(FX_DLL_ENABLE) && (GLIDE_PLATFORM & GLIDE_OS_WIN32)
  { /* GMT: display the DLL pathname for sanity checking */
    char buf[132] = "failed";
    GetModuleFileName(GetModuleHandle("glide2x.dll"), buf, sizeof(buf));
    GDBG_INFO(0,"DLL path: %s\n",buf);
  }
#endif
#endif
  
  /* Check for user environment tweaks */
#define GLIDE_GETENV(__envVar, __regPath, __defVal) \
  (((envStr = GETENV(__envVar, __regPath)) == NULL) ? (__defVal) : atol(envStr))
#define GLIDE_FGETENV(__envVar, __regPath, __defVal) \
  (((envStr = GETENV(__envVar, __regPath)) == NULL) ? (__defVal) : (float) atof(envStr))
#define GLIDE_34GETENV(__envVar, __regPath, __defVal) \
  (((signed char)(atof(((envStr = GETENV(__envVar, __regPath)) == NULL) ? (__defVal) : (envStr))*16.0f)+8)&0x7f)
#define GLIDE_34GETENV_X(__envVar_x, __envVar_y, __regPath, __defVal_x, __defVal_y) \
  (signed char)((((atof(((envStr = GETENV(__envVar_x, __regPath)) == NULL) ? __defVal_x : envStr) * cos(_GlideRoot.environment.aaGridRotation*pi/180)) \
  - (atof(((envStr2 = GETENV(__envVar_y, __regPath)) == NULL) ? __defVal_y : envStr2) * sin(_GlideRoot.environment.aaGridRotation*pi/180)) \
  ) * _GlideRoot.environment.aaJitterDisp + _GlideRoot.environment.aaPixelOffset)*16.0f)&0x7f
#define GLIDE_34GETENV_Y(__envVar_x, __envVar_y, __regPath, __defVal_x, __defVal_y) \
  (signed char)((((atof(((envStr = GETENV(__envVar_x, __regPath)) == NULL) ? __defVal_x : envStr) * sin(_GlideRoot.environment.aaGridRotation*pi/180)) \
  + (atof(((envStr2 = GETENV(__envVar_y, __regPath)) == NULL) ? __defVal_y : envStr2) * cos(_GlideRoot.environment.aaGridRotation*pi/180)) \
  ) * _GlideRoot.environment.aaJitterDisp + _GlideRoot.environment.aaPixelOffset)*16.0f)&0x7f
  
  
//#ifdef GLIDE_TEST_TEXTURE_ALIGNMENT
//  SST_TEXTURE_ALIGN = GLIDE_GETENV("FX_GLIDE_TEX_ALIGN", GC.bInfo->RegPath, 0x10UL);
//#endif
  _GlideRoot.environment.triBoundsCheck    = GETENV("FX_GLIDE_BOUNDS_CHECK", GC.bInfo->RegPath) != NULL;
  GDBG_INFO(80,"    triBoundsCheck: %d\n",_GlideRoot.environment.triBoundsCheck);
#ifdef GLIDE_SPLASH
  _GlideRoot.environment.noSplash          = GETENV("FX_GLIDE_NO_SPLASH", GC.bInfo->RegPath) != NULL;
#else
  _GlideRoot.environment.noSplash          = 1;
#endif

  GDBG_INFO(80,"          noSplash: %d\n",_GlideRoot.environment.noSplash);
  _GlideRoot.environment.shamelessPlug     = GETENV("FX_GLIDE_SHAMELESS_PLUG", GC.bInfo->RegPath) != NULL;
  GDBG_INFO(80,"     shamelessPlug: %d\n",_GlideRoot.environment.shamelessPlug);
  _GlideRoot.environment.ignoreReopen      = GETENV("FX_GLIDE_IGNORE_REOPEN", GC.bInfo->RegPath) != NULL;
  GDBG_INFO(80,"      ignoreReopen: %d\n",_GlideRoot.environment.ignoreReopen);
  _GlideRoot.environment.disableDitherSub  = GETENV("FX_GLIDE_NO_DITHER_SUB", GC.bInfo->RegPath) != NULL; 
  GDBG_INFO(80,"  disableDitherSub: %d\n",_GlideRoot.environment.disableDitherSub);
  //_GlideRoot.environment.fifoSize          = GETENV("FX_GLIDE_FIFO_SIZE", GC.bInfo->RegPath) != NULL;
  //GDBG_INFO(80,"          fifoSize: %d\n",_GlideRoot.environment.fifoSize);
  //_GlideRoot.environment.noHW              = GETENV("FX_GLIDE_NO_HW", GC.bInfo->RegPath) != NULL;
  //GDBG_INFO(80,"              noHW: %d\n",_GlideRoot.environment.noHW);
  _GlideRoot.environment.aaPixelOffset     = GLIDE_FGETENV("FX_GLIDE_AA_PIXELCENTER", GC.bInfo->RegPath, 0.0f); /* default 0.0 for glide2x */
  GDBG_INFO(80,"     aaPixelOffset: %f\n",_GlideRoot.environment.aaPixelOffset);
  //_GlideRoot.environment.aaJitterDisp      = GLIDE_FGETENV("FX_GLIDE_AA_JITTERDISP", GC.bInfo->RegPath, 18.0f); /* original value 16.0f */
  //GDBG_INFO(80,"      aaJitterDisp: %f\n",_GlideRoot.environment.aaJitterDisp);
  //_GlideRoot.environment.aaGridRotation    = GLIDE_FGETENV("FX_GLIDE_AA_GRIDROTATION", GC.bInfo->RegPath, 27.0f); /* original values 2xaa=45deg 4xaa,8xaa=27deg */
  //GDBG_INFO(80,"    aaGridRotation: %f\n",_GlideRoot.environment.aaGridRotation);

  /* set default glide state to not openGL app */
  /* only if it's not already set to openGL app */
  /*if(_GlideRoot.environment.is_opengl != FXTRUE)
    _GlideRoot.environment.is_opengl = FXFALSE;*/

  /* note - glide now uses a string representation for the AA jitter values */
  /* This is the "old" way of doing two-sample AA, where each chip does two samples. */  
  _GlideRoot.environment.aaXOffset[1][0]   = GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_X0", GC.bInfo->RegPath, PRIBUFVTXOFFX_2SMPL_DEF);
  _GlideRoot.environment.aaXOffset[1][1]   = GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_X1", GC.bInfo->RegPath, SECBUFVTXOFFX_2SMPL_DEF);
  _GlideRoot.environment.aaXOffset[1][2]   = GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_X0", GC.bInfo->RegPath, PRIBUFVTXOFFX_2SMPL_DEF);
  _GlideRoot.environment.aaXOffset[1][3]   = GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_X1", GC.bInfo->RegPath, SECBUFVTXOFFX_2SMPL_DEF);

  _GlideRoot.environment.aaYOffset[1][0]   = GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFY_2SMPL_DEF);
  _GlideRoot.environment.aaYOffset[1][1]   = GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFY_2SMPL_DEF);
  _GlideRoot.environment.aaYOffset[1][2]   = GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFY_2SMPL_DEF);
  _GlideRoot.environment.aaYOffset[1][3]   = GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFY_2SMPL_DEF);

  /* This is a nasty evil hack!  This rearranges the sample offsets to deal with the new way of doing two sample AA */
  _GlideRoot.environment.aaXOffset[2][0]   = GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_X0", GC.bInfo->RegPath, PRIBUFVTXOFFX_2SMPL_DEF);
  _GlideRoot.environment.aaXOffset[2][1]   = GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_X0", GC.bInfo->RegPath, PRIBUFVTXOFFX_2SMPL_DEF);
  _GlideRoot.environment.aaXOffset[2][2]   = GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_X1", GC.bInfo->RegPath, SECBUFVTXOFFX_2SMPL_DEF);
  _GlideRoot.environment.aaXOffset[2][3]   = GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_X1", GC.bInfo->RegPath, SECBUFVTXOFFX_2SMPL_DEF);

  _GlideRoot.environment.aaYOffset[2][0]   = GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFY_2SMPL_DEF);
  _GlideRoot.environment.aaYOffset[2][1]   = GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFY_2SMPL_DEF);
  _GlideRoot.environment.aaYOffset[2][2]   = GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFY_2SMPL_DEF);
  _GlideRoot.environment.aaYOffset[2][3]   = GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFY_2SMPL_DEF);

  /* This is the "normal" layout for 4-sample AA */

  _GlideRoot.environment.aaXOffset[3][0]   = GLIDE_34GETENV("FX_GLIDE_AA4_OFFSET_X0", GC.bInfo->RegPath, PRIBUFVTXOFFX_4SMPL_CHP0_DEF);
  _GlideRoot.environment.aaXOffset[3][1]   = GLIDE_34GETENV("FX_GLIDE_AA4_OFFSET_X1", GC.bInfo->RegPath, SECBUFVTXOFFX_4SMPL_CHP0_DEF);
  _GlideRoot.environment.aaXOffset[3][2]   = GLIDE_34GETENV("FX_GLIDE_AA4_OFFSET_X2", GC.bInfo->RegPath, PRIBUFVTXOFFX_4SMPL_CHP1_DEF);
  _GlideRoot.environment.aaXOffset[3][3]   = GLIDE_34GETENV("FX_GLIDE_AA4_OFFSET_X3", GC.bInfo->RegPath, SECBUFVTXOFFX_4SMPL_CHP1_DEF);
  
  _GlideRoot.environment.aaYOffset[3][0]   = GLIDE_34GETENV("FX_GLIDE_AA4_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFY_4SMPL_CHP0_DEF);
  _GlideRoot.environment.aaYOffset[3][1]   = GLIDE_34GETENV("FX_GLIDE_AA4_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFY_4SMPL_CHP0_DEF);
  _GlideRoot.environment.aaYOffset[3][2]   = GLIDE_34GETENV("FX_GLIDE_AA4_OFFSET_Y2", GC.bInfo->RegPath, PRIBUFVTXOFFY_4SMPL_CHP1_DEF);
  _GlideRoot.environment.aaYOffset[3][3]   = GLIDE_34GETENV("FX_GLIDE_AA4_OFFSET_Y3", GC.bInfo->RegPath, SECBUFVTXOFFY_4SMPL_CHP1_DEF);  

  /* these are the correct jitter vaules */
  /* set jitter dispersity for 2xaa */
  _GlideRoot.environment.aaJitterDisp      = GLIDE_FGETENV("FX_GLIDE_AA_JITTERDISP", GC.bInfo->RegPath, 1.0625f);
  GDBG_INFO(80,"      aaJitterDisp: %f\n",_GlideRoot.environment.aaJitterDisp);
  /* set rotation for 2xaa */
  _GlideRoot.environment.aaGridRotation    = GLIDE_FGETENV("FX_GLIDE_AA_GRIDROTATION", GC.bInfo->RegPath, 45.0f) - 45.0f;
  GDBG_INFO(80,"    aaGridRotation: %f\n",_GlideRoot.environment.aaGridRotation);
  _GlideRoot.environment.aaXOffset[4][0]   = GLIDE_34GETENV_X("FX_GLIDE_AA2_OFFSET_X0", "FX_GLIDE_AA2_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFX_2SMPL_CORRECT_DEF, PRIBUFVTXOFFY_2SMPL_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_X0", GC.bInfo->RegPath, PRIBUFVTXOFFX_2SMPL_CORRECT_DEF);
  _GlideRoot.environment.aaXOffset[4][1]   = GLIDE_34GETENV_X("FX_GLIDE_AA2_OFFSET_X1", "FX_GLIDE_AA2_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFX_2SMPL_CORRECT_DEF, SECBUFVTXOFFY_2SMPL_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_X1", GC.bInfo->RegPath, SECBUFVTXOFFX_2SMPL_CORRECT_DEF);
  _GlideRoot.environment.aaXOffset[4][2]   = GLIDE_34GETENV_X("FX_GLIDE_AA2_OFFSET_X0", "FX_GLIDE_AA2_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFX_2SMPL_CORRECT_DEF, PRIBUFVTXOFFY_2SMPL_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_X0", GC.bInfo->RegPath, PRIBUFVTXOFFX_2SMPL_CORRECT_DEF);
  _GlideRoot.environment.aaXOffset[4][3]   = GLIDE_34GETENV_X("FX_GLIDE_AA2_OFFSET_X1", "FX_GLIDE_AA2_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFX_2SMPL_CORRECT_DEF, SECBUFVTXOFFY_2SMPL_CORRECT_DEF);//LIDE_34GETENV("FX_GLIDE_AA2_OFFSET_X1", GC.bInfo->RegPath, SECBUFVTXOFFX_2SMPL_CORRECT_DEF);
  _GlideRoot.environment.aaXOffset[4][4]   = GLIDE_34GETENV_X("FX_GLIDE_AA2_OFFSET_X0", "FX_GLIDE_AA2_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFX_2SMPL_CORRECT_DEF, PRIBUFVTXOFFY_2SMPL_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_X0", GC.bInfo->RegPath, PRIBUFVTXOFFX_2SMPL_CORRECT_DEF);
  _GlideRoot.environment.aaXOffset[4][5]   = GLIDE_34GETENV_X("FX_GLIDE_AA2_OFFSET_X1", "FX_GLIDE_AA2_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFX_2SMPL_CORRECT_DEF, SECBUFVTXOFFY_2SMPL_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_X1", GC.bInfo->RegPath, SECBUFVTXOFFX_2SMPL_CORRECT_DEF);
  _GlideRoot.environment.aaXOffset[4][6]   = GLIDE_34GETENV_X("FX_GLIDE_AA2_OFFSET_X0", "FX_GLIDE_AA2_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFX_2SMPL_CORRECT_DEF, PRIBUFVTXOFFY_2SMPL_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_X0", GC.bInfo->RegPath, PRIBUFVTXOFFX_2SMPL_CORRECT_DEF);
  _GlideRoot.environment.aaXOffset[4][7]   = GLIDE_34GETENV_X("FX_GLIDE_AA2_OFFSET_X1", "FX_GLIDE_AA2_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFX_2SMPL_CORRECT_DEF, SECBUFVTXOFFY_2SMPL_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_X1", GC.bInfo->RegPath, SECBUFVTXOFFX_2SMPL_CORRECT_DEF);

  _GlideRoot.environment.aaYOffset[4][0]   = GLIDE_34GETENV_Y("FX_GLIDE_AA2_OFFSET_X0", "FX_GLIDE_AA2_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFX_2SMPL_CORRECT_DEF, PRIBUFVTXOFFY_2SMPL_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFY_2SMPL_CORRECT_DEF);
  _GlideRoot.environment.aaYOffset[4][1]   = GLIDE_34GETENV_Y("FX_GLIDE_AA2_OFFSET_X1", "FX_GLIDE_AA2_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFX_2SMPL_CORRECT_DEF, SECBUFVTXOFFY_2SMPL_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFY_2SMPL_CORRECT_DEF);
  _GlideRoot.environment.aaYOffset[4][2]   = GLIDE_34GETENV_Y("FX_GLIDE_AA2_OFFSET_X0", "FX_GLIDE_AA2_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFX_2SMPL_CORRECT_DEF, PRIBUFVTXOFFY_2SMPL_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFY_2SMPL_CORRECT_DEF);
  _GlideRoot.environment.aaYOffset[4][3]   = GLIDE_34GETENV_Y("FX_GLIDE_AA2_OFFSET_X1", "FX_GLIDE_AA2_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFX_2SMPL_CORRECT_DEF, SECBUFVTXOFFY_2SMPL_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFY_2SMPL_CORRECT_DEF);
  _GlideRoot.environment.aaYOffset[4][4]   = GLIDE_34GETENV_Y("FX_GLIDE_AA2_OFFSET_X0", "FX_GLIDE_AA2_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFX_2SMPL_CORRECT_DEF, PRIBUFVTXOFFY_2SMPL_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFY_2SMPL_CORRECT_DEF);
  _GlideRoot.environment.aaYOffset[4][5]   = GLIDE_34GETENV_Y("FX_GLIDE_AA2_OFFSET_X1", "FX_GLIDE_AA2_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFX_2SMPL_CORRECT_DEF, SECBUFVTXOFFY_2SMPL_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFY_2SMPL_CORRECT_DEF);
  _GlideRoot.environment.aaYOffset[4][6]   = GLIDE_34GETENV_Y("FX_GLIDE_AA2_OFFSET_X0", "FX_GLIDE_AA2_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFX_2SMPL_CORRECT_DEF, PRIBUFVTXOFFY_2SMPL_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFY_2SMPL_CORRECT_DEF);
  _GlideRoot.environment.aaYOffset[4][7]   = GLIDE_34GETENV_Y("FX_GLIDE_AA2_OFFSET_X1", "FX_GLIDE_AA2_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFX_2SMPL_CORRECT_DEF, SECBUFVTXOFFY_2SMPL_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFY_2SMPL_CORRECT_DEF);

  _GlideRoot.environment.aaXOffset[5][0]   = GLIDE_34GETENV_X("FX_GLIDE_AA2_OFFSET_X0", "FX_GLIDE_AA2_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFX_2SMPL_CORRECT_DEF, PRIBUFVTXOFFY_2SMPL_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_X0", GC.bInfo->RegPath, PRIBUFVTXOFFX_2SMPL_CORRECT_DEF);
  _GlideRoot.environment.aaXOffset[5][1]   = GLIDE_34GETENV_X("FX_GLIDE_AA2_OFFSET_X0", "FX_GLIDE_AA2_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFX_2SMPL_CORRECT_DEF, PRIBUFVTXOFFY_2SMPL_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_X0", GC.bInfo->RegPath, PRIBUFVTXOFFX_2SMPL_CORRECT_DEF);
  _GlideRoot.environment.aaXOffset[5][2]   = GLIDE_34GETENV_X("FX_GLIDE_AA2_OFFSET_X1", "FX_GLIDE_AA2_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFX_2SMPL_CORRECT_DEF, SECBUFVTXOFFY_2SMPL_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_X1", GC.bInfo->RegPath, SECBUFVTXOFFX_2SMPL_CORRECT_DEF);
  _GlideRoot.environment.aaXOffset[5][3]   = GLIDE_34GETENV_X("FX_GLIDE_AA2_OFFSET_X1", "FX_GLIDE_AA2_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFX_2SMPL_CORRECT_DEF, SECBUFVTXOFFY_2SMPL_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_X1", GC.bInfo->RegPath, SECBUFVTXOFFX_2SMPL_CORRECT_DEF);
  _GlideRoot.environment.aaXOffset[5][4]   = GLIDE_34GETENV_X("FX_GLIDE_AA2_OFFSET_X0", "FX_GLIDE_AA2_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFX_2SMPL_CORRECT_DEF, PRIBUFVTXOFFY_2SMPL_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_X0", GC.bInfo->RegPath, PRIBUFVTXOFFX_2SMPL_CORRECT_DEF);
  _GlideRoot.environment.aaXOffset[5][5]   = GLIDE_34GETENV_X("FX_GLIDE_AA2_OFFSET_X0", "FX_GLIDE_AA2_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFX_2SMPL_CORRECT_DEF, PRIBUFVTXOFFY_2SMPL_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_X0", GC.bInfo->RegPath, PRIBUFVTXOFFX_2SMPL_CORRECT_DEF);
  _GlideRoot.environment.aaXOffset[5][6]   = GLIDE_34GETENV_X("FX_GLIDE_AA2_OFFSET_X1", "FX_GLIDE_AA2_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFX_2SMPL_CORRECT_DEF, SECBUFVTXOFFY_2SMPL_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_X1", GC.bInfo->RegPath, SECBUFVTXOFFX_2SMPL_CORRECT_DEF);
  _GlideRoot.environment.aaXOffset[5][7]   = GLIDE_34GETENV_X("FX_GLIDE_AA2_OFFSET_X1", "FX_GLIDE_AA2_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFX_2SMPL_CORRECT_DEF, SECBUFVTXOFFY_2SMPL_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_X1", GC.bInfo->RegPath, SECBUFVTXOFFX_2SMPL_CORRECT_DEF);

  _GlideRoot.environment.aaYOffset[5][0]   = GLIDE_34GETENV_Y("FX_GLIDE_AA2_OFFSET_X0", "FX_GLIDE_AA2_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFX_2SMPL_CORRECT_DEF, PRIBUFVTXOFFY_2SMPL_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFY_2SMPL_CORRECT_DEF);
  _GlideRoot.environment.aaYOffset[5][1]   = GLIDE_34GETENV_Y("FX_GLIDE_AA2_OFFSET_X0", "FX_GLIDE_AA2_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFX_2SMPL_CORRECT_DEF, PRIBUFVTXOFFY_2SMPL_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFY_2SMPL_CORRECT_DEF);
  _GlideRoot.environment.aaYOffset[5][2]   = GLIDE_34GETENV_Y("FX_GLIDE_AA2_OFFSET_X1", "FX_GLIDE_AA2_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFX_2SMPL_CORRECT_DEF, SECBUFVTXOFFY_2SMPL_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFY_2SMPL_CORRECT_DEF);
  _GlideRoot.environment.aaYOffset[5][3]   = GLIDE_34GETENV_Y("FX_GLIDE_AA2_OFFSET_X1", "FX_GLIDE_AA2_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFX_2SMPL_CORRECT_DEF, SECBUFVTXOFFY_2SMPL_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFY_2SMPL_CORRECT_DEF);  
  _GlideRoot.environment.aaYOffset[5][4]   = GLIDE_34GETENV_Y("FX_GLIDE_AA2_OFFSET_X0", "FX_GLIDE_AA2_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFX_2SMPL_CORRECT_DEF, PRIBUFVTXOFFY_2SMPL_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFY_2SMPL_CORRECT_DEF);
  _GlideRoot.environment.aaYOffset[5][5]   = GLIDE_34GETENV_Y("FX_GLIDE_AA2_OFFSET_X0", "FX_GLIDE_AA2_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFX_2SMPL_CORRECT_DEF, PRIBUFVTXOFFY_2SMPL_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFY_2SMPL_CORRECT_DEF);
  _GlideRoot.environment.aaYOffset[5][6]   = GLIDE_34GETENV_Y("FX_GLIDE_AA2_OFFSET_X1", "FX_GLIDE_AA2_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFX_2SMPL_CORRECT_DEF, SECBUFVTXOFFY_2SMPL_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFY_2SMPL_CORRECT_DEF);
  _GlideRoot.environment.aaYOffset[5][7]   = GLIDE_34GETENV_Y("FX_GLIDE_AA2_OFFSET_X1", "FX_GLIDE_AA2_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFX_2SMPL_CORRECT_DEF, SECBUFVTXOFFY_2SMPL_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFY_2SMPL_CORRECT_DEF);  

  /* set jitter dispersity for 4xaa */
  _GlideRoot.environment.aaJitterDisp      = GLIDE_FGETENV("FX_GLIDE_AA_JITTERDISP", GC.bInfo->RegPath, 1.0f);
  GDBG_INFO(80,"      aaJitterDisp: %f\n",_GlideRoot.environment.aaJitterDisp);
  /* set rotation for 4xaa */
  _GlideRoot.environment.aaGridRotation    = GLIDE_FGETENV("FX_GLIDE_AA_GRIDROTATION", GC.bInfo->RegPath, 27.5f) - 27.5f;
  GDBG_INFO(80,"    aaGridRotation: %f\n",_GlideRoot.environment.aaGridRotation);
  _GlideRoot.environment.aaXOffset[6][0]   = GLIDE_34GETENV_X("FX_GLIDE_AA4_OFFSET_X0", "FX_GLIDE_AA4_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFX_4SMPL_CHP0_CORRECT_DEF, PRIBUFVTXOFFY_4SMPL_CHP0_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA4_OFFSET_X0", GC.bInfo->RegPath, PRIBUFVTXOFFX_4SMPL_CHP0_CORRECT_DEF);
  _GlideRoot.environment.aaXOffset[6][1]   = GLIDE_34GETENV_X("FX_GLIDE_AA4_OFFSET_X1", "FX_GLIDE_AA4_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFX_4SMPL_CHP0_CORRECT_DEF, SECBUFVTXOFFY_4SMPL_CHP0_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA4_OFFSET_X1", GC.bInfo->RegPath, SECBUFVTXOFFX_4SMPL_CHP0_CORRECT_DEF);
  _GlideRoot.environment.aaXOffset[6][2]   = GLIDE_34GETENV_X("FX_GLIDE_AA4_OFFSET_X2", "FX_GLIDE_AA4_OFFSET_Y2", GC.bInfo->RegPath, PRIBUFVTXOFFX_4SMPL_CHP1_CORRECT_DEF, PRIBUFVTXOFFY_4SMPL_CHP1_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA4_OFFSET_X2", GC.bInfo->RegPath, PRIBUFVTXOFFX_4SMPL_CHP1_CORRECT_DEF);
  _GlideRoot.environment.aaXOffset[6][3]   = GLIDE_34GETENV_X("FX_GLIDE_AA4_OFFSET_X3", "FX_GLIDE_AA4_OFFSET_Y3", GC.bInfo->RegPath, SECBUFVTXOFFX_4SMPL_CHP1_CORRECT_DEF, SECBUFVTXOFFY_4SMPL_CHP1_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA4_OFFSET_X3", GC.bInfo->RegPath, SECBUFVTXOFFX_4SMPL_CHP1_CORRECT_DEF);
  _GlideRoot.environment.aaXOffset[6][4]   = GLIDE_34GETENV_X("FX_GLIDE_AA4_OFFSET_X0", "FX_GLIDE_AA4_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFX_4SMPL_CHP0_CORRECT_DEF, PRIBUFVTXOFFY_4SMPL_CHP0_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA4_OFFSET_X0", GC.bInfo->RegPath, PRIBUFVTXOFFX_4SMPL_CHP0_CORRECT_DEF);
  _GlideRoot.environment.aaXOffset[6][5]   = GLIDE_34GETENV_X("FX_GLIDE_AA4_OFFSET_X1", "FX_GLIDE_AA4_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFX_4SMPL_CHP0_CORRECT_DEF, SECBUFVTXOFFY_4SMPL_CHP0_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA4_OFFSET_X1", GC.bInfo->RegPath, SECBUFVTXOFFX_4SMPL_CHP0_CORRECT_DEF);
  _GlideRoot.environment.aaXOffset[6][6]   = GLIDE_34GETENV_X("FX_GLIDE_AA4_OFFSET_X2", "FX_GLIDE_AA4_OFFSET_Y2", GC.bInfo->RegPath, PRIBUFVTXOFFX_4SMPL_CHP1_CORRECT_DEF, PRIBUFVTXOFFY_4SMPL_CHP1_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA4_OFFSET_X2", GC.bInfo->RegPath, PRIBUFVTXOFFX_4SMPL_CHP1_CORRECT_DEF);
  _GlideRoot.environment.aaXOffset[6][7]   = GLIDE_34GETENV_X("FX_GLIDE_AA4_OFFSET_X3", "FX_GLIDE_AA4_OFFSET_Y3", GC.bInfo->RegPath, SECBUFVTXOFFX_4SMPL_CHP1_CORRECT_DEF, SECBUFVTXOFFY_4SMPL_CHP1_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA4_OFFSET_X3", GC.bInfo->RegPath, SECBUFVTXOFFX_4SMPL_CHP1_CORRECT_DEF);

  _GlideRoot.environment.aaYOffset[6][0]   = GLIDE_34GETENV_Y("FX_GLIDE_AA4_OFFSET_X0", "FX_GLIDE_AA4_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFX_4SMPL_CHP0_CORRECT_DEF, PRIBUFVTXOFFY_4SMPL_CHP0_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA4_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFY_4SMPL_CHP0_CORRECT_DEF);
  _GlideRoot.environment.aaYOffset[6][1]   = GLIDE_34GETENV_Y("FX_GLIDE_AA4_OFFSET_X1", "FX_GLIDE_AA4_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFX_4SMPL_CHP0_CORRECT_DEF, SECBUFVTXOFFY_4SMPL_CHP0_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA4_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFY_4SMPL_CHP0_CORRECT_DEF);
  _GlideRoot.environment.aaYOffset[6][2]   = GLIDE_34GETENV_Y("FX_GLIDE_AA4_OFFSET_X2", "FX_GLIDE_AA4_OFFSET_Y2", GC.bInfo->RegPath, PRIBUFVTXOFFX_4SMPL_CHP1_CORRECT_DEF, PRIBUFVTXOFFY_4SMPL_CHP1_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA4_OFFSET_Y2", GC.bInfo->RegPath, PRIBUFVTXOFFY_4SMPL_CHP1_CORRECT_DEF);
  _GlideRoot.environment.aaYOffset[6][3]   = GLIDE_34GETENV_Y("FX_GLIDE_AA4_OFFSET_X3", "FX_GLIDE_AA4_OFFSET_Y3", GC.bInfo->RegPath, SECBUFVTXOFFX_4SMPL_CHP1_CORRECT_DEF, SECBUFVTXOFFY_4SMPL_CHP1_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA4_OFFSET_Y3", GC.bInfo->RegPath, SECBUFVTXOFFY_4SMPL_CHP1_CORRECT_DEF);
  _GlideRoot.environment.aaYOffset[6][4]   = GLIDE_34GETENV_Y("FX_GLIDE_AA4_OFFSET_X0", "FX_GLIDE_AA4_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFX_4SMPL_CHP0_CORRECT_DEF, PRIBUFVTXOFFY_4SMPL_CHP0_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA4_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFY_4SMPL_CHP0_CORRECT_DEF);
  _GlideRoot.environment.aaYOffset[6][5]   = GLIDE_34GETENV_Y("FX_GLIDE_AA4_OFFSET_X1", "FX_GLIDE_AA4_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFX_4SMPL_CHP0_CORRECT_DEF, SECBUFVTXOFFY_4SMPL_CHP0_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA4_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFY_4SMPL_CHP0_CORRECT_DEF);
  _GlideRoot.environment.aaYOffset[6][6]   = GLIDE_34GETENV_Y("FX_GLIDE_AA4_OFFSET_X2", "FX_GLIDE_AA4_OFFSET_Y2", GC.bInfo->RegPath, PRIBUFVTXOFFX_4SMPL_CHP1_CORRECT_DEF, PRIBUFVTXOFFY_4SMPL_CHP1_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA4_OFFSET_Y2", GC.bInfo->RegPath, PRIBUFVTXOFFY_4SMPL_CHP1_CORRECT_DEF);
  _GlideRoot.environment.aaYOffset[6][7]   = GLIDE_34GETENV_Y("FX_GLIDE_AA4_OFFSET_X3", "FX_GLIDE_AA4_OFFSET_Y3", GC.bInfo->RegPath, SECBUFVTXOFFX_4SMPL_CHP1_CORRECT_DEF, SECBUFVTXOFFY_4SMPL_CHP1_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA4_OFFSET_Y3", GC.bInfo->RegPath, SECBUFVTXOFFY_4SMPL_CHP1_CORRECT_DEF);

/* jcochrane 4 chip offsets
 *
 * About: The strange ordering would allow FSAA to still
 * work regardless of the SLI/Samples per chip configuration */

  /* 4chip 2xaa */
  _GlideRoot.environment.aaXOffset[7][0]   = GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_X0", GC.bInfo->RegPath, PRIBUFVTXOFFX_2SMPL_CORRECT_DEF);	// 25
  _GlideRoot.environment.aaXOffset[7][1]   = GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_X1", GC.bInfo->RegPath, SECBUFVTXOFFX_2SMPL_CORRECT_DEF);	// 75
  _GlideRoot.environment.aaXOffset[7][2]   = GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_X1", GC.bInfo->RegPath, SECBUFVTXOFFX_2SMPL_CORRECT_DEF);	// 75
  _GlideRoot.environment.aaXOffset[7][3]   = GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_X0", GC.bInfo->RegPath, PRIBUFVTXOFFX_2SMPL_CORRECT_DEF);	// 25
  _GlideRoot.environment.aaXOffset[7][4]   = GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_X0", GC.bInfo->RegPath, PRIBUFVTXOFFX_2SMPL_CORRECT_DEF);	// 25
  _GlideRoot.environment.aaXOffset[7][5]   = GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_X1", GC.bInfo->RegPath, SECBUFVTXOFFX_2SMPL_CORRECT_DEF);	// 75
  _GlideRoot.environment.aaXOffset[7][6]   = GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_X1", GC.bInfo->RegPath, SECBUFVTXOFFX_2SMPL_CORRECT_DEF);	// 75
  _GlideRoot.environment.aaXOffset[7][7]   = GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_X0", GC.bInfo->RegPath, PRIBUFVTXOFFX_2SMPL_CORRECT_DEF);	// 25

  _GlideRoot.environment.aaYOffset[7][0]   = GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFY_2SMPL_CORRECT_DEF);	// 25
  _GlideRoot.environment.aaYOffset[7][1]   = GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFY_2SMPL_CORRECT_DEF);	// 75
  _GlideRoot.environment.aaYOffset[7][2]   = GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFY_2SMPL_CORRECT_DEF);	// 75
  _GlideRoot.environment.aaYOffset[7][3]   = GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFY_2SMPL_CORRECT_DEF);	// 25
  _GlideRoot.environment.aaYOffset[7][4]   = GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFY_2SMPL_CORRECT_DEF);	// 25
  _GlideRoot.environment.aaYOffset[7][5]   = GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFY_2SMPL_CORRECT_DEF);	// 75
  _GlideRoot.environment.aaYOffset[7][6]   = GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFY_2SMPL_CORRECT_DEF);	// 75
  _GlideRoot.environment.aaYOffset[7][7]   = GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFY_2SMPL_CORRECT_DEF);	// 25

/* 4chip 4xaa */
  _GlideRoot.environment.aaXOffset[8][0]   = GLIDE_34GETENV("FX_GLIDE_AA4_OFFSET_X0", GC.bInfo->RegPath, PRIBUFVTXOFFX_4SMPL_CHP0_CORRECT_DEF);	// 375
  _GlideRoot.environment.aaXOffset[8][1]   = GLIDE_34GETENV("FX_GLIDE_AA4_OFFSET_X2", GC.bInfo->RegPath, PRIBUFVTXOFFX_4SMPL_CHP1_CORRECT_DEF); 
  _GlideRoot.environment.aaXOffset[8][2]   = GLIDE_34GETENV("FX_GLIDE_AA4_OFFSET_X1", GC.bInfo->RegPath, SECBUFVTXOFFX_4SMPL_CHP0_CORRECT_DEF);	// 875
  _GlideRoot.environment.aaXOffset[8][3]   = GLIDE_34GETENV("FX_GLIDE_AA4_OFFSET_X3", GC.bInfo->RegPath, SECBUFVTXOFFX_4SMPL_CHP1_CORRECT_DEF); 
  _GlideRoot.environment.aaXOffset[8][4]   = GLIDE_34GETENV("FX_GLIDE_AA4_OFFSET_X2", GC.bInfo->RegPath, PRIBUFVTXOFFX_4SMPL_CHP1_CORRECT_DEF);	// 125 
  _GlideRoot.environment.aaXOffset[8][5]   = GLIDE_34GETENV("FX_GLIDE_AA4_OFFSET_X0", GC.bInfo->RegPath, PRIBUFVTXOFFX_4SMPL_CHP0_CORRECT_DEF);
  _GlideRoot.environment.aaXOffset[8][6]   = GLIDE_34GETENV("FX_GLIDE_AA4_OFFSET_X3", GC.bInfo->RegPath, SECBUFVTXOFFX_4SMPL_CHP1_CORRECT_DEF);	// 625
  _GlideRoot.environment.aaXOffset[8][7]   = GLIDE_34GETENV("FX_GLIDE_AA4_OFFSET_X1", GC.bInfo->RegPath, SECBUFVTXOFFX_4SMPL_CHP0_CORRECT_DEF);	

  _GlideRoot.environment.aaYOffset[8][0]   = GLIDE_34GETENV("FX_GLIDE_AA4_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFY_4SMPL_CHP0_CORRECT_DEF);	// 125
  _GlideRoot.environment.aaYOffset[8][1]   = GLIDE_34GETENV("FX_GLIDE_AA4_OFFSET_Y2", GC.bInfo->RegPath, PRIBUFVTXOFFY_4SMPL_CHP1_CORRECT_DEF);    
  _GlideRoot.environment.aaYOffset[8][2]   = GLIDE_34GETENV("FX_GLIDE_AA4_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFY_4SMPL_CHP0_CORRECT_DEF);	// 
  _GlideRoot.environment.aaYOffset[8][3]   = GLIDE_34GETENV("FX_GLIDE_AA4_OFFSET_Y3", GC.bInfo->RegPath, SECBUFVTXOFFY_4SMPL_CHP1_CORRECT_DEF);    
  _GlideRoot.environment.aaYOffset[8][4]   = GLIDE_34GETENV("FX_GLIDE_AA4_OFFSET_Y2", GC.bInfo->RegPath, PRIBUFVTXOFFY_4SMPL_CHP1_CORRECT_DEF); 
  _GlideRoot.environment.aaYOffset[8][5]   = GLIDE_34GETENV("FX_GLIDE_AA4_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFY_4SMPL_CHP0_CORRECT_DEF);
  _GlideRoot.environment.aaYOffset[8][6]   = GLIDE_34GETENV("FX_GLIDE_AA4_OFFSET_Y3", GC.bInfo->RegPath, SECBUFVTXOFFY_4SMPL_CHP1_CORRECT_DEF); 
  _GlideRoot.environment.aaYOffset[8][7]   = GLIDE_34GETENV("FX_GLIDE_AA4_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFY_4SMPL_CHP0_CORRECT_DEF);    
  
/* these are the old offsets */
/* 4chip 2xaa
  _GlideRoot.environment.aaXOffset[7][0]   = 0x04;
  _GlideRoot.environment.aaXOffset[7][1]   = 0x00;
  _GlideRoot.environment.aaXOffset[7][2]   = 0x0c;  
  _GlideRoot.environment.aaXOffset[7][3]   = 0x00;
  _GlideRoot.environment.aaXOffset[7][4]   = 0x04;
  _GlideRoot.environment.aaXOffset[7][5]   = 0x00;
  _GlideRoot.environment.aaXOffset[7][6]   = 0x0c;
  _GlideRoot.environment.aaXOffset[7][7]   = 0x00;

  _GlideRoot.environment.aaYOffset[7][0]   = 0x04;
  _GlideRoot.environment.aaYOffset[7][1]   = 0x00;
  _GlideRoot.environment.aaYOffset[7][2]   = 0x0c;  
  _GlideRoot.environment.aaYOffset[7][3]   = 0x00;
  _GlideRoot.environment.aaYOffset[7][4]   = 0x04;
  _GlideRoot.environment.aaYOffset[7][5]   = 0x00;
  _GlideRoot.environment.aaYOffset[7][6]   = 0x0c;  
  _GlideRoot.environment.aaYOffset[7][7]   = 0x00;
*/
/* 4chip 4xaa 
  _GlideRoot.environment.aaXOffset[8][0]   = 0x06; 
  _GlideRoot.environment.aaXOffset[8][1]   = 0x00; 
  _GlideRoot.environment.aaXOffset[8][2]   = 0x0e; 
  _GlideRoot.environment.aaXOffset[8][3]   = 0x00; 
  _GlideRoot.environment.aaXOffset[8][4]   = 0x02; 
  _GlideRoot.environment.aaXOffset[8][5]   = 0x00; 
  _GlideRoot.environment.aaXOffset[8][6]   = 0x0a; 
  _GlideRoot.environment.aaXOffset[8][7]   = 0x00; 

  _GlideRoot.environment.aaYOffset[8][0]   = 0x02; 
  _GlideRoot.environment.aaYOffset[8][1]   = 0x00;    
  _GlideRoot.environment.aaYOffset[8][2]   = 0x06; 
  _GlideRoot.environment.aaYOffset[8][3]   = 0x00;    
  _GlideRoot.environment.aaYOffset[8][4]   = 0x0a; 
  _GlideRoot.environment.aaYOffset[8][5]   = 0x00;    
  _GlideRoot.environment.aaYOffset[8][6]   = 0x0e; 
  _GlideRoot.environment.aaYOffset[8][7]   = 0x00;    
*/
/* 4chip 8xaa */
#if 0
  _GlideRoot.environment.aaXOffset[9][0]   = 0x06; 
  _GlideRoot.environment.aaXOffset[9][1]   = 0x04; 
  _GlideRoot.environment.aaXOffset[9][2]   = 0x0e; 
  _GlideRoot.environment.aaXOffset[9][3]   = 0x0a; 
  _GlideRoot.environment.aaXOffset[9][4]   = 0x02; 
  _GlideRoot.environment.aaXOffset[9][5]   = 0x0c; 
  _GlideRoot.environment.aaXOffset[9][6]   = 0x0a; 
  _GlideRoot.environment.aaXOffset[9][7]   = 0x06; 

  _GlideRoot.environment.aaYOffset[9][0]   = 0x02; 
  _GlideRoot.environment.aaYOffset[9][1]   = 0x06; 
  _GlideRoot.environment.aaYOffset[9][2]   = 0x06; 
  _GlideRoot.environment.aaYOffset[9][3]   = 0x04; 
  _GlideRoot.environment.aaYOffset[9][4]   = 0x0a; 
  _GlideRoot.environment.aaYOffset[9][5]   = 0x0a; 
  _GlideRoot.environment.aaYOffset[9][6]   = 0x0e; 
  _GlideRoot.environment.aaYOffset[9][7]   = 0x0c;
#else
  _GlideRoot.environment.aaXOffset[9][0]   = GLIDE_34GETENV("FX_GLIDE_AA8_OFFSET_X0", GC.bInfo->RegPath, PRIBUFVTXOFFX_8SMPL_CHP0_DEF);
  _GlideRoot.environment.aaXOffset[9][1]   = GLIDE_34GETENV("FX_GLIDE_AA8_OFFSET_X1", GC.bInfo->RegPath, SECBUFVTXOFFX_8SMPL_CHP0_DEF);
  _GlideRoot.environment.aaXOffset[9][2]   = GLIDE_34GETENV("FX_GLIDE_AA8_OFFSET_X2", GC.bInfo->RegPath, PRIBUFVTXOFFX_8SMPL_CHP1_DEF);
  _GlideRoot.environment.aaXOffset[9][3]   = GLIDE_34GETENV("FX_GLIDE_AA8_OFFSET_X3", GC.bInfo->RegPath, SECBUFVTXOFFX_8SMPL_CHP1_DEF);
  _GlideRoot.environment.aaXOffset[9][4]   = GLIDE_34GETENV("FX_GLIDE_AA8_OFFSET_X4", GC.bInfo->RegPath, PRIBUFVTXOFFX_8SMPL_CHP2_DEF);
  _GlideRoot.environment.aaXOffset[9][5]   = GLIDE_34GETENV("FX_GLIDE_AA8_OFFSET_X5", GC.bInfo->RegPath, SECBUFVTXOFFX_8SMPL_CHP2_DEF);
  _GlideRoot.environment.aaXOffset[9][6]   = GLIDE_34GETENV("FX_GLIDE_AA8_OFFSET_X6", GC.bInfo->RegPath, PRIBUFVTXOFFX_8SMPL_CHP3_DEF);
  _GlideRoot.environment.aaXOffset[9][7]   = GLIDE_34GETENV("FX_GLIDE_AA8_OFFSET_X7", GC.bInfo->RegPath, SECBUFVTXOFFX_8SMPL_CHP3_DEF);

  _GlideRoot.environment.aaYOffset[9][0]   = GLIDE_34GETENV("FX_GLIDE_AA8_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFY_8SMPL_CHP0_DEF);
  _GlideRoot.environment.aaYOffset[9][1]   = GLIDE_34GETENV("FX_GLIDE_AA8_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFY_8SMPL_CHP0_DEF);
  _GlideRoot.environment.aaYOffset[9][2]   = GLIDE_34GETENV("FX_GLIDE_AA8_OFFSET_Y2", GC.bInfo->RegPath, PRIBUFVTXOFFY_8SMPL_CHP1_DEF);
  _GlideRoot.environment.aaYOffset[9][3]   = GLIDE_34GETENV("FX_GLIDE_AA8_OFFSET_Y3", GC.bInfo->RegPath, SECBUFVTXOFFY_8SMPL_CHP1_DEF);
  _GlideRoot.environment.aaYOffset[9][4]   = GLIDE_34GETENV("FX_GLIDE_AA8_OFFSET_Y4", GC.bInfo->RegPath, PRIBUFVTXOFFY_8SMPL_CHP2_DEF);
  _GlideRoot.environment.aaYOffset[9][5]   = GLIDE_34GETENV("FX_GLIDE_AA8_OFFSET_Y5", GC.bInfo->RegPath, SECBUFVTXOFFY_8SMPL_CHP2_DEF);
  _GlideRoot.environment.aaYOffset[9][6]   = GLIDE_34GETENV("FX_GLIDE_AA8_OFFSET_Y6", GC.bInfo->RegPath, PRIBUFVTXOFFY_8SMPL_CHP3_DEF);
  _GlideRoot.environment.aaYOffset[9][7]   = GLIDE_34GETENV("FX_GLIDE_AA8_OFFSET_Y7", GC.bInfo->RegPath, SECBUFVTXOFFY_8SMPL_CHP3_DEF);
#endif

/* jcochrane 4 chip offsets
 * Not any more, now they are Colourless offsets 
 *
 * About: The strange ordering would allow FSAA to still
 * work regardless of the SLI/Samples per chip configuration */

  /* these are the correct jitter vaules */
  /* 4chip 2xaa */
  /* set jitter dispersity for 2xaa */
  _GlideRoot.environment.aaJitterDisp      = GLIDE_FGETENV("FX_GLIDE_AA_JITTERDISP", GC.bInfo->RegPath, 1.0625f);
  GDBG_INFO(80,"      aaJitterDisp: %f\n",_GlideRoot.environment.aaJitterDisp);
  /* set rotation for 2xaa */
  _GlideRoot.environment.aaGridRotation    = GLIDE_FGETENV("FX_GLIDE_AA_GRIDROTATION", GC.bInfo->RegPath, 45.0f) - 45.0f;
  GDBG_INFO(80,"    aaGridRotation: %f\n",_GlideRoot.environment.aaGridRotation);
  _GlideRoot.environment.aaXOffset[10][0]   = GLIDE_34GETENV_X("FX_GLIDE_AA2_OFFSET_X0", "FX_GLIDE_AA2_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFX_2SMPL_CORRECT_DEF, PRIBUFVTXOFFY_2SMPL_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_X0", GC.bInfo->RegPath, PRIBUFVTXOFFX_2SMPL_CORRECT_DEF);	// 25
  _GlideRoot.environment.aaXOffset[10][1]   = GLIDE_34GETENV_X("FX_GLIDE_AA2_OFFSET_X1", "FX_GLIDE_AA2_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFX_2SMPL_CORRECT_DEF, SECBUFVTXOFFY_2SMPL_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_X1", GC.bInfo->RegPath, SECBUFVTXOFFX_2SMPL_CORRECT_DEF);	// 75
  _GlideRoot.environment.aaXOffset[10][2]   = GLIDE_34GETENV_X("FX_GLIDE_AA2_OFFSET_X1", "FX_GLIDE_AA2_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFX_2SMPL_CORRECT_DEF, SECBUFVTXOFFY_2SMPL_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_X1", GC.bInfo->RegPath, SECBUFVTXOFFX_2SMPL_CORRECT_DEF);	// 75
  _GlideRoot.environment.aaXOffset[10][3]   = GLIDE_34GETENV_X("FX_GLIDE_AA2_OFFSET_X0", "FX_GLIDE_AA2_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFX_2SMPL_CORRECT_DEF, PRIBUFVTXOFFY_2SMPL_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_X0", GC.bInfo->RegPath, PRIBUFVTXOFFX_2SMPL_CORRECT_DEF);	// 25
  _GlideRoot.environment.aaXOffset[10][4]   = GLIDE_34GETENV_X("FX_GLIDE_AA2_OFFSET_X0", "FX_GLIDE_AA2_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFX_2SMPL_CORRECT_DEF, PRIBUFVTXOFFY_2SMPL_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_X0", GC.bInfo->RegPath, PRIBUFVTXOFFX_2SMPL_CORRECT_DEF);	// 25
  _GlideRoot.environment.aaXOffset[10][5]   = GLIDE_34GETENV_X("FX_GLIDE_AA2_OFFSET_X1", "FX_GLIDE_AA2_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFX_2SMPL_CORRECT_DEF, SECBUFVTXOFFY_2SMPL_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_X1", GC.bInfo->RegPath, SECBUFVTXOFFX_2SMPL_CORRECT_DEF);	// 75
  _GlideRoot.environment.aaXOffset[10][6]   = GLIDE_34GETENV_X("FX_GLIDE_AA2_OFFSET_X1", "FX_GLIDE_AA2_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFX_2SMPL_CORRECT_DEF, SECBUFVTXOFFY_2SMPL_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_X1", GC.bInfo->RegPath, SECBUFVTXOFFX_2SMPL_CORRECT_DEF);	// 75
  _GlideRoot.environment.aaXOffset[10][7]   = GLIDE_34GETENV_X("FX_GLIDE_AA2_OFFSET_X0", "FX_GLIDE_AA2_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFX_2SMPL_CORRECT_DEF, PRIBUFVTXOFFY_2SMPL_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_X0", GC.bInfo->RegPath, PRIBUFVTXOFFX_2SMPL_CORRECT_DEF);	// 25

  _GlideRoot.environment.aaYOffset[10][0]   = GLIDE_34GETENV_Y("FX_GLIDE_AA2_OFFSET_X0", "FX_GLIDE_AA2_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFX_2SMPL_CORRECT_DEF, PRIBUFVTXOFFY_2SMPL_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFY_2SMPL_CORRECT_DEF);	// 25
  _GlideRoot.environment.aaYOffset[10][1]   = GLIDE_34GETENV_Y("FX_GLIDE_AA2_OFFSET_X1", "FX_GLIDE_AA2_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFX_2SMPL_CORRECT_DEF, SECBUFVTXOFFY_2SMPL_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFY_2SMPL_CORRECT_DEF);	// 75
  _GlideRoot.environment.aaYOffset[10][2]   = GLIDE_34GETENV_Y("FX_GLIDE_AA2_OFFSET_X1", "FX_GLIDE_AA2_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFX_2SMPL_CORRECT_DEF, SECBUFVTXOFFY_2SMPL_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFY_2SMPL_CORRECT_DEF);	// 75
  _GlideRoot.environment.aaYOffset[10][3]   = GLIDE_34GETENV_Y("FX_GLIDE_AA2_OFFSET_X0", "FX_GLIDE_AA2_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFX_2SMPL_CORRECT_DEF, PRIBUFVTXOFFY_2SMPL_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFY_2SMPL_CORRECT_DEF);	// 25
  _GlideRoot.environment.aaYOffset[10][4]   = GLIDE_34GETENV_Y("FX_GLIDE_AA2_OFFSET_X0", "FX_GLIDE_AA2_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFX_2SMPL_CORRECT_DEF, PRIBUFVTXOFFY_2SMPL_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFY_2SMPL_CORRECT_DEF);	// 25
  _GlideRoot.environment.aaYOffset[10][5]   = GLIDE_34GETENV_Y("FX_GLIDE_AA2_OFFSET_X1", "FX_GLIDE_AA2_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFX_2SMPL_CORRECT_DEF, SECBUFVTXOFFY_2SMPL_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFY_2SMPL_CORRECT_DEF);	// 75
  _GlideRoot.environment.aaYOffset[10][6]   = GLIDE_34GETENV_Y("FX_GLIDE_AA2_OFFSET_X1", "FX_GLIDE_AA2_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFX_2SMPL_CORRECT_DEF, SECBUFVTXOFFY_2SMPL_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFY_2SMPL_CORRECT_DEF);	// 75
  _GlideRoot.environment.aaYOffset[10][7]   = GLIDE_34GETENV_Y("FX_GLIDE_AA2_OFFSET_X0", "FX_GLIDE_AA2_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFX_2SMPL_CORRECT_DEF, PRIBUFVTXOFFY_2SMPL_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA2_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFY_2SMPL_CORRECT_DEF);	// 25

  /* 4chip 4xaa */
  /* set jitter dispersity for 4xaa */
  _GlideRoot.environment.aaJitterDisp      = GLIDE_FGETENV("FX_GLIDE_AA_JITTERDISP", GC.bInfo->RegPath, 1.0f);
  GDBG_INFO(80,"      aaJitterDisp: %f\n",_GlideRoot.environment.aaJitterDisp);
  /* set rotation for 4xaa */
  _GlideRoot.environment.aaGridRotation    = GLIDE_FGETENV("FX_GLIDE_AA_GRIDROTATION", GC.bInfo->RegPath, 27.5f) - 27.5f;
  GDBG_INFO(80,"    aaGridRotation: %f\n",_GlideRoot.environment.aaGridRotation);
  _GlideRoot.environment.aaXOffset[11][0]   = GLIDE_34GETENV_X("FX_GLIDE_AA4_OFFSET_X0", "FX_GLIDE_AA4_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFX_4SMPL_CHP0_CORRECT_DEF, PRIBUFVTXOFFY_4SMPL_CHP0_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA4_OFFSET_X0", GC.bInfo->RegPath, PRIBUFVTXOFFX_4SMPL_CORRECT_CHP0_DEF);	// 375
  _GlideRoot.environment.aaXOffset[11][1]   = GLIDE_34GETENV_X("FX_GLIDE_AA4_OFFSET_X2", "FX_GLIDE_AA4_OFFSET_Y2", GC.bInfo->RegPath, PRIBUFVTXOFFX_4SMPL_CHP1_CORRECT_DEF, PRIBUFVTXOFFY_4SMPL_CHP1_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA4_OFFSET_X2", GC.bInfo->RegPath, PRIBUFVTXOFFX_4SMPL_CORRECT_CHP1_DEF); 
  _GlideRoot.environment.aaXOffset[11][2]   = GLIDE_34GETENV_X("FX_GLIDE_AA4_OFFSET_X1", "FX_GLIDE_AA4_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFX_4SMPL_CHP0_CORRECT_DEF, SECBUFVTXOFFY_4SMPL_CHP0_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA4_OFFSET_X1", GC.bInfo->RegPath, SECBUFVTXOFFX_4SMPL_CORRECT_CHP0_DEF);	// 875
  _GlideRoot.environment.aaXOffset[11][3]   = GLIDE_34GETENV_X("FX_GLIDE_AA4_OFFSET_X3", "FX_GLIDE_AA4_OFFSET_Y3", GC.bInfo->RegPath, SECBUFVTXOFFX_4SMPL_CHP1_CORRECT_DEF, SECBUFVTXOFFY_4SMPL_CHP1_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA4_OFFSET_X3", GC.bInfo->RegPath, SECBUFVTXOFFX_4SMPL_CORRECT_CHP1_DEF); 
  _GlideRoot.environment.aaXOffset[11][4]   = GLIDE_34GETENV_X("FX_GLIDE_AA4_OFFSET_X2", "FX_GLIDE_AA4_OFFSET_Y2", GC.bInfo->RegPath, PRIBUFVTXOFFX_4SMPL_CHP1_CORRECT_DEF, PRIBUFVTXOFFY_4SMPL_CHP1_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA4_OFFSET_X2", GC.bInfo->RegPath, PRIBUFVTXOFFX_4SMPL_CORRECT_CHP1_DEF);	// 125 
  _GlideRoot.environment.aaXOffset[11][5]   = GLIDE_34GETENV_X("FX_GLIDE_AA4_OFFSET_X0", "FX_GLIDE_AA4_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFX_4SMPL_CHP0_CORRECT_DEF, PRIBUFVTXOFFY_4SMPL_CHP0_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA4_OFFSET_X0", GC.bInfo->RegPath, PRIBUFVTXOFFX_4SMPL_CORRECT_CHP0_DEF);
  _GlideRoot.environment.aaXOffset[11][6]   = GLIDE_34GETENV_X("FX_GLIDE_AA4_OFFSET_X3", "FX_GLIDE_AA4_OFFSET_Y3", GC.bInfo->RegPath, SECBUFVTXOFFX_4SMPL_CHP1_CORRECT_DEF, SECBUFVTXOFFY_4SMPL_CHP1_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA4_OFFSET_X3", GC.bInfo->RegPath, SECBUFVTXOFFX_4SMPL_CORRECT_CHP1_DEF);	// 625
  _GlideRoot.environment.aaXOffset[11][7]   = GLIDE_34GETENV_X("FX_GLIDE_AA4_OFFSET_X1", "FX_GLIDE_AA4_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFX_4SMPL_CHP0_CORRECT_DEF, SECBUFVTXOFFY_4SMPL_CHP0_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA4_OFFSET_X1", GC.bInfo->RegPath, SECBUFVTXOFFX_4SMPL_CORRECT_CHP0_DEF);	

  _GlideRoot.environment.aaYOffset[11][0]   = GLIDE_34GETENV_Y("FX_GLIDE_AA4_OFFSET_X0", "FX_GLIDE_AA4_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFX_4SMPL_CHP0_CORRECT_DEF, PRIBUFVTXOFFY_4SMPL_CHP0_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA4_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFY_4SMPL_CORRECT_CHP0_DEF);	// 125
  _GlideRoot.environment.aaYOffset[11][1]   = GLIDE_34GETENV_Y("FX_GLIDE_AA4_OFFSET_X2", "FX_GLIDE_AA4_OFFSET_Y2", GC.bInfo->RegPath, PRIBUFVTXOFFX_4SMPL_CHP1_CORRECT_DEF, PRIBUFVTXOFFY_4SMPL_CHP1_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA4_OFFSET_Y2", GC.bInfo->RegPath, PRIBUFVTXOFFY_4SMPL_CORRECT_CHP1_DEF);    
  _GlideRoot.environment.aaYOffset[11][2]   = GLIDE_34GETENV_Y("FX_GLIDE_AA4_OFFSET_X1", "FX_GLIDE_AA4_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFX_4SMPL_CHP0_CORRECT_DEF, SECBUFVTXOFFY_4SMPL_CHP0_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA4_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFY_4SMPL_CORRECT_CHP0_DEF);	// 
  _GlideRoot.environment.aaYOffset[11][3]   = GLIDE_34GETENV_Y("FX_GLIDE_AA4_OFFSET_X3", "FX_GLIDE_AA4_OFFSET_Y3", GC.bInfo->RegPath, SECBUFVTXOFFX_4SMPL_CHP1_CORRECT_DEF, SECBUFVTXOFFY_4SMPL_CHP1_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA4_OFFSET_Y3", GC.bInfo->RegPath, SECBUFVTXOFFY_4SMPL_CORRECT_CHP1_DEF);    
  _GlideRoot.environment.aaYOffset[11][4]   = GLIDE_34GETENV_Y("FX_GLIDE_AA4_OFFSET_X2", "FX_GLIDE_AA4_OFFSET_Y2", GC.bInfo->RegPath, PRIBUFVTXOFFX_4SMPL_CHP1_CORRECT_DEF, PRIBUFVTXOFFY_4SMPL_CHP1_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA4_OFFSET_Y2", GC.bInfo->RegPath, PRIBUFVTXOFFY_4SMPL_CORRECT_CHP1_DEF); 
  _GlideRoot.environment.aaYOffset[11][5]   = GLIDE_34GETENV_Y("FX_GLIDE_AA4_OFFSET_X0", "FX_GLIDE_AA4_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFX_4SMPL_CHP0_CORRECT_DEF, PRIBUFVTXOFFY_4SMPL_CHP0_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA4_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFY_4SMPL_CORRECT_CHP0_DEF);
  _GlideRoot.environment.aaYOffset[11][6]   = GLIDE_34GETENV_Y("FX_GLIDE_AA4_OFFSET_X3", "FX_GLIDE_AA4_OFFSET_Y3", GC.bInfo->RegPath, SECBUFVTXOFFX_4SMPL_CHP1_CORRECT_DEF, SECBUFVTXOFFY_4SMPL_CHP1_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA4_OFFSET_Y3", GC.bInfo->RegPath, SECBUFVTXOFFY_4SMPL_CORRECT_CHP1_DEF); 
  _GlideRoot.environment.aaYOffset[11][7]   = GLIDE_34GETENV_Y("FX_GLIDE_AA4_OFFSET_X1", "FX_GLIDE_AA4_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFX_4SMPL_CHP0_CORRECT_DEF, SECBUFVTXOFFY_4SMPL_CHP0_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA4_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFY_4SMPL_CORRECT_CHP0_DEF);    

  /* 4chip 8xaa */
  /* set jitter dispersity for 8xaa */
  _GlideRoot.environment.aaJitterDisp      = GLIDE_FGETENV("FX_GLIDE_AA_JITTERDISP", GC.bInfo->RegPath, 1.0f);
  GDBG_INFO(80,"      aaJitterDisp: %f\n",_GlideRoot.environment.aaJitterDisp);
  /* set rotation for 8xaa */
  _GlideRoot.environment.aaGridRotation    = GLIDE_FGETENV("FX_GLIDE_AA_GRIDROTATION", GC.bInfo->RegPath, 27.5f) - 27.5f;
  GDBG_INFO(80,"    aaGridRotation: %f\n",_GlideRoot.environment.aaGridRotation);
  _GlideRoot.environment.aaXOffset[12][0]   = GLIDE_34GETENV_X("FX_GLIDE_AA8_OFFSET_X0", "FX_GLIDE_AA8_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFX_8SMPL_CHP0_CORRECT_DEF, PRIBUFVTXOFFY_8SMPL_CHP0_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA8_OFFSET_X0", GC.bInfo->RegPath, PRIBUFVTXOFFX_8SMPL_CHP0_CORRECT_DEF);
  _GlideRoot.environment.aaXOffset[12][1]   = GLIDE_34GETENV_X("FX_GLIDE_AA8_OFFSET_X1", "FX_GLIDE_AA8_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFX_8SMPL_CHP0_CORRECT_DEF, SECBUFVTXOFFY_8SMPL_CHP0_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA8_OFFSET_X1", GC.bInfo->RegPath, SECBUFVTXOFFX_8SMPL_CHP0_CORRECT_DEF);
  _GlideRoot.environment.aaXOffset[12][2]   = GLIDE_34GETENV_X("FX_GLIDE_AA8_OFFSET_X2", "FX_GLIDE_AA8_OFFSET_Y2", GC.bInfo->RegPath, PRIBUFVTXOFFX_8SMPL_CHP1_CORRECT_DEF, PRIBUFVTXOFFY_8SMPL_CHP1_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA8_OFFSET_X2", GC.bInfo->RegPath, PRIBUFVTXOFFX_8SMPL_CHP1_CORRECT_DEF);
  _GlideRoot.environment.aaXOffset[12][3]   = GLIDE_34GETENV_X("FX_GLIDE_AA8_OFFSET_X3", "FX_GLIDE_AA8_OFFSET_Y3", GC.bInfo->RegPath, SECBUFVTXOFFX_8SMPL_CHP1_CORRECT_DEF, SECBUFVTXOFFY_8SMPL_CHP1_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA8_OFFSET_X3", GC.bInfo->RegPath, SECBUFVTXOFFX_8SMPL_CHP1_CORRECT_DEF);
  _GlideRoot.environment.aaXOffset[12][4]   = GLIDE_34GETENV_X("FX_GLIDE_AA8_OFFSET_X4", "FX_GLIDE_AA8_OFFSET_Y4", GC.bInfo->RegPath, PRIBUFVTXOFFX_8SMPL_CHP2_CORRECT_DEF, PRIBUFVTXOFFY_8SMPL_CHP2_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA8_OFFSET_X4", GC.bInfo->RegPath, PRIBUFVTXOFFX_8SMPL_CHP2_CORRECT_DEF);
  _GlideRoot.environment.aaXOffset[12][5]   = GLIDE_34GETENV_X("FX_GLIDE_AA8_OFFSET_X5", "FX_GLIDE_AA8_OFFSET_Y5", GC.bInfo->RegPath, SECBUFVTXOFFX_8SMPL_CHP2_CORRECT_DEF, SECBUFVTXOFFY_8SMPL_CHP2_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA8_OFFSET_X5", GC.bInfo->RegPath, SECBUFVTXOFFX_8SMPL_CHP2_CORRECT_DEF);
  _GlideRoot.environment.aaXOffset[12][6]   = GLIDE_34GETENV_X("FX_GLIDE_AA8_OFFSET_X6", "FX_GLIDE_AA8_OFFSET_Y6", GC.bInfo->RegPath, PRIBUFVTXOFFX_8SMPL_CHP3_CORRECT_DEF, PRIBUFVTXOFFY_8SMPL_CHP3_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA8_OFFSET_X6", GC.bInfo->RegPath, PRIBUFVTXOFFX_8SMPL_CHP3_CORRECT_DEF);
  _GlideRoot.environment.aaXOffset[12][7]   = GLIDE_34GETENV_X("FX_GLIDE_AA8_OFFSET_X7", "FX_GLIDE_AA8_OFFSET_Y7", GC.bInfo->RegPath, SECBUFVTXOFFX_8SMPL_CHP3_CORRECT_DEF, SECBUFVTXOFFY_8SMPL_CHP3_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA8_OFFSET_X7", GC.bInfo->RegPath, SECBUFVTXOFFX_8SMPL_CHP3_CORRECT_DEF);

  _GlideRoot.environment.aaYOffset[12][0]   = GLIDE_34GETENV_Y("FX_GLIDE_AA8_OFFSET_X0", "FX_GLIDE_AA8_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFX_8SMPL_CHP0_CORRECT_DEF, PRIBUFVTXOFFY_8SMPL_CHP0_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA8_OFFSET_Y0", GC.bInfo->RegPath, PRIBUFVTXOFFY_8SMPL_CHP0_CORRECT_DEF);
  _GlideRoot.environment.aaYOffset[12][1]   = GLIDE_34GETENV_Y("FX_GLIDE_AA8_OFFSET_X1", "FX_GLIDE_AA8_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFX_8SMPL_CHP0_CORRECT_DEF, SECBUFVTXOFFY_8SMPL_CHP0_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA8_OFFSET_Y1", GC.bInfo->RegPath, SECBUFVTXOFFY_8SMPL_CHP0_CORRECT_DEF);
  _GlideRoot.environment.aaYOffset[12][2]   = GLIDE_34GETENV_Y("FX_GLIDE_AA8_OFFSET_X2", "FX_GLIDE_AA8_OFFSET_Y2", GC.bInfo->RegPath, PRIBUFVTXOFFX_8SMPL_CHP1_CORRECT_DEF, PRIBUFVTXOFFY_8SMPL_CHP1_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA8_OFFSET_Y2", GC.bInfo->RegPath, PRIBUFVTXOFFY_8SMPL_CHP1_CORRECT_DEF);
  _GlideRoot.environment.aaYOffset[12][3]   = GLIDE_34GETENV_Y("FX_GLIDE_AA8_OFFSET_X3", "FX_GLIDE_AA8_OFFSET_Y3", GC.bInfo->RegPath, SECBUFVTXOFFX_8SMPL_CHP1_CORRECT_DEF, SECBUFVTXOFFY_8SMPL_CHP1_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA8_OFFSET_Y3", GC.bInfo->RegPath, SECBUFVTXOFFY_8SMPL_CHP1_CORRECT_DEF);
  _GlideRoot.environment.aaYOffset[12][4]   = GLIDE_34GETENV_Y("FX_GLIDE_AA8_OFFSET_X4", "FX_GLIDE_AA8_OFFSET_Y4", GC.bInfo->RegPath, PRIBUFVTXOFFX_8SMPL_CHP2_CORRECT_DEF, PRIBUFVTXOFFY_8SMPL_CHP2_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA8_OFFSET_Y4", GC.bInfo->RegPath, PRIBUFVTXOFFY_8SMPL_CHP2_CORRECT_DEF);
  _GlideRoot.environment.aaYOffset[12][5]   = GLIDE_34GETENV_Y("FX_GLIDE_AA8_OFFSET_X5", "FX_GLIDE_AA8_OFFSET_Y5", GC.bInfo->RegPath, SECBUFVTXOFFX_8SMPL_CHP2_CORRECT_DEF, SECBUFVTXOFFY_8SMPL_CHP2_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA8_OFFSET_Y5", GC.bInfo->RegPath, SECBUFVTXOFFY_8SMPL_CHP2_CORRECT_DEF);
  _GlideRoot.environment.aaYOffset[12][6]   = GLIDE_34GETENV_Y("FX_GLIDE_AA8_OFFSET_X6", "FX_GLIDE_AA8_OFFSET_Y6", GC.bInfo->RegPath, PRIBUFVTXOFFX_8SMPL_CHP3_CORRECT_DEF, PRIBUFVTXOFFY_8SMPL_CHP3_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA8_OFFSET_Y6", GC.bInfo->RegPath, PRIBUFVTXOFFY_8SMPL_CHP3_CORRECT_DEF);
  _GlideRoot.environment.aaYOffset[12][7]   = GLIDE_34GETENV_Y("FX_GLIDE_AA8_OFFSET_X7", "FX_GLIDE_AA8_OFFSET_Y7", GC.bInfo->RegPath, SECBUFVTXOFFX_8SMPL_CHP3_CORRECT_DEF, SECBUFVTXOFFY_8SMPL_CHP3_CORRECT_DEF);//GLIDE_34GETENV("FX_GLIDE_AA8_OFFSET_Y7", GC.bInfo->RegPath, SECBUFVTXOFFY_8SMPL_CHP3_CORRECT_DEF);
  
  /*
   * AJB-  Support the slightly silly way the DirectX gang controls
   *       SLI & AA from the 3dfx tools control panel, just to make
   *       life a little easier for Reid.
   *
   *       Here is a breakdown of the bizarre table of magic numbers:
   *
   *       0 - SLI & AA disable
   *       1 - SLI disabled, 2 sample AA enabled			
   *       2 - 2-way SLI enabled, AA disabled				
   *       3 - 2-way SLI enabled, 2 sample AA enabled		
   *       4 - SLI disabled, 4 sample AA enabled			
   *       5 - 4-way SLI enabled, AA disabled				
   *       6 - 4-way SLI enabled, 2 sample AA enabled		
   *       7 - 2-way SLI enabled, 4 sample AA enabled
   *	   8 - SLI disabled, 8 sample AA enabled
   *
   *       to add to the silliness:
   *
   *       settings 0 & 1 are valid for all configurations
   *       settings 2, 3 & 4 are valid for 2 chip boards only
   *       settings 5, 6 & 7 are valid for 4 chip boards only
   *
   *       to make life easier on everyone, we won't enforce the board
   *       type restriction and we will default to use whatever SLI
   *       is available with no AA if the variable cannot be found.
   */
  _GlideRoot.environment.forceSingleChip = 0 ;
  _GlideRoot.environment.aaSample = 0 ;

  switch(GLIDE_GETENV("SSTH3_SLI_AA_CONFIGURATION", GC.bInfo->RegPath, 2L))
  { 
    case 1:
      _GlideRoot.environment.aaSample = 2 ;
    case 0:
      _GlideRoot.environment.forceSingleChip = 1 ;
      break ;
    case 3:
    case 6:
      _GlideRoot.environment.aaSample = 2 ;
      break ;
    case 4:
    case 7:
      _GlideRoot.environment.aaSample = 4 ;
      break;
//8xaa
    case 8:
      _GlideRoot.environment.aaSample = 8 ;
      break ;
    
    default:
      break ;
  }

  /*
  ** AJB-  This lets Joe bag-o-donuts force 32bpp & AA rendering
  **       for apps that call grSstWinOpen.
  */
  _GlideRoot.environment.outputBpp = GLIDE_GETENV("FX_GLIDE_BPP", GC.bInfo->RegPath, 0L) ;

  /* Note- If the old school Glide env. vars for AA sample & Num chips
   * are active, they should ALWAYS override the control panel variable
   */
  if (GETENV("FX_GLIDE_AA_SAMPLE", GC.bInfo->RegPath))
    _GlideRoot.environment.aaSample = atol(GETENV("FX_GLIDE_AA_SAMPLE", GC.bInfo->RegPath)) ;

  if (GLIDE_GETENV("FX_GLIDE_NUM_CHIPS", GC.bInfo->RegPath, 0L) > 1)
    _GlideRoot.environment.forceSingleChip = 0 ;

  /*
  **  CHD - This let's Joe bag-o-croissant force rendering-column width.
  */
  _GlideRoot.environment.columnWidth = GLIDE_GETENV("FX_GLIDE_COLUMN_WIDTH", GC.bInfo->RegPath, 32L) ;
  /*
   * AJB- This lets those lucky people with non-flaky SLI slave chips
   *      enable the WAX functions that cause us unlucky folks to hang.
   */
  _GlideRoot.environment.waxon = GLIDE_GETENV("FX_GLIDE_WAX_ON", GC.bInfo->RegPath, 1L) ;

  /*
   * KCD- Let user toggle AA on and off on the fly to impress their friends.
   */
  _GlideRoot.environment.aaToggleKey = GLIDE_GETENV("FX_GLIDE_AA_TOGGLE_KEY", GC.bInfo->RegPath, 0L) ;

  /* Save off 32-bit screenshots from four-sample 16-bit AA buffers */
  _GlideRoot.environment.aaScreenshotKey = GLIDE_GETENV("FX_GLIDE_SCREENSHOT_KEY", GC.bInfo->RegPath, 0L) ;

  /* Which way to do 2-sample AA? */
  _GlideRoot.environment.forceOldAA = GLIDE_GETENV("FX_GLIDE_FORCE_OLD_AA", GC.bInfo->RegPath, 0L);

  /*
   * AJB- 1= Always analog sli, 0= Glide decides, -1= always digital
   */
  _GlideRoot.environment.analogSli = GLIDE_GETENV("FX_GLIDE_ANALOG_SLI", GC.bInfo->RegPath, 0L) ;

  _GlideRoot.environment.lodBias = GLIDE_GETENV("FX_GLIDE_LOD_BIAS", GC.bInfo->RegPath, 0L) ;

  /****************************************************************** 
   * 5/4/99 gregk
   * Adding user support for turning on/off dither substraction
   * According to Alpha Blending Quality Tab of control panel choices   
   * Optimal/Sharper -> disable dither subtraction    
   * Smoother        -> enable  dither subtraction                     
   ******************************************************************/  
   ditherMode = GLIDE_GETENV("SSTH3_ALPHADITHERMODE", GC.bInfo->RegPath, 1L);
   switch(ditherMode)     
      {
      default:
      case OPTIMAL: /* Or Automatic? */
      case SHARPER:
        _GlideRoot.environment.disableDitherSub = FXTRUE;
        break;
      case SMOOTHER:
        _GlideRoot.environment.disableDitherSub = FXFALSE;
        break;
      }  
    GDBG_INFO(80,"  disableDitherSub: %d\n",_GlideRoot.environment.disableDitherSub);  
  
  
  _GlideRoot.environment.texLodDither      = GLIDE_GETENV("FX_GLIDE_LOD_DITHER", GC.bInfo->RegPath, 0) ? SST_TLODDITHER : 0x00UL;
  GDBG_INFO(80,"      texLodDither: %d\n",_GlideRoot.environment.texLodDither);
  _GlideRoot.environment.texSubLodDither = GLIDE_GETENV("FX_GLIDE_LOD_SUBSAMPLE_DITHER", GC.bInfo->RegPath, 1L) ;
  GDBG_INFO(80,"   texSubLodDither: %d\n",_GlideRoot.environment.texSubLodDither );
  _GlideRoot.environment.nColorBuffer      = GLIDE_GETENV("FX_GLIDE_ALLOC_COLOR", GC.bInfo->RegPath, -1L);
  GDBG_INFO(80,"      nColorBuffer: %d\n",_GlideRoot.environment.nColorBuffer);
  _GlideRoot.environment.tmuMemory =       GLIDE_GETENV("FX_GLIDE_TMU_MEMSIZE", GC.bInfo->RegPath, -1L);
  GDBG_INFO(80,"\ttmuMemory: %d\n",_GlideRoot.environment.tmuMemory);
  _GlideRoot.environment.nAuxBuffer        = GLIDE_GETENV("FX_GLIDE_ALLOC_AUX", GC.bInfo->RegPath, -1L);    
  GDBG_INFO(80,"        nAuxBuffer: %d\n",_GlideRoot.environment.nAuxBuffer);
  _GlideRoot.environment.swFifoLWM         = GLIDE_GETENV("FX_GLIDE_LWM", GC.bInfo->RegPath, -1L);
  GDBG_INFO(80,"         swFifoLWM: %d\n",_GlideRoot.environment.swFifoLWM);
  _GlideRoot.environment.swapInterval      = GLIDE_GETENV("FX_GLIDE_SWAPINTERVAL", GC.bInfo->RegPath, -1L);
  GDBG_INFO(80,"      swapInterval: %d\n",_GlideRoot.environment.swapInterval);
  _GlideRoot.environment.snapshot          = GLIDE_GETENV("FX_SNAPSHOT", GC.bInfo->RegPath, -1L);
  GDBG_INFO(80,"          snapshot: %d\n",_GlideRoot.environment.snapshot);
  _GlideRoot.environment.guardbandclipping = GLIDE_GETENV("FX_GLIDE_GBC", GC.bInfo->RegPath, 1L);
  GDBG_INFO(80," guardbandclipping: %d\n",_GlideRoot.environment.guardbandclipping);
  /* KoolSmoky - enable 2ppc only in certain condition. -1=disable 1=enable 0=glide desides. disabled for now */
  _GlideRoot.environment.do2ppc            = GLIDE_GETENV("FX_GLIDE_2PPC", GC.bInfo->RegPath, -1L);
  GDBG_INFO(80," do2ppc           : %d\n",_GlideRoot.environment.do2ppc);
  _GlideRoot.environment.band2ppc          = GLIDE_GETENV("FX_GLIDE_2PPC_BAND", GC.bInfo->RegPath, 1L/*2L*/);
  GDBG_INFO(80," band2ppc         : %d\n",_GlideRoot.environment.band2ppc);
  _GlideRoot.environment.sliBandHeight     = GLIDE_GETENV("FX_GLIDE_SLI_BAND_HEIGHT", GC.bInfo->RegPath, 0L);
  GDBG_INFO(80," sliBandHeight    : %d\n",_GlideRoot.environment.sliBandHeight);
  _GlideRoot.environment.aaClip     = GLIDE_GETENV("FX_GLIDE_AA_CLIP", GC.bInfo->RegPath, 1L);
  GDBG_INFO(80," aaClip    : %d\n",_GlideRoot.environment.aaClip);

  /* KoolSmoky - there is a possibility that grEnable(GR_OPENGL_MODE_EXT) is called once
   * but grSstSelect is called multiple times, so we won't retreive the sliBandHeightForce
   * envar if it's already set to openGL app.
   */
  //if( _GlideRoot.environment.sliBandHeightForce != FXTRUE )
  //  _GlideRoot.environment.sliBandHeightForce = GLIDE_GETENV("FX_GLIDE_FORCE_SLI_BAND_HEIGHT", GC.bInfo->RegPath, 0L);
  //GDBG_INFO(80," sliBandHeightForce : %d\n",_GlideRoot.environment.sliBandHeightForce);

  _GlideRoot.environment.swapPendingCount  = GLIDE_GETENV("FX_GLIDE_SWAPPENDINGCOUNT", GC.bInfo->RegPath, 1L);
  if (_GlideRoot.environment.swapPendingCount > 3)
    _GlideRoot.environment.swapPendingCount = 3;
  if (_GlideRoot.environment.swapPendingCount < 0)
    _GlideRoot.environment.swapPendingCount = 0;
  GDBG_INFO(80," swapPendingCount : %d\n",_GlideRoot.environment.swapPendingCount);

  /* KoolSmoky - the default RGB gamma reset to 1.3 */
  _GlideRoot.environment.gammaR = GLIDE_FGETENV("SSTH3_RGAMMA", GC.bInfo->RegPath, 1.3f);
  _GlideRoot.environment.gammaG = GLIDE_FGETENV("SSTH3_GGAMMA", GC.bInfo->RegPath, 1.3f);
  _GlideRoot.environment.gammaB = GLIDE_FGETENV("SSTH3_BGAMMA", GC.bInfo->RegPath, 1.3f);
  
  //_GlideRoot.environment.useAppGamma  = GLIDE_GETENV("FX_GLIDE_USE_APP_GAMMA", GC.bInfo->RegPath, 1L);

  _GlideRoot.CPUType = GLIDE_GETENV("FX_CPU", GC.bInfo->RegPath, _cpu_detect_asm() );    
  GDBG_INFO(0,"               cpu: 0x%x\n",_GlideRoot.CPUType);

  /* Setup the basic proc tables based on the cpu type. */
  {
    /* Default case - rasterization routines */
#if GLIDE_DISPATCH_SETUP
    _GlideRoot.curTriProcs = _triSetupProcs + 0;
#endif /* GLIDE_DISPATCH_SETUP */
    
    /* Default case - texture download procs */
    _GlideRoot.curTexProcs = _texDownloadProcs + 0;
    
    /* null proc case */
#define ARRAY_LAST(__array) ((sizeof(__array) / sizeof((__array)[0])) - 1)
    _GlideRoot.nullTriProcs = _triSetupProcs + ARRAY_LAST(_triSetupProcs);
    _GlideRoot.nullTexProcs = _texDownloadProcs + ARRAY_LAST(_texDownloadProcs);
#undef ARRAY_LAST
    
    /* Check for vendor specific optimization cases */
    switch((_GlideRoot.CPUType & 0xFFFF0000UL) >> 16UL) {
    case kCPUVendorIntel:
      break;

#if GL_AMD3D
    case kCPUVendorAMD:
    case kCPUVendorCyrix:
    case kCPUVendorIDT:
      if ((_GlideRoot.CPUType & 0x2L) == 0x2UL) {  /* MMX and 3DNow! feature bits set */
        GDBG_INFO(0,"using MMX and 3DNow!\n");
        /* rasterization routines */
#if GLIDE_DISPATCH_SETUP
        _GlideRoot.curTriProcs = _triSetupProcs + 1;
#endif /* GLIDE_DISPATCH_SETUP */

        /* texture download procs */
        _GlideRoot.curTexProcs = _texDownloadProcs + 1;
      }
      break;
#endif /* GL_AMD3D */

    case kCPUVendorUnknown:
    default:
      break;
    }
  }
  
#if __POWERPC__ && PCI_BUMP_N_GRIND
  _GlideRoot.environment.autoBump = FXFALSE;
#else  
  _GlideRoot.environment.autoBump = GETENV("FX_GLIDE_BUMP", GC.bInfo->RegPath) == NULL;
#endif  
  GDBG_INFO(80, "\tautoBump:          %s\n",
            _GlideRoot.environment.autoBump ? "FXTRUE" : "FXFALSE");
  
  if (GETENV("FX_GLIDE_BUMPSIZE", GC.bInfo->RegPath))
    sscanf(GETENV("FX_GLIDE_BUMPSIZE", GC.bInfo->RegPath), "%x",
           &_GlideRoot.environment.bumpSize);
  else
#if __POWERPC__  
    _GlideRoot.environment.bumpSize = 0x1000;
#else
    _GlideRoot.environment.bumpSize = 0x10000;
#endif

  _GlideRoot.environment.fenceLimit =
    GLIDE_GETENV("FX_GLIDE_FENCE_LIMIT", GC.bInfo->RegPath, 0x10000);
  if (_GlideRoot.environment.fenceLimit > 0x10000)
    _GlideRoot.environment.fenceLimit = 0x10000;
  
  /* Pre-convert the bump size from bytes to words */
  _GlideRoot.environment.bumpSize >>= 2UL;
  GDBG_INFO(80, "\tbumpSize:          0x%x\n", _GlideRoot.environment.bumpSize);

  /* constant pool */
  _GlideRoot.pool.f0   =   0.0F;
  _GlideRoot.pool.fHalf=   0.5F;
  _GlideRoot.pool.f1   =   1.0F;
  _GlideRoot.pool.f255 = 255.0F;
  
#if GLIDE_PACKED_RGB
  _GlideRoot.pool.fBiasHi = (float)(0x01 << 15);
  _GlideRoot.pool.fBiasLo = (float)(0x01 << 23);
#endif /* GLIDE_PACKED_RGB */

  _GlideRoot.environment.emulRush = (GETENV("FX_GLIDE_EMUL_RUSH", GC.bInfo->RegPath) != NULL);
  GDBG_INFO(80,"\temulate Rush: %d\n", _GlideRoot.environment.emulRush);
#if 0 /* KoolSmoky - disable for now */
  _GlideRoot.environment.grxClk = GLIDE_GETENV("FX_GLIDE_GRXCLK", GC.bInfo->RegPath, -1);
#endif
  _GlideRoot.environment.enUma = 0;

  /* KoolSmoky - current_sst is not always 0  */
  /* _GlideRoot.current_sst = 0; */ /* make sure there's a valid GC */
  _GlideRoot.current_sst = ctx;
  _GlideRoot.curGC       = &_GlideRoot.GCs[ctx];

  grErrorSetCallback(_grErrorDefaultCallback);

  /* KoolSmoky - Moved to grGlideInit
  if ( !_grSstDetectResources() ) {
#ifdef GLIDE_INIT_HWC
    GrErrorCallback( hwcGetErrorString(), FXTRUE );
#endif
  } */
  

  /* KoolSmoky - just get the info for the requested sst
  for (i = 0; i < _GlideRoot.hwConfig.num_sst; i++) {
    displayBoardInfo(i, &_GlideRoot.hwConfig);
  }
  */
  _GlideRoot.GCs[ctx].mm_table.free_mmid = 0;
  displayBoardInfo(ctx, &_GlideRoot.hwConfig);

  _grMipMapInit();
  _GlideRoot.initialized = FXTRUE;               /* save this for the end */
#undef FN_NAME
} /* _GlideInitEnvironment */
