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
** $Header: f:\\cvsroot/Glide3x/h5/minihwc/minihwc.c,v 1.7 2002/05/04 02:07:44 KoolSmoky Exp $
** $Log:
** 108             1.80       11/30/02 KoolSmoky   moved CEASE_ALL_GDBG because we need
**        debug info for certain apps to run.
** 107             1.79       11/27/02 KoolSmoky   make exception for winxp escapecalls.
** 106             1.78       11/13/02 KoolSmoky   fixed flaw in escape calles.
** 105   ve3d      1.77       07/22/02 KoolSmoky   revised QueryContextProc to handle
**         windows hotkeys better (alt-tab).
** 104   ve3d      1.76       05/28/02 KoolSmoky   fixed win9x regpath retrieve routine.
** 103   ve3d      1.75       05/03/02 KoolSmoky   added Colourless's window handle pointer
**         restorer. there should be no more crashes with screen changes within glide.
** 102   ve3d      1.74       04/29/02 KoolSmoky   pave way for multimonitor support. hack to
**         force secondary and beyond sst devices to be primary sst. added FX_GLIDE_SSTSELECT
**         r/w to the secondary and beyond sst device seem to be efed. help me out here...
** 101   ve3d      1.73.7     02/24/02 KoolSmoky   context bug fix faster screen restore.
** 100   ve3d      1.73.6     02/15/02 KoolSmoky   context bug fix related to windows resizing.
**  99   ve3d      1.73.5     12/27/01 KoolSmoky   Cleaned up code.
**  98   ve3d      1.73.4     12/19/01 KoolSmoky   NT5.1 context fix. Still buggy but much better.
**         Someone pleeeeeeez fix this! Added HWC_WINXP_MINIVDD_HACK, IS_ALT_TAB.
**  97   ve3d      1.73.3     12/18/01 KoolSmoky   NT5.1 context madness! Why isn't
**         the Device context obtained from the rendered window? we'll use the state of the
**         rendered window instead.
**  96   ve3d      1.73.2     12/15/01 KoolSmoky    New NT5.1 context fix. But still crashes. why?
**  95   ve3d      1.73.1     12/14/01 KoolSmoky    more NT5.1 context temporal fix.
**         STILL crashes on occasions. fix me!
**  94   ve3d      1.73        12/13/01 KoolSmoky    NT5.1 context temporal fix.
**         still crashes on occasions. fix me!
**  93   ve3d      1.72        12/09/01 KoolSmoky    use dummycontext if NT5.1
**         returns NULL for context.
**  92   3dfx      1.71.1.6.1.4.1.711/08/00 Drew McMinn     Added
**       FX_GLIDE_BRIGHTNESS and FX_GLIDE_CONTRAST modifiers
**  91   3dfx      1.71.1.6.1.4.1.610/11/00 Brent           Forced check in to
**       enforce branching.
**  90   3dfx      1.71.1.6.1.4.1.509/15/00 troy thornton   added code to allow
**       grquerryresolutions to check the refresh rate of the monitor before
**       returning a valid screen mode
**  89   3dfx      1.71.1.6.1.4.1.408/29/00 Jonny Cochrane  Some 8x FSAA code
**  88   3dfx      1.71.1.6.1.4.1.308/07/00 Andy Hanson     Fixed up lfb cfg
**       registers.  Variable bpp was being used without being properly
**       initialized.  Cleaned up source, and put some safety strncpy's for
**       errorString.  Sprintf's are still unsafe.
**  87   3dfx      1.71.1.6.1.4.1.207/21/00 Adam Briggs     ack: the display driver
**       is always returning 1 for the AGPINFO escape whether or not an AGP fifo
**       exists... make sure we use a memory fifo if the agp fifo is zero bytes
**       long
**  86   3dfx      1.71.1.6.1.4.1.107/06/00 Adam Briggs     fixed Ken's AA
**       screenshot function for 32bit & 2-sample AA
**  85   3dfx      1.71.1.6.1.4.1.006/22/00 troy thornton   added
**       FX_GLIDE_USE_APP_GAMMA env. var.
**  84   3dfx      1.71.1.6.1.406/20/00 Joseph Kain     Fixed errors introduced by
**       my previous merge.
**  83   3dfx      1.71.1.6.1.306/20/00 Joseph Kain     Changes to support the
**       Napalm Glide open source release.  Changes include cleaned up offensive
**       comments and new legal headers.
**  82   3dfx      1.71.1.6.1.206/16/00 Adam Briggs     win2k & winnt unmap the
**       board when we release the windowed mode context
**  81   3dfx      1.71.1.6.1.106/07/00 Adam Briggs     allowed NT4 to run in
**       multichip mode
**  80   3dfx      1.71.1.6.1.005/24/00 Kenneth Dyke    Updated MacOS screenshot
**       SLI/AA handling.
**  79   3dfx      1.71.1.6    05/23/00 Adam Briggs     Added multichip support for
**       win2k
**  78   3dfx      1.71.1.5    05/23/00 Kenneth Dyke    Added some MacOS related
**       fixes.
**  77   3dfx      1.71.1.4    05/18/00 Adam Briggs     Changed win9x VDD name to
**       h5vdd.
**  76   3dfx      1.71.1.3    05/12/00 Stephane Huaulme made Mac compile happy by
**       keeping local storage under 32K
**  75   3dfx      1.71.1.2    05/11/00 Stephane Huaulme fixed a few Macintosh bugs
**  74   3dfx      1.71.1.1    05/09/00 Kenneth Dyke    Added code to calculate
**       chip values on Napalm.
**  73   3dfx      1.71.1.0    05/02/00 Kenneth Dyke    Fixed a really effing
**       stupid typo that broke FSAA screenshots. :(
**  72   3dfx      1.71        04/25/00 Kenneth Dyke    Made FX_GLIDE_NO_HW a lot
**       more robust.
**  71   3dfx      1.70        04/21/00 Kenneth Dyke    FX_GLIDE_NO_HW support.
**  70   3dfx      1.69        04/16/00 Kenneth Dyke    Force some configuration
**       parameters on 4-way boards.
**  69   3dfx      1.68        04/13/00 Kenneth Dyke    Added support for new-style
**       two-sample AA.
**  68   3dfx      1.67        04/10/00 Kenneth Dyke    Added code to take
**       full-precision screenshots.
**  67   3dfx      1.66        04/03/00 Kenneth Dyke    Refined startup & shutdown
**       hardware idle.
**  66   3dfx      1.65        03/30/00 Kenneth Dyke    Even better fifo init code
**       to guard against hangs.
**  65   3dfx      1.64        03/29/00 Adam Briggs     i got in before ken.
**  64   3dfx      1.63        03/28/00 Kenneth Dyke    More paranoia for fifo init
**       code.
**  63   3dfx      1.62        03/27/00 Kenneth Dyke    DOS Glide support for
**       two-chip (and maybe four-chip) Napalm boards.
**  62   3dfx      1.61        03/26/00 Adam Briggs     added code to divine the
**       proper registry path for win2k hwcGetEnv
**  61   3dfx      1.60        03/24/00 Kenneth Dyke    Improved Napalm DOS Glide
**       support.
**       Reset FBI & 2D when Glide starts up.
**  60   3dfx      1.59        03/21/00 Kenneth Dyke    Lots of video backend fixes
**       for SLI/AA/32-bit.
**  59   3dfx      1.58        03/15/00 Kenneth Dyke    Don't let the user do
**       something dumb. ;)
**  58   3dfx      1.57        03/14/00 Adam Briggs     let glide decide when to
**       use analog sli
**  57   3dfx      1.56        03/13/00 Kenneth Dyke    Removed A0 read abort
**       workaround.
**  56   3dfx      1.55        03/08/00 Kenneth Dyke    Keep track of whether or
**       not a board is "mapped" or not.
**  55   3dfx      1.54        03/07/00 Don Mullis      Generalize glide3 to handle
**       sub-byte-size texels.
** 
**  54   3dfx      1.53        03/07/00 Adam Briggs     make sure that we don't
**       request a bogus sli mode from the minivdd
**  53   3dfx      1.52        03/07/00 Kenneth Dyke    Fixed Win32 build breakage.
** 
**  52   3dfx      1.51        03/07/00 Stephane Huaulme implemented read and write
**       config reg (using hrm) for Macintosh
**  51   3dfx      1.50        03/07/00 Adam Briggs     fail tiled fifos in
**       windowed mode fifo alloc
**  50   3dfx      1.49        03/07/00 Kenneth Dyke    Do NT check earlier.  Force
**       number of chips to 1 for NT for now.
**  49   3dfx      1.48        03/03/00 Kenneth Dyke    Make sure code doesn't barf
**       for non-Napalm build.
**  48   3dfx      1.47        03/03/00 Kenneth Dyke    Fixed physical -> LFB
**       address calculation once and for all (I hope).  Added code to support SLI
**       read abort workaround.
**  47   3dfx      1.46        03/01/00 Kenneth Dyke    A bunch of fixes for AA LFB
**       accesses (and 32-bit LFB accesses).
** 
**  46   3dfx      1.45        02/29/00 Adam Briggs     ripped out all of the
**       half-baked Ext2 stuff
**  45   3dfx      1.44        02/28/00 Kenneth Dyke    Fixed Napalm FIFO sizing.
**  44   3dfx      1.43        02/24/00 Larry  warner   Don't turn off FIFO in
**       hwcRestoreVideo.  See Ken Dyke for details.
**  43   3dfx      1.42        02/22/00 Kenneth Dyke    Make sure we don't try to
**       enable AA mode on single-chip boards if the user doesn't actually ask for
**       AA.
**  42   3dfx      1.41        02/15/00 Kenneth Dyke    Fixed some incorrect
**       information passed to the minivdd.  Also fixed some MacOS build breakage.
**  41   3dfx      1.40        02/14/00 Kenneth Dyke    Make sure that we calculate
**       the right number of tiles we need when in SLI mode.  Get SLI band height
**       info from Glide rather than checking env var directly.
**  40   3dfx      1.39        02/14/00 Kenneth Dyke    Fix stupid typo.
**  39   3dfx      1.38        02/14/00 Kenneth Dyke    Added support for setting
**       number of chips via FX_GLIDE_NUM_CHIPS.  Also added temporary hack to set
**       PCI fifo threshold value.
**  38   3dfx      1.37        02/08/00 Kenneth Dyke    Further AA code cleanup. 
**       Disable overlay filter in SLI mode.
**  37   3dfx      1.36        02/07/00 Kenneth Dyke    MacOS & PC fixes for
**       multi-chip anti-aliasing.
**  36   3dfx      1.35        02/01/00 Kenneth Dyke    Change in HRM board info
**       struct.
**  35   3dfx      1.34        01/31/00 Adam Briggs     changed the IS_NAPALM macro
**       to cooperate with the display driver version of the same
**  34   3dfx      1.33        01/31/00 Kenneth Dyke    Add in 2 spample AA support
**       for MacOS.
**  33   3dfx      1.32        01/31/00 Adam Briggs     Changed all device ID magic
**       numbers to use those defined in fxhal.h & added IS_NAPALM macro to test
**       against device ID range
**  32   3dfx      1.31        01/28/00 Kenneth Dyke    Removed accidental extra
**       #endif after merge.
**  31   3dfx      1.30        01/28/00 Kenneth Dyke    Update MacOS minihwc code
**       for new HRM protocol.
** 
**  30   3dfx      1.29        01/24/00 Adam Briggs     only query for slave regs
**       when slave chips are present
**  29   3dfx      1.28        01/23/00 Adam Briggs     use the minivdd correctly
**       and 4 sample AA suddenly works
**  28   3dfx      1.27        01/22/00 Adam Briggs     made the build process stop
**       whining
**  27   3dfx      1.26        01/21/00 Adam Briggs     get the correct linear
**       mappings of slave regs
**  26   3dfx      1.25        01/19/00 Kenneth Dyke    Do proper Napalm fifo
**       padding.
**  25   3dfx      1.24        01/19/00 Adam Briggs     for some strange reason,
**       display driver escapes don't seem to work in DOS.
**  24   3dfx      1.23        01/19/00 Adam Briggs     Made minihwc use either
**       EXT_HWC or EXT_HWC_OLD depending on which one the display driver supports
**       so that a new glide can try to run on an old display driver.
**  23   3dfx      1.22        01/18/00 Kenneth Dyke    Fixed more issues with AA
**       buffers and chip configuration.
**  22   3dfx      1.21        01/16/00 Kenneth Dyke    Use new & improved register
**       bit names.
**  21   3dfx      1.20        01/16/00 Kenneth Dyke    Major fixes for 32-bit and
**       AA modes.  
**       Fix to disable FIFO when Glide shuts down.
**       Added code to temporarily move Napalm FIFO out of evil VGA space.
**  20   3dfx      1.19        01/05/00 Larry  warner   Use GetCurrentProcessId()
**       rather than GetCurrentProcess().
**  19   3dfx      1.18        01/03/00 Adam Briggs     added hwcGetGammaTable
**       function to allow OpenGL to later restore the original Gamma ramp.
**  18   3dfx      1.17        12/10/99 Leo Galway      Removed previous hi-res
**       mode information for Glide3. These modes were only necessary for
**       Cornerstone (or future hi-res) support in RT4.2 source branch and
**       proceeded to break the V3 and V2 builds (from 3dfx view), hence they have
**       been removed.
**  17   3dfx      1.16        12/09/99 Leo Galway      Added mode information for
**       1600x1280, 1792x1440, 1920x1080, 1920x1200, 2046x1536 (as a result of
**       glide3 being tested with Cornerstone modes). Although not all of these
**       modes are capable under Glide3, their inclusion prevents Glide3 test apps
**       from being displayed incorrectly in theses hi-res modes. 
**       Search for SUSTAINED_ENGINEERING_CHANGE_BEGIN. 
**  16   3dfx      1.15        11/30/99 Kenneth Dyke    Silently fail if HRM
**       version is not correct.
**  15   3dfx      1.14        11/29/99 Kenneth Dyke    Fixed DOS build breakage.
**  14   3dfx      1.13        11/23/99 Stephane Huaulme version conflict safety
**       net
**  13   3dfx      1.12        11/22/99 Kenneth Dyke    Added hwcIdleWinFifo().
**  12   3dfx      1.11        11/22/99 Adam Briggs     Merged revs 9 & 10 back in.
**        Also protected HMONITOR from being redefined.
**  11   3dfx      1.10        11/12/99 Don Mullis      Fix additional bugs found
**       by source code inspection.  
** 
**       Verified that PRS# 11145 & 10978  remain fixed.
** 
** 
**  10   3dfx      1.9         11/11/99 Michelle Tsui   set sfcEnd properly.
**  9    3dfx      1.8         11/09/99 Adam Briggs     Added support for getting
**       slave chip MMIO pointers from hwcMapBoard and hwcInitRegisters
**  8    3dfx      1.7         10/27/99 Anthony tai     initialize h3pixelSize for
**       DOS
**  7    3dfx      1.6         10/13/99 Anthony tai     back to the oem style unmap
**       memory to make wnt/w2k driver happy
**  6    3dfx      1.5         10/08/99 Anthony tai     fixed getenv with all caps
**  5    3dfx      1.4         10/08/99 Anthony tai     set v3 fifo to low address
**       by default to exercise napalm fifo space. to disable low address fifo, set
**       FX_GLIDE_V3FIFO=1
**  4    3dfx      1.3         10/08/99 Anthony tai     added env vars
**       "FX_GLIDE_DEVICEID"
**  3    3dfx      1.2         10/04/99 Matt McClure    Ported Glide Context
**       Switching code from V3_OEM_100.
**  2    3dfx      1.1         09/13/99 Anthony tai     check chip number
**  1    3dfx      1.0         09/11/99 StarTeam VTS Administrator 
** $
** 
** 175   9/09/99 6:42p Atai
** fixed 32bpp window app
** 
** 174   9/03/99 4:33p Atai
** reset aa/sli pci registers
** 
** 173   9/01/99 2:47p Atai
** for windows only
** 
** 172   8/31/99 5:10p Atai
** program cfgAALfbCtrl
** 
** 171   8/23/99 3:12p Kcd
** Lots of new MacOS 8 stuff.
** 
** 170   8/18/99 8:23p Adamb
** Put Anthony's multichip code back in--Still needs to be updated for
** evolving Ext2 interface, though.
** 
** 169   8/18/99 9:57a Sbrooks
** Fixed erroneous deletion.
** 
** 164   8/04/99 3:35p Atai
** 32 bpp happy
** 
** 163   7/19/99 2:51p Atai
** added variable for sli
** 
** 162   7/16/99 5:08p Sbrooks
** Fixes PRS 6869
** 
** 161   7/02/99 3:41p Kcd
** MacOS HRM mods.
** 
** 160   6/22/99 4:56p Sbrooks
** Now checks for ShareContextDWORD failure.  Was only checking for
** protocol failure.
** 
** 159   6/21/99 7:29p Kcd
** More MacOS work.
** 
** 158   6/21/99 1:25p Atai
** modify for h3pixelSize
** place fifo at 0x0 for Naplam
** 
** 157   6/17/99 4:23p Atai
** remove compiler errors and warnings for dos build
** 
** 156   6/17/99 3:15p Stb_sbrooks
** Fixes PRS 6682
** 
** 155   6/14/99 4:28p Atai
** more on 2nd buffer allocation
** 
** 154   6/14/99 3:17p Atai
** added secondary buffer info
** 
** 153   6/11/99 4:02p Stb_sbrooks
** 
** 151   6/07/99 3:10p Atai
** report as Banshee if it is an 8M board
** 
** 150   6/04/99 11:14a Kcd
** Disable surface stuff when not building for Glide3 on MacOS.
** 
** 149   6/02/99 2:48p Atai
** set min texture size to 0x200000 if FX_GLIDE_TMU_MEMSIZE=1
** 
** 148   5/28/99 11:49a Dow
** tm1/fifo page overlap fix.  Modified by Dow.  Checked in by Brooks.
** 
** 147   5/27/99 11:04a Dow
** Removed FIFO disable at end.
** 
** 146   5/26/99 5:51p Kcd
** MacOS fix for alt-TAB context stuff.
** 
** 145   5/13/99 2:28p Stb_gkincade
** Added user support for switching the video filter from 2x2 to 4x1
** 
** 144   5/06/99 6:37p Dow
** Fix for no alt-tab in driver
** 
** 143   4/25/99 1:04p Atai
** don't call HWCEXT_RELEASECONTEXT when glide release context in NT. The
** driver will restore vidProcCfg and it corrupt alt-entering full screen
** dos mode.
** 
** 142   4/23/99 5:10p Atai
** nt driver returned lostcontext address, not offset
** 
** 141   4/22/99 4:29p Dow
** Alt-Tab on NT
** 
** 140   4/20/99 10:55a Atai
** protect the dos build
** 
** 139   4/16/99 2:41p Kcd
** Mac fixes.
** 
** 138   4/15/99 5:32p Dow
** Alt TAb for NT + palette fix
** 
** 137   4/09/99 4:22p Dow
** Sandbox issues
** 
** 136   4/08/99 6:15p Atai
** added env var FX_GLIDE_REFRESH
** 
** 135   4/05/99 8:24p Dow
** Alt tab happiness--sorta
** 
** 134   4/05/99 10:38a Atai
** protect window only code
** 
** 133   4/04/99 7:13p Atai
** added cast
** 
** 132   4/04/99 7:14p Sreid
** Plumbing for gc->lostcontext flag
** 
** 129   4/01/99 7:52p Peter
** (temp?) fix for query context extescape return value
** 
** 128   4/01/99 10:30a Peter
** check status enum not boolean
** 
** 127   4/01/99 10:08a Peter
** forgot to protect extEscape, doh!
** 
** 126   3/31/99 9:01p Dow
** context check w/ driver
** 
** 125   3/25/99 2:30p Peter
** surface fifo allocation w/ desktop widths < 1024
** 
** 124   3/24/99 10:19a Peter
** required lockabilly
** 
** 123   3/23/99 6:48p Atai
** use max fifo size if 16M board
** 
** 122   3/17/99 3:46p Dow
** Luke Skywalker
** 
** 121   3/11/99 8:37p Peter
** update vidOverlayEndScreenCoord when the overlay dims != screen dims
** (w/ optional scaling)
** 
** 120   3/10/99 11:52a Peter
** initFifo enable hole counting parameter
** 
** 119   3/09/99 12:31p Kcd
** Added MacOS stuff.  Fixed refresh constant table (mis)use.
** 
** 118   3/05/99 10:06p Peter
** allocate independent state buffers in parallel w/ the command buffers
** 
** 117   3/03/99 3:52p Peter
** removed hemorrhaging GetDC and create glide private dc's for ExtEscape
** 
** 116   3/02/99 8:03p Dow
** 
** 115   2/27/99 12:24p Dow
** New resolutions
** 
** 114   2/22/99 5:06p Peter
** cleaned up monitor enum crap
** 
** 113   2/19/99 2:00p Atai
** fixed lfbBuffAddr for 800x600 triple buffering lfb access
** 
** 112   2/17/99 2:36p Peter
** removed extra dd call, fail agp fifo if no dd agp support, release
** window dc after enum
** 
** 111   2/13/99 1:57p Dow
** Added code for new resolutions
** 
** 110   2/08/99 6:42p Atai
** use the active window display as sst 0
** 
** 109   2/02/99 4:37p Peter
** hwGetSurfaceinfo gets depth, releasing memory fifo creation dd object
** 
** 108   1/25/99 6:37p Peter
** removed redundant macros
** 
** 107   1/22/99 4:28p Dow
** Fixed 1600x1200 the right way.
** 
** 106   1/22/99 3:02p Dow
** Turned off 2x2 filtering in 2x mode
** 
** 105   1/21/99 1:43p Dow
** Fencing for Pallette Download
** 
** 104   1/20/99 6:04p Peter
** cleaned up warning
** 
** 103   1/20/99 1:51p Dow
** Pithy workaround for PCI gamma problem
** 
** 102   1/14/99 11:38a Dow
** Left bits alone in vidproccfg
** 
** 101   1/11/99 8:22p Peter
** fixed stride vs width effage that I somehow convinced myself was right
** 
** 100   1/11/99 6:31p Dow
** Added Debugging
** 
** 99    1/04/99 11:58a Peter
** added windowed context support
** 
** 98    12/23/98 12:39p Peter
** hdc cleanup/hwcext_getlinearaddr is not idempotent
** 
** 97    12/22/98 5:43p Atai
** turn on 2x mode for vidProcCfg if greater than or equal to 1280
** 
** 96    12/22/98 3:29p Jeske
** use h3InitGetMemSize() instead so that we don't have to worry about it
** screwing up dramInit1 like h3InitSgram() did in the past...
** 
** 95    12/22/98 2:46p Atai
** fixed for Banshee/DOS draminit1 value
** 
** 94    12/22/98 1:07p Peter
** all dd surface pointers must go through linear map offset to get their
** hw relative offsets
** 
** 93    12/17/98 2:08p Jeske
** comment fix
** 
** 92    12/16/98 7:13p Peter
** query directdraw object for surface memory area caps and set the
** process id when getting the linear addresses via
** 
** 91    12/11/98 1:35p Peter
** window fifo serial # vs pixel cache
** 
** 90    12/09/98 5:17p Dow
** Fixed video scaling for lame resolutions
** 
** 89    12/07/98 2:18p Dow
** Added debug message for process handle
** 
** 88    12/07/98 11:32a Dow
** Fixed 2X resolutions
** 
** 87    12/06/98 11:04a Dow
** 
** 86    12/04/98 8:42a Dow
** Fixed broken DOS build AGAIN
** 
** 85    12/03/98 10:27p Dow
** Added stuff for hwcGetenv
** 
** 84    12/03/98 9:11p Dow
** Fixed resolution check
** 
** 83    12/02/98 9:35p Dow
** Query Resolutions
** 
** 82    12/02/98 2:41p Jeske
** ugh... text badness
** 
** 81    12/02/98 2:38p Jeske
** don't set the clocks unless they use the environment variable
** overrides, even for banshee...
** 
** 80    12/02/98 2:06p Dow
** Gamma Fully Implemented
** 
** 79    11/30/98 6:53p Peter
** video memory fifo's
** 
** 78    11/24/98 12:25p Mikec
** made multi-mon on win95 happy
** 
** 77    11/24/98 9:43a Atai
** detect multiple cards
** 
** 76    11/23/98 3:28p Atai
** fixed the refresh rate stuff
** 
** 75    11/20/98 4:48p Jeske
** avenger-dos, for now, make sure we don't call h3InitPlls() on avenger..
** 
** 74    11/19/98 1:51p Dow
** Fixed refresh/resolution issue
** 
** 73    11/18/98 8:04p Dow
** grxclk
** 
** 72    11/15/98 2:16a Atai
** set device id = 5. Hack for comdex
** 
** 71    11/10/98 6:29p Atai
** added min_tramSize for board information. min_tramSize = 0x200000 if
** device id is 3 or mem size is 4
** 
** 70    10/30/98 3:44p Dow
** Fixed Tiled/Linear color/aux bug
** 
** 69    10/29/98 5:15p Dow
** Fixed 512x384
** 
** 68    10/21/98 11:22a Peter
** dos happiness w/ the new chris pci library (nee tarolli)
** 
** 67    10/14/98 3:37p Dow
** Gamma stuff
** 
** 66    10/13/98 8:48p Dow
** Env Var for Board mem
** 
** 65    10/08/98 10:15a Dow
** Triple buffering fix
** 
** 64    9/18/98 3:08p Dow
** Fixed DOS build
** 
** 63    9/17/98 3:58p Dow
** Vidmode Stuff
** 
** 62    9/11/98 1:07p Peter
** rounding on raw lfb port addr
** 
** 61    9/02/98 1:34p Peter
** watcom warnings
** 
** 60    8/25/98 6:48p Dow
** Added scaling for low-res
** 
** 59    8/25/98 3:07p Dow
** Checked res of hwcRLSEXCLUSIVE
** 
** 58    8/06/98 7:49p Dow
** Moved detection of SDRAM
** 
** 57    8/02/98 5:00p Dow
** Glide Surface Extension
** 
** 56    7/29/98 3:34p Dow
** 
** 55    7/29/98 3:09p Dow
** SDRAM Fixes
** 
** 54    7/24/98 6:34p Dow
** Fixed DOS Build
** 
** 53    7/24/98 2:02p Dow
** AGP Stuff
** 
** 52    7/23/98 1:18a Dow
** Bump & Grind
** 
** 51    7/18/98 12:21a Jdt
** added state abuffer
** 
** 50    7/16/98 10:26p Dow
** GIW Stuff
** 
** 49    7/16/98 2:53p Dow
** Removed useless colBufferAddr frmo surface info
** 
** 48    7/15/98 4:09p Dow
** GIW Stuff & DOS Protection
** 
** 47    7/14/98 7:19p Dow
** protected some win32 stuff
** 
** 46    7/13/98 10:35p Jdt
** Implemented hwcAllocWinFifo, hwcExecuteWinFifo.
** 
** 45    7/08/98 5:45p Dow
** Moved fifo back up 
** 
** 44    7/02/98 12:11p Dow
** LFB fixes
** 
** 43    6/30/98 10:28a Dow
** Fixed fouled checkin
** 
** 41    6/25/98 6:45p Jdt
** Changes to remove the evil of dxdrvr.c
** 
** 40    6/25/98 12:16p Dow
** Added (protected) alternate FIFO layout
** 
** 39    6/16/98 6:11p Dow
** Rearranged texture memory
** 
** 38    6/16/98 9:49a Dow
** Fixed protected stuff
** 
** 37    6/16/98 9:35a Dow
** Comment -> GDBG_INFO conversion
** 
** 36    6/12/98 10:09a Jdt
** Fix broken dos build.
** 
** 35    6/11/98 7:44p Jdt
** Win98/NT5 Mulitmon 1st Pass
** 
** 34    6/10/98 9:49a Peter
** lfb buffer addressing
** 
** 33    6/09/98 10:09a Dow
** Init register performance tweaks.
** 
** 32    6/06/98 11:42a Dow
** Better buffer alignment for all resolutions.
** 
** 31    6/06/98 10:11a Dow
** Changed AUXPAD from 0 to 0x1000 to offset aux buffer.
** 
** 30    6/05/98 7:34p Dow
** Fixed 8x6-12x10 resolutions
** 
** 29    6/05/98 6:21p Jeske
** now everyone will dxClose(), not just windows
** 
** 28    6/05/98 6:37p Dow
** 1600x1200 perf tuning
** 
** 27    6/05/98 5:28p Jeske
** apparently we need to not call h3InitSetVideoMode() for this to work,
** will this work in non640x480 resolutions? we may need to fix that later
** 
** 26    6/05/98 3:27p Dow
** 1600x1200 Works
** 
** 25    6/04/98 9:36p Jeske
** now we leave VGA legacy decoding on in hwcInitRegisters so we can set
** video modes
** 
** 24    6/04/98 6:52p Dow
** Resolutions to 1600x1200
** 
** 23    6/03/98 1:52p Jeske
** added code to convert from Glide's ordinal refresh number to a refresh
** rate in Hz when calling h3InitSetVideoMode(). We really should specify 
** 
** 22    6/01/98 5:48p Jeske
** allow h3InitSetVideoMode to fail and handle it...
** 
** 21    5/31/98 9:02a Dow
** 800x600 Resolution
** 
** 20    5/28/98 6:34p Dow
** Fixed top scanline bug
** 
** 19    5/27/98 3:35p Mc
** Removed blocks of HW_IO_LOAD and HW_IO_STORE that chris put in.
** 
** 18    5/20/98 8:13p Dow
** device rev
** 
** 17    5/20/98 4:16p Dow
** Added env var for FIFO size
** 
** 16    5/15/98 2:46p Dow
** Attempt to patch back regs on NT
** 
** 15    5/13/98 4:26p Dow
** Protected obsolete IO code that was bodging NT
** 
** 14    4/27/98 2:30p Dow
** OpenProcess stuff
** 
** 13    4/22/98 5:29p Dow
** Added calls to HWCEXT_HWCSETEXCLUSIVE and HWCEXT_HWCRLSEXCLUSIVE
** 
** 12    4/16/98 10:14p Dow
** EXT_HWC is default init method
** 
** 11    4/13/98 5:49p Dow
** Turning theory into practice
** 
** 10    4/09/98 9:34p Dow
** synched with change in h3cinit
** 
** 9     4/08/98 12:25p Dow
** LfbMemoryConfig fix
** 
** 8     4/07/98 10:40p Dow
** LFB Fixes
** 
** 7     4/05/98 2:19p Dow
** DOS/WIN32 stepping on each others' dicks
** 
** 6     4/03/98 2:03p Dow
** DOS Glide modes
** 
** 5     3/28/98 10:51a Dow
** Fixes for FIFO bug
** 
** 4     3/20/98 1:11p Dow
** Now checking revision of chip
** 
** 3     3/20/98 11:43a Dow
** fifo placement for windows
** 
** 2     3/11/98 8:27p Dow
** WinGlide
** 
** 1     3/04/98 4:13p Dow
**
*/

#if !defined(GDBG_INFO_ON) || (GDBG_INFO_ON == 0)
#if defined(GDBG_INFO_ON)
#undef GDBG_INFO_ON
#endif /* defined(GDBG_INFO_ON) */
//#ifndef CEASE_ALL_GDBG
#define GDBG_INFO_ON
//#endif
#endif /* !defined(GDBG_INFO_ON) || (GDBG_INFO_ON == 0) */

#include <stddef.h>
#include <stdlib.h>
#include <math.h>

#include <3dfx.h>

#ifdef HWC_EXT_INIT
#include "hwcext.h"
#else
#include <fxpci.h>
#endif

#if macintosh
#include <h3cinitdd_mac.h>
#endif
#include <h3cinit.h>
#include <minihwc.h>
#include "hwcio.h"
#include "setmode.h"

#ifdef __WIN32__

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#include <ddraw.h>
#include "qmodes.h"
#define IS_32
#define Not_VxD
#include <minivdd.h>
#include <vmm.h>
#include <configmg.h>

extern char *opengl_regpath; /* KoolSmoky - registry path passed from grEnable */
extern FxBool is_opengl;     /* KoolSmoky - OpenGL flag */
extern FxBool is_fullscreen; /* KoolSmoky - full screen flag */

#endif

#ifdef macintosh
#include <GraphicsPrivHwc.h>
#include <hdwr_res_mgr.h>
#include <hrm_fifo.h>
#include <hrm_mode.h>
#include <hrm_mem.h>
#include <MacUtil.h>

#define GDX_SLIAA 1

static hrmGetTargetBoardInfoExtPtr _hrmGetTargetBoardInfo;

extern hrmGetVersionInfoPtr _hrmGetVersionInfo;
extern hrmSetExclusiveModePtr _hrmSetExclusiveMode;
extern hrmSetVideoModePtr _hrmSetVideoMode;
extern hrmReleaseExclusiveModePtr _hrmReleaseExclusiveMode;    
extern hrmAllocateBlockPtr _hrmAllocateBlock;
extern hrmFreeBlockPtr _hrmFreeBlock;
extern hrmAllocWinContextPtr _hrmAllocWinContext;
extern hrmFreeWinContextPtr _hrmFreeWinContext;
extern hrmExecuteWinFifoPtr _hrmExecuteWinFifo;
extern hrmGetDeviceConfigPtr _hrmGetDeviceConfig;
extern hrmGetAGPInfoPtr _hrmGetAGPInfo;
extern hrmWriteConfigRegisterPtr _hrmWriteConfigRegister;
extern hrmReadConfigRegisterPtr _hrmReadConfigRegister;
extern hrmGetSlaveRegsPtr _hrmGetSlaveRegs;
extern hrmSLIAAPtr _hrmSLIAA;

#if GLIDE3
#include <mac_sfc.h>
#endif
#endif

#define HWCEXT_PROTOCOL(mon) (hInfo.boardInfo[mon].hwcProtocol==-1)
#define HWCEXT_ESCAPE(mon) hInfo.boardInfo[mon].hwcEscape

/*
 * AJB HACK
 */
//#define HWC_MINIVDD_HACK 1

#if defined(HWC_MINIVDD_HACK) && defined(HWC_EXT_INIT)
#define MINIVDDNAME "\\\\.\\H5VDD"
/*
 * This mess was snipped from win9x\dx\inc\h3g.h
 *
 * hence the '//' comments
 */

#define SLI_AA_ENABLE (6)
#define SLI_AA_DISABLE (7)

typedef struct dioc_data {
   DWORD dwDevNode;           // Device that we are interested in
   DWORD dwSpare;             // Spare DWORD currently used to pass IMASK
   } DIOC_DATA, * PDIOC_DATA;

typedef struct _sli_aa_chipinfo {
   DWORD dwChips;       // dwChips: Number of chips in multi-chip configuration (1-4)
   DWORD dwsliEn;       // dwsliEn: Sli is to be enabled (0,1)
   DWORD dwaaEn;        // dwaaEn: Anti-aliasing is to be enabled (0,1)
   DWORD dwaaSampleHigh;  // dwaaSampleHigh: 0->Enable 2-sample AA, 1->Enable 4-sample AA
   DWORD dwsliAaAnalog;   // dwsliAaAnalog: 0->Enable digital SLI/AA, 1->Enable analog Sli/AA
   DWORD dwsli_nlines;    // dwsli_nLines: Number of lines owned by each chip in SLI (2-128)
   DWORD dwCfgSwapAlgorithm; // Swap Buffer Algorithm
}  SLI_AA_CHIPINFO, * PSLI_AA_CHIPINFO;

typedef struct _sli_aa_meminfo {
   DWORD dwTotalMemory;       // In MB
   DWORD dwTileMark;          // In MB
   DWORD dwTileCmpMark;       // In MB
   DWORD dwaaSecondaryColorBufBegin;    // In MB
   DWORD dwaaSecondaryDepthBufBegin;    // In MB
   DWORD dwaaSecondaryDepthBufEnd;    // In MB
   DWORD dwBpp;                        // Bits Per Pixel
}  SLI_AA_MEMINFO, * PSLI_AA_MEMINFO; 

typedef struct sli_aa_request {
   SLI_AA_CHIPINFO ChipInfo;
   SLI_AA_MEMINFO MemInfo;
} SLI_AA_REQUEST, * PSLI_AA_REQUEST;
#endif

#ifdef GETENV
#undef GETENV
#endif

//#if defined(HWC_EXT_INIT)
#define GETENV(a, b) hwcGetenvEx(a, b)
//#else
//#define GETENV(a, b) hwcGetenv(a)
//#endif

#define MAXFIFOSIZE        0x40000
#define MAXFIFOSIZE_16MB   0xff000

#define HWC_LFB_STRIDE     0x2000UL
#define HWC_RAW_LFB_STRIDE SST_RAW_LFB_ADDR_STRIDE_8K

hwcInfo hInfo;

#define MAX_ERROR_SIZE 1024
static char errorString[MAX_ERROR_SIZE];
static FxU32 fenceVar;

FxU32 hwc_errncpy(char *dst,const char *src);

/* like strncpy, for the error string except it always null terminates */
FxU32 hwc_errncpy(char *dst,const char *src)
{
   FxU32 i,size=MAX_ERROR_SIZE;

   if (size==0)
      return 0;

   for(i=0;i<size;i++)
   {
      *dst++=*src++;
      if (src[-1]==0)
         return i;
   }
   dst[-1]=0;

   return (i-1);
}

#if defined(__WATCOMC__)
/*
 *  P6 Fence
 * 
 *  Here's the stuff to do P6 Fencing.  This is required for the
 *  certain things on the P6
 *
 * dpc - 21 may 1997 - FixMe!
 * This was yoinked from sst1/include/sst1init.h, and should be
 * merged back into something if we decide that we need it later.
 */
void 
p6Fence(void);
#pragma aux p6Fence = \
"xchg eax, fenceVar" \
modify [eax];


#define P6FENCE p6Fence()
#elif defined(__MSC__)
#define P6FENCE {_asm xchg eax, fenceVar}
#elif defined(__POWERPC__) && defined(__MWERKS__)
#define P6FENCE __sync()
#else
#error "P6 Fencing in-line assembler code needs to be added for this compiler"
#endif /* Compiler specific fence commands */

#if defined(HWC_EXT_INIT) || defined(HWC_GDX_INIT)
static FxU32 dummyContextDWORD;
static FxU32 **dummydata;
#endif

#ifdef HWC_DXDRVR
static hwcSwapType swapType;
#endif

static void
lostContext(void);

static FxU32
pow2Round(FxU32 val, FxU32 roundVal);

static FxU32
hwcBufferLfbAddr(const hwcBoardInfo *bInfo, FxU32 physAddress);

static FxU32
calcBufferSize(hwcBoardInfo* bInfo, FxU32 xres, FxU32 yres, FxBool tiled);  

static FxU32
calcBufferStride(hwcBoardInfo* bInfo, FxU32 xres, FxBool tiles);

static FxU32
calcBufferSizeInTiles(hwcBoardInfo* bInfo, FxU32 xres, FxU32 yres);

static FxU32
calcBufferHeightInTiles(hwcBoardInfo* bInfo, FxU32 yres);

static FxBool resolutionSupported[HWC_MAX_BOARDS][0x20][0x10];

#ifdef IS_ALT_TAB
static WNDPROC patchedWndproc = NULL;
static HWND patchedHwnd = NULL;

LRESULT CALLBACK QueryContextProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
#define FN_NAME "QueryContextProc"
  GDBG_INFO(80, FN_NAME ":  Called!\n");
  switch(uMsg) {
  case WM_ACTIVATEAPP:
    GDBG_INFO(80, FN_NAME ":  WM_ACTIVATEAPP\n");
    if (wParam == 0)
    {
      GDBG_INFO(80, FN_NAME ":  WM_ACTIVATEAPP deactivated\n");
      if(is_fullscreen == FXTRUE) {
        GDBG_INFO(80, FN_NAME ":  WM_ACTIVATEAPP resetvideo\n");
        dummyContextDWORD = FXTRUE;
        is_fullscreen = FXFALSE;
        resetVideo();
        ShowWindow(patchedHwnd, SW_MINIMIZE);
//        ShowWindow(hWnd, SW_MINIMIZE);
      }
    }
    break;
#if 0
  case WM_ACTIVATE:
    if(LOWORD(wParam) == WA_INACTIVE) {
      GDBG_INFO(80, FN_NAME ":  WM_ACTIVATEAPP\n");
      dummyContextDWORD = FXTRUE;
      if(is_fullscreen == FXTRUE /*&& is_opengl != FXTRUE*/) {
        GDBG_INFO(80, FN_NAME ":  WM_ACTIVATEAPP resetvideo\n");
        resetVideo();
        ShowWindow(patchedHwnd, SW_MINIMIZE);
      }
    }
    break;
  case WM_SYSCOMMAND:
    if(wParam == SC_MINIMIZE) {
      GDBG_INFO(80, FN_NAME ":  WM_ACTIVATEAPP\n");
      dummyContextDWORD = FXTRUE;
      if(is_fullscreen == FXTRUE /*&& is_opengl != FXTRUE*/) {
        GDBG_INFO(80, FN_NAME ":  WM_ACTIVATEAPP resetvideo\n");
        resetVideo();
        ShowWindow(patchedHwnd, SW_MINIMIZE);
      }
    }
    break;
#endif
  default:
    break;
  }
  return CallWindowProc(patchedWndproc, hWnd, uMsg, wParam, lParam);
  
#undef FN_NAME
}
#endif /* IS_ALT_TAB */

/* 
** DOS-only stuff for multi-chip boards.
*/
extern void 
mapSlavePhysical(hwcBoardInfo *bInfo, FxU32 chipNum);

extern void
initSlave(hwcBoardInfo *bInfo, FxU32 chipNum);
        
/*
**  Function Prototypes
*/

static hwcBoardInfo *curBI = NULL;

#ifdef HWC_EXT_INIT

#if 0
#if !defined(HMONITOR_DECLARED) // AJB- Make def compatible w/ vc6 headers

typedef void *HMONITOR;
//DECLARE_HANDLE(HMONITOR);

#define HMONITOR_DECLARED
#endif
#endif
typedef BOOL (CALLBACK* MONITORENUMPROC)(HMONITOR, HDC, LPRECT, LPARAM);
typedef WINUSERAPI BOOL WINAPI
EnumDisplayMonitors_func( HDC             hdc,
                          LPCRECT         lprcClip,
                          MONITORENUMPROC lpfnEnum,
                          LPARAM          dwData);

typedef struct {
  HDC dc;
  HMONITOR mon;
  char RegPath[255];
  char DeviceName[32];
//  char DeviceKey[128];
} DevEnumRec;
static int num_monitor = 0;

static char *
getRegPathEx() 
{
  char *retVal = NULL;
  OSVERSIONINFO ovi;
  
  ovi.dwOSVersionInfoSize = sizeof ( ovi );
  GetVersionEx ( &ovi );
  if (ovi.dwPlatformId == VER_PLATFORM_WIN32_NT) {
    HKEY hKey;
    DWORD type ;
    static char strval[255];
    DWORD szData = sizeof(strval) ;

    GDBG_INFO(80, "OS == WNT\n");

    /* Go fishing for the registry path on WinNT4 */
    if (RegOpenKey(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\VIDEO", &hKey) == ERROR_SUCCESS)
    {
      if (RegQueryValueEx(hKey, "\\Device\\Video0", NULL, &type, strval, &szData) ==
          ERROR_SUCCESS)
      {
        if (type != REG_SZ)
        {
          /* It is hardcoded on NT via Display Control code. see:
           * $/devel/swtools/bansheecp2 */
          retVal = "SYSTEM\\CurrentControlSet\\Services\\3Dfx\\Device0";
        }
         else
         {
          retVal = (char*)((int)strval + strlen("\\REGISTRY\\Machine\\")) ;
         }
      }
       else
         retVal = "SYSTEM\\CurrentControlSet\\Services\\3Dfx\\Device0";

      RegCloseKey(hKey);
    }
  } else {
    QDEVNODE QDevNode;
    QIN Qin;
    int status;

    GDBG_INFO(80, "OS == W9X\n");

    Qin.dwSubFunc = QUERYDEVNODE;
    {
      HDC curDC = GetDC(NULL);

      status = ExtEscape ( (HDC)curDC, QUERYESCMODE, 
                           sizeof(Qin), (LPCSTR)&Qin, 
                           sizeof(QDevNode), (LPSTR)&QDevNode );
      ReleaseDC(NULL, curDC);
    }

    if ( status > 0 ) {
      static char regPath[255];

      CM_Get_DevNode_Key( QDevNode.dwDevNode, NULL, 
                          &regPath, sizeof(regPath), 
                          CM_REGISTRY_SOFTWARE );

      retVal = regPath;
    }
  }

  return retVal;
} /* getRegPathEx */

static BOOL CALLBACK 
monitorEnum( HMONITOR handle, HDC dc, LPRECT rect, LPARAM param ) 
{
  BOOL rv = TRUE;
  hwcExtRequest_t ctxReq;
  hwcExtResult_t ctxRes;

  hInfo.boardInfo[num_monitor].hwcProtocol = -1;
  ctxReq.which = HWCEXT_GETDEVICECONFIG;
  ctxReq.optData.deviceConfigReq.dc = dc;
  ctxReq.optData.deviceConfigReq.devNo = num_monitor;
  GDBG_INFO(80, "monitorEnum:  ExtEscape:HWCEXT_GETDEVICECONFIG\n");

  /*
   * AJB- Figure out which escape code the display driver is using.
   *      This will allow yo-yos like me to use a new Glide DLL on
   *      an old (H3, for example) display driver.
   */
  /*if (ExtEscape((HDC)dc, EXT_HWC_WXP, sizeof(ctxReq), (LPSTR) &ctxReq, sizeof(ctxRes), (LPSTR) &ctxRes))
    HWCEXT_ESCAPE(num_monitor) = EXT_HWC_WXP ;
  else*/ if (ExtEscape((HDC)dc, EXT_HWC, sizeof(ctxReq), (LPSTR) &ctxReq, sizeof(ctxRes), (LPSTR) &ctxRes)) 
    HWCEXT_ESCAPE(num_monitor) = EXT_HWC ;
  else if (ExtEscape((HDC)dc, EXT_HWC_OLD, sizeof(ctxReq), (LPSTR) &ctxReq, sizeof(ctxRes), (LPSTR) &ctxRes)) 
    HWCEXT_ESCAPE(num_monitor) = EXT_HWC_OLD ;
  else
    HWCEXT_ESCAPE(num_monitor) = 0 ; /* if we don't have a display driver, we're hosed */

  GDBG_INFO(80, "HWCEXT_ESCAPE(num_monitor) = 0x%x\n", HWCEXT_ESCAPE(num_monitor));
 
  if (HWCEXT_ESCAPE(num_monitor))
  { /**/
    FxI32 vendorID;

    GDBG_INFO(80, "HWCEXT_ESCAPE: passed\n");
   
    vendorID = ctxRes.optData.deviceConfigRes.vendorID;

    if ( vendorID == 0x121a ) {
      DevEnumRec* 
        data = (DevEnumRec*)param;
      LPCSTR
        drvName = "DISPLAY",
        devName = NULL;

      OSVERSIONINFO ovi;
      ovi.dwOSVersionInfoSize = sizeof ( ovi );
      GetVersionEx ( &ovi );

      /* If we're on a multi-mon capable system then we may have
       * different display type devices so we have to get the device
       * name explicitly for the CreateDC call.
       */
      if (handle != NULL) {

        HMODULE user32;
        user32 = NULL;
        user32 = GetModuleHandle("user32");
        
        if( user32 != NULL) {
#define CCHDEVICENAME 32
          typedef struct {
            DWORD cbSize;
            RECT  rcMonitor;
            RECT  rcWork;
            DWORD dwFlags;
            TCHAR szDevice[CCHDEVICENAME];
          } MONITORINFOEX, *LPMONITORINFOEX;
          typedef BOOL (CALLBACK* GetMonitorInfoProc)(HMONITOR, LPMONITORINFOEX);
          GetMonitorInfoProc
            monitorInfoProc = (GetMonitorInfoProc)GetProcAddress(user32, "GetMonitorInfoA");

          MONITORINFOEX monInfo;
          
          if (monitorInfoProc != NULL) {
            monInfo.cbSize = sizeof(monInfo);
            if ((*monitorInfoProc)(handle, &monInfo)) {
              devName = monInfo.szDevice;
              drvName = NULL;
              GDBG_INFO(80, "DEVICE: %s\n", devName);
            }
            
          }
        
        } else {
          strcpy(data[num_monitor].RegPath, getRegPathEx());
        }
      }

      /* XXX AJB MAYHEM XXX
       * (Try) to get the devnode of this device
       * so we can use DevIoctls to talk to the
       * minivdd.
       */
      {
        QDEVNODE QDevNode;
        QIN Qin;

        Qin.dwSubFunc = QUERYDEVNODE;

        if (ExtEscape((HDC)dc, QUERYESCMODE, sizeof(Qin), (LPSTR) &Qin, sizeof(QDevNode), (LPSTR) &QDevNode)) 
          hInfo.boardInfo[num_monitor].devNode = QDevNode.dwDevNode ;
        else
          hInfo.boardInfo[num_monitor].devNode = 0 ;
      
      }
    
      /* Make a private 'copy' of the dc so that we're not affected by
       * other people dorking with dc's etc. 
       *
       * FixMe: Is there a better way to do this? I did not see a
       * CopyDC or anything like that.  
       */
      /* KoolSmoky - Microsoft states this is the right way */
      dc = CreateDC(drvName,
                    devName,
                    NULL,
                    NULL);
    
      data[num_monitor].dc = dc;
      data[num_monitor].mon = handle;

      num_monitor++;
      rv = (num_monitor < HWC_MAX_BOARDS);
    
    }
  } else {
     GDBG_INFO(80, "HWCEXT_ESCAPE: not passed\n");
  }

  return rv;
} /* monitorEnum */

/*
** Use the active window display as the first sst device
*/
static BOOL CALLBACK 
displayMonitor( HMONITOR handle, HDC dc, LPRECT rect, LPARAM param ) 
{
  BOOL rv = TRUE;
  hwcExtRequest_t ctxReq;
  hwcExtResult_t  ctxRes;

  ctxReq.which = HWCEXT_GETDEVICECONFIG;
  ctxReq.optData.deviceConfigReq.dc = dc;
  ctxReq.optData.deviceConfigReq.devNo = 0;
  GDBG_INFO(80, "displayMonitor:  ExtEscape:HWCEXT_GETDEVICECONFIG\n");

  if (  ExtEscape((HDC)dc, HWCEXT_ESCAPE(0), sizeof(ctxReq), (LPSTR) &ctxReq, sizeof(ctxRes), (LPSTR) &ctxRes) ) { /**/
    FxI32 vendorID;

    vendorID = ctxRes.optData.deviceConfigRes.vendorID;

    if ( ctxRes.optData.deviceConfigRes.vendorID == 0x121a ) {
      DevEnumRec*
        data   = (DevEnumRec*) param;
      FxI32 
        i;

      for (i = 0; i < num_monitor; i++) {
        HDC      *return_dc   = ( HDC* ) data + (2*i);
        HMONITOR *return_hmon = ( HMONITOR* ) data + (2*i+1);

        if ((data[i].dc == dc) && (data[i].mon == handle)) {
          data[i].dc = data[0].dc;
          data[i].mon = data[0].mon;

          data[0].dc = dc;
          data[0].mon = handle;

          break;
        }
      }
      rv = FALSE;
    }
  } 
  return rv;
} /* displayMonitor */
#endif

hwcInfo *
hwcInit(FxU32 vID, FxU32 dID)
{
#define FN_NAME "hwcInit"
#ifdef HWC_EXT_INIT
  {
    DevEnumRec
      data[HWC_MAX_BOARDS*2];
    int monitor;
    int forceprimary;

    OSVERSIONINFO ovi;

    ovi.dwOSVersionInfoSize = sizeof ( ovi );
    GetVersionEx ( &ovi );

    GDBG_INFO(80, "%s\n", FN_NAME);
    errorString[0] = '\0';

    /* find glide compatible devices */
    GDBG_INFO(80, "%s:  Finding Glide compatible devices\n", FN_NAME);
    {
      /* Grab the DC of the Desktop. */
//      HDC hdc = GetDC(NULL);
      HMODULE user32 = GetModuleHandle( "user32.dll" );

      for (monitor = 0; monitor < HWC_MAX_BOARDS; monitor++) {
        data[monitor].dc  = NULL;
        data[monitor].mon = NULL;
        data[monitor].RegPath[0] = '\0';
        data[monitor].DeviceName[0] = '\0';
//        data[monitor].DeviceKey[0] = '\0';
      }

      if ( user32 != NULL ) {
//        EnumDisplayMonitors_func*
//              enumDisplayMonitors = (void*)GetProcAddress( user32, "EnumDisplayMonitors" );
        
#define CCHDEVICENAME 32
#define CCHFORMNAME 32
        typedef struct {
          DWORD  cb;
          BYTE   DeviceName[32];
          BYTE   DeviceString[128];
          DWORD  StateFlags;
          CHAR   DeviceID[128];
          CHAR   DeviceKey[128];
        } DISPLAY_DEVICE, *PDISPLAY_DEVICE;
        typedef BOOL (CALLBACK* EnumDisplayDevicesProc)(LPCTSTR, DWORD, PDISPLAY_DEVICE, DWORD);
        EnumDisplayDevicesProc
          enumDisplayDevicesProc = (EnumDisplayDevicesProc)GetProcAddress(user32, "EnumDisplayDevicesA");
 
        typedef struct { 
          BCHAR  dmDeviceName[CCHDEVICENAME]; 
          WORD   dmSpecVersion; 
          WORD   dmDriverVersion; 
          WORD   dmSize; 
          WORD   dmDriverExtra; 
          DWORD  dmFields; 
          union {
            struct {
              short dmOrientation;
              short dmPaperSize;
              short dmPaperLength;
              short dmPaperWidth;
            };
            POINTL dmPosition;
          };
          short  dmScale; 
          short  dmCopies; 
          short  dmDefaultSource; 
          short  dmPrintQuality; 
          short  dmColor; 
          short  dmDuplex; 
          short  dmYResolution; 
          short  dmTTOption; 
          short  dmCollate; 
          BCHAR  dmFormName[CCHFORMNAME]; 
          WORD  dmLogPixels; 
          DWORD  dmBitsPerPel; 
          DWORD  dmPelsWidth; 
          DWORD  dmPelsHeight; 
          union {
            DWORD  dmDisplayFlags; 
            DWORD  dmNup;
          };
          DWORD  dmDisplayFrequency; 
          DWORD  dmICMMethod;
          DWORD  dmICMIntent;
          DWORD  dmMediaType;
          DWORD  dmDitherType;
          DWORD  dmReserved1;
          DWORD  dmReserved2;
          DWORD  dmPanningWidth;
          DWORD  dmPanningHeight;
        } DEVMODEEX, *LPDEVMODEEX;
        typedef BOOL (CALLBACK* EnumDisplaySettingsProc)(LPCTSTR, DWORD, LPDEVMODEEX);
        EnumDisplaySettingsProc
          enumDisplaySettingsProc = (EnumDisplaySettingsProc)GetProcAddress(user32, "EnumDisplaySettingsA");
/*
        typedef struct {
          DWORD cbSize;
          RECT  rcMonitor;
          RECT  rcWork;
          DWORD dwFlags;
          TCHAR szDevice[CCHDEVICENAME];
        } MONITORINFOEX, *LPMONITORINFOEX;
        typedef BOOL (CALLBACK* GetMonitorInfoProc)(HMONITOR, LPMONITORINFOEX);
        GetMonitorInfoProc
          monitorInfoProc = (GetMonitorInfoProc)GetProcAddress(user32, "GetMonitorInfoA");
*/
//        typedef HDC CreateDC_func( LPCTSTR, LPCTSTR, LPCTSTR, LPDEVMODE);
//        CreateDC_func* createDC = (void*)GetProcAddress( user32, "CreateDC" );

        GDBG_INFO(80, "hwcInit: user32 passed.\n");
        
        if( enumDisplayDevicesProc != NULL ) { /* for win98/Me/win2k/xp, multi-monitor capable */
//          MONITORINFOEX monInfo;
          RECT rcMonitor;
          DISPLAY_DEVICE DispDev;
          DEVMODEEX devModeEx;
          int nDeviceIndex = 0;
          HMONITOR handle;
          HDC hdc = NULL;

          GDBG_INFO(80, "hwcInit: enumDisplayDevices passed.\n");
          
          ZeroMemory(&DispDev, sizeof(DISPLAY_DEVICE));
          DispDev.cb = sizeof(DISPLAY_DEVICE);

          num_monitor = 0;

          while(enumDisplayDevicesProc(NULL, nDeviceIndex, &DispDev, 0)){

            ZeroMemory(&devModeEx, sizeof(DEVMODEEX));
            devModeEx.dmSize = sizeof(DEVMODEEX);
            devModeEx.dmDriverExtra = 0;

            data[num_monitor].dc  = NULL;
            data[num_monitor].mon = NULL;
            data[num_monitor].RegPath[0] = '\0';

            if( DispDev.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP ) {
              
              if( enumDisplaySettingsProc != NULL ) {

                GDBG_INFO(80, "hwcInit: enumDisplaySettings passed.\n");
                
                enumDisplaySettingsProc( DispDev.DeviceName, ENUM_CURRENT_SETTINGS, &devModeEx );
                
                if( DispDev.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE ) {
                  /* For some reason devmode.dmPosition is not always (0, 0)
                   * for the primary display, so force it.
                   */
                  rcMonitor.left = 0;
                  rcMonitor.top = 0;
                } else {
                  rcMonitor.left = devModeEx.dmPosition.x;
                  rcMonitor.top = devModeEx.dmPosition.y;
                }
                
                rcMonitor.right = rcMonitor.left + devModeEx.dmPelsWidth;
                rcMonitor.bottom = rcMonitor.top + devModeEx.dmPelsHeight;
                handle = MonitorFromRect( &rcMonitor, MONITOR_DEFAULTTONULL );
                data[num_monitor].mon = handle;
                GDBG_INFO(80, "rcmonitor left %d top %d right %d bottom %d hmonitor%d\n", rcMonitor.left, rcMonitor.top, rcMonitor.right, rcMonitor.bottom, handle);
                
                hdc = CreateDC(NULL, DispDev.DeviceName, NULL, NULL);
                data[num_monitor].dc = hdc;
                GDBG_INFO(80, "DeviceName: %s DeviceString: %s\n", DispDev.DeviceName, DispDev.DeviceString);
                
                
                {
                  hwcExtRequest_t ctxReq;
                  hwcExtResult_t ctxRes;
                  hInfo.boardInfo[num_monitor].hwcProtocol = -1;
                  ctxReq.which = HWCEXT_GETDEVICECONFIG;
                  ctxReq.optData.deviceConfigReq.dc = hdc;
                  ctxReq.optData.deviceConfigReq.devNo = num_monitor;
                  
                  /*
                   * AJB- Figure out which escape code the display driver is using.
                   *      This will allow yo-yos like me to use a new Glide DLL on
                   *      an old (H3, for example) display driver.
                   */
                  /*if (ExtEscape((HDC)hdc, EXT_HWC_WXP, sizeof(ctxReq), (LPSTR) &ctxReq, sizeof(ctxRes), (LPSTR) &ctxRes))
                    HWCEXT_ESCAPE(num_monitor) = EXT_HWC_WXP ;
                  else*/ if (ExtEscape((HDC)hdc, EXT_HWC, sizeof(ctxReq), (LPSTR) &ctxReq, sizeof(ctxRes), (LPSTR) &ctxRes)) 
                    HWCEXT_ESCAPE(num_monitor) = EXT_HWC ;
                  else if (ExtEscape((HDC)hdc, EXT_HWC_OLD, sizeof(ctxReq), (LPSTR) &ctxReq, sizeof(ctxRes), (LPSTR) &ctxRes)) 
                    HWCEXT_ESCAPE(num_monitor) = EXT_HWC_OLD ;
                  else
                    HWCEXT_ESCAPE(num_monitor) = 0 ; /* if we don't have a display driver, we're hosed */

                  GDBG_INFO(80, "HWCEXT_ESCAPE(num_monitor) = 0x%x\n", HWCEXT_ESCAPE(num_monitor));
                  
                  if (HWCEXT_ESCAPE(num_monitor)) { /**/
                    FxI32 vendorID;
                    
                    GDBG_INFO(80, "HWCEXT_ESCAPE: passed\n");
                    
                    vendorID = ctxRes.optData.deviceConfigRes.vendorID;
                    
                    if ( vendorID == 0x121a ) {
                      
                      GDBG_INFO(80, "%s:  multi-monitor capable OS ( NT2K/XP/W98/ME )\n", FN_NAME);

                      strcpy(data[num_monitor].DeviceName, DispDev.DeviceName);

                      switch(ovi.dwPlatformId) {
                      case VER_PLATFORM_WIN32_NT:
                        {
                          // win2k/xp
                          int pos;
                          char *pdest;
                          char regpath_temp[255];
                          int i;
                          
                          pdest = strstr(DispDev.DeviceKey, "\\Service");
                          pos = pdest - DispDev.DeviceKey + 1;
                          for(i = pos; i <= sizeof(DispDev.DeviceKey); i++) {
                            regpath_temp[i - pos] = DispDev.DeviceKey[i - 1];
                          }
                          strcpy(data[num_monitor].RegPath, "SYSTEM\\CurrentControlSet");
                          strcat(data[num_monitor].RegPath, regpath_temp);
                        }
                        break;
                      case VER_PLATFORM_WIN32_WINDOWS:
                        {
                          if(ovi.dwMinorVersion < 10) {
                            // win95
                            strcpy(data[num_monitor].RegPath, getRegPathEx());
                          } else {
                            // win98/me
                            strcpy(data[num_monitor].RegPath, DispDev.DeviceKey);
                          }
                        }
                        break;
                      }
                        
                      GDBG_INFO(80, "DeviceKey: %s\n", data[num_monitor].RegPath);
                      
                      /* XXX AJB MAYHEM XXX
                       * (Try) to get the devnode of this device
                       * so we can use DevIoctls to talk to the
                       * minivdd.
                       */
                      {
                        QDEVNODE QDevNode;
                        QIN Qin;
                        
                        Qin.dwSubFunc = QUERYDEVNODE;
                        
                        if (ExtEscape((HDC)hdc, QUERYESCMODE, sizeof(Qin), (LPSTR) &Qin, sizeof(QDevNode), (LPSTR) &QDevNode)) 
                          hInfo.boardInfo[num_monitor].devNode = QDevNode.dwDevNode ;
                        else
                          hInfo.boardInfo[num_monitor].devNode = 0 ;

                        GDBG_INFO(80, "DevNode: %s\n", hInfo.boardInfo[num_monitor].devNode);
                        
                      }
                      
                      num_monitor++;
                      
                      if(!(num_monitor < HWC_MAX_BOARDS)) break;
                      
                    } else DeleteDC(hdc);
                  } else {
                    DeleteDC(hdc);
                    GDBG_INFO(80, "HWCEXT_ESCAPE: not passed\n");
                    
                  }
                  
                }
                
              }
              
            }

            nDeviceIndex++;
          }

#if 0
          {
            HWND curWindow = GetActiveWindow();
            /*
             ** use the active window display (if there is one yet
             ** associated w/ the current thread) as sst 0 
             */
            if (curWindow != NULL) {
              HDC curWindowDC = GetDC(curWindow);
              
              //if (curWindowDC != NULL) {
                EnumDisplayMonitors( curWindowDC, 0, displayMonitor, (LPARAM)data );
                ReleaseDC(curWindow, curWindowDC);
              //}
            }
          }
#endif
          
        } else { /* for win95/nt4, assume we have one board */
          /* grab DC of desktop */
          HDC hdc = GetDC(NULL);
          
          GDBG_INFO(80, "%s:  single-monitor OS ( NT4/W95 )\n", FN_NAME);
          
          monitorEnum(NULL, hdc, NULL, (LPARAM)&data);
          ReleaseDC(NULL, hdc);
        }
      }

    }

    /* KoolSmoky - temporary hack to enable secondary and beyond sst devices
    ** FX_GLIDE_SSTSELECT=1 to force device as sst0
    */
    forceprimary = -1;
    hInfo.nBoards = 0;
    for (monitor = 0; monitor < num_monitor; monitor++) {
      if (GETENV("FX_GLIDE_SSTSELECT", data[monitor].RegPath)) {
        if(atoi(GETENV("FX_GLIDE_SSTSELECT", data[monitor].RegPath)) == 1) {
          forceprimary =  monitor;
        }
      }
    }
    
    hInfo.nBoards = 0;
    for (monitor = 0; monitor < num_monitor; monitor++) {
      hwcExtRequest_t ctxReq;
      hwcExtResult_t  ctxRes;
      HDC hdc;
      HMONITOR hmon;
      int status;

      if(forceprimary != -1) {
        hdc = data[forceprimary].dc;
        hmon = data[forceprimary].mon;
        strcpy(hInfo.boardInfo[monitor].RegPath, data[forceprimary].RegPath);
        strcpy(hInfo.boardInfo[monitor].DeviceName, data[forceprimary].DeviceName);
      } else {
        hdc = data[monitor].dc;
        hmon = data[monitor].mon;
        strcpy(hInfo.boardInfo[monitor].RegPath, data[monitor].RegPath);
        strcpy(hInfo.boardInfo[monitor].DeviceName, data[monitor].DeviceName);
      }

      hInfo.nBoards++;
      hInfo.boardInfo[monitor].boardNum     = monitor;
      hInfo.boardInfo[monitor].hdc          = hdc;
      hInfo.boardInfo[monitor].hMon         = hmon;

      GDBG_INFO(80,"hInfo : [%d] boardNum: %d\n", monitor, hInfo.boardInfo[monitor].boardNum);
      GDBG_INFO(80,"hInfo : [%d] hdc: %d\n", monitor, hInfo.boardInfo[monitor].hdc);
      GDBG_INFO(80,"hInfo : [%d] hmon: %d\n", monitor, hInfo.boardInfo[monitor].hMon);
      GDBG_INFO(80,"hInfo : [%d] RegPath: %s\n", monitor, hInfo.boardInfo[monitor].RegPath);
      GDBG_INFO(80,"hInfo : [%d] DeviceName: %s\n", monitor, hInfo.boardInfo[monitor].DeviceName);
/*      hInfo.boardInfo[monitor].extContextID = ctxRes.Ext.optData.allocContextRes.contextID; */

      if (ovi.dwPlatformId == VER_PLATFORM_WIN32_NT) {
        hInfo.boardInfo[monitor].osNT = FXTRUE;
        if ( ovi.dwMajorVersion >= 5 && ovi.dwMinorVersion >= 1) {
          hInfo.boardInfo[monitor].osNT51 = FXTRUE;  /* KoolSmoky- WinXP flag */
          GDBG_INFO(80, FN_NAME ":  OS:  NT5.1\n");
        } else {
          hInfo.boardInfo[monitor].osNT51 = FXFALSE;  /* KoolSmoky- WinXP flag */
          GDBG_INFO(80, FN_NAME ":  OS:  NT\n");
        }
      } else {
        hInfo.boardInfo[monitor].osNT = FXFALSE;
        hInfo.boardInfo[monitor].osNT51 = FXFALSE;  /* KoolSmoky- WinXP flag */
        GDBG_INFO(80, FN_NAME ":  OS:  9X\n");
      }

      ctxReq.which = HWCEXT_GETDEVICECONFIG;
      ctxReq.optData.deviceConfigReq.dc = hdc;
      ctxReq.optData.deviceConfigReq.devNo = monitor;
      GDBG_INFO(80, FN_NAME ":  ExtEscape:HWCEXT_GETDEVICECONFIG.\n");

//      status = ExtEscape(hdc, HWCEXT_ESCAPE(0), /**/
//                         sizeof(ctxReq), (void *) &ctxReq,
//                         sizeof(ctxRes), (void *) &ctxRes);
      status = ExtEscape((HDC)hdc,
                         HWCEXT_ESCAPE(monitor),
                         sizeof(ctxReq), (LPSTR) &ctxReq,
                         sizeof(ctxRes), (LPSTR) &ctxRes);
      
      if(!status) return NULL;
      
      hInfo.boardInfo[monitor].pciInfo.vendorID = 
        ctxRes.optData.deviceConfigRes.vendorID;
      hInfo.boardInfo[monitor].pciInfo.deviceID = 
        ctxRes.optData.deviceConfigRes.deviceID;
      hInfo.boardInfo[monitor].devRev = 
        ctxRes.optData.deviceConfigRes.chipRev;
      hInfo.boardInfo[monitor].h3Mem = 
        (ctxRes.optData.deviceConfigRes.fbRam >> 20);
      GDBG_INFO(80, FN_NAME ":  vendorID %i deviceID %i ChipRev %i fbRam %i\n",
                hInfo.boardInfo[monitor].pciInfo.vendorID,
                hInfo.boardInfo[monitor].pciInfo.deviceID,
                hInfo.boardInfo[monitor].devRev,
                hInfo.boardInfo[monitor].h3Mem);

#ifdef FX_GLIDE_NAPALM

      if (GETENV("FX_GLIDE_DEVICEID", hInfo.boardInfo[monitor].RegPath)) {
        FxU32 deviceid = atoi(GETENV("FX_GLIDE_DEVICEID", hInfo.boardInfo[monitor].RegPath));
        hInfo.boardInfo[monitor].pciInfo.deviceID = deviceid;
      }

      GDBG_INFO(80, "devNum: %d\n", ctxRes.optData.deviceConfigRes.devNum);
      GDBG_INFO(80, "isMaster: %d\n", ctxRes.optData.deviceConfigRes.isMaster);
      GDBG_INFO(80, "numChips: %d\n", ctxRes.optData.deviceConfigRes.numChips);

      if (IS_NAPALM(hInfo.boardInfo[monitor].pciInfo.deviceID))
      {
        hInfo.boardInfo[monitor].pciInfo.devNum =
           ctxRes.optData.deviceConfigRes.devNum;
        hInfo.boardInfo[monitor].pciInfo.isMaster =
           ctxRes.optData.deviceConfigRes.isMaster;
        hInfo.boardInfo[monitor].pciInfo.numChips =
           ctxRes.optData.deviceConfigRes.numChips;
        if /*(*/(hInfo.boardInfo[monitor].pciInfo.numChips > 4)/* || 
            (hInfo.boardInfo[monitor].pciInfo.numChips < 0))*//* KoolSmoky */ {
          hInfo.boardInfo[monitor].pciInfo.numChips = 0;
        } //else {
          /* KoolSmoky - Napalm framebuffer is unified, where the framebuffer
          ** bound to each chip is effectively divided by SLI mode.
          ** (framebuffer of one chip) = (total framebuffer / SLI mode)
          ** 2 chip device can have 1-way-SLI or 2-way-SLI
          ** h3Mem is the amount of video ram dedicated for one chip.
          hInfo.boardInfo[monitor].h3Mem = 
            (ctxRes.optData.deviceConfigRes.fbRam >> 20);
          */
        //}
      }
#endif /* FX_GLIDE_NAPALM */
            
      hInfo.boardInfo[monitor].pciInfo.initialized = 1;
      hInfo.boardInfo[monitor].h3pixelSize = 2;
      hInfo.boardInfo[monitor].h3nwaySli = 1;

      if (hInfo.boardInfo[monitor].h3Mem == 8) {
        hInfo.boardInfo[monitor].pciInfo.deviceID = SST_DEVICE_ID_H3 ;
      }
      
//#ifndef GLIDE3
      hInfo.boardInfo[monitor].min_tramSize = 
        ((hInfo.boardInfo[monitor].h3Mem == 4 ) ||
         (hInfo.boardInfo[monitor].pciInfo.deviceID == SST_DEVICE_ID_H3)) ? 0x200000 : 0x400000;
#if 0 /* KoolSmoky - assert more texture memory */
      switch(hInfo.boardInfo[monitor].pciInfo.deviceID) {
      case SST_DEVICE_ID_H3: /* Banshee */
        if((hInfo.boardInfo[monitor].h3Mem < 16) {
          hInfo.boardInfo[monitor].min_tramSize = 0x400000;
        } else {
          hInfo.boardInfo[monitor].min_tramSize = 0x800000;
        }
        break;
      case SST_DEVICE_ID_H4: /* Avenger low speed */
      case SST_DEVICE_ID_H4_OEM: /* Avenger high speed */
        if((hInfo.boardInfo[monitor].h3Mem < 16) {
          hInfo.boardInfo[monitor].min_tramSize = 0x400000;
        } else {
          hInfo.boardInfo[monitor].min_tramSize = 0x800000;
        }
        break;
      default:
        if ((hInfo.boardInfo[monitor].pciInfo.deviceID >= SST_DEVICE_ID_L_AP) &&
            (hInfo.boardInfo[monitor].pciInfo.deviceID <= SST_DEVICE_ID_H_AP)) {
          hInfo.boardInfo[monitor].min_tramSize = 0x1600000;
        }else {
          hInfo.boardInfo[monitor].min_tramSize = 0x200000;
        }
        break;
      }
#endif
      
      if (GETENV("FX_GLIDE_TMU_MEMSIZE", hInfo.boardInfo[monitor].RegPath)) {
        FxU32 tmu_mem = atoi(GETENV("FX_GLIDE_TMU_MEMSIZE", hInfo.boardInfo[monitor].RegPath));
        if (tmu_mem == 1) {
          hInfo.boardInfo[monitor].min_tramSize = 0x200000;
        }
      }

      if(GETENV("FX_GLIDE_NUM_CHIPS", hInfo.boardInfo[monitor].RegPath)) {
        FxU32 numChips;
        numChips = atoi(GETENV("FX_GLIDE_NUM_CHIPS", hInfo.boardInfo[monitor].RegPath));
        /* Don't do anything stupid... */
        if(numChips < 1)
          numChips = 1;
        if(numChips <= hInfo.boardInfo[monitor].pciInfo.numChips) {
          hInfo.boardInfo[monitor].pciInfo.numChips = numChips;
        }
      }

      if (GETENV("FX_GLIDE_FBRAM", hInfo.boardInfo[monitor].RegPath)) {
        hInfo.boardInfo[monitor].h3Mem = atoi(GETENV("FX_GLIDE_FBRAM", hInfo.boardInfo[monitor].RegPath));      
      }

      checkResolutions((int *) resolutionSupported[monitor], 
                       (FxU32) sizeof(resolutionSupported[0][0]) / sizeof(FxBool),
                       (void *) hInfo.boardInfo[monitor].hMon,
                       hInfo.boardInfo[monitor].h3pixelSize,
                       (char *)hInfo.boardInfo[monitor].DeviceName);
    }
  }
#elif defined(HWC_GDX_INIT)
   {
      FxU32 i, j = 0, numTargets;
    
      /* Grab hrm extensions we need. */
      _hrmGetVersionInfo = hrmGetExtension("hrmGetVersionInfo");
      _hrmSetExclusiveMode = hrmGetExtension("hrmSetExclusiveMode");
      _hrmSetVideoMode = hrmGetExtension("hrmSetVideoMode");
      _hrmReleaseExclusiveMode = hrmGetExtension("hrmReleaseExclusiveMode");
      _hrmGetTargetBoardInfo = hrmGetExtension("hrmGetTargetBoardInfoExt");
      _hrmAllocateBlock = hrmGetExtension("hrmAllocateBlock");
      _hrmFreeBlock = hrmGetExtension("hrmFreeBlock");
      _hrmAllocWinContext = hrmGetExtension("hrmAllocWinContext");
      _hrmFreeWinContext = hrmGetExtension("hrmFreeWinContext");
      _hrmExecuteWinFifo = hrmGetExtension("hrmExecuteWinFifo");
      _hrmGetDeviceConfig = hrmGetExtension("hrmGetDeviceConfig");
      _hrmGetAGPInfo = hrmGetExtension("hrmGetAGPInfo");
      _hrmWriteConfigRegister = hrmGetExtension("hrmWriteConfigRegister");
      _hrmReadConfigRegister = hrmGetExtension("hrmReadConfigRegister");
      _hrmGetSlaveRegs = hrmGetExtension("hrmGetSlaveRegs");
      _hrmSLIAA = hrmGetExtension("hrmSLIAA");
      
      /* Have to bail out now if these aren't available. */
      if(!(_hrmSetExclusiveMode && _hrmSetVideoMode && 
         _hrmReleaseExclusiveMode && _hrmGetTargetBoardInfo && _hrmGetVersionInfo))
         return NULL;
    
      /* safety net: no exec if not compatible */
      {
         hrmVersionInfo_t        theVersion;
         
         _hrmGetVersionInfo( &theVersion );
         if ( theVersion.major >= 1 && theVersion.minor >= 6 )
         {
            numTargets = hrmGetNumTargets();
         }
         else
         {
            hwc_errncpy(errorString, "Current version of 3dfx Hardware Resource Manager is not compatible with this version of Glide\n");
            GDBG_INFO(80, "%s:  ERROR... Current version of 3dfx Hardware Resource Manager is not compatible\n", FN_NAME);
            numTargets = 0;
         }
      }
    
      for(i = 0; i < numTargets; i++)
      {
         hrmBoard_t *board;
         hrmBoardInfo_t boardInfo;
      
         board = hrmGetTargetAtIndex(i);
      
         /* Set the size of the struct we're passing in so the HRM has a clue
            what kind of info to return in case the HRM's struct grows in the
            future. */
         boardInfo.size = sizeof(boardInfo);
         _hrmGetTargetBoardInfo(board,&boardInfo);
      
         hInfo.nBoards++;
      
         /* Note that we use hMon for something different than the HRM does. */
         /* We use it to store the reference back to the hrm board this data came
         frome.  The HRM uses hMon to store the GDHandle of the board.  However,
         since we really do need the GDHandle too (for grDeviceQuery), we stash that in hdc. */

         hInfo.boardInfo[j].hMon = board;
         hInfo.boardInfo[j].hdc  = (void *)boardInfo.driverRefNum;
         hInfo.boardInfo[j].h3pixelSize = 2;
         hInfo.boardInfo[j].h3nwaySli = 1;
         hInfo.boardInfo[j].h3Mem = boardInfo.h3Mem;
         hInfo.boardInfo[j].devRev = boardInfo.deviceRev;
         hInfo.boardInfo[j].pciInfo.vendorID = boardInfo.vendorID;
         hInfo.boardInfo[j].pciInfo.deviceID = boardInfo.deviceID;
         hInfo.boardInfo[j].pciInfo.pciBaseAddr[0] = boardInfo.pciBaseAddr[0];
         hInfo.boardInfo[j].pciInfo.pciBaseAddr[1] = boardInfo.pciBaseAddr[1];
         hInfo.boardInfo[j].pciInfo.pciBaseAddr[2] = boardInfo.pciBaseAddr[2];
         hInfo.boardInfo[j].pciInfo.initialized = 1;
         hInfo.boardInfo[j].pciInfo.devNum = boardInfo.devNum;
         hInfo.boardInfo[j].pciInfo.isMaster = boardInfo.isMaster;
         hInfo.boardInfo[j].pciInfo.numChips = boardInfo.numChips;
         hInfo.boardInfo[j].pciInfo.swizzleOffset[0] = boardInfo.swizzleOffset[0];                   
         hInfo.boardInfo[j].pciInfo.swizzleOffset[1] = boardInfo.swizzleOffset[1];                   
         hInfo.boardInfo[j].pciInfo.swizzleOffset[2] = boardInfo.swizzleOffset[2];                   
         hInfo.boardInfo[j].pciInfo.swizzleOffset[3] = boardInfo.swizzleOffset[3];                   

         if (GETENV("FX_GLIDE_FBRAM", hInfo.boardInfo[monitor].RegPath)) {
            hInfo.boardInfo[j].h3Mem = atoi(GETENV("FX_GLIDE_FBRAM", hInfo.boardInfo[monitor].RegPath));      
         }

#ifdef FX_GLIDE_NAPALM
         if (GETENV("FX_GLIDE_DEVICEID", hInfo.boardInfo[monitor].RegPath))
         {
            FxU32 deviceid = atoi(GETENV("FX_GLIDE_DEVICEID", hInfo.boardInfo[monitor].RegPath));
            hInfo.boardInfo[j].pciInfo.deviceID = deviceid;
         }
         if(GETENV("FX_GLIDE_NUM_CHIPS", hInfo.boardInfo[monitor].RegPath))
         {
            FxU32 numChips;
            numChips = atoi(GETENV("FX_GLIDE_NUM_CHIPS", hInfo.boardInfo[monitor].RegPath));
            /* Don't do something stupid... */
            if(numChips <= hInfo.boardInfo[j].pciInfo.numChips)
            {
               hInfo.boardInfo[j].pciInfo.numChips = numChips;
            }
         }
#endif
         j++;
      }
   }
#else /* HWC_GDX_INIT */
  {
    int i, chip;
    FxU32 bn;

    pciOpen();

    hInfo.nBoards = 0;

    errorString[0] = '\0';

    for (i = 0; i < HWC_MAX_BOARDS; i++) {
      hInfo.boardInfo[i].pciInfo.initialized = 0;
      if (pciFindCardMulti(vID, dID, &bn, i)) {
        hInfo.nBoards++;
        hInfo.boardInfo[i].boardNum = 0;
        hInfo.boardInfo[i].deviceNum = bn;

        hInfo.boardInfo[i].pciInfo.initialized = 1;
        hInfo.boardInfo[i].pciInfo.vendorID = vID;
        hInfo.boardInfo[i].pciInfo.deviceID = dID;
        hInfo.boardInfo[i].pciInfo.numChips = 1;
        hInfo.boardInfo[i].pciInfo.devNum = 0;
        hInfo.boardInfo[i].pciInfo.isMaster = 1;

        hInfo.boardInfo[i].h3pixelSize = 2;
        hInfo.boardInfo[i].h3pixelSample = 1;
        hInfo.boardInfo[i].h3nwaySli = 1;

        /* 
         * NOTE: in the code above we learn about memsize here:
         *   hInfo.boardInfo[i].h3Mem
         * 
         * However, in DOS, since we have not mapped the board yet, we have
         * to wait until later. (see hwcInitRegisters())  - dwj
         */

        /* Get some board Info */
        pciGetConfigData( PCI_REVISION_ID, bn, &hInfo.boardInfo[i].devRev);

        /* Get all the base addresses */
        pciGetConfigData(PCI_BASE_ADDRESS_0, bn,
                         &hInfo.boardInfo[i].pciInfo.pciBaseAddr[0]);
        pciGetConfigData(PCI_BASE_ADDRESS_1, bn,
                         &hInfo.boardInfo[i].pciInfo.pciBaseAddr[1]);
        pciGetConfigData(PCI_IO_BASE_ADDRESS, bn,
                         &hInfo.boardInfo[i].pciInfo.pciBaseAddr[2]);
        pciGetConfigData(PCI_ROM_BASE_ADDRESS, bn,
                         &hInfo.boardInfo[i].pciInfo.pciBaseAddr[3]);
      
        if (GETENV("FX_GLIDE_DEVICEID", hInfo.boardInfo[monitor].RegPath)) {
          FxU32 deviceid = atoi(GETENV("FX_GLIDE_DEVICEID", hInfo.boardInfo[monitor].RegPath));
          hInfo.boardInfo[i].pciInfo.deviceID = deviceid;
        }

        /* Evilness. Try to auto-detect how many chips we have by trying to
         * read the vendor/deviceID register back from chips 1 through 3. */
        if(IS_NAPALM(hInfo.boardInfo[i].pciInfo.deviceID)) {
          FxU32 device_vendor;

          /* Detect two-chip board */
          device_vendor = hwcReadConfigRegister(&hInfo.boardInfo[i], 1, offsetof(SstPCIConfigRegs, deviceID_vendorID));
          if((device_vendor & 0xFFFF) == 0x121a && (IS_NAPALM((device_vendor >> 16)))) {
            hInfo.boardInfo[i].pciInfo.numChips = 2;
            
            /* Map in the slave's physical memory */
            mapSlavePhysical(&hInfo.boardInfo[i], 1);                       
             
            /* Detect four-chip board */  
            device_vendor = hwcReadConfigRegister(&hInfo.boardInfo[i], 3, offsetof(SstPCIConfigRegs, deviceID_vendorID));
            if((device_vendor & 0xFFFF) == 0x121a && (IS_NAPALM((device_vendor >> 16)))) {
              hInfo.boardInfo[i].pciInfo.numChips = 4;
              /* Map in last two chips */
              mapSlavePhysical(&hInfo.boardInfo[i], 2);
              mapSlavePhysical(&hInfo.boardInfo[i], 3);
            }
          }            

          for(chip = 1; chip < hInfo.boardInfo[i].pciInfo.numChips; chip++) {
            hInfo.boardInfo[i].pciInfo.pciBaseAddr[(chip << 2) + 0] = 
              hwcReadConfigRegister(&hInfo.boardInfo[i], chip, offsetof(SstPCIConfigRegs, memBaseAddr0));
            hInfo.boardInfo[i].pciInfo.pciBaseAddr[(chip << 2) + 1] = 
              hwcReadConfigRegister(&hInfo.boardInfo[i], chip, offsetof(SstPCIConfigRegs, memBaseAddr1));
            hInfo.boardInfo[i].pciInfo.pciBaseAddr[(chip << 2) + 2] = 
              hwcReadConfigRegister(&hInfo.boardInfo[i], chip, offsetof(SstPCIConfigRegs, ioBaseAddr));
          }
        }        
            
        if(GETENV("FX_GLIDE_NUM_CHIPS", hInfo.boardInfo[monitor].RegPath)) {
          FxU32 numChips;
          numChips = atoi(GETENV("FX_GLIDE_NUM_CHIPS", hInfo.boardInfo[monitor].RegPath));
          /* Don't do something stupid... */
          if(numChips <= hInfo.boardInfo[i].pciInfo.numChips) {
            hInfo.boardInfo[i].pciInfo.numChips = numChips;
          }
        }      
      }
    }
  
  }
#endif /* HWC_EXT_INIT */
  if (hInfo.nBoards)
    return &hInfo;
  else
    return NULL;

#undef FN_NAME
} /* hwcInit */

FxBool
hwcMapBoard(hwcBoardInfo *bInfo, FxU32 bAddrMask)
{
#define FN_NAME "hwcMapBoard"

  if (bInfo->pciInfo.initialized == FXFALSE) {
    sprintf(errorString, "%s:  Called before hwcInit\n", FN_NAME);
    return FXFALSE;
  }

  bInfo->linearInfo.initialized = FXTRUE;

#ifdef HWC_EXT_INIT
  {
    hwcExtRequest_t ctxReq;
    hwcExtResult_t  ctxRes;

    bInfo->procHandle = (FxU32) GetCurrentProcessId();
    GDBG_INFO(80, FN_NAME ":  procHandle:  0x%x\n", bInfo->procHandle);

    ctxReq.which = HWCEXT_GETLINEARADDR;
    ctxReq.optData.linearAddrReq.devNum = bInfo->boardNum;
    ctxReq.optData.linearAddrReq.pHandle = bInfo->procHandle;
    GDBG_INFO(80, FN_NAME ":  ExtEscape:HWCEXT_GETLINEARADDR\n");

    ExtEscape((HDC)bInfo->hdc, HWCEXT_ESCAPE(bInfo->boardNum), sizeof(ctxReq), (LPSTR) &ctxReq, /**/
      sizeof(ctxRes), (LPSTR) &ctxRes);

    if (ctxRes.resStatus != 1) {
      hwc_errncpy(errorString, "HWCEXT_GETLINEARADDR Failed");
      return FXFALSE;
    }
    bInfo->isMapped = FXTRUE;

    bInfo->linearInfo.linearAddress[0] =
      ctxRes.optData.linearAddressRes.baseAddresses[0];
    bInfo->linearInfo.linearAddress[1] =
      ctxRes.optData.linearAddressRes.baseAddresses[1];
    
    /* WTF????? */
    bInfo->pciInfo.pciBaseAddr[2] = 
      ctxRes.optData.linearAddressRes.baseAddresses[1];

    GDBG_INFO(80, "hdc: %d\n", bInfo->hdc);
    GDBG_INFO(80, "linearAddress[0]: 0x%x\n", ctxRes.optData.linearAddressRes.baseAddresses[0]);
    GDBG_INFO(80, "linearAddress[1]: 0x%x\n", ctxRes.optData.linearAddressRes.baseAddresses[1]);
    GDBG_INFO(80, "pciBaseAddr[2]: 0x%x\n", ctxRes.optData.linearAddressRes.baseAddresses[1]);

    /*
    ** AJB - if there are slave chips, get slave chip regs
    */
    if (bInfo->pciInfo.numChips > 1)
    {
      FxU32 chip ;
      FxU32 i ;

      for (chip = 1 ;
           chip < bInfo->pciInfo.numChips ;
           chip++)
      {
        ctxReq.which = HWCEXT_GET_SLAVE_REGS ;
        ctxReq.optData.slaveRegReq.DeviceId = chip ;
        GDBG_INFO(80, FN_NAME ":  ExtEscape:HWCEXT_GET_SLAVE_REGS\n") ;

        ExtEscape((HDC)bInfo->hdc, HWCEXT_ESCAPE(bInfo->boardNum), sizeof(ctxReq), (LPSTR) &ctxReq, sizeof(ctxRes), (LPSTR) &ctxRes) ;

        if (ctxRes.resStatus != 1) 
        {
          hwc_errncpy(errorString, "HWCEXT_GET_SLAVE_REGS Failed") ;
          return FXFALSE ;
        }

        for (i = 0 ; /* Get back 4 register space mappings for each chip */
             i < 4 ;
             i++)
        {
          bInfo->linearInfo.linearAddress[(chip << 2) + i] = ctxRes.optData.slaveRegRes.Regs[i] ;
        }
      }
    }
  }
#elif defined(HWC_GDX_INIT)
        /* Pretty simple, because MacOS is basically lame */
  {
    FxU32
      bAddr;

    /* memory mapped register spaces */
    for (bAddr = 0; bAddr < 2; bAddr++) {
      if ((bAddrMask & (0x01UL << bAddr)) != 0x00UL) {
        bInfo->linearInfo.linearAddress[bAddr] = bInfo->pciInfo.pciBaseAddr[bAddr];
      }
    }
    /* FixMe: This gets used to set the pll's so I guess we need it here
     * unconditionally
     */
    bInfo->isMapped = FXTRUE;
    bInfo->linearInfo.linearAddress[2] = bInfo->pciInfo.pciBaseAddr[2];
    
    /* Sorry, ROM mapping on MacOS seems whacked out for some reason. */
    bInfo->linearInfo.linearAddress[3] = 0;

    if (bInfo->pciInfo.numChips > 1)
    {
      FxU32 chip ;
      FxU32 i ;
      FxU32 regs[4];
      
      for (chip = 1 ;
           chip < bInfo->pciInfo.numChips ;
           chip++)
      {
        _hrmGetSlaveRegs((hrmBoard_t *)bInfo->hMon, chip, regs);
          
        for (i = 0 ; /* Get back 4 register space mappings for each chip */
             i < 4 ;
             i++)
        {
          bInfo->linearInfo.linearAddress[(chip << 2) + i] = regs[i];
        }
      }
    }
    
  }
#else
  {
    FxU32
      bAddr, length;

    bInfo->isMapped = FXTRUE;

    if (IS_NAPALM(bInfo->pciInfo.deviceID))
      length = 32*1024*1024;
    else
      length = 16*1024*1024;
    
    /* memory mapped register spaces */
    for (bAddr = 0; bAddr < 2; bAddr++) {
      if ((bAddrMask & (0x01UL << bAddr)) != 0x00UL) {
        bInfo->linearInfo.linearAddress[bAddr] = 
          (FxU32)pciMapCardMulti(bInfo->pciInfo.vendorID, bInfo->pciInfo.deviceID,
                                 length, &bInfo->deviceNum, bInfo->boardNum, bAddr);
      }
    }

    /* FixMe: This gets used to set the pll's so I guess we need it here
     * unconditionally
     */
    bInfo->linearInfo.linearAddress[2] = 
      (FxU32)pciMapCardMulti(bInfo->pciInfo.vendorID, bInfo->pciInfo.deviceID,
                             length, &bInfo->deviceNum, bInfo->boardNum, 2);

    /* Does the caller want the rom bios? */
    if ((bAddrMask & 0x08UL) != 0x00UL) {
      bInfo->linearInfo.linearAddress[3] = 
        (FxU32)pciMapCardMulti(bInfo->pciInfo.vendorID, bInfo->pciInfo.deviceID,
                               0x1000000, &bInfo->deviceNum, bInfo->boardNum, 3);
    }

    /* Map in slaves too */
    if(bInfo->pciInfo.numChips > 1) {
      FxU32 chip;
      FxBool success;
      FxU32 length = 32*1024*1024;
        
      for(chip = 1 ; chip < bInfo->pciInfo.numChips; chip++) {
        /* The PCI library seemed a bit whacked for mapping in multi-function
         * devices, so I use the more low-level routines to map in the two 
         * physical memory chunks I need. */
        success = pciMapPhysicalToLinear(&bInfo->linearInfo.linearAddress[(chip << 2) + 0],
                                         bInfo->pciInfo.pciBaseAddr[(chip << 2) + 0],
                                         &length);
        if(!success) {
            /* We failed to map.  Punt. */
            bInfo->pciInfo.numChips = 1;
            break;
        }
        success = pciMapPhysicalToLinear(&bInfo->linearInfo.linearAddress[(chip << 2) + 1],
                                         bInfo->pciInfo.pciBaseAddr[(chip << 2) + 1],
                                         &length);
        if(!success) {
            /* We failed to map.  Punt. */
            bInfo->pciInfo.numChips = 1;
            break;
        }
      }
    }
  }
#endif  
  
  return FXTRUE;
#undef FN_NAME
} /* hwcMapBoard */

FxBool
hwcInitRegisters(hwcBoardInfo *bInfo) 
{
#define FN_NAME "hwcInitRegisters"
  FxU32
    grxSpeedInMHz, memSpeedInMHz,
    sgramMode, sgramMask, sgramColor;
  
  if (bInfo->linearInfo.initialized == FXFALSE) {
    sprintf(errorString, "%s:  Called before hwcMapBoard\n", FN_NAME);
    return FXFALSE;
  }
      
  bInfo->regInfo.initialized = FXTRUE;

  bInfo->regInfo.ioMemBase =
    bInfo->linearInfo.linearAddress[0] + SST_IO_OFFSET;
  bInfo->regInfo.cmdAGPBase =
    bInfo->linearInfo.linearAddress[0] + SST_CMDAGP_OFFSET;
  bInfo->regInfo.waxBase =
    bInfo->linearInfo.linearAddress[0] + SST_2D_OFFSET;
  bInfo->regInfo.sstBase =
    bInfo->linearInfo.linearAddress[0] + SST_3D_OFFSET;
  bInfo->regInfo.lfbBase =
    bInfo->linearInfo.linearAddress[0] + SST_LFB_OFFSET;

  GDBG_INFO(80, "ioMemBase: 0x%x\n", bInfo->regInfo.ioMemBase);
  GDBG_INFO(80, "cmdAGPBase: 0x%x\n", bInfo->regInfo.cmdAGPBase);
  GDBG_INFO(80, "waxBase: 0x%x\n", bInfo->regInfo.waxBase);
  GDBG_INFO(80, "sstBase: 0x%x\n", bInfo->regInfo.sstBase);
  GDBG_INFO(80, "lfbBase: 0x%x\n", bInfo->regInfo.lfbBase);

  /*
  // AJB: Init slave chip regs
  // Someday I may define these magic numbers.
  */
#if defined (HWC_EXT_INIT) ||  defined(HWC_GDX_INIT)
  {
    FxU32 chip ;

    for (chip = 1 ;
         chip < bInfo->pciInfo.numChips ;
         chip++)
    {
      bInfo->regInfo.slaveSstBase[chip - 1] = bInfo->linearInfo.linearAddress[(chip << 2) + 3] ;
      bInfo->regInfo.slaveCmdBase[chip - 1] = bInfo->linearInfo.linearAddress[(chip << 2) + 1] ;
      bInfo->regInfo.slaveIOBase[chip - 1]  = bInfo->linearInfo.linearAddress[(chip << 2) + 0] ;
    }
  }
#else 
  /* DOS is a bit weirder. In this case we have both memBase0 and memBase1's linear
   * addresses stored in linearAddress[0] and linearAddress[1], which is not what
   * the Windows and Mac code expect.  However, the important thing is really just
   * the slave* stuff in regInfo, so we fix that up here.
   */
  {
    FxU32 chip ;

    for (chip = 1 ;
         chip < bInfo->pciInfo.numChips ;
         chip++)
    {
      bInfo->regInfo.slaveSstBase[chip - 1] = bInfo->linearInfo.linearAddress[(chip << 2) + 0] + SST_3D_OFFSET;
      bInfo->regInfo.slaveCmdBase[chip - 1] = bInfo->linearInfo.linearAddress[(chip << 2) + 0] + SST_CMDAGP_OFFSET ;
      bInfo->regInfo.slaveIOBase[chip - 1]  = bInfo->linearInfo.linearAddress[(chip << 2) + 0];
      /* We need this for VGA crap, which Win32 and DOS don't need. */
      bInfo->regInfo.slaveIOPortBase[chip - 1] = bInfo->pciInfo.pciBaseAddr[(chip << 2) + 2] & ~0x1;
    }
  }

#endif

  bInfo->regInfo.rawLfbBase =
    bInfo->linearInfo.linearAddress[1];
#if __POWERPC__    
  bInfo->regInfo.ioPortBase = bInfo->pciInfo.pciBaseAddr[2] & ~0x1;
#else
  bInfo->regInfo.ioPortBase = (FxU16) bInfo->pciInfo.pciBaseAddr[2] & ~0x1;
#endif
  /* Figure out if it's SDRAM */
  {
    FxU32 dramInit1;
    
    HWC_IO_LOAD(bInfo->regInfo, dramInit1, dramInit1);
    bInfo->sdRAM = ((dramInit1 & SST_MCTL_TYPE_SDRAM) != 0x00UL);

    if (GETENV("SSTH3_SDRAM", bInfo->RegPath))
      bInfo->sdRAM = FXTRUE;
  }

#if !defined(HWC_EXT_INIT) && !defined(HWC_GDX_INIT)
  {
    FxU32 
      pciInit0,
      pciCommandReg =
        BIT(0) |              /* enable i/o decode */
        BIT(1);               /* enable memory decode */
  
    /* Enable PCI memory and I/O decode */
    pciSetConfigData(PCI_COMMAND, bInfo->deviceNum, &pciCommandReg);
    
    HWC_IO_LOAD(bInfo->regInfo, pciInit0, pciInit0);
    pciInit0 |= SST_PCI_READ_WS | SST_PCI_WRITE_WS;
    HWC_IO_STORE(bInfo->regInfo, pciInit0, pciInit0);  
  }
#endif

  if (GETENV("SSTH3_SGRAM_MODE", bInfo->RegPath))
    sgramMode = atoi(GETENV("SSTH3_SGRAM_MODE", bInfo->RegPath));
  else if (GETENV("SSTH3_SGRAM_222", bInfo->RegPath) &&
    (atoi(GETENV("SSTH3_SGRAM_222", bInfo->RegPath)) != 0))
    sgramMode = 0x27;
  else
    sgramMode = 0x37;

  if (GETENV("SSTH3_SGRAM_MASK", bInfo->RegPath))
    sgramMask = atoi(GETENV("SSTH3_SGRAM_MASK", bInfo->RegPath));
  else
    sgramMask = 0xFFFFFFFF;

  if (GETENV("SSTH3_SGRAM_COLOR", bInfo->RegPath))
    sgramColor = atoi(GETENV("SSTH3_SGRAM_COLOR", bInfo->RegPath));
  else
    sgramColor = 0;

  if (GETENV("SSTH3_GRXCLOCK", bInfo->RegPath))
    grxSpeedInMHz = atoi(GETENV("SSTH3_GRXCLOCK", bInfo->RegPath));
  else
    grxSpeedInMHz = 100;

  if (GETENV("SSTH3_MEMCLOCK", bInfo->RegPath))
    memSpeedInMHz = atoi(GETENV("SSTH3_MEMCLOCK", bInfo->RegPath));
  else
    memSpeedInMHz = 100;

#if !defined(HWC_ACCESS_DDRAW) && !defined(HWC_GDX_INIT)
  if (GETENV("HAL_NOINIT") == NULL || atoi(GETENV("HAL_NOINIT")) == 0) {

    /* 
     * final DOS initialiation
     */

    /* don't set the clocks unless they used the environment variables */

    if (GETENV("SSTH3_GRXCLOCK") || GETENV("SSTH3_MEMCLOCK")) {
     switch (bInfo->pciInfo.deviceID) {
        case SST_DEVICE_ID_H3: /* banshee */
           h3InitPlls(bInfo->regInfo.ioPortBase,  grxSpeedInMHz, memSpeedInMHz);
           break;
        case SST_DEVICE_ID_H4: /* voodoo3/avenger */
            break;
        case SST_DEVICE_ID_H4_OEM: /* voodoo3/avenger */
           break;
        default: /* unknown board type!!! */
           return FXFALSE;
     }
    }

    /* read back the memory size, since we 
     * don't know it under DOS  (see hwcInit) - dwj 
     */
    bInfo->h3Mem = h3InitGetMemSize(bInfo->regInfo.ioPortBase);
    /* Ugh, lame.  We can only report back 16MB to Glide since we can't
     * really map in all 64MB necessary for LFB accesses. */
    if(IS_NAPALM(bInfo->pciInfo.deviceID)) {
        if(bInfo->h3Mem > 16) {
            bInfo->h3Mem = 16;
        }
    }

    h3InitVga(bInfo->regInfo.ioPortBase, FXTRUE); 
  }
#endif
  
  return FXTRUE;

#undef FN_NAME
} /* hwcInitRegisters */

#define FIFOPAD         0x0000
#define AUXPAD          0x1000

FxBool
hwcAllocBuffers(hwcBoardInfo *bInfo, FxU32 nColBuffers, FxU32 nAuxBuffers) 
{
#define FN_NAME "hwcAllocBuffers"
  FxBool
    bufferAlignP, napalmfifo = FXTRUE;
  FxU32
    bNum,
    h3Mem = bInfo->h3Mem << 20,
    freeMem,
    bufStride, 
    bufSize;
  FxI32
    i,
    tramSize, fifoSize;

  if (bInfo->vidInfo.initialized == FXFALSE) {
    sprintf(errorString, "%s:  Called before video initialization\n", FN_NAME);
    return FXFALSE;
  }
    
  GDBG_INFO(80, "%s(0x%x, 0x%x, 0x%x)\n", FN_NAME, bInfo, nColBuffers, nAuxBuffers);

  /* I've decided on > 2 instead of == 3 because we may support more
    than 3 buffers in the future, and want 4 to set the
    triple-buffering bit in dramInit1, also */
  bInfo->vidInfo.tripleBuffering = (nColBuffers > 2);

  bInfo->vidInfo.stride = bufStride =
    calcBufferStride(bInfo, bInfo->vidInfo.xRes, bInfo->vidInfo.tiled);   

  /* We want to place the FIFO after the tram but before the color
     buffers with some pad */
  bufSize = calcBufferSize(bInfo, bInfo->vidInfo.xRes, bInfo->vidInfo.yRes,
    bInfo->vidInfo.tiled); 

  bInfo->buffInfo.bufStride = bufStride;
  bInfo->buffInfo.bufSize = bufSize;
  bInfo->buffInfo.bufLfbStride = HWC_LFB_STRIDE;

  if (bInfo->vidInfo.tiled) {
    bInfo->buffInfo.bufStrideInTiles = (bufStride >> 7);
    bInfo->buffInfo.bufSizeInTiles =
      calcBufferSizeInTiles(bInfo, bInfo->vidInfo.xRes, bInfo->vidInfo.yRes);
    bInfo->buffInfo.bufHeightInTiles =
      calcBufferHeightInTiles(bInfo, bInfo->vidInfo.yRes);
  }

  bInfo->buffInfo.initialized = FXTRUE;
  bInfo->buffInfo.nColBuffers = nColBuffers;
  bInfo->buffInfo.nAuxBuffers = nAuxBuffers;

  /* First, do the primary buffer allocation */        
  freeMem = h3Mem;

  if (nAuxBuffers > 0) {
    bInfo->buffInfo.auxBuffEnd0  =
    bInfo->buffInfo.auxBuffStart0 = freeMem;

    bInfo->buffInfo.auxBuffStart0 -= bufSize;

    /* auxBuffers start on odd pages, so we need to check to see if
     * it's on an even page and, if so, make it odd. 
     *
     * NB: We need to do the same sort of 'alignment' thing here as for
     * the color buffers.
     */
    bufferAlignP = ((bInfo->buffInfo.auxBuffStart0 & 0x1000UL) == 0);
    if (bufferAlignP) bInfo->buffInfo.auxBuffStart0 -= 0x1000;
    
    freeMem = bInfo->buffInfo.auxBuffStart0;
  }

  for (i = nColBuffers - 1; i >= 0; i--) {

    bInfo->buffInfo.colBuffStart0[i] = 
      bInfo->buffInfo.colBuffEnd0[i] = freeMem;

    bInfo->buffInfo.colBuffStart0[i] -= bufSize;

    /* As a memory access optmization colorBuffers start on even
     * pages, while aux buffers start on odd pages.  Thus we must
     * check to see if we're startding on an odd page here and, if so,
     * add a page to the start.  
     */
    bufferAlignP = ((bInfo->buffInfo.colBuffStart0[i] & 0x1000UL) != 0);
    if (bufferAlignP) bInfo->buffInfo.colBuffStart0[i] -= 0x1000;

    freeMem = bInfo->buffInfo.colBuffStart0[i];
  }

  /* Now we can calculate some other stuff... */
  bInfo->fbOffset = bInfo->primaryOffset = bInfo->buffInfo.colBuffStart0[0];

  /* if the secondary buffer is enabled */
  if (bInfo->buffInfo.enable2ndbuffer) {
    if (nAuxBuffers > 0) {
      bInfo->buffInfo.auxBuffEnd1  =
        bInfo->buffInfo.auxBuffStart1 = freeMem;

      bInfo->buffInfo.auxBuffStart1 -= bufSize;

      /* auxBuffers start on odd pages, so we need to check to see if
       * it's on an even page and, if so, make it odd. 
       *
       * NB: We need to do the same sort of 'alignment' thing here as for
       * the color buffers.
       */
      bufferAlignP = ((bInfo->buffInfo.auxBuffStart1 & 0x1000UL) == 0);
      if (bufferAlignP) bInfo->buffInfo.auxBuffStart1 -= 0x1000;
      
      freeMem = bInfo->buffInfo.auxBuffStart1;
    }
    
    for (i = nColBuffers - 1; i >= 0; i--) {
      
      bInfo->buffInfo.colBuffStart1[i] = 
        bInfo->buffInfo.colBuffEnd1[i] = freeMem;
      
      bInfo->buffInfo.colBuffStart1[i] -= bufSize;
      
      /* As a memory access optmization colorBuffers start on even
       * pages, while aux buffers start on odd pages.  Thus we must
       * check to see if we're startding on an odd page here and, if so,
       * add a page to the start.  
       */
      bufferAlignP = ((bInfo->buffInfo.colBuffStart1[i] & 0x1000UL) != 0);
      if (bufferAlignP) bInfo->buffInfo.colBuffStart1[i] -= 0x1000;
      
      freeMem = bInfo->buffInfo.colBuffStart1[i];
    }

    /* Now we can calculate some other stuff... */
    bInfo->fbOffset = bInfo->secondaryOffset = bInfo->buffInfo.colBuffStart1[0];
  }


  /*
  ** Napalm restricts its cmd fifo within 16MB range so
  ** we place it at low address 0x0.
  */
  if (IS_NAPALM(bInfo->pciInfo.deviceID)) {
    napalmfifo = FXTRUE;
  }

  if (getenv("FX_GLIDE_V3FIFO")) {
    napalmfifo = FXFALSE;
  }
  if ((IS_NAPALM(bInfo->pciInfo.deviceID)) && (napalmfifo)) {
    FxU32 pad = 96*1024; /* Get out of the way of VGA */

    if(GETENV("FX_GLIDE_FIFOEXTRA", bInfo->RegPath)) 
      pad = atoi(GETENV("FX_GLIDE_FIFOEXTRA", bInfo->RegPath));

    bInfo->tramOffset = fifoSize = MAXFIFOSIZE_16MB + pad;

    tramSize =  bInfo->fbOffset - bInfo->tramOffset;
    if (tramSize < (FxI32)bInfo->min_tramSize) {
      /* Now we have to shrink the FIFO */
      tramSize = bInfo->min_tramSize;
      fifoSize = bInfo->fbOffset - bInfo->min_tramSize;
      /* We don't want a FIFO smaller than one page */
      if (fifoSize < (FxI32)(pad+4096)) {
        GDBG_INFO(80, "%s:  Not enough memory for resolution + min texture\n",
                  FN_NAME);
        sprintf(errorString, 
                "%s:  Not enough memory for resolution + min texture\n",
                FN_NAME);
        return FXFALSE;
      } 
    }
    
    bInfo->fifoInfo.fifoLength = (FxU32) fifoSize - pad;
    bInfo->fifoInfo.fifoStart = pad;
    bInfo->tramOffset = (FxU32) fifoSize;
    bInfo->tramSize = (FxU32) tramSize;
  }
  else {
    fifoSize = (bInfo->h3Mem > 8) ? MAXFIFOSIZE_16MB : MAXFIFOSIZE;
    tramSize =  bInfo->fbOffset - bInfo->tramOffset - fifoSize;
    if (tramSize < (FxI32)bInfo->min_tramSize) {
      /* Now we have to shrink the FIFO */
      tramSize = bInfo->min_tramSize;
      fifoSize = bInfo->fbOffset - bInfo->min_tramSize;
      /* We don't want a FIFO smaller than one page */
      if (fifoSize < 4096) {
        GDBG_INFO(80, "%s:  Not enough memory for resolution + min texture\n",
                  FN_NAME);
        sprintf(errorString, 
                "%s:  Not enough memory for resolution + min texture\n",
                FN_NAME);
        return FXFALSE;
      } 
    }
    
    bInfo->fifoInfo.fifoLength = (FxU32) fifoSize;
    bInfo->fifoInfo.fifoStart = bInfo->fbOffset - fifoSize;
    
    /* Huh? Why is this here? */
    bInfo->fifoInfo.fifoLength -= 0x2000;
    
    bInfo->tramSize = (FxU32) tramSize;
  }

  {
    for (bNum = 0; bNum < nColBuffers; bNum++) {
      bInfo->buffInfo.lfbBuffAddr0[bNum] = 
        hwcBufferLfbAddr(bInfo, bInfo->buffInfo.colBuffStart0[bNum]);
      /* Not really used, but here for consistancy */
      bInfo->buffInfo.lfbBuffAddr0End[bNum] = 
        hwcBufferLfbAddr(bInfo, bInfo->buffInfo.colBuffEnd0[bNum]);
    }

    if (nAuxBuffers > 0) {
      bInfo->buffInfo.lfbBuffAddr0[nColBuffers] =
        hwcBufferLfbAddr(bInfo, bInfo->buffInfo.auxBuffStart0);
      /* I'm not sure if this is right or not... */
      bInfo->buffInfo.lfbBuffAddr0End[nColBuffers] = 
        hwcBufferLfbAddr(bInfo, bInfo->buffInfo.auxBuffEnd0);
    }
  }

  bInfo->buffInfo.initialized = FXTRUE;

  GDBG_INFO(80, "%s:  Board Info:\n", FN_NAME);
  GDBG_INFO(80, "\thdc:             0x%x\n", bInfo->hdc);
/*  GDBG_INFO(80, "\textContextID:    0x%x\n", bInfo->extContextID); */
  GDBG_INFO(80, "\tdevRev:          0x%x\n", bInfo->devRev);
  GDBG_INFO(80, "\tfbOffset:        0x%x\n", bInfo->fbOffset);
  GDBG_INFO(80, "\tprimaryOffset:   0x%x\n", bInfo->primaryOffset);
  GDBG_INFO(80, "\tsecondaryOffset: 0x%x\n", bInfo->secondaryOffset);
  GDBG_INFO(80, "\th3Rev:           0x%x\n", bInfo->h3Rev);
  GDBG_INFO(80, "\th3Mem:           0x%x\n", bInfo->h3Mem);
  GDBG_INFO(80, "\tboardNum:        0x%x\n", bInfo->boardNum);
  GDBG_INFO(80, "\tdeviceNum:       0x%x\n", bInfo->deviceNum);

  GDBG_INFO(80, "%s:  Buffer Info:\n", FN_NAME);
  GDBG_INFO(80, "\tbufSize:         0x%x\n", bInfo->buffInfo.bufSize);
  GDBG_INFO(80, "\tbufSizeInTiles:  0x%x\n", bInfo->buffInfo.bufSizeInTiles);
  GDBG_INFO(80, "\tbufStride:       0x%x\n", bInfo->buffInfo.bufStride);
  GDBG_INFO(80, "\tbufStrideInTiles:0x%x\n", bInfo->buffInfo.bufStrideInTiles);
  GDBG_INFO(80, "\tbufHeightInTiles:0x%x\n", bInfo->buffInfo.bufHeightInTiles);
  GDBG_INFO(80, "\tnColBuffers:     0x%x\n", bInfo->buffInfo.nColBuffers);
  for (i = 0; i < (FxI32) nColBuffers; i++) {
    GDBG_INFO(80, "\tcolBuff0[%d] Start:    0x%x\n", i, bInfo->buffInfo.colBuffStart0[i]);
    GDBG_INFO(80, "\tcolBuff0[%d] End:      0x%x\n", i, bInfo->buffInfo.colBuffEnd0[i]);
  }
  for (i = 0; i < (FxI32) nColBuffers; i++) {
    GDBG_INFO(80, "\tcolBuff1[%d] Start:    0x%x\n", i, bInfo->buffInfo.colBuffStart1[i]);
    GDBG_INFO(80, "\tcolBuff1[%d] End:      0x%x\n", i, bInfo->buffInfo.colBuffEnd1[i]);
  }
  for (i = 0; i < (FxI32) nColBuffers; i++) {
    GDBG_INFO(80, "\tcolBuff0Lfb[%d] Start: 0x%x\n", i, bInfo->buffInfo.lfbBuffAddr0[i]);
    GDBG_INFO(80, "\tcolBuff0Lfb[%d] End:   0x%x\n", i, bInfo->buffInfo.lfbBuffAddr0End[i]);
  }
  GDBG_INFO(80, "\tnAuxBuffers:     0x%x\n", bInfo->buffInfo.nAuxBuffers);
  GDBG_INFO(80, "\tauxBuffStart0:    0x%x\n", bInfo->buffInfo.auxBuffStart0);
  GDBG_INFO(80, "\tauxBuffEnd0:      0x%x\n", bInfo->buffInfo.auxBuffEnd0);
  GDBG_INFO(80, "\tauxBuffStart1:    0x%x\n", bInfo->buffInfo.auxBuffStart1);
  GDBG_INFO(80, "\tauxBuffEnd1:      0x%x\n", bInfo->buffInfo.auxBuffEnd1);
  GDBG_INFO(80, "\tauxBuffStart0Lfb: 0x%x\n", bInfo->buffInfo.lfbBuffAddr0[nColBuffers]);
  GDBG_INFO(80, "\tauxBuffEnd0Lfb:   0x%x\n", bInfo->buffInfo.lfbBuffAddr0End[nColBuffers]);

  GDBG_INFO(80, "%s:  FIFO Info:\n", FN_NAME);
  GDBG_INFO(80, "\tfifoStart:       0x%x\n", bInfo->fifoInfo.fifoStart);
  GDBG_INFO(80, "\tfifoLength:      0x%x\n", bInfo->fifoInfo.fifoLength);

  GDBG_INFO(80, "%s:  TRAM Info:\n", FN_NAME);
  GDBG_INFO(80, "\ttramOffset:      0x%x\n", bInfo->tramOffset);
  GDBG_INFO(80, "\ttramSize:        0x%x\n", bInfo->tramSize);
  GDBG_INFO(80, "\tMin TramSize:    0x%x\n", bInfo->min_tramSize);

  return FXTRUE;
#undef FN_NAME
} /* hwcAllocBuffers */

void hwcIdleHardwareWithTimeout(hwcBoardInfo *bInfo)
{
  FxU32 
    miscInit0, miscInit1, status, statusSlave, idle, timeout, i;

  /* Wait for hardware to idle. */
  idle = 0;
  timeout = 0;

checkforidle:
  do {
    if(idle > 0) {
      GDBG_INFO(80,"waiting for idle...\n");
    }
    HWC_IO_LOAD(bInfo->regInfo, status, status);
    for(i = 1; i < bInfo->pciInfo.numChips; i++) {
      HWC_IO_LOAD_SLAVE(i, bInfo->regInfo, status, statusSlave);
      status |= statusSlave;
    }
    /* Make sure we see an idle 3 times in a row from all chips. */
    if(status & SST_BUSY) {
     idle = 0;
    } else {
     idle++;
    }
    timeout++;
    /* Nothing the hardware does should take as long as reading the
     * status registers a billion times... */
    if(timeout >= 1000000000) {
      break;
    }        
  } while(idle < 3);  

  if(timeout >= 1000000000) {
    GDBG_INFO(80,"Hardware timeout on idle, resetting...\n");
    /* Reset FBI, 2D, and command streams. */
    HWC_IO_LOAD(bInfo->regInfo, miscInit0, miscInit0);
    /* Also be sure to make sure miscInit1's addressing is correct on Napalm */
    HWC_IO_STORE(bInfo->regInfo, miscInit0, (miscInit0 & ~BIT(30)) | SST_GRX_RESET | SST_2D_RESET);
    HWC_IO_LOAD(bInfo->regInfo, miscInit1, miscInit1);
    HWC_IO_STORE(bInfo->regInfo, miscInit1, miscInit1 | SST_CMDSTREAM_RESET);

    /* Give it a little time to propagate */
    for(idle = 0; idle < 1000; idle++) {
      HWC_IO_LOAD(bInfo->regInfo, status, status);    
    }  
    /* Let hardware out of reset */
    HWC_IO_STORE(bInfo->regInfo, miscInit1, miscInit1);
    HWC_IO_STORE(bInfo->regInfo, miscInit0, miscInit0);
    
    /* Give it a little time to propagate */
    for(idle = 0; idle < 1000; idle++) {
      HWC_IO_LOAD(bInfo->regInfo, status, status);    
    }  

    /* And make sure it's really idle... */
    goto checkforidle;
  }
}
            
FxBool
hwcInitFifo(hwcBoardInfo *bInfo, FxBool enableHoleCounting)
{
#define FN_NAME "hwcInitFifo"
  FxBool
    agpEnable = FXFALSE;  
  FxU32
    cagpRegs;                   /* pointer to Cmd/AGP regs */
#if 0
  /* scratch for the 'fill the fifo' debugging trick */
  FxU32 *filler;
#endif

  if (bInfo->regInfo.initialized == FXFALSE) {
    sprintf(errorString, "%s:  Called before hwcMapBoard\n", FN_NAME);
    return FXFALSE;
  }
  cagpRegs = bInfo->regInfo.cmdAGPBase;

  if (bInfo->buffInfo.initialized == FXFALSE) {
    sprintf(errorString, "%s:  Called before hwcInitBuffers\n", FN_NAME);
    return FXFALSE;
  }

  hwcIdleHardwareWithTimeout(bInfo);

  /* disable the CMD fifo */
  HWC_CAGP_STORE(bInfo->regInfo, cmdFifo0.baseSize, 0);

#if 0
  /* Debugging.  Fill the command fifo with something so we can see if someone
   * else was playing in our sandbox. */
  for(filler = (FxU32 *)(bInfo->fifoInfo.fifoStart + bInfo->regInfo.rawLfbBase);
      filler < (FxU32 *)(bInfo->fifoInfo.fifoStart + bInfo->fifoInfo.fifoLength + bInfo->regInfo.rawLfbBase);
      filler++) {
    *filler = 0xdeadbeef;
  }
#endif

  HWC_CAGP_STORE(bInfo->regInfo, cmdFifo0.baseAddrL,
    bInfo->fifoInfo.fifoStart>>12);
  HWC_CAGP_STORE(bInfo->regInfo, cmdFifo0.readPtrL, bInfo->fifoInfo.fifoStart);
  HWC_CAGP_STORE(bInfo->regInfo, cmdFifo0.readPtrH, 0);
  HWC_CAGP_STORE(bInfo->regInfo, cmdFifo0.aMin, bInfo->fifoInfo.fifoStart-4);
  HWC_CAGP_STORE(bInfo->regInfo, cmdFifo0.aMax, bInfo->fifoInfo.fifoStart-4);
  
  HWC_CAGP_STORE(bInfo->regInfo, cmdFifo0.depth, 0);
  HWC_CAGP_STORE(bInfo->regInfo, cmdFifo0.holeCount, 0);
  /* Fifo LWM /HWM/ THRESHOLD */
  if (bInfo->pciInfo.deviceID == SST_DEVICE_ID_H3) { /* banshee */
    HWC_CAGP_STORE(bInfo->regInfo, cmdFifoThresh,
                   (0x09 << SST_HIGHWATER_SHIFT) | 0x2);
  } else {
    HWC_CAGP_STORE(bInfo->regInfo, cmdFifoThresh,
                   (0xf << SST_HIGHWATER_SHIFT) | 0x8);
  }

  /* Keep writes from actually going to the HW. */
  if(GETENV("FX_GLIDE_NO_HW", bInfo->RegPath)) {
    return FXTRUE;
  }

  HWC_CAGP_STORE(bInfo->regInfo, cmdFifo0.baseSize, (((bInfo->fifoInfo.fifoLength >> 12) - 1) | 
                                                     SST_EN_CMDFIFO |
                                                     (enableHoleCounting ? 0 : SST_CMDFIFO_DISABLE_HOLES) |
                                                     (agpEnable ?  SST_CMDFIFO_AGP : 0)));
  
  GDBG_INFO(80, "%s:  CMD FIFO placed at physical addr 0x%x\n", 
            FN_NAME,
            bInfo->fifoInfo.fifoStart);

  return FXTRUE;

#undef FN_NAME
} /* hwcInitFifo */

#if 0
/* Currently unused. */
static SstIORegs savedIORegs;
#endif

void
hwcInitVideoOverlaySurface(
  hwcRegInfo *rInfo,
    FxU32 enable,               /* 1=enable Overlay surface (OS), 1=disable */
    FxU32 stereo,               /* 1=enable OS stereo, 0=disable */
    FxU32 horizScaling,         /* 1=enable horizontal scaling, 0=disable */
    FxU32 dudx,                 /* horizontal scale factor (ignored if not */
      /* scaling) */
    FxU32 verticalScaling,      /* 1=enable vertical scaling, 0=disable */
    FxU32 dvdy,                 /* vertical scale factor (ignored if not */
      /* scaling) */
    FxU32 filterMode,           /* duh */
    FxU32 tiled,                /* 0=OS linear, 1=tiled */
    FxU32 pixFmt,               /* pixel format of OS */
    FxU32 clutBypass,           /* bypass clut for OS? */
    FxU32 clutSelect,           /* 0=lower 256 CLUT entries, 1=upper 256 */
    FxU32 startAddress,         /* board address of beginning of OS */
    FxU32 stride)               /* distance between scanlines of the OS, in */
  /* units of bytes for linear OS's and tiles for */
  /* tiled OS's */
{
  FxU32 doStride;
  FxU32 vidProcCfg;

  HWC_IO_LOAD((*rInfo), vidProcCfg, vidProcCfg);

  vidProcCfg &= ~(SST_OVERLAY_TILED_EN |
                  SST_OVERLAY_STEREO_EN |  
                  SST_OVERLAY_HORIZ_SCALE_EN |
                  SST_OVERLAY_VERT_SCALE_EN |
                  SST_OVERLAY_TILED_EN |
                  SST_OVERLAY_PIXEL_FORMAT |
                  SST_OVERLAY_CLUT_BYPASS |
                  SST_OVERLAY_CLUT_SELECT);

  if (enable)
    vidProcCfg |= SST_OVERLAY_EN;

  if (stereo)
    vidProcCfg |= SST_OVERLAY_STEREO_EN;

  if (horizScaling)
    vidProcCfg |= SST_OVERLAY_HORIZ_SCALE_EN;

  if (verticalScaling)
    vidProcCfg |= SST_OVERLAY_VERT_SCALE_EN;

  if (tiled) {
    vidProcCfg |= SST_OVERLAY_TILED_EN;
  }

  vidProcCfg |= pixFmt; /* pixFmt is preshifted */
  vidProcCfg &= ~SST_CURSOR_EN; /* Turn off HW Cursor */
  vidProcCfg |= SST_OVERLAY_TILED_EN; /* Overlay tile space enable */    


  if (clutBypass)
    vidProcCfg |= SST_OVERLAY_CLUT_BYPASS;

  if (clutSelect)
    vidProcCfg |= SST_OVERLAY_CLUT_SELECT;

  HWC_IO_STORE((*rInfo), vidProcCfg, vidProcCfg);

  /* */
  HWC_IO_LOAD((*rInfo), vidDesktopOverlayStride, doStride);
  doStride &= ~(SST_OVERLAY_LINEAR_STRIDE | SST_OVERLAY_TILE_STRIDE);

  stride <<= SST_OVERLAY_STRIDE_SHIFT;
  if (tiled)
    stride &= SST_OVERLAY_TILE_STRIDE;
  else
    stride &= SST_OVERLAY_LINEAR_STRIDE;
  doStride |= stride;

  HWC_IO_STORE((*rInfo), vidDesktopOverlayStride, doStride);

} /* hwcInitVideoOverlaySurface */

FxU32 
hwcInitLookupRefresh(FxU32 ord_refresh) 
{
#define FN_NAME "hwcInitLookupRefresh"
  FxU32 refresh_hz;

  switch(ord_refresh) {
  case(GR_REFRESH_60Hz):
    refresh_hz = 60;
    break;
  case(GR_REFRESH_70Hz):
    refresh_hz = 70;
    break;
  case(GR_REFRESH_72Hz):
    refresh_hz = 72;
    break;
  case(GR_REFRESH_75Hz):
    refresh_hz = 75;
    break;
  case(GR_REFRESH_80Hz):
    refresh_hz = 80;
    break;
  case(GR_REFRESH_85Hz):
    refresh_hz = 85;
    break;
  case(GR_REFRESH_90Hz):
    refresh_hz = 90;
    break;
  case(GR_REFRESH_100Hz):
    refresh_hz = 100;
    break;
  case(GR_REFRESH_120Hz):
    refresh_hz = 120;
    break;
  default:
    GDBG_ERROR(FN_NAME, "Unsupported Refresh Rate -- defaulting to 60hz\n");
    refresh_hz = 60;
    break;
  }
  return (refresh_hz);
#undef FN_NAME
} /* hwcInitLookupRefresh */

#ifdef HWC_ACCESS_DDRAW

#define LINEAR_STRIDE_ALIGN 16UL

static FxBool
_hwcLinear2HWAddr(const FxU32 linearAddr, 
                  const FxU32 linearBaseAddr,
                  const FxU32 linearStride,
                  const hwcBoardInfo* bInfo,
                  hwcSurfaceInfo* ret)
{
#define FN_NAME "_hwcLinear2HWAddr"
  FxU32
    pciStride,                  /* page stride */
    hwStride,                   /* tile stride */
    tileMark,                   /* delineation between linear and tiled */
    lpTileBase;                 /* linear addr of tileMark */
  FxBool
    retVal = FXFALSE,
    isTiled;                    /* is the surface tiled? */

  /* determine lfb baseAddress and hw offset to buffer */
  {
    hwcExtRequest_t ctxReq;
    hwcExtResult_t  ctxRes;

    /* query for tile watermark & compute tile characteristics */
    ctxReq.which = HWCEXT_GETDEVICECONFIG;
    ctxReq.optData.deviceConfigReq.dc = bInfo->hdc;
    ctxReq.optData.deviceConfigReq.devNo = bInfo->boardNum;
    GDBG_INFO(80, FN_NAME ":  ExtEscape:HWCEXT_GETDEVICECONFIG\n");

    retVal = (ExtEscape((HDC)bInfo->hdc, HWCEXT_ESCAPE(bInfo->boardNum),  /**/ 
                        sizeof(ctxReq), (LPSTR)&ctxReq, 
                        sizeof(ctxRes), (LPSTR)&ctxRes) > 0);
    if (!retVal) {
      hwc_errncpy(errorString, "HWCEXT_GETDEVICECONFIG failed");
      GDBG_INFO(80, "%s: %s.\n", FN_NAME, errorString);
      goto __errExit;
    }

    pciStride = ctxRes.optData.deviceConfigRes.pciStride;
    hwStride = ctxRes.optData.deviceConfigRes.hwStride;
    tileMark = ctxRes.optData.deviceConfigRes.tileMark;
    
    lpTileBase = linearBaseAddr + tileMark;
    isTiled = (linearAddr >= lpTileBase);
  }

  ret->pciStride = pciStride;
  ret->tileBase = tileMark;
  ret->lpSurface = linearAddr;
  ret->lpLFB = linearBaseAddr;
  ret->hwStride = hwStride;

  /* Compute things that depend on the offset wrt the tile mark */
  ret->isTiled = isTiled;
  if (ret->isTiled) {
    const FxU32
      tileStride = hwStride,
      tilePitch = pciStride,
      tileOffset = linearAddr - lpTileBase,
      y = tileOffset / tilePitch, 
      x = tileOffset - (y * tilePitch),
      tile = ((y / HWC_TILE_HEIGHT) * tileStride) + (x / HWC_TILE_WIDTH);

    ret->fbOffset = (tile << 12UL) + tileMark;
    ret->fbStride = ret->hwStride | SST_BUFFER_MEMORY_TILED;
  } else {
    /* NB: Banshee (and derivatives) have a 16-byte alignment
     * restriction on the stride and offset for color/aux buffers, but
     * it is left to the client to correctly adjust for this when
     * setting the values.  
     */
    ret->fbOffset = linearAddr - linearBaseAddr;
    ret->fbStride = linearStride;
  }

 __errExit:
  return retVal;
#undef FN_NAME
} /* _hwcLinear2HWAddr */

static FxU32
_hwcPixelFormatToBitDepth(const DDPIXELFORMAT* pixelFormat)
{
#define FN_NAME "_hwcPixelFormatToBitDepth"
  const DWORD
    formatFlags = pixelFormat->dwFlags;
  FxU32 
    retVal = 0x00UL;

  if (formatFlags & DDPF_FOURCC) {
    retVal = 4;
  } else if (formatFlags & DDPF_PALETTEINDEXED8) {
    retVal = 8;
  } else if (formatFlags & DDPF_RGB) {
    retVal = pixelFormat->dwRGBBitCount;
    if ((retVal == 0) || (retVal > 32)) {
      retVal = 0;
      GDBG_INFO(0, FN_NAME": Invalid surface rgb bit count(0x%X)\n",
                pixelFormat->dwRGBBitCount);
    }
  } else {
    GDBG_INFO(0, FN_NAME": Invalid surface pixel format flags(0x%X)\n",
              formatFlags);
  }

  return retVal;
#undef FN_NAME
} /* _hwcPixelFormatToBitDepth */

/*-------------------------------------------------------------------
  Function: hwcGetSurfaceInfo
  Date: 14-Jul-98
  Implementor(s): dow
  Description:
  Returns information about a Glide (or DDraw) surface

  Arguments:
  
  Return:
  -------------------------------------------------------------------*/
FxBool
hwcGetSurfaceInfo(const hwcBoardInfo* bInfo,
                  FxU32 *sfc, 
                  hwcSurfaceInfo *ret)
{
#define FN_NAME "hwcGetSurfaceInfo"
  /* AssUMe it's a DDraw surface for now */
  LPDIRECTDRAWSURFACE2
    surf = (LPDIRECTDRAWSURFACE2) sfc;
  DDSURFACEDESC
    desc;
  DWORD
    ddErr;
  FxBool 
    retVal;
  FxU32
    lfbBase,                    /* linear addr for base of LFB */
    lpSurface;                  /* Linear addr for the surface */

  /* Lock the surface and get some info */
  desc.dwSize = sizeof(desc);
  ddErr = IDirectDrawSurface2_Lock(surf, 0, &desc, 
                                   DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, 0);
  retVal = (ddErr == DD_OK);
  if (!retVal) {
    sprintf(errorString, "%s: IDirectDrawSurface2_Lock (0x%X)\n", 
            FN_NAME, ddErr);
    GDBG_INFO(80, "%s", errorString);
    goto __errExit;
  }

  /* Grab the memory pointer */
  lpSurface = (DWORD) desc.lpSurface;

  /* Unlock the surface */
  desc.dwSize = sizeof(desc);
  IDirectDrawSurface2_Unlock(surf, desc.lpSurface);

  /* Get surface dimensions here rather than re-getting the surface
   * description later since these should all be valid here.  
   */
  ret->height = desc.dwHeight;
  ret->width  = desc.dwWidth;
  ret->bitdepth  = _hwcPixelFormatToBitDepth(&desc.ddpfPixelFormat);
  retVal = (ret->bitdepth != 0x00UL);
  if (!retVal) {
    sprintf(errorString, "%s: Invalid surface pixel format (0x%X)\n", 
            FN_NAME, desc.ddpfPixelFormat.dwFlags);
    GDBG_INFO(80, "%s", errorString);
    goto __errExit;    
  }

  /* Get base addresses of the board */
  {
    hwcExtRequest_t ctxReq;
    hwcExtResult_t  ctxRes;

    /* This address was set when the board was mapped in hwcMapBoard */
    lfbBase = bInfo->linearInfo.linearAddress[1];
    
    /* The DirectDraw driver may have re-mapped memory different than
     * the 2d driver's hw mapping. We have to figure out the correct
     * hw address for packet offsets and color buffer stuff.  
     */ 

        ctxReq.which = HWCEXT_LINEAR_MAP_OFFSET;
        ctxReq.optData.mapInfoReq.mapAddr = lfbBase;
        ctxReq.optData.mapInfoReq.remapAddr = lpSurface;
        GDBG_INFO(80, FN_NAME ":  ExtEscape:HWCEXT_LINEAR_MAP_OFFSET\n");
    
      retVal = (ExtEscape((HDC)bInfo->hdc, HWCEXT_ESCAPE(bInfo->boardNum),     /**/
                          sizeof(ctxReq), (LPCSTR)&ctxReq,
                          sizeof(ctxRes), (LPSTR)&ctxRes) > 0);
      if (!retVal) {
        hwc_errncpy(errorString, "HWCEXT_LINEAR_MAP_OFFSET failed");
        GDBG_INFO(80, "%s: %s.\n", FN_NAME, errorString);
      }
      
      /* lpSurface is now an address relative to the 2d driver's base
       * address as returned by the HWCEXT_GETLINEARADDR callback.  
       */
      
        lpSurface = (lfbBase + ctxRes.optData.mapInfoRes.linAddrOffset);
  }
  if (!retVal) goto __errExit;

  /* Get the rest of the information about the hw relative address
   * from the linear address's location in memory.
   */
  retVal = _hwcLinear2HWAddr(lpSurface, 
                             lfbBase,
                             desc.lPitch,
                             bInfo,
                             ret);

 __errExit:  
  return retVal;
#undef FN_NAME
} /* hwcGetSurfaceInfo */

FxU32
hwcAllocWinContext(hwcBoardInfo* bInfo)
{
#define FN_NAME "hwcAllocWinContext"
  FxU32
    retVal = 0x00UL;

  hwcExtRequest_t ctxReq;
  hwcExtResult_t  ctxRes;

  /* Allocate a context with the Driver */
  ctxReq.which = HWCEXT_ALLOCCONTEXT;
  ctxReq.optData.allocContextReq.protocolRev = HWCEXT_PROTOCOLREV;
  ctxReq.optData.allocContextReq.appType = HWCEXT_ABAPPTYPE_WIND;
  GDBG_INFO(80, FN_NAME ":  ExtEscape:HWCEXT_ALLOCCONTEXT\n");

  if (ExtEscape((HDC)bInfo->hdc, HWCEXT_ESCAPE(bInfo->boardNum), /**/
                sizeof(ctxReq), (LPSTR)&ctxReq,
                sizeof(ctxRes), (LPSTR)&ctxRes) < 1) {
    hwc_errncpy(errorString, FN_NAME": HWCEXT_ALLOCCONTEXT failed");
  } else {
    retVal = ctxRes.optData.allocContextRes.contextID;
    GDBG_INFO(80, FN_NAME ":  contextID = %u\n", retVal);
  }    

  return retVal;
#undef FN_NAME
} /* hwcAllocWinContext */

FxBool
hwcFreeWinContext(hwcBoardInfo* bInfo,
                  FxU32         winContextId)
{
#define FN_NAME "hwcFreeWinContext"
  hwcExtRequest_t ctxReq;
  hwcExtResult_t  ctxRes;

  ctxReq.which = HWCEXT_RELEASECONTEXT;
  ctxReq.optData.releaseContextReq.contextID = winContextId;
#if ENABLE_V3_W2K_GLIDE_CHANGES
  ctxReq.optData.releaseContextReq.procID = bInfo->procHandle;
#endif
  GDBG_INFO(80, FN_NAME ":  ExtEscape:HWCEXT_RELEASECONTEXT\n");

  bInfo->isMapped = FXFALSE ;

  return (ExtEscape((HDC)bInfo->hdc, HWCEXT_ESCAPE(bInfo->boardNum),     /**/
                      sizeof(ctxReq), (LPSTR)&ctxReq,
                      sizeof(ctxRes), (LPSTR)&ctxRes) > 0);
#undef FN_NAME
} /* hwcFreeWinContext */

static HRESULT CALLBACK
enumSurfaceCallback(LPDIRECTDRAWSURFACE ddSurface, 
                    LPDDSURFACEDESC ddDesc, 
                    LPVOID procData)
{
  HRESULT retVal = DDENUMRET_OK;

  if (((ddDesc->dwFlags & DDSD_CAPS) != 0x00UL) &&
      ((ddDesc->ddsCaps.dwCaps & DDSCAPS_PRIMARYSURFACE) != 0x00UL)) {

    *(DDSURFACEDESC*)procData = *ddDesc;
    retVal = DDENUMRET_CANCEL;
  }

  /* The surface's refcount is bumped so we need to release since we
   * really only care about the surface's properties.
   */
  IDirectDrawSurface_Release(ddSurface);

  return retVal;
}


FxBool 
hwcAllocWinFifo(hwcBoardInfo* bInfo, 
                HwcWinFifo*   fifo,
                FxU32*        surface)
{
#define FN_NAME "hwcAllocWinFifo"
  FxBool 
    retVal = FXFALSE;
  FxU32 fifoType;

  hwcExtRequest_t ctxReq;
  hwcExtResult_t  ctxRes;

  DDSURFACEDESC
    primaryDesc;


  /* Set undefined fifo type to start, and always clear the serial
   * number for this context.
   */
  fifo->fifoType = 0xFFFFFFFFUL;

  /* Check to see that we have valid surface owner to hang
   * the command fifo surface off of.
   */
  retVal = (surface != NULL);
  if (retVal) {
    LPDIRECTDRAW
      objDD = NULL;
    LPDIRECTDRAWSURFACE
      objSentinal = NULL,
      objFifo = NULL;
    DDSURFACEDESC
      fifoDesc,
      sentinalDesc;
    DWORD
      surfaceWidth;

    /* Get the surface owner so that we can try to allocate a new
     * surface for the fifo. On windows this will be a direct draw
     * object, and we need a dd4 object for agp surfaces.
     */
    {
      LPDIRECTDRAWSURFACE2
        objSurface = NULL;
      
      /* Make sure that this is a Surface2 object so that we can get
       * the DirectDraw object that owns this surface and piggy back
       * our objects ontop of it.
       */
      retVal = (IDirectDrawSurface_QueryInterface((LPDIRECTDRAWSURFACE)surface, 
                                                  &IID_IDirectDrawSurface2,
                                                  (LPVOID *)&objSurface) == DD_OK);
      if (!retVal) {
        GDBG_INFO(80, "%s: Require atleast IDirectDrawSurface2.\n", FN_NAME);
        goto __errSurfaceFifo;
      }
      
      /* Get the dd object for the surface creation */
      retVal = (IDirectDrawSurface2_GetDDInterface(objSurface, (LPVOID *)&objDD) == DD_OK);
      if (!retVal) {
        GDBG_INFO(80, "%s: Could not acquire DirectDraw object.\n", FN_NAME);
        goto __errSurfaceDDObj;
      }

                                    
  __errSurfaceDDObj:
      if (objSurface != NULL) IDirectDrawSurface_Release(objSurface);
      if (!retVal) goto __errSurfaceFifo;
    }

    /* Is the 2d command fifo in agp or local frame buffer space? */
    memset(&ctxReq, 0, sizeof(ctxReq));
    memset(&ctxRes, 0, sizeof(ctxRes));
 
    ctxReq.which = HWCEXT_FIFOINFO;
    GDBG_INFO(80, FN_NAME ":  ExtEscape:HWCEXT_FIFOINFO\n");

    retVal = (ExtEscape((HDC)bInfo->hdc, HWCEXT_ESCAPE(bInfo->boardNum),       /**/
                        sizeof(ctxReq), (LPSTR)&ctxReq,
                        sizeof(ctxRes), (LPSTR)&ctxRes) > 0);
    if (!retVal) {
      GDBG_INFO(80, "%s: HWCEXT_FIFOINFO failed to determine current 2d fifo type.\n",
                FN_NAME);
      goto __errSurfaceFifo;
    }

    fifoType = ctxRes.optData.fifoInfoRes.fifoType;

    /* Basic surface type for the fifo */
    memset(&fifoDesc, 0, sizeof(fifoDesc));
    fifoDesc.dwSize  = sizeof(fifoDesc);
    fifoDesc.dwFlags = (DDSD_CAPS | 
                        DDSD_WIDTH | DDSD_HEIGHT |
                        DDSD_PITCH | 
                        DDSD_PIXELFORMAT);

    /* We want the command fifo someplace that the hw can get to
     * directly. Whether this is is local to the board or agp is
     * controlled by where the 2d driver has its fifo.  
     */
    fifoDesc.ddsCaps.dwCaps = (DDSCAPS_OFFSCREENPLAIN |
                               DDSCAPS_VIDEOMEMORY);

    {
      DDCAPS
        ddCaps;
      /* Figure out the things that the current DirectDraw driver
       * knows about. We need to know whether it can support creating
       * agp and wider than primary surfaces.  
       */
      memset(&ddCaps, 0, sizeof(ddCaps));
      ddCaps.dwSize = sizeof(ddCaps);
      
      retVal = (IDirectDraw_GetCaps(objDD, &ddCaps, NULL) == DD_OK);
      if (!retVal) {
        GDBG_INFO(80, "%s: Could not determine if DirectDraw handles separate memory types.\n",
                  FN_NAME);
        goto __errSurfaceFifo;
      }
      
      /* Make the fifo type match the 2d fifo type if the driver can
       * handle different memory types.  
       */

      switch (fifoType) {        
      case HWCEXT_FIFO_AGP:
        if ((ddCaps.dwCaps2 & DDCAPS2_NONLOCALVIDMEM) != 0x00UL) {
          fifoDesc.ddsCaps.dwCaps |= DDSCAPS_NONLOCALVIDMEM;
        } else {
          GDBG_INFO(80, "%s: AGP fifo w/o DirectDraw support for AGP surfaces.\n", FN_NAME);
          retVal = FXFALSE;
        }
        break;

      case HWCEXT_FIFO_FB:
        if ((ddCaps.ddsCaps.dwCaps & DDSCAPS_LOCALVIDMEM) != 0x00UL) 
          fifoDesc.ddsCaps.dwCaps |= DDSCAPS_LOCALVIDMEM;
        break;
        
      case HWCEXT_FIFO_HOST:
      default:
        GDBG_INFO(80, "%s: Unknown driver fifo type.\n", FN_NAME);
        retVal = FXFALSE;
        break;
      }
      if (!retVal) goto __errSurfaceFifo;

      /* Get the primary surface so that we can figure out other whacked
       * surface creation constraints taht we have to adhere to. 
       */
      primaryDesc.dwSize = 0x00UL;
      retVal = (IDirectDraw_EnumSurfaces(objDD,
                                         DDENUMSURFACES_DOESEXIST | DDENUMSURFACES_ALL,
                                         NULL, 
                                         &primaryDesc,
                                         enumSurfaceCallback) == DD_OK);
      if (!retVal || (primaryDesc.dwSize == 0x00UL)) {
        GDBG_INFO(80, "%s: Could not find primary surface.\n", FN_NAME);
        goto __errSurfaceFifo;
      }

      /* If the DirectDraw driver supports wide surfaces then we can
       * specify what will align well w/ the underlying hw's page
       * size.  Otherwise we need to check the primary surface's
       * dimensions to fit the 'old' rule that no surface's dimensions
       * can exceed the primary's. After we allocate the surface we
       * check to make sure that the width and stride are the 'same'
       * since it is probably not a good thing to use the 'slop' when
       * the stride > width.
       */
      if ((ddCaps.dwCaps2 & DDCAPS2_WIDESURFACES) == 0x00UL) {
        retVal = ((primaryDesc.dwFlags & DDSD_WIDTH) != 0x00UL);
        if (!retVal) {
          GDBG_INFO(80, "%s: Primary surface does not have width.\n", FN_NAME);
          goto __errSurfaceFifo;
        }
        surfaceWidth = primaryDesc.dwWidth;
      } else {
        GDBG_INFO(80, "%s: DirectDraw supports wide surfaces.\n", FN_NAME);
        surfaceWidth = (0x1000UL >> 2UL);
      }
    }

    /* Allocate a sentinal surface so that the client can 'know' when
     * a given fifo segment has actually been executed by reading a
     * client specified serial # out of the buffer. This only needs to
     * be big enough to hold the size of a serial #, but we need to
     * work around some sort of whacked agp bug when reading back. See
     * hwcExecuteStatusWinFifo() for details.
     */
      fifoDesc = primaryDesc;
      fifoDesc.ddsCaps.dwCaps = (DDSCAPS_OFFSCREENPLAIN |
                               DDSCAPS_VIDEOMEMORY);
      fifoDesc.dwFlags = (DDSD_CAPS | 
                        DDSD_WIDTH | DDSD_HEIGHT |
                        DDSD_PITCH | 
                        DDSD_PIXELFORMAT);

      sentinalDesc = fifoDesc;
        {
                int Size;
                switch( sentinalDesc.ddpfPixelFormat.dwRGBBitCount )
                {
                        case 8:
                                Size = 16;
                                break;
                        case 16:
                                Size = 8;
                                break;
                        case 24:
                                Size = 6;
                                break;
                        default:        
                                Size = 4;
                }
            sentinalDesc.dwHeight = Size;
                sentinalDesc.dwWidth  = Size;
        }    
    retVal = (IDirectDraw_CreateSurface(objDD,
                                        &sentinalDesc,
                                        &objSentinal,
                                        0) == DD_OK);
    if (!retVal) {
      GDBG_INFO(80, "%s: Could not allocate surface for serial #'s.\n", FN_NAME);
      goto __errSurfaceFifo;
    }

    /* Make stride and width the 'same' so that we get the entire
     * surface as teh command fifo for the hw to just jsr to.
     */
    fifoDesc.dwWidth = surfaceWidth;
    fifoDesc.lPitch  = (fifoDesc.dwWidth*(fifoDesc.ddpfPixelFormat.dwRGBBitCount>>3) );

    /* Try to allocate a surface that contains some # of command
     * stream and persistant state buffers. This is
     * allocated in hw page units (4k) rather than the client
     * allocation units, but that should be transparent since
     * the client will just not use the extra slop.
     */
#define MAX(__a, __b) (((__a) > (__b)) ? (__a) : (__b))
    {
      const FxU32
        allocFifo  = MAX(fifo->cmdBuf.allocUnit, 0x1000UL),
        allocState = MAX(fifo->stateBuf.allocUnit, 0x1000UL),
        allocUnit  = pow2Round(allocFifo + allocState + 0xFFFUL, 0x1000UL);
      const char*
        numAllocStr = GETENV("FX_WINFIFO_INIT_ALLOC", bInfo->RegPath);
      FxU32
        numAlloc   = ((numAllocStr == NULL)
                      ? 0x8UL
                      : atoi(numAllocStr));
      DWORD 
        ddErr;

      while(numAlloc > 0) {
        fifoDesc.dwHeight = (allocUnit * numAlloc) / fifoDesc.lPitch;
        ddErr = IDirectDraw_CreateSurface(objDD,
                                          &fifoDesc,
                                          &objFifo,
                                          0);

        if (ddErr == DD_OK)
        {
          /* AJB- Check for tiled fifo */
          hwcSurfaceInfo       theSurfaceInfo ; 
       
          hwcGetSurfaceInfo(bInfo, (DWORD*)objFifo, &theSurfaceInfo) ;
          retVal = (theSurfaceInfo.isTiled == FXFALSE) ;
        }

        if (retVal) break;

        if ((ddErr != DDERR_OUTOFVIDEOMEMORY) ||
            (ddErr != DDERR_OUTOFMEMORY)) break;

        /* Try a smaller allocation */
        numAlloc--;
      }
      if (!retVal) {
        GDBG_INFO(80, "%s: Could not get cmdFifo DirectDraw surface. (0x%X)\n",
                  FN_NAME, ddErr);
        goto __errSurfaceFifo;
      }

      /* Fill in the fifo type for lock. */
      fifo->fifoType = fifoType;
      
      /* Do the lock so that we can write the sentinal, but the
       * client is responsible for locking before use.
       */
          fifo->surfaceFifo     = (FxU32*)objFifo;
          fifo->surfaceSentinal = (FxU32*)objSentinal;

      retVal = hwcLockWinFifo(bInfo, fifo);
      if (!retVal) goto __errSurfaceFifo;
      
      /* Write bogus serial #. */
      *(volatile FxU32*)fifo->sentinalBufferAddr = 0x0UL;    
      hwcUnlockWinFifo(bInfo, fifo);
      
      /* Its now safe to overwrite the client allocUnit's to the
       * rounded allocation units because we've finally done all of
       * the stuff that could fail.
       */
      fifo->cmdBuf.size = allocFifo * numAlloc;
      fifo->cmdBuf.allocUnit = allocFifo;
      
      fifo->stateBuf.size = allocState * numAlloc;
      fifo->stateBuf.allocUnit = allocState;      
    }
#undef MAX
    
 __errSurfaceFifo:
    /* Free our directdraw objects if something bad happened. */
    if (!retVal) {
      if (objSentinal != NULL) IDirectDrawSurface_Release(objSentinal);
      if (objFifo != NULL) IDirectDrawSurface_Release(objFifo);
    }
    if (objDD != NULL) IDirectDraw_Release(objDD);
  }

  /* Could not allocate directly accessible fifo so setup the host
   * memory backed fifo which will get dumped at execute time.  
   */
  if (!retVal && (bInfo->hdc != 0x00UL)) {
    fifo->fifoType          = HWCEXT_FIFO_HOST;

    fifo->cmdBuf.hwOffset    = 0x00UL;
    fifo->cmdBuf.size   = fifo->cmdBuf.allocUnit;
    fifo->stateBuf.hwOffset  = 0x00UL;
    fifo->stateBuf.size = fifo->stateBuf.allocUnit;

    /* This can never fail since the client is responsible for
     * allocating this memory.  
     */
    retVal = FXTRUE;
  }

  GDBG_INFO(80, "%s: retVal(0x%X) Type(0x%X)\n", 
            FN_NAME, retVal, fifo->fifoType);

  return retVal;
#undef FN_NAME
} /* hwcAllocWinFifo */

FxBool
hwcLockWinFifo(hwcBoardInfo* bInfo,
               HwcWinFifo* fifo)
{
#define FN_NAME "hwcLockWinFifo"
  FxBool 
    retVal = FXFALSE;

  if (fifo->fifoType == HWCEXT_FIFO_HOST) {
    retVal = FXTRUE;
  } else if ((fifo->fifoType == HWCEXT_FIFO_FB) ||
             (fifo->fifoType == HWCEXT_FIFO_AGP)) {
    DDSURFACEDESC
      fifoDesc,
      sentinalDesc;

    if (fifo->lockCount > 0) goto __alreadyLocked;
    if ((fifo->surfaceFifo == NULL) ||
        (fifo->surfaceSentinal == NULL)) goto __errFifoLock;

    /* Get base pointer to the fifo surfaces.
     *
     * NB: In theory we should leave these locked for the duration of
     * the of the fifo lock and we would need to set the
     * DDLOCK_NOSYSLOCK flag when doing the lock. However, when we do
     * this type of lock the directDraw driver re-maps the physical
     * address to a new set of linear address pages. So we take advantage
     * of the fact taht the current implementation will not move memory
     * behind our backs outside of a surface lock.
     */
#define HWC_DIRECTDRAW_DRIVER_NOSYSLOCK_FLAGS 0 
    fifoDesc.dwSize = sizeof(fifoDesc);
    retVal = (IDirectDrawSurface_Lock((LPDIRECTDRAWSURFACE)fifo->surfaceFifo,
                                      NULL, 
                                      &fifoDesc, 
                                      HWC_DIRECTDRAW_DRIVER_NOSYSLOCK_FLAGS |
                                      DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT,
                                      NULL) == DD_OK);

    /* NB: Since there's no working DDLOCK_NOSYSLOCK we unlock the surface
     * here, and rely on the current implementation to leave the linear 
     * mapping to the hw intact after the unlocking.
     */
    IDirectDrawSurface_Unlock((LPDIRECTDRAWSURFACE)fifo->surfaceFifo, NULL);
    if (!retVal) {
      hwc_errncpy(errorString, "Could not lock cmdFifo surface");
      GDBG_INFO(80, "%s: %s.\n", FN_NAME, errorString);
      goto __errFifoLock;
    }

    sentinalDesc.dwSize  = sizeof(sentinalDesc);
    retVal = (IDirectDrawSurface_Lock((LPDIRECTDRAWSURFACE)fifo->surfaceSentinal, 
                                      NULL, 
                                      &sentinalDesc, 
                                      HWC_DIRECTDRAW_DRIVER_NOSYSLOCK_FLAGS |
                                      DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, 
                                      NULL) == DD_OK);

    /* NB: Since there's no working DDLOCK_NOSYSLOCK we unlock the surface
     * here, and rely on the current implementation to leave the linear 
     * mapping to the hw intact after the unlocking.
     */
    IDirectDrawSurface_Unlock((LPDIRECTDRAWSURFACE)fifo->surfaceSentinal, NULL);
    if (!retVal) {
      hwc_errncpy(errorString, "Could not lock serial # surface");
      GDBG_INFO(80, "%s: %s.\n", FN_NAME, errorString);
      goto __errFifoLock;
    }

    fifo->cmdBuf.baseAddr   = (FxU32)fifoDesc.lpSurface;
    fifo->stateBuf.baseAddr = (fifo->cmdBuf.baseAddr + 
                                      fifo->cmdBuf.size);
    fifo->sentinalBufferAddr       = (FxU32)sentinalDesc.lpSurface;

    /* The DirectDraw driver may have re-mapped memory different than
     * the 2d driver's hw mapping. We have to figure out the correct
     * hw address for packet offsets and color buffer stuff.  
     */
    {
      hwcExtRequest_t ctxReq;
      hwcExtResult_t  ctxRes;

      const FxU32
        lfbBase = bInfo->linearInfo.linearAddress[1]; /* lfb space */
      FxU32
        tileMark;      
      
      /* query for tile watermark & compute tile characteristics */
      ctxReq.which = HWCEXT_GETDEVICECONFIG;
      ctxReq.optData.deviceConfigReq.dc = bInfo->hdc;
      ctxReq.optData.deviceConfigReq.devNo = bInfo->boardNum;
      GDBG_INFO(80, FN_NAME ":  ExtEscape:HWCEXT_GETDEVICECONFIG\n");
      
      retVal = (ExtEscape((HDC)bInfo->hdc, HWCEXT_ESCAPE(bInfo->boardNum), /**/
                          sizeof(ctxReq), (LPSTR)&ctxReq, 
                          sizeof(ctxRes), (LPSTR)&ctxRes) > 0);
      if (!retVal) {
        hwc_errncpy(errorString, "HWCEXT_GETDEVICECONFIG failed");
        GDBG_INFO(80, "%s: %s.\n", FN_NAME, errorString);
        goto __errFifoLock;
      }
      
      tileMark = ctxRes.optData.deviceConfigRes.tileMark;
      

      {
        struct remapRec {
          FxU32  remapAddr;
          FxU32* remapOffset;
          FxU32  surfaceType;
        } remapAddrList[] = {
          {    fifo->cmdBuf.baseAddr,      &fifo->cmdBuf.hwOffset, fifo->fifoType },
          {  fifo->stateBuf.baseAddr,    &fifo->stateBuf.hwOffset, fifo->fifoType },
          { fifo->sentinalBufferAddr, &fifo->sentinalBufferOffset, HWCEXT_FIFO_FB }
        };
        FxU32 i;

        for(i = 0; i < sizeof(remapAddrList) / sizeof(remapAddrList[0]); i++) {
          FxU32 hwOffset;

          ctxReq.which = HWCEXT_LINEAR_MAP_OFFSET;
          ctxReq.optData.mapInfoReq.mapAddr = lfbBase;
          ctxReq.optData.mapInfoReq.remapAddr = remapAddrList[i].remapAddr;
          GDBG_INFO(80, FN_NAME ":  ExtEscape:HWCEXT_LINEAR_MAP_OFFSET\n");

          retVal = (ExtEscape((HDC)bInfo->hdc, HWCEXT_ESCAPE(bInfo->boardNum), /**/
                              sizeof(ctxReq), (LPCSTR)&ctxReq,
                              sizeof(ctxRes), (LPSTR)&ctxRes) > 0);
          if (!retVal) {
            hwc_errncpy(errorString, "HWCEXT_LINEAR_MAP_OFFSET failed");
            GDBG_INFO(80, "%s: %s.\n", FN_NAME, errorString);
            goto __errFifoLock;
          }

          /* Make sure we have the right offset for tiled/linear addresses */
          hwOffset = ctxRes.optData.mapInfoRes.linAddrOffset;
          
          if ((remapAddrList[i].surfaceType == HWCEXT_FIFO_FB) && (hwOffset >= tileMark)) {
            hwcSurfaceInfo surfaceInfo;

            retVal = _hwcLinear2HWAddr(remapAddrList[i].remapAddr, 
                                       remapAddrList[i].remapAddr - hwOffset,
                                       0x00UL,
                                       bInfo,
                                       &surfaceInfo);
            if (!retVal) goto __errFifoLock;
            hwOffset = surfaceInfo.fbOffset;
          }
          *remapAddrList[i].remapOffset = hwOffset;
        }
      }
    }

   __alreadyLocked:
    /* Finally, declare success */
    retVal = FXTRUE;
    
   __errFifoLock:
    ;
  }

  if (retVal) fifo->lockCount++;

  return retVal;
#undef FN_NAME
} /* hwcLockWinFifo */

FxBool
hwcUnlockWinFifo(hwcBoardInfo* bInfo,
                 HwcWinFifo* fifo)
{
#define FN_NAME "hwcUnlockWinFifo"
  FxBool
    retVal = ((fifo->lockCount > 0) &&
              ((fifo->fifoType == HWCEXT_FIFO_HOST) ||
               (fifo->fifoType == HWCEXT_FIFO_FB) ||
               (fifo->fifoType == HWCEXT_FIFO_AGP)));

  if (retVal) {
    fifo->lockCount--;

    if (fifo->lockCount == 0) {
      if ((fifo->fifoType == HWCEXT_FIFO_FB) ||
          (fifo->fifoType == HWCEXT_FIFO_AGP)) {
        /* Do nothing for now since the surfaces were unlocked in
         * hwcLockWinFifo since we're not using the NOSYSLOCK flags
         * when doing the DirectDraw surface lock.  
         */
      }
    }
  }

  return retVal;
#undef FN_NAME
} /* hwcUnlockWinFifo */

FxBool
hwcFreeWinFifo( hwcBoardInfo* bInfo,
                HwcWinFifo* fifo )
{
#define FN_NAME "hwcFreeWinFifo"

  /* What type of fifo do we have? */
  if ((fifo->fifoType == HWCEXT_FIFO_FB) ||
      (fifo->fifoType == HWCEXT_FIFO_AGP)) {
    if (fifo->surfaceFifo != NULL) {
      LPDIRECTDRAWSURFACE
        objFifo = (LPDIRECTDRAWSURFACE)fifo->surfaceFifo;

      IDirectDrawSurface_Release(objFifo);

      fifo->surfaceFifo = NULL;
      fifo->cmdBuf.baseAddr = 
      fifo->cmdBuf.hwOffset = 
      fifo->cmdBuf.size = 
      fifo->stateBuf.baseAddr = 
      fifo->stateBuf.hwOffset = 0x00UL;
    }
    
    if (fifo->surfaceSentinal != NULL) {
      LPDIRECTDRAWSURFACE
        objSentinal = (LPDIRECTDRAWSURFACE)fifo->surfaceSentinal;

      IDirectDrawSurface_Release(objSentinal);

      fifo->surfaceSentinal = NULL;
      fifo->sentinalBufferAddr =
      fifo->sentinalBufferOffset = 0x00UL;
    }
  }
  
  fifo->fifoType = HWCEXT_FIFO_INVALID;

  return FXTRUE;
#undef FN_NAME
} /* hwcFreeWinFifo */

FxBool
hwcExecuteWinFifo(hwcBoardInfo*     bInfo, 
                  const FxU32       winContextId,
                  const HwcWinFifo* fifo,
                  const FxU32       serialNumber)
{
#define FN_NAME "hwcExecuteWinFifo"

  hwcExtRequest_t ctxReq;
  hwcExtResult_t  ctxRes;

  memset( &ctxReq, 0, sizeof( ctxReq ) );
  memset( &ctxRes, 0, sizeof( ctxRes ) );

  ctxReq.which = HWCEXT_EXECUTEFIFO;
  ctxReq.contextID = winContextId;

  ctxReq.optData.executeFifoReq.fifoType     = fifo->fifoType;

  ctxReq.optData.executeFifoReq.fifoPtr      = fifo->cmdBuf.baseAddr;
  ctxReq.optData.executeFifoReq.fifoSize     = fifo->cmdBuf.size >> 2UL;
  ctxReq.optData.executeFifoReq.statePtr     = fifo->stateBuf.baseAddr;
  ctxReq.optData.executeFifoReq.stateSize    = fifo->stateBuf.size >> 2UL;
  ctxReq.optData.executeFifoReq.serialNumber = serialNumber;

  if (fifo->fifoType != HWCEXT_FIFO_HOST) {
    ctxReq.optData.executeFifoReq.fifoOffset     = fifo->cmdBuf.hwOffset;
    ctxReq.optData.executeFifoReq.stateOffset    = fifo->stateBuf.hwOffset;
    ctxReq.optData.executeFifoReq.sentinalOffset = fifo->sentinalBufferOffset;
  }
  GDBG_INFO(80, FN_NAME": Id(0x%X) state(0x%X) cmd(0x%X)\n",
          winContextId, 
          ctxReq.optData.executeFifoReq.stateSize,
          ctxReq.optData.executeFifoReq.fifoSize);
  GDBG_INFO(80, FN_NAME ":  ExtEscape:HWCEXT_EXECUTEFIFO\n");

#if ENABLE_V3_W2K_GLIDE_CHANGES
  ctxReq.optData.executeFifoReq.procID = bInfo->procHandle;
#endif

  return ( ExtEscape( (HDC)bInfo->hdc, HWCEXT_ESCAPE(bInfo->boardNum),  /**/
                      sizeof( ctxReq ), (LPSTR)&ctxReq, 
                      sizeof( ctxRes ), (LPSTR)&ctxRes )  > 0 );
#undef FN_NAME
} /* hwcExecuteWinFifo */

FxU32
hwcExecuteStatusWinFifo(hwcBoardInfo*     bInfo,
                        const HwcWinFifo* fifo,
                        const FxU32       serialNumber)
{
#define FN_NAME "hwcExecuteStatusWinFifo"
  FxU32
    retVal = ~0x00UL;

  switch(fifo->fifoType) {
  /* Host fifo always executes synchronously */
  case HWCEXT_FIFO_HOST:
    retVal = serialNumber;
    break;

  case HWCEXT_FIFO_FB:
  case HWCEXT_FIFO_AGP:
    {
      volatile FxU32*
        bufAddr = (volatile FxU32*)fifo->sentinalBufferAddr;

      /* We need to do some extra reads here so that we make sure taht
       * the data we're returning is coherent w/ the actual frame
       * buffer due to the intervening pixel cache. The cache is,
       * currently, 4 dwords, and appears to be aligned on this
       * boundary as well so reading 4 dwords away should be flushing.
       *
       * NB: The surface for the serial # was allocated big enough to
       * handle this so we would not have to worry about it here.  
       */
      retVal = bufAddr[0];
      retVal = bufAddr[0 + 4];
      retVal = bufAddr[0];
    }
    break;

  case HWCEXT_FIFO_INVALID:
  default:
    GDBG_ERROR(FN_NAME, "Invalid fifoType");
    break;
  }

  return retVal;
#undef FN_NAME
} /* hwcExecuteStatusWinFifo */

#endif /* HWC_ACCESS_DDRAW */

#ifdef HWC_ACCESS_GDX

#if GLIDE3
FxBool
hwcGetSurfaceInfo(const hwcBoardInfo* bInfo,
                  FxU32 *sfc, 
                  hwcSurfaceInfo *ret)
{
  GrSurfaceDesc_t desc;
  
  hrmDeviceConfig_t config;
  
  _hrmGetDeviceConfig((hrmBoard_t *)bInfo->hMon, &config);
  
  ret->tileBase  = config.tileMark;
  ret->lpLFB     = config.lfbBase;
  ret->pciStride = config.pciStride;
  ret->hwStride  = config.hwStride;
  
  gdxSurfaceGetDesc(sfc, &desc);
  
  ret->lpSurface = desc.surface;
  ret->width = desc.width;
  ret->height = desc.height;
  ret->bitdepth = desc.bytesPerPixel * 8;
  ret->fbStride = desc.pitch;
  ret->fbOffset = ret->lpSurface - ret->lpLFB;
  ret->isTiled = (ret->fbOffset >= ret->tileBase) ? FXTRUE : FXFALSE;
  
  return FXTRUE;
} /* hwcGetSurfaceInfo */
#endif

FxU32
hwcAllocWinContext(hwcBoardInfo* bInfo)
{       
  return _hrmAllocWinContext((hrmBoard_t *)bInfo->hMon);  
} /* hwcAllocWinContext */

FxBool
hwcFreeWinContext(hwcBoardInfo* bInfo,
                  FxU32         winContextId)
{
  _hrmFreeWinContext((hrmBoard_t *)bInfo->hMon, winContextId);
  return FXTRUE;  
} /* hwcFreeWinContext */

#if GLIDE3
FxBool 
hwcAllocWinFifo(hwcBoardInfo* bInfo, 
                HwcWinFifo*   fifo,
                FxU32*        surface)
{
#define FN_NAME "hwcAllocWinFifo"
  FxBool 
    retVal = FXFALSE;

  /* Set undefined fifo type to start, and always clear the serial
   * number for this context.
   */
  fifo->fifoType = 0xFFFFFFFFUL;

  /* Check to see that we have valid surface owner to hang
   * the command fifo surface off of.
   */
  retVal = (surface != NULL);
  if (retVal) {
    hrmBoard_t *theBoard;
    GrSurface_t objSentinal = NULL,
                objFifo = NULL;
    GrSurfaceDesc_t
                fifoDesc,
                sentinalDesc;
                 
    /* We always create our surfaces based on the bInfo */
    theBoard = (hrmBoard_t *)bInfo->hMon;

    memset(&fifoDesc, 0, sizeof(fifoDesc));

    /* Allocate a sentinal surface so that the client can 'know' when
     * a given fifo segment has actually been executed by reading a
     * client specified serial # out of the buffer. This only needs to
     * be big enough to hold the size of a serial #, but we need to
     * work around some sort of whacked agp bug when reading back. See
     * hwcExecuteStatusWinFifo() for details.
     */
    sentinalDesc = fifoDesc;

    sentinalDesc.height = 4*4;
    sentinalDesc.width  = 4;
    sentinalDesc.bytesPerPixel = 1;
    
    objSentinal = gdxSurfaceAlloc(bInfo, theBoard, &sentinalDesc);

    
    if (!objSentinal) {
      GDBG_INFO(80, "%s: Could not allocate surface for serial #'s.\n", FN_NAME);
      retVal = FXFALSE;
      goto __errSurfaceFifo;
    }

        /* Assume for now that 2D command fifo is in local framebuffer space */
 
    /* Make pitch and width the same so that indexing is easy. 
     * Setup the allocation in hw page unit sizes. (4k bytes)
     */
    fifoDesc.pitch      = 0x1000UL;
    fifoDesc.width  = (fifoDesc.pitch >> 2UL);
    fifoDesc.bytesPerPixel = 4;
    
    /* Try to allocate a surface that contains some # of command
     * stream buffers and one persistant state buffer. This is
     * allocated in hw page units (4k) rather than the client
     * allocation units, but that should be transparent since
     * the client will just not use the extra slop.
     */
#define MAX(__a, __b) (((__a) > (__b)) ? (__a) : (__b))
    {
      const FxU32
        allocFifo  = MAX(fifo->cmdBuf.allocUnit, 0x1000UL),
        allocState = MAX(fifo->stateBuf.allocUnit, 0x1000UL),
        allocUnit  = pow2Round(allocFifo + allocState + 0xFFFUL, 0x1000UL);
      const char*
        numAllocStr = GETENV("FX_WINFIFO_INIT_ALLOC", bInfo->RegPath);
      FxU32
        numAlloc   = ((numAllocStr == NULL)
                      ? 0x8UL
                      : atoi(numAllocStr));

      while(numAlloc > 0) {
        fifoDesc.height = (allocUnit * numAlloc) / fifoDesc.pitch;
        objFifo = NULL;
        /* objFifo = gdxSurfaceAlloc(theBoard,&fifoDesc); */
        if(objFifo)
                break;

        /* Try a smaller allocation */
        numAlloc--;
      }
      if(!objFifo) {
        GDBG_INFO(80, "%s: Could not get cmdFifo GDX Surface.\n",
                  FN_NAME);
        retVal = FXFALSE;
        goto __errSurfaceFifo;
      }
      
      /* Its now safe to overwrite the client allocUnit's to the
       * rounded allocation units because we've finally done all of
       * the stuff that could fail.
       */
      fifo->cmdBuf.size = allocFifo * numAlloc;
      fifo->cmdBuf.allocUnit = allocFifo;

      fifo->stateBuf.size = allocState * numAlloc;
      fifo->stateBuf.allocUnit = allocState;
    }
#undef MAX
      
        retVal = FXTRUE;
    /* Fill in some more of the return buffer info, but were not quite
     * done and ready to declare success until we lock the fifo down.  
     */
    fifo->fifoType = HWCEXT_FIFO_FB;

    fifo->surfaceFifo     = (FxU32*)objFifo;
    fifo->surfaceSentinal = (FxU32*)objSentinal;

    /* Do the lock so that we can write the sentinal, but the
     * client is responsible for locking before use.
     */
    if (hwcLockWinFifo(bInfo, fifo)) {
      /* Write bogus serial # */
      *(volatile FxU32*)fifo->sentinalBufferAddr = 0x0UL;
    
      hwcUnlockWinFifo(bInfo, fifo);
    }
    
 __errSurfaceFifo:
    /* Free our directdraw objects if something bad happened. */
    if (!retVal) {
      if (objSentinal != NULL) gdxSurfaceFree(objSentinal);
      if (objFifo != NULL) gdxSurfaceFree(objFifo);
    }
  }

  /* Could not allocate directly accessible fifo so setup the host
   * memory backed fifo which will get dumped at execute time.  
   */
  if (!retVal) {
    fifo->fifoType          = HWCEXT_FIFO_HOST;

    fifo->cmdBuf.hwOffset    = 0x00UL;
    fifo->cmdBuf.size   = fifo->cmdBuf.allocUnit;
    fifo->stateBuf.hwOffset  = 0x00UL;
    fifo->stateBuf.size = fifo->stateBuf.allocUnit;

    /* This can never fail since the client is responsible for
     * allocating this memory.  
     */
    retVal = FXTRUE;
  }

  return retVal;
#undef FN_NAME
}                

FxBool
hwcLockWinFifo(hwcBoardInfo* bInfo,
               HwcWinFifo* fifo)
{
#define FN_NAME "hwcLockWinFifo"
  FxBool 
    retVal = FXFALSE;

  if (fifo->fifoType == HWCEXT_FIFO_HOST) {
    retVal = FXTRUE;
  } else if ((fifo->fifoType == HWCEXT_FIFO_FB) ||
             (fifo->fifoType == HWCEXT_FIFO_AGP)) {
       
    GrSurfaceDesc_t 
        fifoDesc,
        sentinalDesc;
              
    if (fifo->lockCount > 0) goto __alreadyLocked;
    if ((fifo->surfaceFifo == NULL) ||
        (fifo->surfaceSentinal == NULL)) goto __errFifoLock;

    /* Get base pointer to the fifo surfaces.
     */
     
    gdxSurfaceGetDesc(fifo->surfaceFifo,&fifoDesc);
    gdxSurfaceGetDesc(fifo->surfaceSentinal,&sentinalDesc);
    
    /* Since there's no remapping crap in MacOS, this is easy. */
    fifo->cmdBuf.baseAddr               = fifoDesc.surface; 
    fifo->stateBuf.baseAddr     = fifo->cmdBuf.baseAddr + fifo->cmdBuf.size;
    fifo->sentinalBufferAddr    = sentinalDesc.surface;

        /* Compute hardware offsets as well */
    fifo->cmdBuf.hwOffset =
      fifo->cmdBuf.baseAddr - bInfo->regInfo.rawLfbBase;
    fifo->stateBuf.hwOffset =
      fifo->stateBuf.baseAddr - bInfo->regInfo.rawLfbBase;
    fifo->sentinalBufferOffset =
      fifo->sentinalBufferAddr - bInfo->regInfo.rawLfbBase;
                
    __alreadyLocked:

    /* Finally, declare success */
    retVal = FXTRUE;
    
   __errFifoLock:
    ;
  }
  
  if (retVal) fifo->lockCount++;
  
  return retVal;
#undef FN_NAME
} /* hwcLockWinFifo */

FxBool
hwcUnlockWinFifo(hwcBoardInfo* bInfo,
                 HwcWinFifo* fifo)
{
#define FN_NAME "hwcUnlockWinFifo"
  FxBool
    retVal = ((fifo->lockCount > 0) &&
              ((fifo->fifoType == HWCEXT_FIFO_HOST) ||
               (fifo->fifoType == HWCEXT_FIFO_FB) ||
               (fifo->fifoType == HWCEXT_FIFO_AGP)));

  if (retVal) {
    fifo->lockCount--;

    if (fifo->lockCount == 0) {
      if ((fifo->fifoType == HWCEXT_FIFO_FB) ||
          (fifo->fifoType == HWCEXT_FIFO_AGP)) {
        /* Don't currently do anything since surfaces don't get moved 
         * around yet on MacOS.
         */
      }
    }
  }

  return retVal;
#undef FN_NAME
} /* hwcUnlockWinFifo */

FxBool
hwcFreeWinFifo( hwcBoardInfo* bInfo,
                HwcWinFifo* fifo )
{
#define FN_NAME "hwcFreeWinFifo"

  /* What type of fifo do we have? */
  if ((fifo->fifoType == HWCEXT_FIFO_FB) ||
      (fifo->fifoType == HWCEXT_FIFO_AGP)) {
    if (fifo->surfaceFifo != NULL) {
        gdxSurfaceFree(fifo->surfaceFifo);

      fifo->surfaceFifo = NULL;
      fifo->cmdBuf.baseAddr = 
      fifo->cmdBuf.hwOffset = 
      fifo->cmdBuf.size = 
      fifo->stateBuf.baseAddr = 
      fifo->stateBuf.hwOffset = 0x00UL;
    }
    
    if (fifo->surfaceSentinal != NULL) {
        gdxSurfaceFree(fifo->surfaceSentinal);

      fifo->surfaceSentinal = NULL;
      fifo->sentinalBufferAddr =
      fifo->sentinalBufferOffset = 0x00UL;
    }
  }
  
  fifo->fifoType = HWCEXT_FIFO_INVALID;

  return FXTRUE;
#undef FN_NAME
} /* hwcFreeWinFifo */

FxBool
hwcExecuteWinFifo(hwcBoardInfo*     bInfo, 
                  const FxU32       winContextId,
                  const HwcWinFifo* fifo,
                  const FxU32       serialNumber)
{
  OSErr       myErr;
  hrmExecuteFifoRequest_t executeFifoReq;
  
  executeFifoReq.contextID = winContextId;
  executeFifoReq.fifoType     = fifo->fifoType;
  executeFifoReq.fifoPtr      = fifo->cmdBuf.baseAddr;
  executeFifoReq.fifoSize     = fifo->cmdBuf.size >> 2UL;
  executeFifoReq.statePtr     = fifo->stateBuf.baseAddr;
  executeFifoReq.stateSize    = fifo->stateBuf.size >> 2UL;
  executeFifoReq.serialNumber = serialNumber;
  
  if (fifo->fifoType != HWCEXT_FIFO_HOST) {
    executeFifoReq.fifoOffset     = fifo->cmdBuf.hwOffset;
    executeFifoReq.stateOffset    = fifo->stateBuf.hwOffset;
    executeFifoReq.sentinalOffset = fifo->sentinalBufferOffset;
  }
  
  myErr = _hrmExecuteWinFifo((hrmBoard_t *)bInfo->hMon,&executeFifoReq);
  
  return myErr == noErr ? FXTRUE : FXFALSE;

  
} /* hwcExecuteWinFifo */

FxU32
hwcExecuteStatusWinFifo(hwcBoardInfo*     bInfo,
                        const HwcWinFifo* fifo,
                        const FxU32       serialNumber)
{
#define FN_NAME "hwcExecuteStatusWinFifo"
  FxU32
    retVal = ~0x00UL;

  switch(fifo->fifoType) {
  /* Host fifo always executes synchronously */
  case HWCEXT_FIFO_HOST:
    retVal = serialNumber;
    break;

  case HWCEXT_FIFO_FB:
  case HWCEXT_FIFO_AGP:
    {
      volatile FxU32*
        bufAddr = (volatile FxU32*)fifo->sentinalBufferAddr;

      /* We need to do some extra reads here so that we make sure taht
       * the data we're returning is coherent w/ the actual frame
       * buffer due to the intervening pixel cache. The cache is,
       * currently, 4 dwords, and appears to be aligned on this
       * boundary as well so reading 4 dwords away should be flushing.
       *
       * NB: The surface for the serial # was allocated big enough to
       * handle this so we would not have to worry about it here.  */
      retVal = (bufAddr[0]);
      retVal = (bufAddr[0 + 4]);
      retVal = (bufAddr[0]);
    }
    break;

  case HWCEXT_FIFO_INVALID:
  default:
    GDBG_ERROR(FN_NAME, "Invalid fifoType");
    break;
  }
  
  return retVal;
#undef FN_NAME
}                        
#endif

/* #define LINEAR_STRIDE_ALIGN 16UL */
/* #define TILE_BIT            0x00008000UL */

#endif /* HWC_ACCESS_GDX */

FxBool
hwcInitVideo(hwcBoardInfo *bInfo, FxBool tiled, FxVideoTimingInfo *vidTiming,
             FxU32 pixFmt, FxBool overlay)
{
#define FN_NAME "hwcInitVideo"
#if HWC_EXT_INIT
  FxI32 status;
#endif  
  FxU32
    stride= (tiled) ? bInfo->buffInfo.bufStrideInTiles : bInfo->vidInfo.stride;
  FxU32
    scrWidth, scrHeight,
    ovlWidth, ovlHeight,
    vidProcCfg, vidScreenSize, vidOverlayEndScreenCoord,
    vidOverlayDudx, vidOverlayDvdy,
    pixelShift, bpp,
    dramInit1;

  FxBool
    lfbMemoryConfig,
    miscInit0;

  float
    scale;

#ifdef HWC_EXT_INIT 
    hwcExtRequest_t ctxReq;
    hwcExtResult_t  ctxRes;
#endif
  
   {
      FxU32 refresh;
      static FxU32 refConstToRefreshHz[] =
      {
         60,                       /* GR_REFRESH_60Hz   */
         70,                       /* GR_REFRESH_70Hz   */
         72,                       /* GR_REFRESH_72Hz   */
         75,                       /* GR_REFRESH_75Hz   */
         80,                       /* GR_REFRESH_80Hz   */
         90,                       /* GR_REFRESH_90Hz   */
         100,                      /* GR_REFRESH_100Hz  */
         85,                       /* GR_REFRESH_85Hz   */
         120,                      /* GR_REFRESH_120Hz  */
         0
      };

      if (bInfo->vidInfo.vRefresh > GR_REFRESH_120Hz)
         refresh = 0;
      else
         refresh = bInfo->vidInfo.vRefresh;

      refresh = refConstToRefreshHz[refresh]; /* Make sure we use the table, otherwise
                                               we will always pass 0Hz to setVideoMode */
      if (GETENV("FX_GLIDE_REFRESH", bInfo->RegPath))
         refresh = atoi(GETENV("FX_GLIDE_REFRESH", bInfo->RegPath));

      GDBG_INFO( 80, "REFRESHRATE: %i\n", refresh );

#ifdef HWC_DXDRVR
      if ( !dxOpen( (FxU32)bInfo->vidInfo.hWnd,		/* hWindow */
	          bInfo->vidInfo.sRes,			/* sRes */
	          bInfo->vidInfo.vRefresh,		/* vRefresh */
	          HWC_COLORFORMAT_ARGB,			/* cFormat */
	          HWC_ORIGIN_UPPER_LEFT,		/* yOrigin */
	          bInfo->buffInfo.nColBuffers,		/* nColBuffers */
	          bInfo->buffInfo.nAuxBuffers,		/* nAuxBuffers */
                  				/* return values */
	          &bInfo->buffInfo.buffers, 		/* pBufDesc */
	          &bInfo->vidInfo.xRes,			/* width */
	          &bInfo->vidInfo.yRes,			/* height */
        	  swapType,				/* swapType */
	          bInfo->hMon ) )			/* hmon */
#else
      if ( !setVideoMode( (void *)bInfo->vidInfo.hWnd, 
                          bInfo->vidInfo.xRes,
                          bInfo->vidInfo.yRes,
                          bInfo->h3pixelSize,
                          refresh,
                          (void *)bInfo->hMon,
                          (char *)bInfo->RegPath,
                          (char *)bInfo->DeviceName) )
#endif /* HWC_DXDRVR */
      {
         GDBG_INFO(80, "%s:  dxOpen() failed!\n", FN_NAME);
         return FXFALSE;
      }
#if SET_SWIZZLEHACK
      setLfbSwizzleMode(bInfo->regInfo.rawLfbBase,bInfo->regInfo.ioMemBase,8);
#endif
   }

#ifdef HWC_EXT_INIT
   ctxReq.which = HWCEXT_HWCSETEXCLUSIVE;
  
   ctxReq.optData.linearAddrReq.devNum = bInfo->boardNum;
   GDBG_INFO(80, FN_NAME ":  ExtEscape:HWCEXT_HWCSETEXCLUSIVE\n");
  
   ExtEscape((HDC) bInfo->hdc, HWCEXT_ESCAPE(bInfo->boardNum), sizeof(ctxReq), (LPSTR) &ctxReq, /**/
            sizeof(ctxRes), (LPSTR) &ctxRes);
  
   status = ctxRes.resStatus;
  
   if (status != 1)
   {
      hwc_errncpy(errorString,"HWCEXT_HWCSETEXCLUSIVE Failed");
      return FXFALSE;
   }
#else
  /* This is off for now until the rest of the alt-tab type things are done. */
#if 0
  /* Before letting glide party on the hw check to see if we're on a
   * system w/ a 2d environment, and make sure taht its happy about us
   * doing this before dorking w/ things that are going to make it
   * unhappy.  
   */
   if (Dpmi2DEnvironmentP())
   {
      bInfo->hdc = DpmiDeviceContextGet("3DFXVB");
      if (bInfo->hdc == 0x00UL)
         return FXFALSE;

      if (!DpmiDeviceContextDispatch(bInfo->hdc, FxDCIsFullscreenP))
         return FXFALSE;
      if (!DpmiDeviceContextDispatch(bInfo->hdc, FxDCExclusiveLock))
         return FXFALSE;
   }
#endif
#endif
  
   switch(pixFmt)
   {
      case SST_OVERLAY_PIXEL_RGB565D:
      case SST_OVERLAY_PIXEL_RGB565U:
         pixelShift = 1;
         bpp=16;
         break;
      case SST_OVERLAY_PIXEL_RGB1555U:
      case SST_OVERLAY_PIXEL_RGB1555D:        
         pixelShift = 1;
         bpp=15;
         break;
      case SST_OVERLAY_PIXEL_RGB32U:
         pixelShift = 2;
         bpp=32;
         break;
      default:
         GDBG_INFO(80, "%s:  Invalid overlay pixel format: %08lx failed!\n", FN_NAME, pixFmt);
         return FXFALSE;        
   }

   HWC_IO_STORE(bInfo->regInfo, vidOverlayDudxOffsetSrcWidth,
      ((bInfo->vidInfo.xRes << pixelShift) << 19));

   /* Video pixel buffer threshold */
   {
      FxU32 vidPixelBufThold;
      FxU32 thold = 32;

      if (GETENV("SSTVB_PIXTHOLD", bInfo->RegPath))
         thold = atoi(GETENV("SSTVB_PIXTHOLD", bInfo->RegPath));

      thold &= 0x3f;

      vidPixelBufThold = (thold | (thold << 6) | (thold << 12));

      HWC_IO_STORE(bInfo->regInfo, vidPixelBufThold, vidPixelBufThold);
   }

#ifdef __WIN32__
   if (vidTiming)
   {
      hwcExtRequest_t ctxReq;
      hwcExtResult_t ctxRes;

      if( HWCEXT_PROTOCOL(bInfo->boardNum) )
      {
         ctxReq.which = HWCEXT_VIDTIMING;
         ctxReq.optData.vidTimingReq.vidTiming = (void *) vidTiming;
         GDBG_INFO(80, FN_NAME ":  ExtEscape:HWCEXT_VIDTIMING\n");
         ExtEscape((HDC) bInfo->hdc, HWCEXT_ESCAPE(bInfo->boardNum), sizeof(ctxReq), (LPSTR) &ctxReq, /**/
         sizeof(ctxRes), (LPSTR) &ctxRes);
      }
      /* Ignore failure */
   }
#endif

#ifdef __DOS32__
  /* Now call the cinit code */

  h3InitVideoOverlaySurface(
    bInfo->regInfo.ioPortBase,
    FXTRUE,                     /* 1=enable Overlay surface (OS), 1=disable */
    FXFALSE,                    /* 1=enable OS stereo, 0=disable */
    FXFALSE,                    /* 1=enable horizontal scaling, 0=disable */
    0,                          /* horizontal scale factor (ignored if not) */
    FXFALSE,                    /* 1=enable vertical scaling, 0=disable */
    0,                          /* vertical scale factor (ignored if not) */
    0,                          /* Filter mode */
    FXTRUE,                     /* tiled */
    pixFmt,                     /* pixel format of OS */
    FXTRUE,                    /* bypass clut for OS? */
    0,                          /* 0=lower 256 CLUT entries, 1=upper 256 */
    bInfo->buffInfo.colBuffStart0[0],/* board address of beginning of OS */
    stride);                    /* distance between scanlines of the OS, in
                                   units of bytes for linear OS's and tiles for
                                   tiled OS's */
  HWC_IO_STORE(bInfo->regInfo, vidOverlayStartCoords, 0);
  HWC_IO_STORE(bInfo->regInfo, vidOverlayEndScreenCoord,
    (bInfo->vidInfo.yRes  << SST_OVERLAY_Y_SHIFT) |
    (bInfo->vidInfo.xRes & SST_OVERLAY_X) );
#else
  hwcInitVideoOverlaySurface(
    &bInfo->regInfo,
    FXTRUE,                     /* 1=enable Overlay surface (OS), 0=disable */
    FXFALSE,                    /* 1=enable OS stereo, 0=disable */
    FXFALSE,                    /* 1=enable horizontal scaling, 0=disable */
    0,                          /* horizontal scale factor (ignored if not) */
    FXFALSE,                    /* 1=enable vertical scaling, 0=disable */
    0,                          /* vertical scale factor (ignored if not) */
    0,                          /* Filter mode */
    FXTRUE,                     /* tiled */
    pixFmt,                     /* pixel format of OS */
    FXFALSE,                    /* bypass clut for OS? */
    0,                          /* 0=lower 256 CLUT entries, 1=upper 256 */
    bInfo->buffInfo.colBuffStart0[0],/* board address of beginning of OS */
    stride);                    /* distance between scanlines of the OS, in
                                   units of bytes for linear OS's and tiles for
                                   tiled OS's */
  HWC_IO_STORE(bInfo->regInfo, vidOverlayStartCoords, 0);
  
#endif
  
  /* 
     Setup video scaling for half-modes
   */

  /* Get some important info */
  HWC_IO_LOAD(bInfo->regInfo, vidScreenSize, vidScreenSize);
  HWC_IO_LOAD(bInfo->regInfo, vidProcCfg, vidProcCfg);
  HWC_IO_LOAD(bInfo->regInfo, vidOverlayEndScreenCoord, vidOverlayEndScreenCoord);

  scrWidth = (vidScreenSize >> SST_VIDEO_SCREEN_WIDTH_SHIFT) & 0xfff;
  scrHeight = (vidScreenSize >> SST_VIDEO_SCREEN_HEIGHT_SHIFT) & 0xfff;

  ovlWidth = ((vidOverlayEndScreenCoord) >> SST_OVERLAY_X_SHIFT) & 0xfff;
  ovlHeight = ((vidOverlayEndScreenCoord) >> SST_OVERLAY_Y_SHIFT) & 0xfff;  

  ovlWidth += 1;
  ovlHeight += 1;

  /* Check to see if the screen and overlay dimensions match. 
   * There are two cases that can happen in reality.
   *
   * scrXXX > appXXX: This is a 'real' case, and the overlay dimension
   * needs to mag scaled so that it fits the requested size.
   *
   * (scrXXX == appXXX) && (ovlXXX != scrXXX): This is a somewhat artificial
   * case where someone left the overlay set to some value, and these did
   * not get reset in the setVideoMode processing. (For example, if the user is
   * running an application which bus masters data directly to our video overlay
   * when launching a glide application). In this case we need to fiddle w/ the
   * overlay dimension so that it matches the requested resolution.
   *
   * (scrXXX < appXXX): If setVideoMode is actually working correctly, this cannot
   * happen because that code has to know that we can't do min scaling.
   */

  vidOverlayDudx = 0UL;
  if (scrWidth > bInfo->vidInfo.xRes) {
    vidProcCfg |= SST_OVERLAY_HORIZ_SCALE_EN;

    ovlWidth = scrWidth;

    scale = ((float) bInfo->vidInfo.xRes) / ((float) ovlWidth);

    vidOverlayDudx = (FxU32) (scale * ((float) (1 << 20)));
    HWC_IO_STORE(bInfo->regInfo, vidOverlayDudx, vidOverlayDudx);
  } else if (ovlWidth != scrWidth) {
    ovlWidth = scrWidth;
  }

  HWC_IO_STORE(bInfo->regInfo, vidOverlayDudx, vidOverlayDudx);

  vidOverlayDvdy = 0UL;
  if (scrHeight > bInfo->vidInfo.yRes) {
    vidProcCfg |= SST_OVERLAY_VERT_SCALE_EN;

    ovlHeight = scrHeight;

    scale = ((float) bInfo->vidInfo.yRes) / ((float) ovlHeight);

    vidOverlayDvdy = (FxU32) (scale * ((float) (1 << 20)));
    HWC_IO_STORE(bInfo->regInfo, vidOverlayDvdy, vidOverlayDvdy);
  } else if (ovlHeight != scrHeight) {
    ovlHeight = scrHeight;
  }

  HWC_IO_STORE(bInfo->regInfo, vidOverlayDvdy, vidOverlayDvdy);

  vidOverlayEndScreenCoord = (((ovlHeight - 1) << SST_OVERLAY_Y_SHIFT) |
                              ((ovlWidth - 1) << SST_OVERLAY_X_SHIFT));
  HWC_IO_STORE(bInfo->regInfo, vidOverlayEndScreenCoord, vidOverlayEndScreenCoord);

  vidProcCfg &= ~SST_OVERLAY_FILTER_MODE;
  /* 4/30/99 srogers
  ** Adding user support for switching the video filter from 2x2 to 4x1
  */

   if (NULL == GETENV("SSTH3_OVERLAYMODE", bInfo->RegPath))
   {
      /* We are in optimal mode by default */
      if(bInfo->vidInfo.xRes < 1024)
         vidProcCfg |= SST_OVERLAY_FILTER_2X2;
      else
         vidProcCfg |= SST_OVERLAY_FILTER_4X4;
   }
   else
   {
      switch(atoi(GETENV("SSTH3_OVERLAYMODE", bInfo->RegPath)))
      {
         default:
         case 1: /* Optimal */
            if(bInfo->vidInfo.xRes < 1024)
               vidProcCfg |= SST_OVERLAY_FILTER_2X2;
            else
               vidProcCfg |= SST_OVERLAY_FILTER_4X4;
            break;
         case 2: /* Normal */
            vidProcCfg |= SST_OVERLAY_FILTER_4X4;
            break;
         case 3: /* High */
            /* make sure that if 2x video mode is enabled, we use the 4x1 filter. */
            if(vidProcCfg & SST_VIDEO_2X_MODE_EN)
               vidProcCfg |= SST_OVERLAY_FILTER_4X4;
            else
               vidProcCfg |= SST_OVERLAY_FILTER_2X2;
            break;
         case 4: /* Very High */
            vidProcCfg |= SST_OVERLAY_FILTER_BILINEAR;
            break;
         case -1: /* Disabled */
            /* use this just in case */
            vidProcCfg |= SST_OVERLAY_FILTER_POINT;
            break;
      }
   }      
  
  if (bInfo->h3pixelSample < 2)
  {
    vidScreenSize &= ~SST_VIDEO_SCREEN_DESKTOPADDR_FIFO_ENABLE;
    HWC_IO_STORE(bInfo->regInfo, vidScreenSize, vidScreenSize);
    vidProcCfg &= ~(SST_CHROMA_EN|SST_DESKTOP_EN);
  }
  else if (bInfo->h3pixelSample > 1 && bInfo->buffInfo.enable2ndbuffer)
  {
    vidScreenSize |= SST_VIDEO_SCREEN_DESKTOPADDR_FIFO_ENABLE;

    /*
    ** vidProcCfg
    */

    /* enable Chroma Keying */
    vidProcCfg |= SST_CHROMA_EN;

    /* enable desktop surface */
    vidProcCfg |= SST_DESKTOP_EN;

    /* Overlay surface is already enabled above */

    /* set up desktop pixel format */
    /* Also turn off all filtering, as it seems to screw up multi-chip configs. */
    vidProcCfg &= ~(SST_DESKTOP_PIXEL_FORMAT|SST_OVERLAY_FILTER_MODE);
    switch(vidProcCfg & SST_OVERLAY_PIXEL_FORMAT) {
      case SST_OVERLAY_PIXEL_RGB32U:
        vidProcCfg |= SST_DESKTOP_PIXEL_RGB32;
        bpp = 32;
        break;
      case SST_OVERLAY_PIXEL_RGB565D:
      case SST_OVERLAY_PIXEL_RGB565U:
        vidProcCfg |= SST_DESKTOP_PIXEL_RGB565;
        bpp = 16;
        break;
      case SST_OVERLAY_PIXEL_RGB1555D:
      case SST_OVERLAY_PIXEL_RGB1555U:
        vidProcCfg |= SST_DESKTOP_PIXEL_RGB1555U;
        bpp = 15;
        break;
      default:
        GDBG_INFO(80,"Unknown overlay pixel format: %d\n",
          (vidProcCfg & SST_OVERLAY_PIXEL_FORMAT) >> SST_OVERLAY_PIXEL_FORMAT_SHIFT);
    }

    /* Make sure desktop is tiled */
    vidProcCfg |= SST_DESKTOP_TILED_EN;

    HWC_IO_STORE(bInfo->regInfo, vidScreenSize, vidScreenSize);

  } 

  /* Sorry, can only use 4x1 filter in this mode. */
  if ( bInfo->h3nwaySli > 1 ) {
    if( ((vidProcCfg & SST_OVERLAY_FILTER_BILINEAR) == SST_OVERLAY_FILTER_BILINEAR) ||
        ((vidProcCfg & SST_OVERLAY_FILTER_2X2) == SST_OVERLAY_FILTER_2X2) ) {
      vidProcCfg &= ~SST_OVERLAY_FILTER_MODE;
      vidProcCfg |= SST_OVERLAY_FILTER_4X4;
    }
  }      
  
  /* Disable video filter in 32-bit mode */
#if 1 /* looks better with filter when in 16bpp 2xfsaa */
  if(bpp == 32 || bInfo->h3pixelSample > 2) {
#else /* KoolSmoky - can't have filter in FSAA */
  if(bpp == 32 || bInfo->h3pixelSample > 1) {
#endif
    vidProcCfg &= ~SST_OVERLAY_FILTER_MODE;
    vidProcCfg |= SST_OVERLAY_FILTER_POINT;
  }  

  HWC_IO_STORE(bInfo->regInfo, vidProcCfg, vidProcCfg);

  GDBG_INFO(80, FN_NAME ":  Video Overlay Info:\n\
            Screen Width =       0x%x (%d) \n\
            Screen Height =      0x%x (%d) \n\
            Render Width =      0x%x (%d) \n\
            Render Height =     0x%x (%d) \n",
            ovlWidth, ovlWidth, ovlHeight, ovlHeight, 
            bInfo->vidInfo.xRes, bInfo->vidInfo.xRes, 
            bInfo->vidInfo.yRes, bInfo->vidInfo.yRes);
  
  /* Get miscInit0 for y-sub  */
  HWC_IO_LOAD(bInfo->regInfo, miscInit0, miscInit0);

  /* Clear out relavent bits */
  miscInit0 &= ~SST_YORIGIN_TOP;
  miscInit0 |= ((bInfo->vidInfo.yRes - 1)  << SST_YORIGIN_TOP_SHIFT);

  HWC_IO_STORE(bInfo->regInfo, miscInit0, miscInit0);
  
  /* 
  if (IS_NAPALM(bInfo->pciInfo.deviceID)) {
    HWC_IO_STORE(bInfo->regInfo, lfbMemoryConfig, SST_RAW_LFB_UPDATE_CONTROL);
  } 
  */
   
  /* Set up lfbMemoryConfig */
  lfbMemoryConfig =
    SST_RAW_LFB_TILE_BEGIN_PAGE_MUNGE((bInfo->buffInfo.colBuffStart0[0] >> 12))
      | HWC_RAW_LFB_STRIDE
        | (bInfo->buffInfo.bufStrideInTiles << SST_RAW_LFB_TILE_STRIDE_SHIFT);

  GDBG_INFO(80, FN_NAME ":  Setting lfbMemoryConfig to %08ulx\n",
            lfbMemoryConfig);

  HWC_IO_STORE(bInfo->regInfo, lfbMemoryConfig, lfbMemoryConfig);
  
  if (IS_NAPALM(bInfo->pciInfo.deviceID)) {
    HWC_IO_STORE(bInfo->regInfo, lfbMemoryConfig, SST_RAW_LFB_WRITE_CONTROL);
  }
  

  /* Filthy memclock hack */
  if (GETENV("H3_MEM_CLOCK", bInfo->RegPath)) {
    int mHz;
    FxU32 pllVal;
    mHz = atoi(GETENV("H3_MEM_CLOCK", bInfo->RegPath));

    switch (mHz) {
    case 50:
      pllVal = 0x2806;
      break;
      
    case 75:
      pllVal = 0x7125;
      break;
      
    case 80:
      pllVal = 0x7925;
      break;

    case 100:
      pllVal = 0x2805;
      break;

    default:
      pllVal = 0;
      gdbg_printf("%s:  Bogus MEMCLOCK setting!\n", FN_NAME);
      break;
    }

    if (pllVal) {
      HWC_IO_STORE(bInfo->regInfo, pllCtrl1, pllVal);
      HWC_IO_STORE(bInfo->regInfo, pllCtrl2, pllVal);
      gdbg_printf("%s:  Setting memory clock to %d MHz\n", FN_NAME,
        mHz);
    }

  }

  /* Set up dramInit1 for triple or double buffering */
  HWC_IO_LOAD(bInfo->regInfo, dramInit1, dramInit1);
  if (bInfo->vidInfo.tripleBuffering)
    dramInit1 |= SST_TRIPLE_BUFFER_EN;
  else
    dramInit1 &= ~SST_TRIPLE_BUFFER_EN;

  HWC_IO_STORE(bInfo->regInfo, dramInit1, dramInit1);

  HWC_IO_STORE(bInfo->regInfo, vidMaxRGBDelta,/* bpp == 15 ? 0x101010 : */ 0x100810);

  HWC_IO_STORE( bInfo->regInfo, vidDesktopOverlayStride,
                ( bInfo->buffInfo.bufStrideInTiles << 16 ) |
                  bInfo->buffInfo.bufStrideInTiles );

  /* initialize pci registers */
#ifdef FX_GLIDE_NAPALM
#if 0
  /* Force certain things on 4-way baords */
  if(bInfo->pciInfo.numChips == 4) {
    /* For 4-way SLI we must use analog SLI */
    if(bInfo->h3nwaySli == 4) {
      bInfo->h3analogSli = FXTRUE;
    } else if(bInfo->h3nwaySli == 2 && bInfo->h3pixelSample == 4) {
      /* Force digital AA with analog SLI */
      bInfo->h3analogSli = FXFALSE;        
    }    
  }  
#else
  /* always use analog SLI for 4-way boards */
  if(bInfo->pciInfo.numChips >= 4) {
    bInfo->h3analogSli = FXTRUE;
  }
#endif
#ifdef HWC_EXT_INIT

#ifdef HWC_MINIVDD_HACK
  /*
   * Since I am a lazy bastard, try to use the minivdd to setup SLI/AA mode for
   * multi-chip configurations.
   */
  if (((IS_NAPALM(bInfo->pciInfo.deviceID)) &&
       (bInfo->pciInfo.numChips > 1)) &&
      ((bInfo->h3nwaySli > 1) ||
       (bInfo->h3pixelSample > 2) ||
       (bInfo->h3pixelSample == 2 && !bInfo->buffInfo.enable2ndbuffer)))
  {

    GDBG_INFO(80, FN_NAME ": HWC_MINIVDD_HACK\n");

    if (bInfo->osNT)
    {
      FxU32 retVal = FXTRUE;
      ctxReq.which = HWCEXT_SLI_AA_REQUEST ;

      GDBG_INFO(80, FN_NAME ": HWC_MINIVDD_HACK: OS is NT\n");

      /* Request SLI or AA enable */
      ctxReq.optData.sliAAReq.ChipInfo.dwaaEn             = (bInfo->h3pixelSample > 1) ;
      ctxReq.optData.sliAAReq.ChipInfo.dwsliEn            = (bInfo->h3nwaySli > 1) ;

      ctxReq.optData.sliAAReq.ChipInfo.dwsliAaAnalog      = bInfo->h3analogSli ;
      ctxReq.optData.sliAAReq.ChipInfo.dwsli_nlines       = bInfo->h3sliBandHeight;
      ctxReq.optData.sliAAReq.ChipInfo.dwCfgSwapAlgorithm = 1 ; /* What the *F* is this about? */

      /* Fill out the memory configuration. */
      ctxReq.optData.sliAAReq.MemInfo.dwTotalMemory       = bInfo->h3Mem * 1024 * 1024 ;
      ctxReq.optData.sliAAReq.MemInfo.dwTileMark          = bInfo->buffInfo.colBuffStart0[0] ;
      ctxReq.optData.sliAAReq.MemInfo.dwTileCmpMark       = bInfo->buffInfo.colBuffStart0[0] ;

      /* Fill out SLI mode information. */
      ctxReq.optData.sliAAReq.ChipInfo.dwChips            = bInfo->pciInfo.numChips ;

      /* Fill out AA mode information. */
      /* 8xaa */
   	  ctxReq.optData.sliAAReq.ChipInfo.dwaaSampleHigh     = 0;
	  if(bInfo->h3pixelSample == 4)
      	ctxReq.optData.sliAAReq.ChipInfo.dwaaSampleHigh   = 1;
	  else if(bInfo->h3pixelSample == 8)
       	ctxReq.optData.sliAAReq.ChipInfo.dwaaSampleHigh   = 2;

      ctxReq.optData.sliAAReq.MemInfo.dwaaSecondaryColorBufBegin  = bInfo->buffInfo.colBuffStart1[0] ;
      ctxReq.optData.sliAAReq.MemInfo.dwaaSecondaryDepthBufBegin  = bInfo->buffInfo.lfbBuffAddr0[bInfo->buffInfo.nColBuffers];
      ctxReq.optData.sliAAReq.MemInfo.dwaaSecondaryDepthBufEnd    = bInfo->buffInfo.lfbBuffAddr0End[bInfo->buffInfo.nColBuffers];
      ctxReq.optData.sliAAReq.MemInfo.dwBpp = bpp;

      GDBG_INFO(80, FN_NAME ": HWC_MINIVDD_HACK: ExEscape:HWCEXT_SLI_AA_REQUEST\n");
      ExtEscape((HDC)bInfo->hdc, HWCEXT_ESCAPE(bInfo->boardNum), 
                sizeof(ctxReq), (LPSTR) &ctxReq,
                sizeof(ctxRes), (LPSTR) &ctxRes);
      GDBG_INFO(80, FN_NAME ": HWC_MINIVDD_HACK: ExtEscape retVal=%d\n", retVal);

      /* the w2k miniport doesn't copy this value to the slave chips */
      /* so for now re-write it here */
      HWC_IO_STORE(bInfo->regInfo, vidScreenSize, vidScreenSize);
    }
     else
    {
    DIOC_DATA DIOC_Data;
    SLI_AA_REQUEST Sli_AA_Request;
    HANDLE hDevice;

    GDBG_INFO(80, FN_NAME ": HWC_MINIVDD_HACK: OS is 9x\n");

    /* Request SLI or AA enable */
    Sli_AA_Request.ChipInfo.dwaaEn             = (bInfo->h3pixelSample > 1) ;
    Sli_AA_Request.ChipInfo.dwsliEn            = (bInfo->h3nwaySli > 1) ;

    /* 8xaa */
    Sli_AA_Request.ChipInfo.dwsliAaAnalog      = bInfo->h3analogSli ;
    Sli_AA_Request.ChipInfo.dwsli_nlines       = bInfo->h3sliBandHeight;
    Sli_AA_Request.ChipInfo.dwCfgSwapAlgorithm = 1 ; /* What the *F* is this about? */

    /* Fill out the memory configuration. */
    Sli_AA_Request.MemInfo.dwTotalMemory       = bInfo->h3Mem * 1024 * 1024 ;
    Sli_AA_Request.MemInfo.dwTileMark          = bInfo->buffInfo.colBuffStart0[0] ;
    Sli_AA_Request.MemInfo.dwTileCmpMark       = bInfo->buffInfo.colBuffStart0[0] ;

    /* Fill out SLI mode information. */
    Sli_AA_Request.ChipInfo.dwChips            = bInfo->pciInfo.numChips ;

    /* Fill out AA mode information. */
    /* 8xaa */
    Sli_AA_Request.ChipInfo.dwaaSampleHigh     = 0;
    if(bInfo->h3pixelSample == 4)
      	Sli_AA_Request.ChipInfo.dwaaSampleHigh = 1;
    else if(bInfo->h3pixelSample == 8)
       	Sli_AA_Request.ChipInfo.dwaaSampleHigh = 2;
 
    Sli_AA_Request.MemInfo.dwaaSecondaryColorBufBegin  = bInfo->buffInfo.colBuffStart1[0] ;
    Sli_AA_Request.MemInfo.dwaaSecondaryDepthBufBegin  = bInfo->buffInfo.lfbBuffAddr0[bInfo->buffInfo.nColBuffers];
    Sli_AA_Request.MemInfo.dwaaSecondaryDepthBufEnd    = bInfo->buffInfo.lfbBuffAddr0End[bInfo->buffInfo.nColBuffers];
    Sli_AA_Request.MemInfo.dwBpp = bpp;

    /*
     * Call the MiniVDD
     */
    hDevice = CreateFile(MINIVDDNAME, 0, 0, NULL, 0, 0, NULL);
    if (INVALID_HANDLE_VALUE != hDevice)
    {
      DIOC_Data.dwDevNode = bInfo->devNode ;
      DIOC_Data.dwSpare = (DWORD)&Sli_AA_Request;
      DeviceIoControl(hDevice, SLI_AA_ENABLE, &DIOC_Data, sizeof(DIOC_Data), NULL, 0x0, NULL, NULL);
      GDBG_INFO(80, FN_NAME ": HWC_MINIVDD_HACK: Called MiniVDD!\n");
    } else 
      GDBG_INFO(80, FN_NAME ": HWC_MINIVDD_HACK: MiniVDD call failed!\n");
    CloseHandle(hDevice);
    }

    /* The minivdd screws with lfbMemoryConfig, so we have to fix it. */
    HWC_IO_STORE(bInfo->regInfo, lfbMemoryConfig, lfbMemoryConfig);
    HWC_IO_STORE(bInfo->regInfo, lfbMemoryConfig, SST_RAW_LFB_WRITE_CONTROL);

    /* This will automagically get broadcast to all chips. */
    HWC_IO_STORE(bInfo->regInfo, vidMaxRGBDelta, bpp == 15 ? 0x101010 : 0x100810);
  }
   else
#endif
  if ((IS_NAPALM(bInfo->pciInfo.deviceID)) &&
      (bInfo->h3pixelSample == 2))
  {
    hwcExtRequest_t
      ctxReq;
    hwcExtResult_t
      ctxRes;
    FxU32 
      function_number;
    FxBool
      retVal = FXFALSE;
                          
    ctxReq.which = HWCEXT_PCI_OP;
                              
    for (function_number = 0; function_number < bInfo->pciInfo.numChips; function_number++) {

      /* All these are the same for each config cycle */
      ctxReq.optData.pciOpReq.DeviceId 
        = function_number & 0x3;
      ctxReq.optData.pciOpReq.Operation
        = HWCEXT_PCI_WRITE;

      /*
      **  Set up cfgSliLfbCtrl
      */

      ctxReq.optData.pciOpReq.Offset
        = offsetof(SstPCIConfigRegs, cfgSliLfbCtrl);

      ctxReq.optData.pciOpReq.Value = 0;
        //= SST_SLI_LFB_CPU_WRITE_ENABLE |
        //SST_SLI_LFB_DISPATCH_WRITE_ENABLE |
        //SST_SLI_LFB_READ_ENABLE;

      retVal = ExtEscape((HDC)bInfo->hdc, HWCEXT_ESCAPE(bInfo->boardNum), 
                         sizeof(ctxReq), (LPSTR) &ctxReq,
                         sizeof(ctxRes), (LPSTR) &ctxRes);
                                       
      /*
      ** Set up cfgAALfbCtrl
      */
      /* Why are we doing this?
       */
      ctxReq.optData.pciOpReq.Offset
        = offsetof(SstPCIConfigRegs, cfgAALfbCtrl);

      ctxReq.optData.pciOpReq.Value
        = SST_AA_LFB_CPU_WRITE_ENABLE |
          SST_AA_LFB_DISPATCH_WRITE_ENABLE |
          SST_AA_LFB_READ_ENABLE |
          bInfo->buffInfo.colBuffStart1[0];

      GDBG_INFO(80, "storing cfgAALfbCtrl: %08lx\n",ctxReq.optData.pciOpReq.Value);

      /* Pixel Format */
      switch (bInfo->h3pixelSize) {
      case 4:
        ctxReq.optData.pciOpReq.Value |= SST_AA_LFB_READ_FORMAT_32BPP;
        break;
      case 2:
        ctxReq.optData.pciOpReq.Value |= SST_AA_LFB_READ_FORMAT_16BPP;
        break;
      }

      /* Perhaps this should change for 2-sample AA?  */
      if (bInfo->h3pixelSample == 4)
        ctxReq.optData.pciOpReq.Value |= SST_AA_LFB_RD_DIVIDE_BY_FOUR;

      retVal = ExtEscape((HDC)bInfo->hdc, HWCEXT_ESCAPE(bInfo->boardNum), 
                         sizeof(ctxReq), (LPSTR) &ctxReq,
                         sizeof(ctxRes), (LPSTR) &ctxRes);
      
      /*
      **  Set Up cfgAADepthBufferAperture
      */
      ctxReq.optData.pciOpReq.Offset
        = offsetof(SstPCIConfigRegs, cfgAADepthBufferAperture);


      ctxReq.optData.pciOpReq.Value
//        = ( ((bInfo->buffInfo.auxBuffStart0 >> 12) << SST_AA_DEPTH_BUFFER_APERTURE_BEGIN_SHIFT) |
//            ((bInfo->buffInfo.auxBuffEnd0 >> 12)   << SST_AA_DEPTH_BUFFER_APERTURE_END_SHIFT));
        = ( ((bInfo->buffInfo.lfbBuffAddr0[bInfo->buffInfo.nColBuffers] >> 12) << SST_AA_DEPTH_BUFFER_APERTURE_BEGIN_SHIFT) |
            ((bInfo->buffInfo.lfbBuffAddr0End[bInfo->buffInfo.nColBuffers] >> 12)   << SST_AA_DEPTH_BUFFER_APERTURE_END_SHIFT));

      retVal = ExtEscape((HDC)bInfo->hdc, HWCEXT_ESCAPE(bInfo->boardNum), 
                         sizeof(ctxReq), (LPSTR) &ctxReq,
                         sizeof(ctxRes), (LPSTR) &ctxRes);
      
      if (bInfo->h3pixelSample == 2) {
        /*
        ** set up cfgVideoCtrl0
        */
        ctxReq.optData.pciOpReq.Offset
          = offsetof(SstPCIConfigRegs, cfgVideoCtrl0);

        ctxReq.optData.pciOpReq.Value =
          SST_CFG_ENHANCED_VIDEO_EN | SST_CFG_VIDEO_LOCALMUX_SEL |
          SST_CFG_DIVIDE_VIDEO_BY_2;

        retVal = ExtEscape((HDC)bInfo->hdc, HWCEXT_ESCAPE(bInfo->boardNum), 
                           sizeof(ctxReq), (LPSTR) &ctxReq,
                           sizeof(ctxRes), (LPSTR) &ctxRes);

        /*
        ** set up cfgVideoCtrl1
        */
        ctxReq.optData.pciOpReq.Offset
          = offsetof(SstPCIConfigRegs, cfgVideoCtrl1);

        ctxReq.optData.pciOpReq.Value = 0;

        retVal = ExtEscape((HDC)bInfo->hdc, HWCEXT_ESCAPE(bInfo->boardNum), 
                           sizeof(ctxReq), (LPSTR) &ctxReq,
                           sizeof(ctxRes), (LPSTR) &ctxRes);

        /*
        ** set up cfgVideoCtrl2
        */
        ctxReq.optData.pciOpReq.Offset
          = offsetof(SstPCIConfigRegs, cfgVideoCtrl2);

        ctxReq.optData.pciOpReq.Value = 0; //SST_CFG_SLI_COMPAREMASK_AAFIFO;

        retVal = ExtEscape((HDC)bInfo->hdc, HWCEXT_ESCAPE(bInfo->boardNum), 
                           sizeof(ctxReq), (LPSTR) &ctxReq,
                           sizeof(ctxRes), (LPSTR) &ctxRes);

      }
    }
  }

  /*
  **  Let's make sure we aren't a bunch of pigeffers.
  */
  if (bInfo->h3pixelSample > 1 || bInfo->h3nwaySli > 1) { /* and thus it must be Napalm */

    FxU32
      tileLinearMark,
      aaMark,
      cfgAALfbCtrl,
      cfgDepthBufferAperture,
      locLFBMemCfg,
      lfbTileCompare,
      strideInTiles,
      chipNum;
    hwcExtRequest_t ctxReq;
    hwcExtResult_t ctxRes;

    for(chipNum = 0; chipNum < bInfo->pciInfo.numChips; chipNum++) {
      /* Load */
      if(chipNum == 0) {
        HWC_IO_STORE(bInfo->regInfo, lfbMemoryConfig, SST_RAW_LFB_UPDATE_CONTROL);
        HWC_IO_LOAD(bInfo->regInfo, lfbMemoryConfig, locLFBMemCfg);
        HWC_IO_STORE(bInfo->regInfo, lfbMemoryConfig, SST_RAW_LFB_UPDATE_CONTROL|SST_RAW_LFB_READ_CONTROL);
        HWC_IO_LOAD(bInfo->regInfo, lfbMemoryConfig, lfbTileCompare);
        HWC_IO_STORE(bInfo->regInfo, lfbMemoryConfig, SST_RAW_LFB_UPDATE_CONTROL);
      } else {
        HWC_IO_STORE_SLAVE(chipNum, bInfo->regInfo, lfbMemoryConfig, SST_RAW_LFB_UPDATE_CONTROL);
        HWC_IO_LOAD_SLAVE(chipNum, bInfo->regInfo, lfbMemoryConfig, locLFBMemCfg);
        HWC_IO_STORE_SLAVE(chipNum, bInfo->regInfo, lfbMemoryConfig, SST_RAW_LFB_UPDATE_CONTROL|SST_RAW_LFB_READ_CONTROL);
        HWC_IO_LOAD_SLAVE(chipNum, bInfo->regInfo, lfbMemoryConfig, lfbTileCompare);
        HWC_IO_STORE_SLAVE(chipNum, bInfo->regInfo, lfbMemoryConfig, SST_RAW_LFB_UPDATE_CONTROL);        
      }

      /* pull the tile aperture begin page bits out and shift them up by
        12 since they represent the 4K-aligned page */
      tileLinearMark = (SST_RAW_LFB_TILE_BEGIN_PAGE_UNMUNGE(locLFBMemCfg)) << 12;
      strideInTiles  = (locLFBMemCfg & SST_RAW_LFB_TILE_STRIDE) >> SST_RAW_LFB_TILE_STRIDE_SHIFT;

      ctxReq.which = HWCEXT_PCI_OP;
    
      /* All these are the same for each config cycle */
      ctxReq.optData.pciOpReq.DeviceId  = chipNum;
    
      ctxReq.optData.pciOpReq.Operation = HWCEXT_PCI_READ;

      ctxReq.optData.pciOpReq.Offset
          = offsetof(SstPCIConfigRegs, cfgAALfbCtrl);

      /*
        NB:  Some lazy bugger didn't check the return value!
        */
      ExtEscape((HDC)bInfo->hdc, HWCEXT_ESCAPE(bInfo->boardNum), 
                        sizeof(ctxReq), (LPSTR) &ctxReq,
                        sizeof(ctxRes), (LPSTR) &ctxRes);

      cfgAALfbCtrl = ctxRes.optData.pciOpRes.Value;

      /* Grab cfgAADepthBufferAperture Register */
      /* All these are the same for each config cycle */
      ctxReq.optData.pciOpReq.DeviceId  = chipNum;
    
      ctxReq.optData.pciOpReq.Operation = HWCEXT_PCI_READ;

      ctxReq.optData.pciOpReq.Offset
          = offsetof(SstPCIConfigRegs, cfgAADepthBufferAperture);

      /*
        NB:  Some lazy bugger didn't check the return value!
        */
      ExtEscape((HDC)bInfo->hdc, HWCEXT_ESCAPE(bInfo->boardNum), 
                        sizeof(ctxReq), (LPSTR) &ctxReq,
                        sizeof(ctxRes), (LPSTR) &ctxRes);

      cfgDepthBufferAperture = ctxRes.optData.pciOpRes.Value;

      /* Clear bits [31:26] and bits [3:0] */
      aaMark = cfgAALfbCtrl & ~(0xfc00000f);

      GDBG_INFO(80, FN_NAME ":  Offsets of buffers from respective bases %d:\n",chipNum);
      GDBG_INFO(80, "\tcfgAALfbCtrol:       %08lx\n",cfgAALfbCtrl);
      GDBG_INFO(80, "\tlocLFBMemCfg:        %08lx\n",locLFBMemCfg);
      GDBG_INFO(80, "\tlfbTileCompare:      %08lx\n",lfbTileCompare);
      GDBG_INFO(80, "\tstrideInTiles:       %08lx\n",strideInTiles);
      GDBG_INFO(80, "\ttileMark:            %08lx\n",tileLinearMark);
      GDBG_INFO(80, "\taaMark:              %08lx\n",aaMark);
      GDBG_INFO(80, "\tcolBufferStart0[0]:  %08lx\n",
                bInfo->buffInfo.colBuffStart0[0] - tileLinearMark);
      GDBG_INFO(80, "\tcolBufferStart0[1]:  %08lx\n",  
                bInfo->buffInfo.colBuffStart0[1] - tileLinearMark);
      GDBG_INFO(80, "\tauxBufferStart0:  %08lx\n",
                bInfo->buffInfo.auxBuffStart0 - tileLinearMark);

      GDBG_INFO(80, "\tcolBufferStart1[0]:  %08lx\n",
                bInfo->buffInfo.colBuffStart1[0] - aaMark);
      GDBG_INFO(80, "\tcolBufferStart1[1]:  %08lx\n",
                bInfo->buffInfo.colBuffStart1[1] - aaMark);
      GDBG_INFO(80, "\tauxBufferStart1:  %08lx\n",
                bInfo->buffInfo.auxBuffStart1 - aaMark);

      GDBG_INFO(80, "\tcolorBufferBegin1:   %08lx\n",
          (cfgAALfbCtrl & SST_SECONDARY_BUFFER_BASE));
      GDBG_INFO(80, "\tdepthBufferBegin:    %08lx\n",
          ((cfgDepthBufferAperture & SST_AA_DEPTH_BUFFER_APERTURE_BEGIN) >>
           SST_AA_DEPTH_BUFFER_APERTURE_BEGIN_SHIFT) * 4096);
      GDBG_INFO(80, "\tdepthBufferEnd:      %08lx\n",
          ((cfgDepthBufferAperture & SST_AA_DEPTH_BUFFER_APERTURE_END) >>
           SST_AA_DEPTH_BUFFER_APERTURE_END_SHIFT) * 4096);

    }
  }           
#endif /* HWC_EXT_INIT */

#if HWC_GDX_INIT
  if (((IS_NAPALM(bInfo->pciInfo.deviceID)) &&
       (bInfo->pciInfo.numChips > 1)) &&
      ((bInfo->h3nwaySli > 1) ||
       (bInfo->h3pixelSample > 2) ||
       (bInfo->h3pixelSample == 2 && !bInfo->buffInfo.enable2ndbuffer)))
  {
    hrmSLIAAChipInfo_t chipInfo;
    hrmSLIAAMemInfo_t  memInfo;
    
    chipInfo.size = sizeof(chipInfo); 
    chipInfo.aaEnable = (bInfo->h3pixelSample > 1);
    chipInfo.sliEnable = (bInfo->h3nwaySli > 1);
    
    chipInfo.sliAaAnalog = (GETENV("FX_GLIDE_ANALOG_SLI", bInfo->RegPath) ?
                            atol(GETENV("FX_GLIDE_ANALOG_SLI", bInfo->RegPath)) : 0);
    chipInfo.sli_nlines  = bInfo->h3sliBandHeight;
    chipInfo.swapAlgorithm = 1;
    chipInfo.numChips = bInfo->pciInfo.numChips;
    chipInfo.aaSampleHigh = (bInfo->h3pixelSample == 4);
    
    memInfo.size = sizeof(memInfo);
    memInfo.totalMemory = bInfo->h3Mem * 1024 * 1024;
    memInfo.tileMark = bInfo->buffInfo.colBuffStart0[0];
    memInfo.tileCmpMark = bInfo->buffInfo.colBuffStart0[0];
    memInfo.aaSecondaryColorBufBegin = bInfo->buffInfo.colBuffStart1[0];
    memInfo.aaSecondaryDepthBufBegin = bInfo->buffInfo.lfbBuffAddr0[bInfo->buffInfo.nColBuffers];
    memInfo.aaSecondaryDepthBufEnd   = bInfo->buffInfo.lfbBuffAddr0End[bInfo->buffInfo.nColBuffers];
    memInfo.bpp = bpp;
    
    if(chipInfo.aaEnable || chipInfo.sliEnable) {
      _hrmSLIAA((hrmBoard_t *)bInfo->hMon,&chipInfo,&memInfo);
    }

    /* The minivdd screws with lfbMemoryConfig, so we have to fix it. */
    HWC_IO_STORE(bInfo->regInfo, lfbMemoryConfig, lfbMemoryConfig);

    /* This will automagically get broadcast to all chips. */
    HWC_IO_STORE(bInfo->regInfo, vidMaxRGBDelta, /*bpp == 15 ? 0x101010 :*/ 0x100810);

  /*
  **  Let's make sure we aren't a bunch of pigeffers.
  */
  if (bInfo->h3pixelSample > 1 || bInfo->h3nwaySli > 1) { /* and thus it must be Napalm */

    FxU32
      tileLinearMark,
      aaMark,
      cfgAALfbCtrl,
      cfgDepthBufferAperture,
      locLFBMemCfg,
      lfbTileCompare,
      strideInTiles,
      dramInit0,
      dramInit1,
      miscInit0,
      miscInit1,
      chipNum;

    for(chipNum = 0; chipNum < bInfo->pciInfo.numChips; chipNum++) {
      /* Load */
      if(chipNum == 0) {
        HWC_IO_STORE(bInfo->regInfo, lfbMemoryConfig, SST_RAW_LFB_UPDATE_CONTROL);
        HWC_IO_LOAD(bInfo->regInfo, lfbMemoryConfig, locLFBMemCfg);
        HWC_IO_STORE(bInfo->regInfo, lfbMemoryConfig, SST_RAW_LFB_UPDATE_CONTROL|SST_RAW_LFB_READ_CONTROL);
        HWC_IO_LOAD(bInfo->regInfo, lfbMemoryConfig, lfbTileCompare);
        HWC_IO_STORE(bInfo->regInfo, lfbMemoryConfig, SST_RAW_LFB_UPDATE_CONTROL);
        HWC_IO_LOAD(bInfo->regInfo, dramInit0, dramInit0);
        HWC_IO_LOAD(bInfo->regInfo, dramInit1, dramInit1);
        HWC_IO_LOAD(bInfo->regInfo, miscInit0, miscInit0);
        HWC_IO_LOAD(bInfo->regInfo, miscInit1, miscInit1);
      } else {
        HWC_IO_STORE_SLAVE(chipNum, bInfo->regInfo, lfbMemoryConfig, SST_RAW_LFB_UPDATE_CONTROL);
        HWC_IO_LOAD_SLAVE(chipNum, bInfo->regInfo, lfbMemoryConfig, locLFBMemCfg);
        HWC_IO_STORE_SLAVE(chipNum, bInfo->regInfo, lfbMemoryConfig, SST_RAW_LFB_UPDATE_CONTROL|SST_RAW_LFB_READ_CONTROL);
        HWC_IO_LOAD_SLAVE(chipNum, bInfo->regInfo, lfbMemoryConfig, lfbTileCompare);
        HWC_IO_STORE_SLAVE(chipNum, bInfo->regInfo, lfbMemoryConfig, SST_RAW_LFB_UPDATE_CONTROL);        
        HWC_IO_LOAD_SLAVE(chipNum, bInfo->regInfo, dramInit0, dramInit0);
        HWC_IO_LOAD_SLAVE(chipNum, bInfo->regInfo, dramInit1, dramInit1);
        HWC_IO_LOAD_SLAVE(chipNum, bInfo->regInfo, miscInit0, miscInit0);
        HWC_IO_LOAD_SLAVE(chipNum, bInfo->regInfo, miscInit1, miscInit1);
      }

      /* pull the tile aperture begin page bits out and shift them up by
        12 since they represent the 4K-aligned page */
      tileLinearMark = (SST_RAW_LFB_TILE_BEGIN_PAGE_UNMUNGE(locLFBMemCfg)) << 12;
      strideInTiles  = (locLFBMemCfg & SST_RAW_LFB_TILE_STRIDE) >> SST_RAW_LFB_TILE_STRIDE_SHIFT;

    _hrmReadConfigRegister((hrmBoard_t *)bInfo->hMon,
                            0,
                            offsetof(SstPCIConfigRegs, cfgAALfbCtrl),
                            &cfgAALfbCtrl);

    _hrmReadConfigRegister((hrmBoard_t *)bInfo->hMon,
                            0,
                            offsetof(SstPCIConfigRegs, cfgAADepthBufferAperture),
                            &cfgDepthBufferAperture);

      /* Clear bits [31:26] and bits [3:0] */
      aaMark = cfgAALfbCtrl & ~(0xfc00000f);

      GDBG_INFO(80, FN_NAME ":  Offsets of buffers from respective bases %d:\n",chipNum);
      GDBG_INFO(80, "\tcfgAALfbCtrol:       %08lx %08lx\n",cfgAALfbCtrl,(cfgAALfbCtrl & SST_AA_LFB_READ_FORMAT) >> SST_AA_LFB_READ_FORMAT_SHIFT);
      GDBG_INFO(80, "\tlocLFBMemCfg:        %08lx\n",locLFBMemCfg);
      GDBG_INFO(80, "\tlfbTileCompare:      %08lx\n",lfbTileCompare);
      GDBG_INFO(80, "\tstrideInTiles:       %08lx\n",strideInTiles);
      GDBG_INFO(80, "\ttileMark:            %08lx\n",tileLinearMark);
      GDBG_INFO(80, "\taaMark:              %08lx\n",aaMark);
      GDBG_INFO(80, "\tcolBufferStart0[0]:  %08lx\n",
                bInfo->buffInfo.colBuffStart0[0] - tileLinearMark);
      GDBG_INFO(80, "\tcolBufferStart0[1]:  %08lx\n",  
                bInfo->buffInfo.colBuffStart0[1] - tileLinearMark);
      GDBG_INFO(80, "\tauxBufferStart0:  %08lx\n",
                bInfo->buffInfo.auxBuffStart0 - tileLinearMark);

      GDBG_INFO(80, "\tcolBufferStart1[0]:  %08lx\n",
                bInfo->buffInfo.colBuffStart1[0] - aaMark);
      GDBG_INFO(80, "\tcolBufferStart1[1]:  %08lx\n",
                bInfo->buffInfo.colBuffStart1[1] - aaMark);
      GDBG_INFO(80, "\tauxBufferStart1:  %08lx\n",
                bInfo->buffInfo.auxBuffStart1 - aaMark);

      GDBG_INFO(80, "\tcolorBufferBegin1:   %08lx\n",
          (cfgAALfbCtrl & SST_SECONDARY_BUFFER_BASE));
      GDBG_INFO(80, "\tdepthBufferBegin:    %08lx\n",
          ((cfgDepthBufferAperture & SST_AA_DEPTH_BUFFER_APERTURE_BEGIN) >>
           SST_AA_DEPTH_BUFFER_APERTURE_BEGIN_SHIFT) * 4096);
      GDBG_INFO(80, "\tdepthBufferEnd:      %08lx\n",
          ((cfgDepthBufferAperture & SST_AA_DEPTH_BUFFER_APERTURE_END) >>
           SST_AA_DEPTH_BUFFER_APERTURE_END_SHIFT) * 4096);

      GDBG_INFO(80, "\tbpp:                 %08lx\n",bpp);
      GDBG_INFO(80, "\tdramInit0:           %08lx\n",dramInit0);
      GDBG_INFO(80, "\tdramInit1:           %08lx\n",dramInit1);
      GDBG_INFO(80, "\tmiscInit0:           %08lx\n",miscInit0);
      GDBG_INFO(80, "\tmiscInit1:           %08lx\n",miscInit1);
    }
    }
  }
  else if(IS_NAPALM(bInfo->pciInfo.deviceID) && bInfo->h3pixelSample == 2) {
    
    FxU32 value, pixFmt;
    /* Single board two sample AA setup */
    
    /* cfgSliLfbCtrl */
    _hrmWriteConfigRegister((hrmBoard_t *)bInfo->hMon,
                            0,
                            offsetof(SstPCIConfigRegs, cfgSliLfbCtrl),
                            0);
    /* cfgAALfbCtrl */                            
    value = SST_AA_LFB_CPU_WRITE_ENABLE |
            SST_AA_LFB_DISPATCH_WRITE_ENABLE |
            SST_AA_LFB_READ_ENABLE |
            bInfo->buffInfo.colBuffStart1[0];

    pixFmt = (vidProcCfg & SST_OVERLAY_PIXEL_FORMAT);
    
    switch(pixFmt) {
    case SST_OVERLAY_PIXEL_RGB32U:
      value |= SST_AA_LFB_READ_FORMAT_32BPP;
      break;
    case SST_OVERLAY_PIXEL_RGB565D:
    case SST_OVERLAY_PIXEL_RGB565U:
      value |= SST_AA_LFB_READ_FORMAT_16BPP;
      break;
    case SST_OVERLAY_PIXEL_RGB1555D:
    case SST_OVERLAY_PIXEL_RGB1555U:
      value |= SST_AA_LFB_READ_FORMAT_15BPP;
      break;
    }                            
    _hrmWriteConfigRegister((hrmBoard_t *)bInfo->hMon,
                            0,
                            offsetof(SstPCIConfigRegs, cfgAALfbCtrl),
                            value);
    /* cfgAADepthBufferAperture */
    _hrmWriteConfigRegister((hrmBoard_t *)bInfo->hMon,
                            0,
                            offsetof(SstPCIConfigRegs, cfgAADepthBufferAperture),
                            ((bInfo->buffInfo.auxBuffStart0 >> 12) << SST_AA_DEPTH_BUFFER_APERTURE_BEGIN_SHIFT) |
                            ((bInfo->buffInfo.auxBuffEnd0 >> 12)   << SST_AA_DEPTH_BUFFER_APERTURE_END_SHIFT));
    /* cfgVideoCtrl0 */
    _hrmWriteConfigRegister((hrmBoard_t *)bInfo->hMon,
                            0,
                            offsetof(SstPCIConfigRegs, cfgVideoCtrl0),
                            SST_CFG_ENHANCED_VIDEO_EN | SST_CFG_VIDEO_LOCALMUX_SEL |
                            SST_CFG_DIVIDE_VIDEO_BY_2);
    /* cfgVideoCtrl1 */
    _hrmWriteConfigRegister((hrmBoard_t *)bInfo->hMon,
                            0,
                            offsetof(SstPCIConfigRegs, cfgVideoCtrl1),
                            0);
    /* cfgVideoCtrl2 */
    _hrmWriteConfigRegister((hrmBoard_t *)bInfo->hMon,
                            0,
                            offsetof(SstPCIConfigRegs, cfgVideoCtrl2),
                            0);
                            
    
  }
#endif /* HWC_INIT_GDX */

#if !HWC_EXT_INIT && !HWC_GDX_INIT
  if((IS_NAPALM(bInfo->pciInfo.deviceID)) && (bInfo->pciInfo.numChips > 1))
  {
    hwcSetSLIAAMode(bInfo,
                 (bInfo->h3nwaySli > 1),     /* sliEnable */
                 (bInfo->h3pixelSample > 1), /* aaEnable */
                 bInfo->h3analogSli,         /* analogSLI */
                 bInfo->h3sliBandHeight,     /* sliBandHeight */
                 bInfo->h3Mem * 1024 * 1024, /* totalMem */
                 bInfo->pciInfo.numChips,    /* numChips */
                 (bInfo->h3pixelSample == 4),/* aaSampleHigh */
                 bInfo->buffInfo.colBuffStart1[0],       /* aaColorBuffStart */
                 bInfo->buffInfo.lfbBuffAddr0[bInfo->buffInfo.nColBuffers],     /* aaDepthBuffStart */
                 bInfo->buffInfo.lfbBuffAddr0End[bInfo->buffInfo.nColBuffers],  /* aaDepthBuffEnd */
                 bpp);

    /* The minivdd screws with lfbMemoryConfig, so we have to fix it. */
    HWC_IO_STORE(bInfo->regInfo, lfbMemoryConfig, lfbMemoryConfig);
    HWC_IO_STORE(bInfo->regInfo, vidMaxRGBDelta, bpp == 15 ? 0x101010 : 0x100810);
  }
  /* Single chip AA config... probably not needed as a seperate chunk of code. */
  else if(bInfo->pciInfo.numChips == 1 && bInfo->h3pixelSample == 2) {
  
    FxU32 value, pixFmt;
    /* Single board two sample AA setup */
    
    /* cfgSliLfbCtrl */
    hwcWriteConfigRegister(bInfo, 0, offsetof(SstPCIConfigRegs, cfgSliLfbCtrl), 0);
    
    /* cfgAALfbCtrl */                            
    value = SST_AA_LFB_CPU_WRITE_ENABLE |
            SST_AA_LFB_DISPATCH_WRITE_ENABLE |
            SST_AA_LFB_READ_ENABLE |
            bInfo->buffInfo.colBuffStart1[0];

    pixFmt = (vidProcCfg & SST_OVERLAY_PIXEL_FORMAT);
    
    switch(pixFmt) {
    case SST_OVERLAY_PIXEL_RGB32U:
      value |= SST_AA_LFB_READ_FORMAT_32BPP;
      break;
    case SST_OVERLAY_PIXEL_RGB565D:
    case SST_OVERLAY_PIXEL_RGB565U:
      value |= SST_AA_LFB_READ_FORMAT_16BPP;
      break;
    case SST_OVERLAY_PIXEL_RGB1555D:
    case SST_OVERLAY_PIXEL_RGB1555U:
      value |= SST_AA_LFB_READ_FORMAT_15BPP;
      break;
    }                            
    hwcWriteConfigRegister(bInfo,
                            0,
                            offsetof(SstPCIConfigRegs, cfgAALfbCtrl),
                            value);
    /* cfgAADepthBufferAperture */
    hwcWriteConfigRegister(bInfo,
                            0,
                            offsetof(SstPCIConfigRegs, cfgAADepthBufferAperture),
                            ((bInfo->buffInfo.auxBuffStart0 >> 12) << SST_AA_DEPTH_BUFFER_APERTURE_BEGIN_SHIFT) |
                            ((bInfo->buffInfo.auxBuffEnd0 >> 12)   << SST_AA_DEPTH_BUFFER_APERTURE_END_SHIFT));
    /* cfgVideoCtrl0 */
    hwcWriteConfigRegister(bInfo,
                            0,
                            offsetof(SstPCIConfigRegs, cfgVideoCtrl0),
                            SST_CFG_ENHANCED_VIDEO_EN | SST_CFG_VIDEO_LOCALMUX_SEL |
                            SST_CFG_DIVIDE_VIDEO_BY_2);
    /* cfgVideoCtrl1 */
    hwcWriteConfigRegister(bInfo,
                            0,
                            offsetof(SstPCIConfigRegs, cfgVideoCtrl1),
                            0);
    /* cfgVideoCtrl2 */
    hwcWriteConfigRegister(bInfo,
                            0,
                            offsetof(SstPCIConfigRegs, cfgVideoCtrl2),
                            0);
  }      

#endif /* !HWC_EXT_INIT && !HWC_INIT_GDX */
#endif /* FX_GLIDE_NAPALM */

  /* Temp hack */
  if(GETENV("SST_PCI_LOWTHRESH", bInfo->RegPath)) {
  FxU32 pciInit0, thresh;
    thresh = atoi(GETENV("SST_PCI_LOWTHRESH", bInfo->RegPath));
    if(thresh > 15) thresh = 15;
    HWC_IO_LOAD(bInfo->regInfo, pciInit0, pciInit0);
    pciInit0 &= ~SST_PCI_LOWTHRESH;        
    pciInit0 |= thresh << SST_PCI_LOWTHRESH_SHIFT;
    HWC_IO_STORE(bInfo->regInfo, pciInit0, pciInit0);
  }

  return FXTRUE;

#undef FN_NAME
} /* hwcInitVideo */

FxBool
hwcRestoreVideo(hwcBoardInfo *bInfo)
{
#define FN_NAME "hwcRestoreVideo"
  #if 1
  hwcIdleHardwareWithTimeout(bInfo);

  /* disable the CMD fifo */
  HWC_CAGP_STORE(bInfo->regInfo, cmdFifo0.baseSize, 0);
  #endif

  /* reset pci registers */
#ifdef FX_GLIDE_NAPALM
#ifdef HWC_EXT_INIT

#ifdef HWC_MINIVDD_HACK
  /*
   * Since I am a lazy bastard, try to use the minivdd to setup SLI/AA mode
   */
  if (((IS_NAPALM(bInfo->pciInfo.deviceID)) &&
       (bInfo->pciInfo.numChips > 1)) &&
      ((bInfo->h3nwaySli > 1) ||
       (bInfo->h3pixelSample > 2) ||
       (bInfo->h3pixelSample == 2 && !bInfo->buffInfo.enable2ndbuffer)))
  {

    GDBG_INFO(80, FN_NAME ": HWC_MINIVDD_HACK\n");

    if (bInfo->osNT)
    {
      hwcExtRequest_t ctxReq ;
      hwcExtResult_t  ctxRes ;
      FxU32 retVal = FXTRUE;

      GDBG_INFO(80, FN_NAME ": HWC_MINIVDD_HACK: OS is NT\n");

      ctxReq.which = HWCEXT_SLI_AA_REQUEST ;

      ctxReq.optData.sliAAReq.ChipInfo.dwaaEn             = (bInfo->h3pixelSample > 1) ;
      /* Request SLI or AA disable */
      ctxReq.optData.sliAAReq.ChipInfo.dwaaEn             = 0 ;
      ctxReq.optData.sliAAReq.ChipInfo.dwsliEn            = 0 ;

      ctxReq.optData.sliAAReq.ChipInfo.dwsli_nlines       = bInfo->h3sliBandHeight ;
      
      /* Fill out SLI mode information. */
      ctxReq.optData.sliAAReq.ChipInfo.dwChips            = bInfo->pciInfo.numChips ;
      ctxReq.optData.sliAAReq.ChipInfo.dwaaSampleHigh     = 0 ;

      GDBG_INFO(80, FN_NAME ": HWC_MINIVDD_HACK: ExEscape:HWCEXT_SLI_AA_REQUEST\n");
      retVal = ExtEscape((HDC)bInfo->hdc, HWCEXT_ESCAPE(bInfo->boardNum), 
                sizeof(ctxReq), (LPSTR) &ctxReq,
                sizeof(ctxRes), (LPSTR) &ctxRes);
      GDBG_INFO(80, FN_NAME ":  ExEscape retVal=%x\n", retVal);

    }
     else
    {
    DIOC_DATA DIOC_Data;
    SLI_AA_REQUEST Sli_AA_Request;
    HANDLE hDevice;

    GDBG_INFO(80, FN_NAME ": HWC_MINIVDD_HACK: OS is 9x\n");

    /* Request SLI or AA enable */
    Sli_AA_Request.ChipInfo.dwaaEn             = 0 ;
    Sli_AA_Request.ChipInfo.dwsliEn            = 0 ;

    Sli_AA_Request.ChipInfo.dwsli_nlines       = bInfo->h3sliBandHeight;
    
    /* Fill out SLI mode information. */
    Sli_AA_Request.ChipInfo.dwChips            = bInfo->pciInfo.numChips ;
    Sli_AA_Request.ChipInfo.dwaaSampleHigh     = 0;
 
    /*
     * Call the MiniVDD
     */
    hDevice = CreateFile(MINIVDDNAME, 0, 0, NULL, 0, 0, NULL);
    if (INVALID_HANDLE_VALUE != hDevice)
    {
      DIOC_Data.dwDevNode = bInfo->devNode ;
      DIOC_Data.dwSpare = (DWORD)&Sli_AA_Request;
      DeviceIoControl(hDevice, SLI_AA_DISABLE, &DIOC_Data, sizeof(DIOC_Data), NULL, 0x0, NULL, NULL);
      GDBG_INFO(80, FN_NAME ": HWC_MINIVDD_HACK: Called MiniVDD!\n");
    } else
      GDBG_INFO(80, FN_NAME ": HWC_MINIVDD_HACK: MiniVDD call failed!\n");
      CloseHandle(hDevice);
    }

  }
   else
#endif /* HWC_MINIVDD_HACK */

  if (IS_NAPALM(bInfo->pciInfo.deviceID))
  {
    hwcExtRequest_t
      ctxReq;
    hwcExtResult_t
      ctxRes;
    FxU32 
      function_number;
    FxBool
      retVal = FXFALSE;
                          
    ctxReq.which = HWCEXT_PCI_OP;

    GDBG_INFO(80, FN_NAME ": Not using HWC_MINIVDD_HACK\n");
                              
    for (function_number = 0; function_number < bInfo->pciInfo.numChips; function_number++) {

      ctxReq.optData.pciOpReq.DeviceId 
        = function_number & 0x3;
      ctxReq.optData.pciOpReq.Operation
        = HWCEXT_PCI_WRITE;
      ctxReq.optData.pciOpReq.Offset
        = offsetof(SstPCIConfigRegs, cfgSliLfbCtrl);
      ctxReq.optData.pciOpReq.Value
        = 0;
      retVal = ExtEscape((HDC)bInfo->hdc, HWCEXT_ESCAPE(bInfo->boardNum), 
                         sizeof(ctxReq), (LPSTR) &ctxReq,
                         sizeof(ctxRes), (LPSTR) &ctxRes);
                                       
      ctxReq.optData.pciOpReq.Offset
        = offsetof(SstPCIConfigRegs, cfgAADepthBufferAperture);
      ctxReq.optData.pciOpReq.Value
        = 0;
      retVal = ExtEscape((HDC)bInfo->hdc, HWCEXT_ESCAPE(bInfo->boardNum), 
                         sizeof(ctxReq), (LPSTR) &ctxReq,
                         sizeof(ctxRes), (LPSTR) &ctxRes);

      ctxReq.optData.pciOpReq.Offset
        = offsetof(SstPCIConfigRegs, cfgAALfbCtrl);
      ctxReq.optData.pciOpReq.Value
        = 0;

      retVal = ExtEscape((HDC)bInfo->hdc, HWCEXT_ESCAPE(bInfo->boardNum), 
                         sizeof(ctxReq), (LPSTR) &ctxReq,
                         sizeof(ctxRes), (LPSTR) &ctxRes);
                                                                                        
      ctxReq.optData.pciOpReq.Offset
        = offsetof(SstPCIConfigRegs, cfgVideoCtrl0);
      ctxReq.optData.pciOpReq.Value
        = 0;

      retVal = ExtEscape((HDC)bInfo->hdc, HWCEXT_ESCAPE(bInfo->boardNum), 
                         sizeof(ctxReq), (LPSTR) &ctxReq,
                         sizeof(ctxRes), (LPSTR) &ctxRes);

      ctxReq.optData.pciOpReq.Offset
        = offsetof(SstPCIConfigRegs, cfgVideoCtrl2);
      ctxReq.optData.pciOpReq.Value
        = 0;

      retVal = ExtEscape((HDC)bInfo->hdc, HWCEXT_ESCAPE(bInfo->boardNum), 
                         sizeof(ctxReq), (LPSTR) &ctxReq,
                         sizeof(ctxRes), (LPSTR) &ctxRes);
      ctxReq.optData.pciOpReq.Offset
        = offsetof(SstPCIConfigRegs, cfgVideoCtrl2);
      ctxReq.optData.pciOpReq.Value
        = 0;

      retVal = ExtEscape((HDC)bInfo->hdc, HWCEXT_ESCAPE(bInfo->boardNum), 
                         sizeof(ctxReq), (LPSTR) &ctxReq,
                         sizeof(ctxRes), (LPSTR) &ctxRes);
    }
  }
#endif

#if HWC_GDX_INIT
  if (((IS_NAPALM(bInfo->pciInfo.deviceID)) &&
       (bInfo->pciInfo.numChips > 1)) &&
      ((bInfo->h3nwaySli > 1) ||
       (bInfo->h3pixelSample > 2) ||
       (bInfo->h3pixelSample == 2 && !bInfo->buffInfo.enable2ndbuffer)))
  {
    hrmSLIAAChipInfo_t chipInfo;
    hrmSLIAAMemInfo_t  memInfo;
   
    chipInfo.size = sizeof(chipInfo); 
    chipInfo.aaEnable = 0;
    chipInfo.sliEnable = 0;
    chipInfo.numChips = bInfo->pciInfo.numChips;
    
    chipInfo.sliAaAnalog = 0;
    chipInfo.sli_nlines  = bInfo->h3sliBandHeight;
    chipInfo.swapAlgorithm = 0;
    chipInfo.aaSampleHigh = 0;
    
    memInfo.size = sizeof(memInfo);
    memInfo.totalMemory = bInfo->h3Mem;
    memInfo.tileMark = bInfo->buffInfo.colBuffStart0[0];
    memInfo.tileCmpMark = bInfo->buffInfo.colBuffStart0[0];
    memInfo.aaSecondaryColorBufBegin = bInfo->buffInfo.colBuffStart1[0];
    memInfo.aaSecondaryDepthBufBegin = bInfo->buffInfo.auxBuffStart1;
    memInfo.aaSecondaryDepthBufEnd = bInfo->buffInfo.auxBuffEnd1;
    memInfo.bpp = 0;
    
    _hrmSLIAA((hrmBoard_t *)bInfo->hMon,&chipInfo,&memInfo);
  }
  else if(IS_NAPALM(bInfo->pciInfo.deviceID)) {
     GDBG_INFO(80, "Cleaning up AA/SLI config registers\n");
   /* NOTE: Move this code to the ROM, so it can always
      clear out the AA/SLI config stuff after any mode change
      or whenever exclusive mode is released.  Doing it here
      will not always work. */
   /* cfgSliLfbCtrl */
    _hrmWriteConfigRegister((hrmBoard_t *)bInfo->hMon,
                            0,
                            offsetof(SstPCIConfigRegs, cfgSliLfbCtrl),
                            0);
    /* cfgAALfbCtrl */                            
    _hrmWriteConfigRegister((hrmBoard_t *)bInfo->hMon,
                            0,
                            offsetof(SstPCIConfigRegs, cfgAALfbCtrl),
                            0);
    /* cfgAADepthBufferAperture */
    _hrmWriteConfigRegister((hrmBoard_t *)bInfo->hMon,
                            0,
                            offsetof(SstPCIConfigRegs, cfgAADepthBufferAperture),
                            0);
    /* cfgVideoCtrl0 */
    _hrmWriteConfigRegister((hrmBoard_t *)bInfo->hMon,
                            0,
                            offsetof(SstPCIConfigRegs, cfgVideoCtrl0),
                            0);
    /* cfgVideoCtrl1 */
    _hrmWriteConfigRegister((hrmBoard_t *)bInfo->hMon,
                            0,
                            offsetof(SstPCIConfigRegs, cfgVideoCtrl1),
                            0);
    /* cfgVideoCtrl2 */
    _hrmWriteConfigRegister((hrmBoard_t *)bInfo->hMon,
                            0,
                            offsetof(SstPCIConfigRegs, cfgVideoCtrl2),
                            0);
  }
#endif /* HWC_INIT_GDX */

#if !HWC_EXT_INIT && !HWC_GDX_INIT
  if(IS_NAPALM(bInfo->pciInfo.deviceID)) {
    if(bInfo->pciInfo.numChips > 1) {
      hwcSetSLIAAMode(bInfo,
                      0,     /* sliEnable */
                      0,     /* aaEnable */
                      bInfo->h3analogSli,         /* analogSLI */
                      bInfo->h3sliBandHeight,     /* sliBandHeight */
                      bInfo->h3Mem * 1024 * 1024, /* totalMem */
                      bInfo->pciInfo.numChips,    /* numChips */
                      (bInfo->h3pixelSample == 4),/* aaSampleHigh */
                      bInfo->buffInfo.colBuffStart1[0],       /* aaColorBuffStart */
                      bInfo->buffInfo.lfbBuffAddr0[bInfo->buffInfo.nColBuffers],     /* aaDepthBuffStart */
                      bInfo->buffInfo.lfbBuffAddr0End[bInfo->buffInfo.nColBuffers],  /* aaDepthBuffEnd */
                      0);
    } else {  
      /* NOTE: Move this code to the ROM, so it can always
          clear out the AA/SLI config stuff after any mode change
          or whenever exclusive mode is released.  Doing it here
          will not always work. */
      /* cfgSliLfbCtrl */
        hwcWriteConfigRegister(bInfo,
                                0,
                                offsetof(SstPCIConfigRegs, cfgSliLfbCtrl),
                                0);
        /* cfgAALfbCtrl */                            
        hwcWriteConfigRegister(bInfo,
                                0,
                                offsetof(SstPCIConfigRegs, cfgAALfbCtrl),
                                0);
        /* cfgAADepthBufferAperture */
        hwcWriteConfigRegister(bInfo,
                                0,
                                offsetof(SstPCIConfigRegs, cfgAADepthBufferAperture),
                                0);
        /* cfgVideoCtrl0 */
        hwcWriteConfigRegister(bInfo,
                                0,
                                offsetof(SstPCIConfigRegs, cfgVideoCtrl0),
                                0);
        /* cfgVideoCtrl1 */
        hwcWriteConfigRegister(bInfo,
                                0,
                                offsetof(SstPCIConfigRegs, cfgVideoCtrl1),
                                0);
        /* cfgVideoCtrl2 */
        hwcWriteConfigRegister(bInfo,
                                0,
                                offsetof(SstPCIConfigRegs, cfgVideoCtrl2),
                                0);
    }
  }
#endif

#endif /* FX_GLIDE_NAPALM */

#ifdef HWC_EXT_INIT
  {
  FxI32 status;

  hwcExtRequest_t ctxReq;
  hwcExtResult_t  ctxRes;

  FxU32 retVal = FXTRUE;

  ctxReq.which = HWCEXT_HWCRLSEXCLUSIVE;
  ctxReq.optData.linearAddrReq.devNum = bInfo->boardNum;
  GDBG_INFO(90, FN_NAME ":  ExtEscape:HWCEXT_HWCRLSEXCLUSIVE\n");

  retVal = ExtEscape((HDC) bInfo->hdc, HWCEXT_ESCAPE(bInfo->boardNum), sizeof(ctxReq), (LPSTR) &ctxReq, /**/
      sizeof(ctxRes), (LPSTR) &ctxRes);

   GDBG_INFO(80, "%s:  sizeof(ctxRes) = %d\n", FN_NAME, sizeof(ctxRes));
   GDBG_INFO(80, "%s:  sizeof(ctxRes) = %d\n", FN_NAME, sizeof(ctxRes));
   GDBG_INFO(80, "%s:  &ctxRes = 0x%x\n", FN_NAME, &ctxRes);

   status = ctxRes.resStatus;

   GDBG_INFO(80, "%s:  ctxRes.resStatus = %d\n", FN_NAME, status);

    if (ctxRes.resStatus != 1) {
      hwc_errncpy(errorString, "HWCEXT_HWCRLSEXCLUSIVE Failed");
      return FXFALSE;
    }
    
  }
#endif /* HWC_EXT_INIT */

  /* Restore display */
#ifdef HWC_DXDRVR
  dxClose();
#else
  resetVideo();
#endif /* HWC_DXDRVR */

#ifdef IS_ALT_TAB
  if((WNDPROC)GetWindowLongPtr( (HWND)bInfo->vidInfo.hWnd, GWLP_WNDPROC ) == QueryContextProc) {
    GDBG_INFO(80, "%s:  restoring Wndproc\n", FN_NAME);
    SetWindowLongPtr( (HWND)bInfo->vidInfo.hWnd, GWLP_WNDPROC, (LONG_PTR)patchedWndproc );
    patchedWndproc = NULL;
    patchedHwnd = NULL;
  }
#endif
  
  return FXTRUE;
#undef FN_NAME
} /* hwcRestoreVideo */


#ifdef FX_GLIDE_NAPALM
/* These are platform dependant */
FxU32 hwcReadConfigRegister(hwcBoardInfo *bInfo, FxU32 chipNumber, FxU32 offset)
{
#if HWC_EXT_INIT    
  hwcExtRequest_t ctxReq;
  hwcExtResult_t ctxRes;
  
  ctxReq.which = HWCEXT_PCI_OP;
    
  ctxReq.optData.pciOpReq.DeviceId  = chipNumber;    
  ctxReq.optData.pciOpReq.Operation = HWCEXT_PCI_READ;
  ctxReq.optData.pciOpReq.Offset    = offset;

  /*
  NB:  Some lazy bugger didn't check the return value!
  */
  ExtEscape((HDC)bInfo->hdc, HWCEXT_ESCAPE(bInfo->boardNum), 
            sizeof(ctxReq), (LPSTR) &ctxReq,
            sizeof(ctxRes), (LPSTR) &ctxRes);

  return ctxRes.optData.pciOpRes.Value;
#elif HWC_GDX_INIT
  FxU32 value;

  _hrmReadConfigRegister( (hrmBoard_t *)bInfo->hMon, chipNumber, offset, &value );
  
  return value; 
#else 
  /* DOS */
  FxU32 data, result;
  PciRegister reg;
  reg.regAddress = offset;
  reg.sizeInBytes = 4;
  
  result = pciGetConfigDataRaw(reg, bInfo->deviceNum | (chipNumber << 13), &data);
  return data;
#endif
}

void hwcWriteConfigRegister(hwcBoardInfo *bInfo, FxU32 chipNumber, FxU32 offset, FxU32 value)
{
#if HWC_EXT_INIT    
  hwcExtRequest_t ctxReq;
  hwcExtResult_t ctxRes;
  
  ctxReq.which = HWCEXT_PCI_OP;
  
  ctxReq.optData.pciOpReq.DeviceId  = chipNumber;
  ctxReq.optData.pciOpReq.Operation = HWCEXT_PCI_WRITE;
  ctxReq.optData.pciOpReq.Offset    = offset;
  ctxReq.optData.pciOpReq.Value     = value;
  
  ExtEscape((HDC)bInfo->hdc, HWCEXT_ESCAPE(bInfo->boardNum), 
            sizeof(ctxReq), (LPSTR) &ctxReq,
            sizeof(ctxRes), (LPSTR) &ctxRes);
#elif HWC_GDX_INIT
  _hrmWriteConfigRegister( (hrmBoard_t *)bInfo->hMon, chipNumber, offset, value );
#else
  /* DOS */
  PciRegister reg;
  reg.regAddress = offset;
  reg.sizeInBytes = 4;

  pciSetConfigDataRaw(reg, bInfo->deviceNum | (chipNumber << 13), &value);
#endif
}

/* These are not */                        
void hwcSLIReadEnable(hwcBoardInfo *bInfo)
{
  FxU32 cfgSliLfbCtrl;
  FxU32 cfgAALfbCtrl;
  FxU32 chipNumber;

  if(GETENV("FX_GLIDE_A0_READ_ABORT", bInfo->RegPath)) {
    if(bInfo->h3nwaySli > 1) {
      for(chipNumber = 0; chipNumber < bInfo->pciInfo.numChips; chipNumber++) {
        /* Turn on SLI Read */
        cfgSliLfbCtrl = hwcReadConfigRegister(bInfo, chipNumber, offsetof(SstPCIConfigRegs, cfgSliLfbCtrl));
        cfgSliLfbCtrl |= (SST_SLI_LFB_READ_ENABLE);
        hwcWriteConfigRegister(bInfo, chipNumber, offsetof(SstPCIConfigRegs, cfgSliLfbCtrl), cfgSliLfbCtrl);
        
        /* Turn off AA read if it is not supposed to be enabled */
        if(bInfo->h3pixelSample < 2) {
            cfgAALfbCtrl = hwcReadConfigRegister(bInfo, chipNumber, offsetof(SstPCIConfigRegs, cfgAALfbCtrl));
            cfgAALfbCtrl &= ~(SST_AA_LFB_READ_ENABLE);
            hwcWriteConfigRegister(bInfo, chipNumber, offsetof(SstPCIConfigRegs, cfgAALfbCtrl), cfgAALfbCtrl);
        } 
      }
    }           
  }
}

void hwcSLIReadDisable(hwcBoardInfo *bInfo)
{
  FxU32 cfgSliLfbCtrl;
  FxU32 cfgAALfbCtrl;
  FxU32 chipNumber;
  
  if(GETENV("FX_GLIDE_A0_READ_ABORT", bInfo->RegPath)) {
    if(bInfo->h3nwaySli > 1) {
        for(chipNumber = 0; chipNumber < bInfo->pciInfo.numChips; chipNumber++) {
        /* Turn off SLI Read */
        cfgSliLfbCtrl = hwcReadConfigRegister(bInfo, chipNumber, offsetof(SstPCIConfigRegs, cfgSliLfbCtrl));
        cfgSliLfbCtrl &= ~(SST_SLI_LFB_READ_ENABLE);
        hwcWriteConfigRegister(bInfo, chipNumber, offsetof(SstPCIConfigRegs, cfgSliLfbCtrl), cfgSliLfbCtrl);
        
        /* Turn on AA Read */
        cfgAALfbCtrl = hwcReadConfigRegister(bInfo, chipNumber, offsetof(SstPCIConfigRegs, cfgAALfbCtrl));
        cfgAALfbCtrl |= (SST_AA_LFB_READ_ENABLE);
        hwcWriteConfigRegister(bInfo, chipNumber, offsetof(SstPCIConfigRegs, cfgAALfbCtrl), cfgAALfbCtrl);
      }
    }           
  }
}
#endif

char *
hwcGetErrorString()
{
#define FN_NAME "hwcGetErrorString"
  return errorString;
#undef FN_NAME
} /* hwcGetErrorString */

FxBool
hwcCheckMemSize(hwcBoardInfo *bInfo, FxU32 xres, FxU32 yres, FxU32 nColBuffers,
                FxU32 nAuxBuffers, FxBool tiled)
{
#define FN_NAME "hwcCheckMemSize"
  FxU32
    bufSize, totSize;

  bufSize = calcBufferSize(bInfo, xres, yres, tiled);

  totSize = (nColBuffers + nAuxBuffers) * bufSize;

  if (totSize < ((bInfo->h3Mem << 20) - bInfo->min_tramSize)) /* Need 2M for texture */
    return FXTRUE;
  else
    return FXFALSE;    
#undef FN_NAME
} /* hwcCheckMemSize */

static FxU32
calcBufferStride(hwcBoardInfo *bInfo, FxU32 xres, FxBool tiled)
{
  FxU32
    strideInTiles;
  FxU32 shift = (bInfo->h3pixelSize >> 1);

  if (tiled == FXTRUE) {
    /* Calculate tile width stuff */
    strideInTiles = (xres << shift) >> 7;
    if ((xres << shift) & (HWC_TILE_WIDTH - 1))
      strideInTiles++;
    
    return (strideInTiles * HWC_TILE_WIDTH);

  } else {
    return (xres << shift);
  }
} /* calcBufferStride */

static FxU32
calcBufferHeightInTiles(hwcBoardInfo *bInfo, FxU32 yres)
{
  FxU32
    heightInTiles,            /* Height of buffer in tiles */
    numSLIBands;              /* Number of SLI bands needed */


  yres = yres >> (bInfo->h3nwaySli >> 1);

  /* yres is per-chip, which must be a multiple of the
   * SLI band height */
  if(bInfo->h3nwaySli > 1) {
    /* Round up to the number of bands we need */
    numSLIBands = (yres + (bInfo->h3sliBandHeight - 1)) / bInfo->h3sliBandHeight;
    /* Recalc new height and fall through */
    yres = numSLIBands * bInfo->h3sliBandHeight;
  }
  
  /* Calculate tile height stuff */
  heightInTiles = yres >> 5;
  
  /* This makes sure the number of tiles is 
   * enough to cover the resolution we want. */
  if (yres & (HWC_TILE_HEIGHT - 1))
    heightInTiles++;

  return heightInTiles;

} /* calcBufferHeightInTiles */

static FxU32
calcBufferSizeInTiles(hwcBoardInfo *bInfo, FxU32 xres, FxU32 yres) {
  FxU32
    bufSizeInTiles;           /* Size of buffer in tiles */

  bufSizeInTiles =
    calcBufferHeightInTiles(bInfo, yres) * (calcBufferStride(bInfo, xres, FXTRUE) >> 7);

  return bufSizeInTiles;

} /* calcBufferSizeInTiles */

static FxU32
calcBufferSize(hwcBoardInfo *bInfo, FxU32 xres, FxU32 yres, FxBool tiled)
{
  FxU32
    stride,
    height,
    bufSize;                  /* Size of buffer in bytes */
  FxU32 shift = (bInfo->h3pixelSize >> 1);

  if (tiled) {
    stride = calcBufferStride(bInfo, xres, tiled);
    height = HWC_TILE_HEIGHT * calcBufferHeightInTiles(bInfo, yres);
  } else {
    stride = xres << shift;
    height = yres >> (bInfo->h3nwaySli >> 1);
  }

  bufSize = stride * height;
  
  return bufSize;

} /* calcBufferSize */

/* How the hw treats lfb accesses are dependent on the 'type' of
 * memory (tiled/linear) that the color/aux buffers are in. We
 * pre-compute the actual lfb address here while we know about the
 * memory space and if we adjusted the page alignment above.
 *
 * NB: If we are in tiled mode then the fact that we align the color
 * buffers on page boundaries means that the y offset of the buffers
 * may not actually be on a boundary for the tile addressing scheme.
 * The 'rounding' done to HWC_TILED_BUFFER_Y_ALIGN adjust for this.
 *
 * NB: The memory optimization of aligning color buffers on even page
 * boundaries will cause the tiled lfb access to be off by a page so
 * we add in the width of a page (HWC_TILED_BUFFER_X_ADJUST) here.
 */
#define HWC_TILED_BUFFER_BYTES    HWC_LFB_STRIDE  /* 128 Bytes x 32 lines */
#define HWC_TILED_BUFFER_Y_ALIGN  (HWC_LFB_STRIDE * 32)
#define HWC_TILED_BUFFER_X_ADJUST 0x80UL

static FxU32
hwcBufferLfbAddr(const hwcBoardInfo *bInfo, FxU32 physAddress)
{
  FxU32 retVal = 0x00UL;

  FxU32 tileAddress;
  FxU32 tileNumber;
  FxU32 tileOffset;
  FxU32 tileXOffset;
  FxU32 tileScanline;
  FxU32 tileRow;
  FxU32 lfbAddress;
  FxU32 lfbYOffset;

  if (bInfo->vidInfo.tiled) {    
    GDBG_INFO(80, "\tphysAddress: 0x%08lx\n",physAddress);

    /* Compute address in tile space */
    tileAddress = physAddress - bInfo->primaryOffset;
    GDBG_INFO(80, "\ttileAddress: 0x%08lx\n",tileAddress);

    /* Compute tile number we're in (each tile is 4K bytes) */
    tileNumber = tileAddress >> 12;
    GDBG_INFO(80, "\ttileNumber: 0x%08lx (%d)\n",tileNumber,tileNumber);

    /* Compute base tile row we're in */
    tileRow = tileNumber / bInfo->buffInfo.bufStrideInTiles;
    GDBG_INFO(80, "\ttileRow: %d  (stride = %d)\n",tileNumber,bInfo->buffInfo.bufStrideInTiles);

    /* Compute offset within the tile */
    tileOffset = tileAddress - (tileNumber << 12);
    GDBG_INFO(80, "\ttileOffset: 0x%08lx\n",tileOffset);

    /* Compute scanline within the tile */
    tileScanline = tileOffset >> 7;
    GDBG_INFO(80, "\ttileScanline: 0x%08lx\n",tileScanline);

    /* Compute tile X offset within the row */
    tileXOffset = tileNumber - (tileRow * bInfo->buffInfo.bufStrideInTiles);
    GDBG_INFO(80, "\ttileXOffset: %d\n",tileXOffset);

    /* Compute Y offset in LFB space */
    lfbYOffset = ((tileRow * 32 + tileScanline) << (bInfo->h3nwaySli >> 1));

    /* Compute LFB address of tile start */
    lfbAddress =  bInfo->primaryOffset + lfbYOffset * HWC_LFB_STRIDE + tileXOffset * 128;

    GDBG_INFO(80, "\tlfbAddress: %08lx\n", lfbAddress);
    retVal = lfbAddress;
  } else {
    retVal = physAddress;
  }
  return retVal;
}

static FxU32
pow2Round(FxU32 val, FxU32 pow2Const)
{
  const FxU32 pow2Mask = (pow2Const - 1UL);

  return ((val + pow2Mask) & ~pow2Mask);
}

FxU32 
hwcInitAGPFifo(hwcBoardInfo *bInfo, FxBool enableHoleCounting) 
{
#define FN_NAME "hwcInitAGPFifo"
#if HWC_EXT_INIT || HWC_GDX_INIT
  FxU32
    agpSize, agpLAddr, agpPAddr;
#if HWC_EXT_INIT
  FxU32
    cagpRegs;                   /* pointer to Cmd/AGP regs */
  FxI32 status;

  hwcExtRequest_t ctxReq;
  hwcExtResult_t  ctxRes;

  if (bInfo->regInfo.initialized == FXFALSE) {
    sprintf(errorString, "%s:  Called before hwcMapBoard\n", FN_NAME);
    return FXFALSE;
  }
  cagpRegs = bInfo->regInfo.cmdAGPBase;

  if (bInfo->buffInfo.initialized == FXFALSE) {
    sprintf(errorString, "%s:  Called before hwcInitBuffers\n", FN_NAME);
    return FXFALSE;
  }

  ctxReq.which = HWCEXT_GETAGPINFO;
  GDBG_INFO(80, FN_NAME ":  ExtEscape:HWCEXT_GETAGPINFO\n");

  ExtEscape((HDC) bInfo->hdc, HWCEXT_ESCAPE(bInfo->boardNum), sizeof(ctxReq), (LPSTR) &ctxReq, /**/
    sizeof(ctxRes), (LPSTR) &ctxRes);

  status = ctxRes.resStatus;
  agpLAddr = ctxRes.optData.agpInfoRes.lAddr;
  agpPAddr = ctxRes.optData.agpInfoRes.pAddr;
  agpSize = ctxRes.optData.agpInfoRes.size;

  /* If we fail, bail and go to memory fifo */
  if ((status != 1) ||
     (agpSize == 0))
  {
    return hwcInitFifo(bInfo, enableHoleCounting); 
  }

#elif defined(HWC_GDX_INIT)
  if(!_hrmGetAGPInfo) {
    return hwcInitFifo(bInfo, enableHoleCounting);
  }
  
  if(0 != _hrmGetAGPInfo((hrmBoard_t *)bInfo->hMon,&agpLAddr,&agpPAddr,&agpSize)) {
      return hwcInitFifo(bInfo, enableHoleCounting);
  }  
#endif

  bInfo->fifoInfo.agpFifo = 1;
  bInfo->fifoInfo.agpVirtAddr = agpLAddr;
  bInfo->fifoInfo.agpPhysAddr = agpPAddr;
  bInfo->fifoInfo.fifoStart = agpPAddr;
  bInfo->fifoInfo.agpSize = agpSize;

  GDBG_INFO(80, "%s AGP linear address = 0x%x\n", FN_NAME, agpLAddr);
  GDBG_INFO(80, "%s AGP physical address = 0x%x\n", FN_NAME, agpPAddr);
  GDBG_INFO(80, "%s AGP Size = 0x%x\n", FN_NAME, agpSize);

#if 0
  From Windows code:

  SETDW(_FF(lpCRegs)->cmdFifo0.baseSize, 0);
  SETDW(_FF(lpCRegs)->cmdFifo0.baseAddrL,
    (_FF(agpPhysAddr) >> 12) );
  SETDW(_FF(lpCRegs)->cmdFifo0.readPtrL, _FF(agpPhysAddr) );
  SETDW(_FF(lpCRegs)->cmdFifo0.readPtrH,  0);
  SETDW(_FF(lpCRegs)->cmdFifo0.aMin,  _FF(agpPhysAddr) - 4);
  SETDW(_FF(lpCRegs)->cmdFifo0.aMax,  _FF(agpPhysAddr) - 4);
  SETDW(_FF(lpCRegs)->cmdFifo0.depth, 0);
  SETDW(_FF(lpCRegs)->cmdFifo0.holeCount, 0);
  SETDW(_FF(lpCRegs)->cmdFifoThresh, 0x122);
  SETDW(_FF(lpCRegs)->cmdFifo0.baseSize, 0x700);
#endif

  HWC_CAGP_STORE(bInfo->regInfo, cmdFifo0.baseAddrL,
    bInfo->fifoInfo.fifoStart>>12);
  HWC_CAGP_STORE(bInfo->regInfo, cmdFifo0.readPtrL, bInfo->fifoInfo.fifoStart);
  HWC_CAGP_STORE(bInfo->regInfo, cmdFifo0.readPtrH, 0);
  HWC_CAGP_STORE(bInfo->regInfo, cmdFifo0.aMin, bInfo->fifoInfo.fifoStart-4);
  HWC_CAGP_STORE(bInfo->regInfo, cmdFifo0.aMax, bInfo->fifoInfo.fifoStart-4);
  
  HWC_CAGP_STORE(bInfo->regInfo, cmdFifo0.depth, 0);
  HWC_CAGP_STORE(bInfo->regInfo, cmdFifo0.holeCount, 0);
  /* Fifo LWM /HWM/ THRESHOLD */
  HWC_CAGP_STORE(bInfo->regInfo, cmdFifoThresh, (0x09 << SST_HIGHWATER_SHIFT) | 0x2); 
  HWC_CAGP_STORE(bInfo->regInfo, cmdFifo0.baseSize,
    ((bInfo->fifoInfo.fifoLength >> 12) - 1) | SST_EN_CMDFIFO |
    SST_CMDFIFO_DISABLE_HOLES | SST_CMDFIFO_AGP);
  
  GDBG_INFO(2,"%s:  CMD FIFO placed at physical addr 0x%x\n", FN_NAME,
    bInfo->fifoInfo.fifoStart);

  return FXTRUE;
#else /* !HWC_EXT_INIT */
  return hwcInitFifo(bInfo, enableHoleCounting);
#endif /* !HWC_EXT_INIT */
#undef FN_NAME
} /* hwcInitAGPFifo */ 


FxBool
hwcAllocAuxRenderingBuffer(hwcBoardInfo *bInfo, hwcBufferDesc *bp, int width,
                           int height)  
{
#define FN_NAME "hwcAllocAuxRenderingBuffer"
  /* For now, these buffers  */
  FxBool
    rVal = FXFALSE;
  
  FxU32
    bufSize,
    offset;

  bufSize = (width * height) << 2;

  offset = bInfo->fbOffset - bufSize;

  if ((offset - bInfo->tramSize) < bInfo->min_tramSize) {
    sprintf(errorString, "%s:  Insufficient memory", FN_NAME);
  } else {
    bp->bufOffset = offset;
    bp->bufStride = width << 2; /* Below tiled memory for now */
    bp->bufBPP = 16;
    bp->tiled = 0;
    bp->bufType = HWC_BUFFER_AUXRENDER;

    bInfo->fbOffset -= bufSize;

    rVal = FXTRUE;
  }

  return rVal;

#undef FN_NAME
} /* hwcAllocAuxRenderingBuffer */

static void hwcReadBuffer565(hwcBoardInfo *bInfo, FxU32 src, FxU32 strideInBytes, FxU8 *dst, FxU32 renderMask, FxU32 compareMask, FxU32 aaShift)
{
  FxU32 width, height;
  FxU32 x, y, srcY;
  FxU16 *s;
  FxU8  *d;
  FxU16 pix16;
  FxU8  r, g, b;

#if __POWERPC__
  src += bInfo->pciInfo.swizzleOffset[3];
#endif

  width = bInfo->vidInfo.xRes;
  height = bInfo->vidInfo.yRes;

  srcY = 0;

  for(y = 0; y < height; y++) {    
    /* Dst always walks down entire framebuffer (or up, since this is Targa) */
    d = dst + width*3*(height-y-1);

    if((y & renderMask) == compareMask) {
      /* This chip owns this scanline. */
      s = (FxU16 *)(src + srcY*strideInBytes);
      for(x = 0; x < width; x++) {
        pix16 = *s++;
        r = (pix16 >> 11) & 0x1f;
        g = (pix16 >> 5) & 0x3f;
        b = pix16 & 0x1f;
        /* Targa is bgr. */
        *d++ += (b << (1 + aaShift));
        *d++ += (g << aaShift);
        *d++ += (r << (1 + aaShift));
      }  
      srcY++;
    }
  }  
}

static void hwcReadBuffer1555(hwcBoardInfo *bInfo, FxU32 src, FxU32 strideInBytes, FxU8 *dst, FxU32 renderMask, FxU32 compareMask, FxU32 aaShift)
{
  FxU32 width, height;
  FxU32 x, y, srcY;
  FxU16 *s;
  FxU8  *d;
  FxU16 pix16;
  FxU8  r, g, b;

#if __POWERPC__
  src += bInfo->pciInfo.swizzleOffset[3];
#endif

  width = bInfo->vidInfo.xRes;
  height = bInfo->vidInfo.yRes;
  
  srcY = 0;

  for(y = 0; y < height; y++) {    
    /* Dst always walks down entire framebuffer (or up, since this is Targa) */
    d = dst + width*3*(height-y-1);

    if((y & renderMask) == compareMask) {
      /* This chip owns this scanline. */
      s = (FxU16 *)(src + srcY*strideInBytes);
      for(x = 0; x < width; x++) {
        pix16 = *s++;
        r = (pix16 >> 10) & 0x1f;
        g = (pix16 >> 5) & 0x1f;
        b = pix16 & 0x1f;
        /* Targa is bgr. */
        *d++ += (b << (1 + aaShift));
        *d++ += (g << (1 + aaShift));
        *d++ += (r << (1 + aaShift));
      }  
      srcY++;
    }
  }  
}

static void hwcReadBuffer8888(hwcBoardInfo *bInfo, FxU32 src, FxU32 strideInBytes, FxU8 *dst, FxU32 renderMask, FxU32 compareMask, FxU32 aaShift)
{
  FxU32 width, height;
  FxU32 x, y, srcY;
  FxU32 *s;
  FxU8  *d;
  FxU32 pix32;
  FxU8  r, g, b;

  width = bInfo->vidInfo.xRes;
  height = bInfo->vidInfo.yRes;

#if __POWERPC__
  src += bInfo->pciInfo.swizzleOffset[1];
#endif

  srcY = 0;

  for(y = 0; y < height; y++) {    
    /* Dst always walks down entire framebuffer (or up, since this is Targa) */
    d = dst + width*3*(height-y-1);

    if((y & renderMask) == compareMask) {
      /* This chip owns this scanline. */
      s = (FxU32 *)(src + srcY*strideInBytes);
      for(x = 0; x < width; x++) {
        pix32 = *s++;
        r = (FxU8)((pix32 >> 16) & 0xff);
        g = (FxU8)((pix32 >> 8) & 0xff);
        b = (FxU8)(pix32 & 0xff);

        /* Targa is bgr. */
        *d++ += b >> (aaShift) ;
        *d++ += g >> (aaShift) ;
        *d++ += r >> (aaShift) ;
      }  
      srcY++;
    }
  }  
}
  
static void hwcGammaCorrect(hwcBoardInfo *bInfo, FxU8 *buffer, FxU32 width, FxU32 height)
{
  FxU32 red[256], green[256], blue[256];
  FxU32 i;

  /* Read current hardware gamma table. */
  if(hwcGetGammaTable(bInfo, 256, red, green, blue)) {
    for(i = 0; i < width*height; i++) {
      *buffer = (FxU8)blue[*buffer]; buffer++;
      *buffer = (FxU8)green[*buffer]; buffer++;
      *buffer = (FxU8)red[*buffer]; buffer++;
    }  
  }
}

/* Amazingly nasty code follows. */
void hwcAAScreenShot(hwcBoardInfo *bInfo, FxU32 colBufNum)
{
  /* We want to save off a 32-bit .TGA file (or whatever), even though
   * we only have 16-bit buffers.  In order to do this we have to individually
   * read from each AA sample buffer and merge the results by hand.   The merging
   * is the easy part.  Reading from each buffer individually is the really hard
   * part.  It would have ruled to have the read hardware have some way to "select"
   * which sample we want to read from, rather than always read from all of them.
   * 
   * So, the strategy is this.  First, figure out where the master and slave chips
   * are mapped and save off their config info.  Then, map everything to where the
   * slaves live and deconfigure the magic AA read stuff.  Then, for each chip we'll
   * map it to where the master was supposed to be.  Then we move the tileMark to the
   * base of the primary AA buffer and snarf the data.  Then we do the same for the
   * secondary AA buffer.  We then put the lfb config back where we're supposed to and
   * then map the chip back to the slave location.
   * 
   * After we've done all that we map everything back the way it was originally and then
   * (finally) save off the image file to disk.  */
  
  /* Note: This routine currently doesn't grok anything more than four chip configs. */
  FxU32 memBase1[4];
  FxU32 cfgInitEnable[4];
  FxU32 cfgPciDecode[4];
  FxU32 cfgAALfbCtrl[4];
  FxU32 cfgSliLfbCtrl[4];
  FxU32 cfgAADepthBufferAperture[4];
  FxU32 chip;
  FxU32 temp, vidProcCfg;
  FxU32 compareMask, renderMask, aaShift;
  FxU32 lfbMemoryConfig, lfbMemoryConfigTemp;
  void (*hwcReadBufferFunc)(hwcBoardInfo *bInfo, FxU32 src, FxU32 strideInBytes, FxU8 *dst, FxU32 renderMask, FxU32 compareMask, FxU32 aaShift);
  FxU8  header[18];
  FILE  *file;

  static FxU32 fileNameNum;
  char fileName[256];

  FxU8 *buffer;

  /* Allocate buffer to store frame into... */
  buffer = calloc(bInfo->vidInfo.xRes * bInfo->vidInfo.yRes, 3);

  if(!buffer)
    return;
  
  /* Have to do it the hard way for 16-bit 
   * or for the new way of doing 2-sample */
  if ((bInfo->h3pixelSize == 2) ||
     (bInfo->h3pixelSample == 2)) {
    
    /* Figure out framebuffer format (1555 or 565) */
    HWC_IO_LOAD(bInfo->regInfo, vidProcCfg, vidProcCfg);
    switch(vidProcCfg & SST_OVERLAY_PIXEL_FORMAT) {
      case SST_OVERLAY_PIXEL_RGB1555D:
      case SST_OVERLAY_PIXEL_RGB1555U:
        hwcReadBufferFunc = hwcReadBuffer1555;
        break;

      case SST_OVERLAY_PIXEL_RGB565D:
      case SST_OVERLAY_PIXEL_RGB565U:
        hwcReadBufferFunc = hwcReadBuffer565;
        break;

      case SST_OVERLAY_PIXEL_RGB32U:
        hwcReadBufferFunc = hwcReadBuffer8888;
        break ;
    }  
    
    /* First, figure out where everything is. */
    for(chip = 0; chip < bInfo->pciInfo.numChips; chip++) {
      memBase1[chip] = hwcReadConfigRegister(bInfo, chip, offsetof(SstPCIConfigRegs, memBaseAddr1));
      cfgInitEnable[chip] = hwcReadConfigRegister(bInfo, chip, offsetof(SstPCIConfigRegs, cfgInitEnable_FabID));
      cfgPciDecode[chip] = hwcReadConfigRegister(bInfo, chip, offsetof(SstPCIConfigRegs, cfgPciDecode));
      cfgSliLfbCtrl[chip] = hwcReadConfigRegister(bInfo, chip, offsetof(SstPCIConfigRegs, cfgSliLfbCtrl));
      cfgAALfbCtrl[chip] = hwcReadConfigRegister(bInfo, chip, offsetof(SstPCIConfigRegs, cfgAALfbCtrl));
      cfgAADepthBufferAperture[chip] = hwcReadConfigRegister(bInfo, chip, offsetof(SstPCIConfigRegs, cfgAADepthBufferAperture));
    }
  
    /* Map everyone to where the slave lives and disable snooping, SLI && AA reads */
    for(chip = 0; chip < bInfo->pciInfo.numChips; chip++) {
      temp = cfgInitEnable[chip] >> 8;
      temp &= ~(SST_ADDRESS_SNOOP_ENABLE|
                SST_MEMBASE0_SNOOP_ENABLE|
                SST_MEMBASE1_SNOOP_ENABLE|
                SST_ADDRESS_SNOOP_SLAVE);
      temp |= SST_ENABLE_BASE_ADDR_WRITES;
      hwcWriteConfigRegister(bInfo, chip, offsetof(SstPCIConfigRegs, cfgInitEnable_FabID), temp << 8);
      hwcWriteConfigRegister(bInfo, chip, offsetof(SstPCIConfigRegs, cfgSliLfbCtrl),0);
      hwcWriteConfigRegister(bInfo, chip, offsetof(SstPCIConfigRegs, cfgAALfbCtrl),0);
      hwcWriteConfigRegister(bInfo, chip, offsetof(SstPCIConfigRegs, cfgAADepthBufferAperture),0);
      /* For PowerPC systems we have to futz around with cfgPciDecode a bit more since the master would normally try to decode
       * 256MB of address space, which will screw up when we map it to where the slaves live. */
#if __POWERPC__      
      hwcWriteConfigRegister(bInfo, chip, offsetof(SstPCIConfigRegs, cfgPciDecode),
        SST_PCI_IOBASE0_DECODE_256 | SST_PCI_MEMBASE0_DECODE_32MB | SST_PCI_MEMBASE1_DECODE_64MB);
#else
      hwcWriteConfigRegister(bInfo, chip, offsetof(SstPCIConfigRegs, cfgPciDecode),cfgPciDecode[chip] & ~SST_MEMBASE1_SNOOP);
#endif      
      hwcWriteConfigRegister(bInfo, chip, offsetof(SstPCIConfigRegs, memBaseAddr1),memBase1[1]);
    }  

    /* Map chips and read buffers */
    for(chip = 0; chip < bInfo->pciInfo.numChips; chip++) {

      /* Grab SLI config stuff */
      if(cfgSliLfbCtrl[chip] & SST_SLI_LFB_READ_ENABLE) {
        renderMask = (cfgSliLfbCtrl[chip] & SST_SLI_LFB_RENDERMASK) >> SST_SLI_LFB_RENDERMASK_SHIFT;
        compareMask = (cfgSliLfbCtrl[chip] & SST_SLI_LFB_COMPAREMASK) >> SST_SLI_LFB_COMPAREMASK_SHIFT;
      } else {
        /* Non-SLI, use all scanlines */
        compareMask = renderMask = 0x00;
      }   

      /* Grab AA config stuff */
      if(cfgAALfbCtrl[chip] & SST_AA_LFB_READ_ENABLE) {
        /* Grab the number of samples from bInfo rather than the hardware.  This
         * makes it easier to deal with the new style 2-sample AA mode that uses
         * one buffer per chip, even though the backend LFB stuff is still configured
         * for two (that point to the same place). */
        if(bInfo->h3pixelSample == 4) {
          aaShift = 0;
        } else {
          aaShift = 1;
        }  
      } else {
        aaShift = 2;
      }
       
      /* Map chip to master address space. */
      hwcWriteConfigRegister(bInfo, chip, offsetof(SstPCIConfigRegs, memBaseAddr1),memBase1[0]);    
      /* Again, on PowerPC we have to reprogram the chip we're looking at to decode the proper amount of memory. (256MB) */
#if __POWERPC__
      hwcWriteConfigRegister(bInfo, chip, offsetof(SstPCIConfigRegs, cfgPciDecode),
              SST_PCI_IOBASE0_DECODE_256 | SST_PCI_MEMBASE0_DECODE_32MB | SST_PCI_MEMBASE1_DECODE_256MB);
#endif      

      /* This will always read from the master, but all chips share the same values. */
      if(chip == 0) {
        HWC_IO_LOAD(bInfo->regInfo, lfbMemoryConfig, lfbMemoryConfig);
      } else {
        HWC_IO_LOAD_SLAVE(chip, bInfo->regInfo, lfbMemoryConfig, lfbMemoryConfig);
      }  

      /* Just bang on lfbMemConfig to point to the base of the color buffer we want. */
      lfbMemoryConfigTemp =
        SST_RAW_LFB_TILE_BEGIN_PAGE_MUNGE((bInfo->buffInfo.colBuffStart0[colBufNum] >> 12))
          | HWC_RAW_LFB_STRIDE
          | (bInfo->buffInfo.bufStrideInTiles << SST_RAW_LFB_TILE_STRIDE_SHIFT);        

      if(chip == 0) {
        HWC_IO_STORE(bInfo->regInfo, lfbMemoryConfig, lfbMemoryConfigTemp);
      } else {
        HWC_IO_STORE_SLAVE(chip,bInfo->regInfo, lfbMemoryConfig, lfbMemoryConfigTemp);
      }  

      /* Read primary AA buffer */
      hwcReadBufferFunc(bInfo, bInfo->regInfo.rawLfbBase + bInfo->buffInfo.colBuffStart0[colBufNum], bInfo->buffInfo.bufLfbStride, buffer,
          renderMask, compareMask, aaShift);
        
      /* Now do secondary AA buffer, if enabled. */
      if(bInfo->buffInfo.enable2ndbuffer) {
        lfbMemoryConfigTemp =
          SST_RAW_LFB_TILE_BEGIN_PAGE_MUNGE((bInfo->buffInfo.colBuffStart1[colBufNum] >> 12))
            | HWC_RAW_LFB_STRIDE
            | (bInfo->buffInfo.bufStrideInTiles << SST_RAW_LFB_TILE_STRIDE_SHIFT);        
    
        if(chip == 0) {
          HWC_IO_STORE(bInfo->regInfo, lfbMemoryConfig, lfbMemoryConfigTemp);
        } else {
          HWC_IO_STORE_SLAVE(chip, bInfo->regInfo, lfbMemoryConfig, lfbMemoryConfigTemp);
        }  
                
        /* Read seconday AA buffer */
        hwcReadBufferFunc(bInfo, bInfo->regInfo.rawLfbBase + bInfo->buffInfo.colBuffStart1[colBufNum], bInfo->buffInfo.bufLfbStride, buffer,
            renderMask, compareMask, aaShift);
      }
        
      /* Restore lfbMemoryConfig */
      if(chip == 0) {
        HWC_IO_STORE(bInfo->regInfo, lfbMemoryConfig, lfbMemoryConfig);
      } else {
        HWC_IO_STORE_SLAVE(chip,bInfo->regInfo, lfbMemoryConfig, lfbMemoryConfig);
      }  
    
      /* Put chip back in temp location with the rest of the slaves. */
#if __POWERPC__
      hwcWriteConfigRegister(bInfo, chip, offsetof(SstPCIConfigRegs, cfgPciDecode),
              SST_PCI_IOBASE0_DECODE_256 | SST_PCI_MEMBASE0_DECODE_32MB | SST_PCI_MEMBASE1_DECODE_64MB);
#endif      
      hwcWriteConfigRegister(bInfo, chip, offsetof(SstPCIConfigRegs, memBaseAddr1),memBase1[1]);    

    } 
   
    /* Map everyone back */
    for(chip = 0; chip < bInfo->pciInfo.numChips; chip++) {
      hwcWriteConfigRegister(bInfo, chip, offsetof(SstPCIConfigRegs, memBaseAddr1),  memBase1[chip]);  
    }

    /* Re-enable stuff */
    for(chip = 0; chip < bInfo->pciInfo.numChips; chip++) {
      hwcWriteConfigRegister(bInfo, chip, offsetof(SstPCIConfigRegs, cfgInitEnable_FabID), cfgInitEnable[chip]);
      hwcWriteConfigRegister(bInfo, chip, offsetof(SstPCIConfigRegs, cfgSliLfbCtrl), cfgSliLfbCtrl[chip]);
      hwcWriteConfigRegister(bInfo, chip, offsetof(SstPCIConfigRegs, cfgAALfbCtrl),  cfgAALfbCtrl[chip]);
      hwcWriteConfigRegister(bInfo, chip, offsetof(SstPCIConfigRegs, cfgAADepthBufferAperture), cfgAADepthBufferAperture[chip]);
      hwcWriteConfigRegister(bInfo, chip, offsetof(SstPCIConfigRegs, cfgPciDecode),cfgPciDecode[chip]);
    }  
  } else {
    /* 32-bit case is easy.  We only have to read from the primary buffer on the master and let
     * the hardware do the rest */
    hwcReadBuffer8888(bInfo, bInfo->regInfo.rawLfbBase + bInfo->buffInfo.lfbBuffAddr0[colBufNum], bInfo->buffInfo.bufLfbStride, buffer,
        0x00, 0x00, 0);    
  }  

  /* Gamma correct image based on DAC contents. */
  hwcGammaCorrect(bInfo,buffer,bInfo->vidInfo.xRes,bInfo->vidInfo.yRes);

  /* Write buffer to disk */
  sprintf(fileName,"glide%04ld.tga",fileNameNum++);
  memset (header, 0, 18);
  header[2] = 2;    /* Uncompressed targa */
  header[12] = (FxU8)(bInfo->vidInfo.xRes & 255);
  header[13] = (FxU8)(bInfo->vidInfo.xRes >> 8);
  header[14] = (FxU8)(bInfo->vidInfo.yRes & 255);
  header[15] = (FxU8)(bInfo->vidInfo.yRes >> 8);
  header[16] = 24;  /* bytes per pixel */

  if(file = fopen(fileName,"wb")) {
    fwrite(header,18,1,file);
    fwrite(buffer,bInfo->vidInfo.xRes * bInfo->vidInfo.yRes * 3, 1, file);
    fclose(file);
  }

  /* Free memory */
  free(buffer);
}      

#define RED_SHIFT       16
#define GREEN_SHIFT     8
#define BLUE_SHIFT      0


#define CLAMP(val, min, max) if (val > max) val = max; /*else if (val < min) val = min;*//* KoolSmoky */
#define ADJUST(val, lowest, low, high, typ) if (high < lowest) val=(typ)(low); else val = (typ)(high)
#define GETFLOATENV(s, r, v) if (GETENV(s, r)) v = (FxFloat)(atof(GETENV(s, r)))

/*---------------------------------------------------------------------------
 * Function    : adjustBrightnessAndContrast_m    
 *
 * Description :
 *               Routine to generate and apply a new gamma table based on 
 *               passed in Brightness and Contrast values.
 *               contrast is >=0.0
 *               Brightness may be negative.
 *---------------------------------------------------------------------------*/

static FxBool adjustBrightnessAndContrast_m(FxFloat contrast,
					    FxFloat brightness,
					    FxU32 nEntries,
					    FxU32 *pR, FxU32 *pG, FxU32 *pB)
{
  FxU16 i;
  FxFloat r,g,b;
  
  if (pR == NULL || pG == NULL || pG == NULL)
      return FXFALSE;
  
  GDBG_INFO(69, ":\tSlot\t\tRed\t\tGreen\t\tBlue\n");

  /* If the 0th row of the table contains non zero values then we get strange banding effects (Napalm) on
   * returning to the desktop to ensure this does not happen these values are intialised to zero.
   * Note this will not happen unless a brightness modifier is supplied, which highlighted the issue. */
   GDBG_INFO(69, "Note r[0]=g[0]=b[0]=special default \n");
   pB[0] = pG[0] = pR[0] = 0;

  for (i=1; i<nEntries; i++) 
  {
      /* Note to resolve rouding/sign errors, keep everything in the Fp domain until the last minute */

      /* R */
      r = (pR[i] * contrast) + brightness;
      ADJUST(pR[i], 0.0f, 0, r, FxU32);	
      CLAMP(pR[i], 0, 255);
      
      /* G */
      g = (pG[i] * contrast) + brightness;
      ADJUST(pG[i], 0.0f, 0, g, FxU32);	
      CLAMP(pG[i], 0, 255);
      
      /* B */
      b = (pB[i] * contrast) + brightness;
      ADJUST(pB[i], 0.0f, 0, b, FxU32);
      CLAMP(pB[i], 0, 255);

      GDBG_INFO(69, "\t%d\t\t%d\t\t%d\t\t%d\n", i, pR[i], pG[i], pB[i]);
  }
  
  return FXTRUE;
}
/* Gamma */


FxBool
hwcGammaTable(hwcBoardInfo *bInfo, FxU32 nEntries, FxU32 *r, FxU32 *g, FxU32 *b)
{
#define FN_NAME "hwcGammaTable"
#if 1
  FxU32 gRamp[256];
  FxU32 i;
  FxU32 vidProcCfg;
  FxU32 dacBase;
  FxU32 rDacBase;
  FxU32 rDacData;
  char *psBrightness = "FX_GLIDE_BRIGHTNESS";
  char *psContrast = "FX_GLIDE_CONTRAST";
  
  FxFloat brightness = 0.0f;
  FxFloat contrast = 1.0f;

  /* override */
  /* Adjust Gamma as user selected */
  if( GETENV(psBrightness, bInfo->RegPath) )
    GETFLOATENV(psBrightness, bInfo->RegPath, brightness);
  if( GETENV(psContrast, bInfo->RegPath) )
    GETFLOATENV(psContrast, bInfo->RegPath, contrast);
  
  /* clamp contrast to > 0.0 */
  if (contrast <= 0.0f) {
    contrast = 1.0f;
    GDBG_INFO(69,": minihwc Readjusting contrast to %3.3f as <=0.0 \n", contrast);
  }

  GDBG_INFO(69,": minihwc Overriding Brightness with %3.3f and Contrast with %3.3f\n", brightness, contrast);

  adjustBrightnessAndContrast_m(contrast, brightness, nEntries, r,g,b);

  /* Load the table into the Display driver as above */
  for (i = 0; i < nEntries; i++) {
    gRamp[i] =
      ((r[i] & 0xff) << RED_SHIFT) |
        ((g[i] & 0xff) << GREEN_SHIFT) |  
          ((b[i] & 0xff) << BLUE_SHIFT);
		GDBG_INFO(69,": gRamp[%d] = %d\n", i, gRamp[i]);
  }
  
  /*
   **  On W9X/DOS, we can do this ourselves--which is much easier than
   **  mucking about with a bunch of 32-bit data in 16-bit driver
   **  code.
   */
  HWC_IO_LOAD( bInfo->regInfo, vidProcCfg, vidProcCfg);
  
  /* Determin which set of CLUT entries are selected */
  if (vidProcCfg & SST_OVERLAY_CLUT_SELECT)
    dacBase = 256;
  else
    dacBase = 0;
 
  /* Print out some useful info RE the vidProcCfg register */
  GDBG_INFO(80, "%s:  vidProcCFG(SST_OVERLAY_CLUT_SELECT) = %d\n",
            FN_NAME, (vidProcCfg & SST_OVERLAY_CLUT_SELECT) ? 1 : 0);
  GDBG_INFO(80, "%s:  vidProcCFG(SST_OVERLAY_EN) = %d\n",
            FN_NAME, (vidProcCfg & SST_OVERLAY_EN) ? 1 : 0);
  GDBG_INFO(80, "%s:  vidProcCFG(SST_OVERLAY_CLUT_BYPASS) = %d\n",
            FN_NAME, (vidProcCfg & SST_OVERLAY_CLUT_BYPASS) ? 1 : 0);

  for (i = 0; i < nEntries; i++) {
    int repeat = 100;
    while (repeat) {
      HWC_IO_STORE( bInfo->regInfo, dacAddr, dacBase + i);
      P6FENCE;
      HWC_IO_STORE( bInfo->regInfo, dacData, gRamp[i]);
      P6FENCE;
      HWC_IO_LOAD( bInfo->regInfo, dacAddr, rDacBase);
      P6FENCE;
      HWC_IO_LOAD( bInfo->regInfo, dacData, rDacData);
      P6FENCE;
      if ((rDacBase == (dacBase + i)) && (rDacData == gRamp[i]))
        break;
      repeat --;
    }
    if (!repeat) {
      GDBG_INFO(0, "%s: Error Writing DacData [%d, %x]. DacBase = %d\n",
                FN_NAME, i, gRamp[i], dacBase);
    }
  }

  return FXTRUE;
  
#else
  /*
   * Here lies the GDIish implementation of hwcGammaTable.
   * Although slower than just banging on the hardware, this
   * version is WinNT-friendly.
   */
  DDGAMMARAMP theRamp ;
  FxU32       index ;

  char *psBrightness = "FX_GLIDE_BRIGHTNESS";
  char *psContrast = "FX_GLIDE_CONTRAST";
  
  FxFloat brightness = 0.0f;
  FxFloat contrast = 1.0f;

  /* override */
  /* Adjust Gamma as user selected */
  if( GETENV(psBrightness, bInfo->RegPath) )
    GETFLOATENV(psBrightness, bInfo->RegPath, brightness);
  if( GETENV(psContrast, bInfo->RegPath) )
    GETFLOATENV(psContrast, bInfo->RegPath, contrast);
  
  /* clamp contrast to > 0.0 */
  if (contrast <= 0.0f) {
    contrast = 1.0f;
    GDBG_INFO(69,": minihwc Readjusting contrast to %3.3f as <=0.0 \n", contrast);
  }

  GDBG_INFO(69,": minihwc Overriding Brightness with %3.3f and Contrast with %3.3f\n", brightness, contrast);

  adjustBrightnessAndContrast_m(contrast, brightness, nEntries, r,g,b);

  /*
   * Since the GDI SetDeviceGammaRamp has no nEntries argument
   * we may have to call GetDeviceGammaRamp first.
   */
  if (nEntries < 256)
    GetDeviceGammaRamp((HDC)bInfo->hdc, &theRamp) ; 

  for (index = 0 ;
       index < nEntries ;
       index++)
  {
    theRamp.red[index] = (FxU16)r[index] ;
    theRamp.green[index] = (FxU16)g[index] ;
    theRamp.blue[index] = (FxU16)b[index] ;
  }

  SetDeviceGammaRamp((HDC)bInfo->hdc, &theRamp) ;

  return FXTRUE ;
#endif

#undef FN_NAME
} /* hwcGammaTable */

FxBool
hwcGetGammaTable(hwcBoardInfo *bInfo, FxU32 nEntries, FxU32 *r, FxU32 *g, FxU32 *b)
{
#define FN_NAME "hwcGetGammaTable"
#if 1
  FxU32 i;
  FxU32 vidProcCfg;
  FxU32 dacBase;
  FxU32 dacAddr ;
  FxU32 dacData ;
 
  /*
   * AJB- Don't believe the hype.  Although we can do this
   * ourselves on Win9x/DOS, it's actually a piece of cake
   * to call the GDI Gamma ramp interface. This non-GDIish
   * implementation is here because it is faster than 
   * calling the GDI function (which thunks to the display
   * driver).
   */
  HWC_IO_LOAD( bInfo->regInfo, vidProcCfg, vidProcCfg);
  
  /* Determine which set of CLUT entries are selected */
  if (vidProcCfg & SST_OVERLAY_CLUT_SELECT)
    dacBase = 256;
  else
    dacBase = 0;
 
  /* Print out some useful info RE the vidProcCfg register */
  GDBG_INFO(80, "%s:  vidProcCFG(SST_OVERLAY_CLUT_SELECT) = %d\n",
            FN_NAME, (vidProcCfg & SST_OVERLAY_CLUT_SELECT) ? 1 : 0);
  GDBG_INFO(80, "%s:  vidProcCFG(SST_OVERLAY_EN) = %d\n",
            FN_NAME, (vidProcCfg & SST_OVERLAY_EN) ? 1 : 0);
  GDBG_INFO(80, "%s:  vidProcCFG(SST_OVERLAY_CLUT_BYPASS) = %d\n",
            FN_NAME, (vidProcCfg & SST_OVERLAY_CLUT_BYPASS) ? 1 : 0);
  
  for (i = 0; i < nEntries; i++) 
  {
    int repeat = 100 ;

    do
    {
      HWC_IO_STORE(bInfo->regInfo, dacAddr, dacBase + i);
      P6FENCE;
      HWC_IO_LOAD(bInfo->regInfo, dacAddr, dacAddr);
      P6FENCE;
      HWC_IO_LOAD(bInfo->regInfo, dacData, dacData);
      P6FENCE;
    } while (--repeat && (dacAddr != dacBase + i)) ;

    if (!repeat) 
    {
      GDBG_INFO(0, "%s: Error Reading DacData [%d, %x]. DacBase = %d\n",
                FN_NAME, i, dacData, dacBase);
    }

    r[i] = (dacData >> RED_SHIFT) & 0xFF ;
    g[i] = (dacData >> GREEN_SHIFT) & 0xFF ;
    b[i] = (dacData >> BLUE_SHIFT) & 0xFF ;
  }

  return FXTRUE;
  
#else
  /*
   * Here lies the GDIish implementation of hwcGetGammaTable.
   * Although slower than just banging on the hardware, this
   * version is WinNT-friendly.
   */
  DDGAMMARAMP theRamp ;
  FxU32       index ;

  GetDeviceGammaRamp((HDC)bInfo->hdc, &theRamp) ; 

  for (index = 0 ;
       index < nEntries ;
       index++)
  {
    r[index] = (FxU32)theRamp.red[index] ;
    g[index] = (FxU32)theRamp.green[index] ;
    b[index] = (FxU32)theRamp.blue[index] ;
  }

  return FXTRUE ;
#endif
  
#undef FN_NAME
} /* hwcGetGammaTable */


FxBool
hwcGammaRGB(hwcBoardInfo *bInfo, float gammaR, float gammaG, float gammaB)
{
#define FN_NAME "hwcGammaRGB"
  FxU32
    grRamp[256], ggRamp[256], gbRamp[256];
  int
    i;

  GDBG_INFO(69, FN_NAME "(0x%x, %1.2f, %1.2f, %1.2f)\n",
            bInfo, gammaR, gammaG, gammaB);

  /*
  ** NB:  The system eventually devised by Bob and Ken *may* require
  ** separate R, G, and B vectors.
  */

  for (i = 0; i < 256; i++) {
    grRamp[i] = (FxU32)((pow(i/255.0F, 1.0F/gammaR)) * 255.0F + 0.5F);
    ggRamp[i] = (FxU32)((pow(i/255.0F, 1.0F/gammaG)) * 255.0F + 0.5F);
    gbRamp[i] = (FxU32)((pow(i/255.0F, 1.0F/gammaB)) * 255.0F + 0.5F);
  }

  hwcGammaTable(bInfo, 256, grRamp, ggRamp, gbRamp); 

  /*
  **  Now that we have a gamma table, we can give it to the driver via
  **  a call to ExtEscape() when that is defined.....
  */
  return FXFALSE;

#undef FN_NAME
} /* hwcGammaRGB */


#define M 1
#define K 1

FxBool
hwcSetGrxClock(hwcBoardInfo *bInfo, FxU32 speedInMHz)
{
#define FN_NAME "hwcSetGrxClock"
  FxU32
    pllCtrl1,
    dramInit0 = 0xc17ae29, 
    dramInit1 = 0x26c031,
    n,
    m = 1;

  n = (FxU32) ((speedInMHz - 4.76f)/2.38f);

  pllCtrl1 =
    (K << SST_PLL_K_SHIFT) | (m << SST_PLL_M_SHIFT) | (n << SST_PLL_N_SHIFT);

  GDBG_INFO(80, "%s:  Setting Graphics Clock to %d\n", FN_NAME, speedInMHz);

  HWC_IO_STORE( bInfo->regInfo, dramInit0, dramInit0);
  HWC_IO_STORE( bInfo->regInfo, dramInit1, dramInit1);
  HWC_IO_STORE( bInfo->regInfo, pllCtrl1, pllCtrl1);
  
  return FXTRUE;

#undef FN_NAME  
} /* hwcSetGrxClock */

FxBool
hwcSetMemClock(hwcBoardInfo *bInfo, FxU32 speedInMHz)
{
#define FN_NAME "hwcSetMemClock"

  return FXFALSE;
#undef FN_NAME  
} /* hwcSetMemClock */


FxBool
hwcResolutionSupported(hwcBoardInfo *bInfo, GrScreenResolution_t res, GrScreenRefresh_t ref)
{
#define FN_NAME "hwcResolutionSupported"
  static char *resNames[] = {
    "GR_RESOLUTION_320x200",
    "GR_RESOLUTION_320x240",
    "GR_RESOLUTION_400x256",
    "GR_RESOLUTION_512x384",
    "GR_RESOLUTION_640x200",
    "GR_RESOLUTION_640x350",
    "GR_RESOLUTION_640x400",
    "GR_RESOLUTION_640x480",
    "GR_RESOLUTION_800x600",
    "GR_RESOLUTION_960x720",
    "GR_RESOLUTION_856x480",
    "GR_RESOLUTION_512x256",
    "GR_RESOLUTION_1024x768",
    "GR_RESOLUTION_1280x1024",
    "GR_RESOLUTION_1600x1200",
    "GR_RESOLUTION_400x300",
    "GR_RESOLUTION_1152x864",
    "GR_RESOLUTION_1280x960",
    "GR_RESOLUTION_1600x1024",
    "GR_RESOLUTION_1792x1344",
    "GR_RESOLUTION_1856x1392",
    "GR_RESOLUTION_1920x1440",
    "GR_RESOLUTION_2048x1536",
    "GR_RESOLUTION_2048x2048"
  };

#if 0
  struct WidthHeight_s {
    FxU32 width; 
    FxU32 height;
  } whByRes[] = {
    {320, 200},                 /* GR_RESOLUTION_320x200 */
    {320, 240},                 /* GR_RESOLUTION_320x240 */
    {400, 256},                 /* GR_RESOLUTION_400x256 */
    {512, 384},                 /* GR_RESOLUTION_512x384 */
    {640, 200},                 /* GR_RESOLUTION_640x200 */
    {640, 350},                 /* GR_RESOLUTION_640x350 */
    {640, 400},                 /* GR_RESOLUTION_640x400 */
    {640, 480},                 /* GR_RESOLUTION_640x480 */
    {800, 600},                 /* GR_RESOLUTION_800x600 */
    {960, 720},                 /* GR_RESOLUTION_960x720 */
    {856, 480},                 /* GR_RESOLUTION_856x480 */
    {512, 256},                 /* GR_RESOLUTION_512x256 */
    {1024, 768},                /* GR_RESOLUTION_1024x768 */
    {1280, 1024},               /* GR_RESOLUTION_1280x1024 */
    {1600, 1200},               /* GR_RESOLUTION_1600x1200 */
    {400, 300},                 /* GR_RESOLUTION_400x300 */
    {1152, 864},                /* GR_RESOLUTION_1152x864 */
    {1280, 960},                /* GR_RESOLUTION_1280x960 */
    {1600, 1024},               /* GR_RESOLUTION_1600x1024 */
    {1792, 1344},               /* GR_RESOLUTION_1792x1344 */
    {1856, 1392},               /* GR_RESOLUTION_1856x1392 */
    {1920, 1440},               /* GR_RESOLUTION_1920x1440 */
    {2048, 1536},               /* GR_RESOLUTION_2048x1536 */
    {2048, 2048}                /* GR_RESOLUTION_2048x2048 */
  };
#endif  

  static char *refresh[] = {
    "GR_REFRESH_60Hz",
    "GR_REFRESH_70Hz",
    "GR_REFRESH_72Hz",
    "GR_REFRESH_75Hz",
    "GR_REFRESH_80Hz",
    "GR_REFRESH_90Hz",
    "GR_REFRESH_100Hz",
    "GR_REFRESH_85Hz",
    "GR_REFRESH_120Hz"    
  };


  GDBG_INFO(80, FN_NAME ":  res == %s (0x%x) ref == %s, supported == %s\n",
            resNames[res], resolutionSupported[bInfo->boardNum][res][ref], refresh[ref],
            resolutionSupported[bInfo->boardNum][res][ref] ? "FXTRUE" : "FXFALSE");
  

  /* Glide has very good checking to see if the memory required is
  available, so we'll just return whether the driver can do it. */
  return resolutionSupported[bInfo->boardNum][res][ref];

#undef FN_NAME
} /* hwcResolutionSupported */

#ifdef __WIN32__
static char *
getRegPath() 
{
  char *retVal = NULL;
  OSVERSIONINFO ovi;
  
  ovi.dwOSVersionInfoSize = sizeof ( ovi );
  GetVersionEx ( &ovi );
  if (ovi.dwPlatformId == VER_PLATFORM_WIN32_NT) {
    HKEY hKey;
    DWORD type ;
    static char strval[255];
    DWORD szData = sizeof(strval) ;

    GDBG_INFO(80, "OS == WNT\n");

    /* Go fishing for the registry path on WinNT4 */
    if (RegOpenKey(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\VIDEO", &hKey) == ERROR_SUCCESS)
    {
      if (RegQueryValueEx(hKey, "\\Device\\Video0", NULL, &type, strval, &szData) ==
          ERROR_SUCCESS)
      {
        if (type != REG_SZ)
        {
          /* It is hardcoded on NT via Display Control code. see:
           * $/devel/swtools/bansheecp2 */
          retVal = "SYSTEM\\CurrentControlSet\\Services\\3Dfx\\Device0\\glide";
        }
         else
         {
          strcat(strval, "\\Glide") ;
          retVal = (char*)((int)strval + strlen("\\REGISTRY\\Machine\\")) ;
         }
      }
       else
         retVal = "SYSTEM\\CurrentControlSet\\Services\\3Dfx\\Device0\\glide";

      RegCloseKey(hKey);
    }
  } else {
    QDEVNODE QDevNode;
    QIN Qin;
    int status;

    GDBG_INFO(80, "OS == W9X\n");

    Qin.dwSubFunc = QUERYDEVNODE;
    {
      HDC curDC = GetDC(NULL);

      status = ExtEscape ( (HDC)curDC, QUERYESCMODE, 
                           sizeof(Qin), (LPCSTR)&Qin, 
                           sizeof(QDevNode), (LPSTR)&QDevNode );
      ReleaseDC(NULL, curDC);
    }

    if ( status > 0 ) {
      static char regPath[255];

      CM_Get_DevNode_Key( QDevNode.dwDevNode, NULL, 
                          &regPath, sizeof(regPath), 
                          CM_REGISTRY_SOFTWARE );
      strcat(regPath, "\\glide");

      retVal = regPath;
    }
  }

  return retVal;
} /* getRegPath */
#endif

#if __WIN32__
char *
hwcGetenv(char *a) 
{
  char *retVal = NULL;
  static char *regPath;
  HKEY hKey;
  DWORD type ;
  DWORD szData = 255 ;
  static char strval[255];

  /* This should work for both NT and Win95/98 (getRegPath works) */
  /* the system environment variables override the registry settings */
  if (retVal = getenv(a))
    return retVal;

  /* KoolSmoky - if we have a valid registry path */
  if(opengl_regpath)
    return hwcGetenvEx(a, opengl_regpath);
  
  szData = sizeof(strval);

  if (regPath == NULL) {
    regPath = getRegPath();
    
    GDBG_INFO(80, "_grGetEnv:  regPath = %s\n", regPath);

    if (regPath == NULL)
      return NULL;
  }

  GDBG_INFO(80, "hwcGetEnv: %s  %s\n", a, regPath);

  if (RegOpenKey(HKEY_CURRENT_USER, regPath, &hKey) == ERROR_SUCCESS) {
    if (RegQueryValueEx(hKey, a, NULL, &type, strval, &szData) ==
        ERROR_SUCCESS) {
      if (type != REG_SZ) {
        retVal = NULL;
      } else {
        retVal = strval;
      }
    }
    RegCloseKey(hKey);
  }
  
  if ((retVal == NULL) &&
     RegOpenKey(HKEY_LOCAL_MACHINE, regPath, &hKey) == ERROR_SUCCESS)
  {
    if (RegQueryValueEx(hKey, a, NULL, &type, strval, &szData) ==
        ERROR_SUCCESS) {
      if (type != REG_SZ) {
        retVal = NULL;
      } else {
        retVal = strval;
      }
    }
    RegCloseKey(hKey);
  }
  return (char*)retVal;
#else
char *
hwcGetenv(char *a) 
{
  return getenv(a);
#endif
} /* _grGetenv */

#if __WIN32__
char *
hwcGetenvEx(char *a, char *b) 
{
  char *retVal = NULL;
  char regPath[255];
  HKEY hKey;
  DWORD type ;
  DWORD szData = 255 ;
  static char strval[255];

  /* the system environment variables override the registry settings */
  if (retVal = getenv(a))
    return retVal;

  szData = sizeof(strval);

  strcpy(regPath, b);
  
  if (regPath == NULL) {
    strcpy(regPath, getRegPathEx());
    
    if (regPath == NULL)
      return NULL;
  }

  strcat(regPath, "\\glide");

  GDBG_INFO(80, "hwcGetEnvEx: %s  %s\n", a, regPath);
  
  if (RegOpenKey(HKEY_CURRENT_USER, regPath, &hKey) == ERROR_SUCCESS) {
    if (RegQueryValueEx(hKey, a, NULL, &type, strval, &szData) ==
        ERROR_SUCCESS) {
      if (type != REG_SZ) {
        retVal = NULL;
      } else {
        retVal = strval;
      }
    }
    RegCloseKey(hKey);
  }
  
  if ((retVal == NULL) &&
     RegOpenKey(HKEY_LOCAL_MACHINE, regPath, &hKey) == ERROR_SUCCESS)
  {
    if (RegQueryValueEx(hKey, a, NULL, &type, strval, &szData) ==
        ERROR_SUCCESS) {
      if (type != REG_SZ) {
        retVal = NULL;
      } else {
        retVal = strval;
      }
    }
    RegCloseKey(hKey);
  }
  return (char*)retVal;
} /* _grGetenvEx */
#endif

FxU32
hwcQueryContext(hwcBoardInfo *bInfo)
{
#define FN_NAME "hwcQueryContext"
  FxU32
    retVal = FXTRUE;
  FxU32
    retExtVal = FXTRUE;

  GDBG_INFO(80, FN_NAME ":  Called!\n");

#if HWC_EXT_INIT
  {
    if( HWCEXT_PROTOCOL( bInfo->boardNum ) )
    {
      hwcExtRequest_t ctxReq;
      hwcExtResult_t ctxRes;
      GDBG_INFO(80, FN_NAME ":  ExtEscape:HWCEXT_QUERYCONTEXT\n");
      ctxReq.which = HWCEXT_QUERYCONTEXT;
      retExtVal = ExtEscape((HDC)bInfo->hdc, HWCEXT_ESCAPE(bInfo->boardNum), /**/
                                          sizeof(ctxReq), (LPSTR) &ctxReq,
                                          sizeof(ctxRes), (LPSTR) &ctxRes);
      GDBG_INFO(80, FN_NAME ":  ExtEscape retVal=%d\n", retExtVal);
      retVal = (ctxRes.resStatus != HWC_CONTEXT_LOST);
    }
    else
    {
      retVal = *((FxU32*)bInfo->lostContextDWORD);
    }
  }
#endif /* HWC_EXT_INIT */

  return retVal;
#undef FN_NAME
} /* hwcQueryContext */

FxU32
hwcShareContextData(hwcBoardInfo *bInfo, FxU32 **data)
{
#define FN_NAME "hwcShareContextData"
  FxU32
    retVal = FXTRUE;
#if HWC_EXT_INIT
  hwcExtRequest_t ctxReq;
  hwcExtResult_t  ctxRes;

  GDBG_INFO(80, FN_NAME "\n");  

  //is_patchedHwnd = FXFALSE;
  
  if( HWCEXT_PROTOCOL( bInfo->boardNum ) )
  {
    if (bInfo->osNT) {
      hwcExtRequest_t
        ctxReq;
      hwcExtResult_t
        ctxRes;

      curBI = bInfo;
      ctxReq.optData.contextDwordNTReq.procId = GetCurrentProcessId();

      GDBG_INFO(80, FN_NAME ":  ProcID=%d\n", ctxReq.optData.contextDwordNTReq.procId);  
      GDBG_INFO(80, FN_NAME ":  NT Branch\n");  
      {
#if 1
        FxU32 ohWell;

        ctxReq.which = HWCEXT_CONTEXT_DWORD_NT;
        
        /* Now for the NASTY stuff: */
        __asm {
          __asm  mov eax, 0;
          __asm  mov ax, cs;
          __asm  mov ohWell, eax;
        }

        ctxReq.optData.contextDwordNTReq.codeSegment = ohWell;

        __asm {
          __asm  mov eax, 0;
          __asm  mov ax, ds;
          __asm  mov ohWell, eax;
        }
    
        ctxReq.optData.contextDwordNTReq.dataSegment = ohWell;
        
#else /* the way MS does this */
        
        CONTEXT Context;
        HANDLE hThread;

        ctxReq.which = HWCEXT_CONTEXT_DWORD_NT;

        Context.ContextFlags = CONTEXT_FULL;
        hThread = GetCurrentThread();
        GetThreadContext(hThread, &Context);
        ctxReq.optData.contextDwordNTReq.codeSegment = Context.SegCs;
        ctxReq.optData.contextDwordNTReq.dataSegment = Context.SegDs;

#endif
        
        /* oh, yeah */
      }
      GDBG_INFO(80, FN_NAME ":  codeSegment =%d\n", ctxReq.optData.contextDwordNTReq.codeSegment ); 
      GDBG_INFO(80, FN_NAME ":  dataSegment =%d\n", ctxReq.optData.contextDwordNTReq.dataSegment ); 
      GDBG_INFO(80, FN_NAME ":  Calling ExtEscape(HWCEXT_CONTEXT_DWORD_NT)\n");  
      retVal = ExtEscape((HDC) bInfo->hdc, HWCEXT_ESCAPE(bInfo->boardNum), 
                         sizeof(ctxReq), (LPSTR) &ctxReq,
                         sizeof(ctxRes), (LPSTR) &ctxRes);

      /* NT5.1 returns NULL for the pointer to context ... this is BAD. If the 
       * escape is not implemented retVal==0, retVal<0 indicates an error. So
       * check the retVal and the pointer. This also screws with ALT-TAB.
       */
      GDBG_INFO(80, FN_NAME ":  ExtEscape retVal=%d, dwordOffset=%d, contextDWORD=%d\n", retVal, ctxRes.optData.contextDwordNTRes.dwordOffset, ctxRes.optData.shareContextDWORDRes.contextDWORD);
      if( (retVal <= 0) || (ctxRes.optData.contextDwordNTRes.dwordOffset == 0)) { /* make exceptions for winxp escapecalls */
        *data = &dummyContextDWORD;
        GDBG_INFO(80, FN_NAME ":  using dummyContext \n");
#ifdef IS_ALT_TAB
        /* patch app's winproc to allow alt-tab */
        if((WNDPROC)GetWindowLongPtr( (HWND)bInfo->vidInfo.hWnd, GWLP_WNDPROC ) != QueryContextProc /*||
           (HWND)bInfo->vidInfo.hWnd != patchedHwnd*/) {
          GDBG_INFO(80, FN_NAME ":  patching Wndproc\n");
          patchedHwnd = (HWND)bInfo->vidInfo.hWnd;
          patchedWndproc = (WNDPROC)GetWindowLongPtr( (HWND)bInfo->vidInfo.hWnd, GWLP_WNDPROC );
          SetWindowLongPtr( (HWND)bInfo->vidInfo.hWnd, GWLP_WNDPROC, (LONG_PTR)QueryContextProc );
          GDBG_INFO(80, FN_NAME ":  patchedWndproc = %i, patchedHwnd = %i\n", patchedWndproc, (HWND)bInfo->vidInfo.hWnd);
        }
        GDBG_INFO(80, FN_NAME ":  Wndproc = %i, Hwnd = %i\n",
                  (WNDPROC)GetWindowLongPtr( (HWND)bInfo->vidInfo.hWnd, GWLP_WNDPROC ),
                  (HWND)bInfo->vidInfo.hWnd);
#endif	/* IS_ALT_TAB */
      } else {
        *data = (FxU32 *) ctxRes.optData.contextDwordNTRes.dwordOffset;
      }
    
    } else {      
      /* context DWORD is rather poorly named now, but oh, well. */
      ctxReq.which = HWCEXT_SHARE_CONTEXT_DWORD;
          
      /*ctxReq.optData.contextDwordNTReq.procId = GetCurrentProcessId();*/

      bInfo->contextHandle = GetCurrentProcessId ();

      /* Piggy back the context handle on the unmapmemory structure for 
       * safety
           *
           * XXX:FIXME!! Add procId to 9x shareContext structure
       */
      ctxReq.optData.unmapMemoryReq.procHandle = bInfo->contextHandle;

      /* MDM-Richardson - Removed because it corrupts the req structure that
       * gets passed down through the ExtEscape.  Theory is that it messes 
       * with the LPSTR pointer due to a NULL 
       */
      /* ctxReq.Ext.optData.shareContextDWORDReq.contextDWORD =
        (FxU32) &bInfo->lostContextDWORD; */
    
      retVal = ExtEscape((HDC)bInfo->hdc, HWCEXT_ESCAPE(bInfo->boardNum), /**/
                         sizeof(ctxReq), (LPSTR) &ctxReq,
                         sizeof(ctxRes), (LPSTR) &ctxRes);
    
      if (retVal == 0xffffffff || ctxRes.resStatus == 0xffffffff ) {
        *data = &dummyContextDWORD;
      } else
        *data = (FxU32 *) ctxRes.optData.shareContextDWORDRes.contextDWORD;
    }
    
    GDBG_INFO(80, FN_NAME ":  pointer to context = 0x%x\n",
              ctxRes.optData.shareContextDWORDRes.contextDWORD); 
  }
#endif
#if HWC_GDX_INIT
  *data = &dummyContextDWORD;
#endif
  return retVal;
#undef FN_NAME
} /* hwcShareContextData */

#ifdef HWC_EXT_INIT
void
hwcUnmapMemory() 
{
  FxU32 i;
  hwcExtRequest_t ctxReq;
  hwcExtResult_t  ctxRes;

  if ( curBI ) {
    if ( curBI->osNT ) {
      hwcExtRequest_t
        ctxReq;
      hwcExtResult_t
        ctxRes;
      ctxReq.which = HWCEXT_UNMAP_MEMORY;
      
      /* Assure that we free up the node by passing the known Process ID that
         was used to open the context
      */
      ctxReq.optData.unmapMemoryReq.procHandle = (ULONG)GetCurrentProcessId();
      
          ExtEscape((HDC)curBI->hdc, HWCEXT_ESCAPE(curBI->boardNum), 
                    sizeof(ctxReq), (LPSTR) &ctxReq,
                    sizeof(ctxRes), (LPSTR) &ctxRes);
      curBI->isMapped = FXFALSE;
    }
    curBI = NULL;
  }
  else {
    for(i=0;i<hInfo.nBoards;i++)
      {
        ctxReq.which = HWCEXT_UNMAP_MEMORY;
        ctxReq.optData.unmapMemoryReq.procHandle = (ULONG)GetCurrentProcessId();
        GDBG_INFO(80, "hwcUnmapMemory:  Calling ExtEscape(HWCEXT_UNMAP_MEMORY)\n");  
        ExtEscape((HDC)hInfo.boardInfo[i].hdc, HWCEXT_ESCAPE(i), /**/
                  sizeof(ctxReq), (LPSTR) &ctxReq,
                  sizeof(ctxRes), (LPSTR) &ctxRes);
        hInfo.boardInfo[i].isMapped = FXFALSE;
      }
  }

} /* hwcUnmapMemory */

void
hwcUnmapMemory9x(hwcBoardInfo *bInfo) 
{
#ifdef HWC_EXT_INIT
  hwcExtRequest_t ctxReq;
  hwcExtResult_t  ctxRes;

  ctxReq.which = HWCEXT_UNMAP_MEMORY;
  ctxReq.optData.unmapMemoryReq.procHandle = bInfo->contextHandle;

  /* Assure that we free up the node by passing the known Process ID that
     was used to open the context
  */
        
  ExtEscape((HDC)bInfo->hdc, HWCEXT_ESCAPE(bInfo->boardNum), 
            sizeof(ctxReq), (LPSTR) &ctxReq,
            sizeof(ctxRes), (LPSTR) &ctxRes);
  
  bInfo->isMapped = FXFALSE;
  /* Catch dumb bugs.  Nuke all linear register pointers. */

  
#endif // #ifdef HWC_EXT_INIT
} /* hwcUnmapMemory9x */

void
hwcClearContextData() 
{
#define FN_NAME "hwcClearContextData"
  return;
#undef FN_NAME
} /* hwcClearContextData */
#endif

void
hwcIdleWinFifo(hwcBoardInfo*     bInfo,
               const HwcWinFifo* fifo,
               const FxU32       serialNumber)
{
#if HWC_EXT_INIT || HWC_GDX_INIT
  while(hwcExecuteStatusWinFifo(bInfo, 
                                fifo,
                                serialNumber) != serialNumber) {
    /* Do Nothing? */;
  }
  if(fifo->fifoType == HWCEXT_FIFO_HOST) {
    FxU32 status;
    /* %%KCD - Hack.  In the host memory fifo case, we don't have a sentinal, so
       we just idle right here.  It's not a perfect soluction on a preemptive OS 
       since the hardware may take a long time to idle, but the host memory fifo
       case is going to suck for speed anyway.  Note that I don't look for all
       chips to idle since we never use windowed mode and SLI at the same time. */      
    do {
      HWC_SST_LOAD(bInfo->regInfo, status, status);        
    } while((status & SST_BUSY) != 0);      
  }
#endif
}
#if HWC_GDX_INIT
void
hwcCheckTarget(FxU32 sfcOffset, FxU32 sfcWidth, FxU32 sfcHeight,
               FxU32 sfcDepth, FxU32 auxFifoStart, FxU32 auxFifoSize)
{
}
#endif

#if HWC_EXT_INIT

void
hwcCheckTarget(FxU32 sfcOffset, FxU32 sfcWidth, FxU32 sfcHeight,
               FxU32 sfcDepth, FxU32 auxFifoStart, FxU32 auxFifoSize)
{
#define FN_NAME "hwcCheckTarget"
#ifdef GDBG_INFO_ON
  hwcBoardInfo *bI = &hInfo.boardInfo[0];
  FxU32
    winFifoBase, winFifoSize;
  FxU32
    vidProcCfg, vidScreenSize,
    vidDesktopStartAddr, vidDesktopOverlayStride,
    baseAddrL, baseSize,
    lfbMemoryConfig;
  FxU32
    shiftAdd,
    sfcEnd, winFifoEnd, auxFifoEnd,
    screenWidth, screenHeight,
    dtPixelFormat, 
    dtPixDepth, 
    dtStride,
    tileBase, tileStride,
    desktopBase, desktopEnd, desktopStride;


  /*
  ** Compute a bunch of stuff we'll need later...
  */

  /* Compute some convenient values */
  sfcEnd = sfcOffset + (sfcWidth * sfcHeight * sfcDepth / 8) - 1;
  auxFifoEnd = auxFifoSize + auxFifoSize;

  /* Compute base address and size of windows FIFO */
  HWC_CAGP_LOAD(bI->regInfo, cmdFifo0.baseAddrL, baseAddrL);
  HWC_CAGP_LOAD(bI->regInfo, cmdFifo0.baseSize, baseSize);
  winFifoBase = baseAddrL;
  winFifoSize = ((baseSize & SST_BASESIZE) >> SST_BASESIZE_SHIFT);
  winFifoEnd = winFifoBase + winFifoSize;


  /* Compute global tile stride */
  HWC_IO_LOAD(bI->regInfo, lfbMemoryConfig, lfbMemoryConfig);
  tileBase =
    (
     (lfbMemoryConfig & SST_RAW_LFB_TILE_BEGIN_PAGE) >>
     SST_RAW_LFB_TILE_BEGIN_PAGE_SHIFT
     );
  
  shiftAdd =
    ((lfbMemoryConfig & SST_RAW_LFB_ADDR_STRIDE) >>
      SST_RAW_LFB_ADDR_STRIDE_SHIFT);

  tileStride = (1 << (10 + shiftAdd));

  /* Get relevant info from vidProcCfg */
  HWC_IO_LOAD(bI->regInfo, vidProcCfg, vidProcCfg);
  dtPixelFormat = vidProcCfg & SST_DESKTOP_PIXEL_FORMAT;
  switch (dtPixelFormat) {
  case SST_DESKTOP_PIXEL_PAL8:
    dtPixDepth = 1;
    break;
  case SST_DESKTOP_PIXEL_RGB565:
    dtPixDepth = 2;
    break;
  case SST_DESKTOP_PIXEL_RGB24:
    dtPixDepth = 3;
    break;
  case SST_DESKTOP_PIXEL_RGB32:
    dtPixDepth = 4;
    break;
  }
  
  /* Compute Screen Size */
  HWC_IO_LOAD(bI->regInfo, vidScreenSize, vidScreenSize);
  screenWidth =
    (
     (vidScreenSize & SST_VIDEO_SCREEN_WIDTH) >>
     SST_VIDEO_SCREEN_WIDTH_SHIFT
     );
  screenHeight =
    (
     (vidScreenSize & SST_VIDEO_SCREEN_HEIGHT) >>
     SST_VIDEO_SCREEN_HEIGHT_SHIFT
     );
  
  /* Compute the end of the surface */

  /* Compute the desktop start and size (possibly width, too) */
  HWC_IO_LOAD(bI->regInfo,  vidDesktopStartAddr, vidDesktopStartAddr);
  desktopBase = vidDesktopStartAddr;

  HWC_IO_LOAD(bI->regInfo,  vidDesktopOverlayStride, vidDesktopOverlayStride);
  dtStride = vidDesktopOverlayStride & 0x7fff;

  if (desktopBase > tileBase) {
    desktopStride = dtStride << 7;
  } else {
    desktopStride = dtStride;
  }
  
  desktopEnd = desktopBase + screenWidth * screenHeight * dtPixDepth / 8;

  /* ++++ the CONSTANT below needs to be a variable--CHD */
  if (sfcOffset > 0x2000000) {
    GDBG_INFO(80, FN_NAME ":  surface offset greater than memory range\n");
    goto __errSurfaceFifo;
  }


  /* 
  **  Check for the Very Bad Things...
  ** 
  ** XXdwm  Why not just more simply check for interval overlap?  
  **        E.g. "!(Abegin >= Bend || Bbegin >= Aend)"
  */
  if (sfcOffset > tileBase) {   /* Tiled  case */
    /* Does it start in the windows FIFO? No:FIFOs can't be in tiled memory */
    /* Does it run into the windows FIFO? No:FIFOs can't be in tiled memory */
    /* Does it start in the aux FIFO? No:FIFOs can't be in tiled memory */
    /* Does it run into the aux FIFO? No:FIFOs can't be in tiled memory */


    /* Does it start in the desktop? */    
    /* Tiled memory is linear, so we have to check for overlapping
       rectangles.   */
    if (sfcOffset >= desktopBase && sfcOffset < desktopEnd) {
      GDBG_INFO(80, FN_NAME ":  Surface starts in desktop\n");
      goto __errSurfaceFifo;
    }

    /* Does it run into the desktop? */    
    if (sfcEnd > desktopBase && sfcEnd <= desktopEnd) {
      GDBG_INFO(80, FN_NAME ":  Surface ends in desktop\n");
      goto __errSurfaceFifo;
    }

  } else {                      /* Linear case */
    GDBG_INFO(80, FN_NAME ":  Linear Surface\n");

    /* Does it start in the windows FIFO? */    
    if (sfcOffset >= winFifoBase && sfcOffset < winFifoEnd) {
      GDBG_INFO(80, FN_NAME ":  Surface starts in Windows FIFO\n");
      goto __errSurfaceFifo;
    }

    /* Does it run into the windows FIFO? */
    if (sfcEnd > winFifoBase && sfcEnd <= winFifoEnd) {
      GDBG_INFO(80, FN_NAME ":  Surface runs into Windows FIFO\n");
      goto __errSurfaceFifo;
    }

    /* Does it start in the desktop? */
    if (sfcOffset >= desktopBase && sfcOffset < desktopEnd) {
      GDBG_INFO(80, FN_NAME ":  Surface starts in desktop\n");
      goto __errSurfaceFifo;
    }

    /* Does it run into the desktop? */
    if (sfcEnd > desktopBase && sfcEnd <= desktopEnd) {
      GDBG_INFO(80, FN_NAME ":  Surface ends in desktop\n");
      goto __errSurfaceFifo;
    }

    /* Does it start in the aux FIFO? */
    if (sfcOffset >= auxFifoStart && sfcOffset < auxFifoEnd) {
      GDBG_INFO(80, FN_NAME ":  Surface starts in aux FIFO\n");
      goto __errSurfaceFifo;
    }

    /* Does it run into the aux FIFO? */
    if (sfcEnd > auxFifoStart && sfcEnd <= auxFifoEnd) {
      GDBG_INFO(80, FN_NAME ":  Surface starts in aux FIFO\n");
      goto __errSurfaceFifo;
    }
  }
  
  return;

 __errSurfaceFifo:
  GDBG_INFO(80, FN_NAME ":  sfcStart = 0x%x, sfcEnd = 0x%x\n",
            sfcOffset, sfcEnd);
  exit(-1);
#endif /* GDBG_INFO_ON */
  return;
#undef FN_NAME
} /* hwcCheckTarget */

#endif /* HWC_EXT_INIT */

/* This routine only really works on Napalm since someone decided to change the register
 * layout from Voodoo3.  Redefine them here. */
#undef SST_SIPROCESS_OSC_CNTR_RUN         
#undef SST_SIPROCESS_OSC_NOR_SEL          
#undef SST_SIPROCESS_OSC_FORCE_ENABLE     

#define SST_SIPROCESS_OSC_CNTR_RUN         BIT(29)
#define SST_SIPROCESS_OSC_NOR_SEL          BIT(30)
#define SST_SIPROCESS_OSC_FORCE_ENABLE     BIT(31)

void hwcCalcSipValue(hwcBoardInfo *bInfo, FxU32 chipNum, FxU32 *nandChain, FxU32 *norChain)
{
  FxU32 pllCtrl1, pllCtrl1_save;
  FxU32 sipMonitor, temp;

  int min, max, iters;     // some storage vars
  FxI32 count, val;
static  FxI32 valarray[SST_SIPROCESS_OSC_CNTR + 1];    // is this how you do an array? God, I suck...

  int i;                   // some misc vars

  double total, E, SD, VAR, N, avg;   // these might have to be floats instead of ints

  // Don't do anything stupid. 
  if(chipNum >= bInfo->pciInfo.numChips) {
    *nandChain = 0;
    *norChain = 0;
    return;
  }
  // Okay, this is ripped from our diag environment, so 
  // obviously isn't going to necessarily work in this case...
  // This is just to get the base addresses into a var.

  // Read out old PLL value so we can restore it later.
  pllCtrl1 = 0x3205; // 125Mhz
  if(chipNum > 0) {
    HWC_IO_LOAD_SLAVE(chipNum, bInfo->regInfo, pllCtrl1, pllCtrl1_save);
    HWC_IO_STORE_SLAVE(chipNum, bInfo->regInfo, pllCtrl1, pllCtrl1);
  } else {
    HWC_IO_LOAD(bInfo->regInfo, pllCtrl1, pllCtrl1_save);
    HWC_IO_STORE(bInfo->regInfo, pllCtrl1, pllCtrl1);
  }  

  // set number of iterations
  iters = 1000;

  // initialize the storage array
  for (i=0; i<(SST_SIPROCESS_OSC_CNTR + 1); i++) {
	 valarray[i] = 0;
  }

  // start nand chain test
  min = 0; max = 0;
  for (i=0; i<iters; i++) {
    // program to count for 256 grxclk cycles
    count = 0x0ff;

    // write reset and count to SIPMONITOR register
    sipMonitor = count << SST_SIPROCESS_PCI_CNTR_SHIFT;
    if(chipNum > 0) {
      HWC_IO_STORE_SLAVE(chipNum, bInfo->regInfo, sipMonitor, sipMonitor | SST_SIPROCESS_OSC_CNTR_RESET_N | SST_SIPROCESS_OSC_NAND_SEL);
      HWC_IO_STORE_SLAVE(chipNum, bInfo->regInfo, sipMonitor, sipMonitor | SST_SIPROCESS_OSC_CNTR_RESET_N | SST_SIPROCESS_OSC_NAND_SEL);
      HWC_IO_LOAD_SLAVE(chipNum, bInfo->regInfo, sipMonitor, temp);
      HWC_IO_STORE_SLAVE(chipNum, bInfo->regInfo, sipMonitor, sipMonitor | SST_SIPROCESS_OSC_CNTR_RUN | SST_SIPROCESS_OSC_NAND_SEL);
    } else {
      HWC_IO_STORE(bInfo->regInfo, sipMonitor, sipMonitor | SST_SIPROCESS_OSC_CNTR_RESET_N | SST_SIPROCESS_OSC_NAND_SEL);
      HWC_IO_STORE(bInfo->regInfo, sipMonitor, sipMonitor | SST_SIPROCESS_OSC_CNTR_RESET_N | SST_SIPROCESS_OSC_NAND_SEL);
      HWC_IO_LOAD(bInfo->regInfo, sipMonitor, temp);
      HWC_IO_STORE(bInfo->regInfo, sipMonitor, sipMonitor | SST_SIPROCESS_OSC_CNTR_RUN | SST_SIPROCESS_OSC_NAND_SEL);
    } 

    while (count != 0) {
      // read back countdown counter to see when countdown has ended
      if(chipNum > 0) {
        HWC_IO_LOAD_SLAVE(chipNum, bInfo->regInfo, sipMonitor, count);
      } else {
        HWC_IO_LOAD(bInfo->regInfo, sipMonitor, count);
      }   
      count = (count & SST_SIPROCESS_PCI_CNTR) >> SST_SIPROCESS_PCI_CNTR_SHIFT;
    }
	 
    // read back actual SIP nand count
    if(chipNum > 0) {
      HWC_IO_LOAD_SLAVE(chipNum, bInfo->regInfo, sipMonitor, val);
    } else {
      HWC_IO_LOAD(bInfo->regInfo, sipMonitor, val);
    } 

    val &= SST_SIPROCESS_OSC_CNTR;

    // some statistics here
    if (val < min) { min = val; }
    if (val > max) { max = val; }
	 
    valarray[val]++;   // Do you have to initialize the array first? I forget...
  }

  // Please change the formatting of any text to however you see fit
  //printf("\n\n--- Process Values (NAND chain) ---\n");
  //printf("\n sip value: occurences (probability)\n");
  total = 0.0f;
  for (i = 0; i <= max; i++) {
    if (valarray[i] != 0) {
      //printf("  %4d: %4d (%f)\n", i, valarray[i], ((float)valarray[i])/iters);
      total += i * valarray[i];
    }
  }
  
  // some statistical stuff
  E = total / iters;
  total = 0;
  for (i = 0; i <= max; i++) {
    if (valarray[i] != 0) {
      total += (i - E)*(i - E) * valarray[i];
    }
  }

  VAR = total / iters;
  SD  = sqrt(VAR);    // how do you do sqrt??

  total = 0; N = 0;
  for (i = 0; i <= max; i++) {
    if ((valarray[i] != 0) && (i >= (E - SD)) && (i <= (E + SD))) {
      total += i * valarray[i];
      N += valarray[i];
    }
  }
  avg = total / N;

  //printf("E   = %g\n", E);
  //printf("VAR = %g\n", VAR);
  //printf("SD  = %g\n", SD);
  //printf("NAND avg = %d (using %d values about E)\n", (FxU32)avg, N);
	 
  *nandChain = (FxU32)avg;

  // initialize the storage array
  for (i=0; i<(SST_SIPROCESS_OSC_CNTR + 1); i++) {
    valarray[i] = 0;
  }

  // start nor chain test
  min = 0; max = 0;
  for (i=0; i<iters; i++) {
    // program to count for 256 grxclk cycles
    count = 0x0ff;

    // write reset and count to SIPMONITOR register
    sipMonitor = count << SST_SIPROCESS_PCI_CNTR_SHIFT;
    if(chipNum > 0) {        
      HWC_IO_STORE_SLAVE(chipNum, bInfo->regInfo, sipMonitor, sipMonitor | SST_SIPROCESS_OSC_CNTR_RESET_N | SST_SIPROCESS_OSC_NOR_SEL);
      HWC_IO_STORE_SLAVE(chipNum, bInfo->regInfo, sipMonitor, sipMonitor | SST_SIPROCESS_OSC_CNTR_RESET_N | SST_SIPROCESS_OSC_NOR_SEL);
      HWC_IO_LOAD_SLAVE(chipNum, bInfo->regInfo, sipMonitor, temp);
      HWC_IO_STORE_SLAVE(chipNum, bInfo->regInfo, sipMonitor, sipMonitor | SST_SIPROCESS_OSC_CNTR_RUN | SST_SIPROCESS_OSC_NOR_SEL);
    } else {
      HWC_IO_STORE(bInfo->regInfo, sipMonitor, sipMonitor | SST_SIPROCESS_OSC_CNTR_RESET_N | SST_SIPROCESS_OSC_NOR_SEL);
      HWC_IO_STORE(bInfo->regInfo, sipMonitor, sipMonitor | SST_SIPROCESS_OSC_CNTR_RESET_N | SST_SIPROCESS_OSC_NOR_SEL);
      HWC_IO_LOAD(bInfo->regInfo, sipMonitor, temp);
      HWC_IO_STORE(bInfo->regInfo, sipMonitor, sipMonitor | SST_SIPROCESS_OSC_CNTR_RUN | SST_SIPROCESS_OSC_NOR_SEL);
    }    

    while (count != 0) {
      // read back countdown counter to see when countdown has ended
      if(chipNum > 0) {
        HWC_IO_LOAD_SLAVE(chipNum, bInfo->regInfo, sipMonitor, count);
      } else {
        HWC_IO_LOAD(bInfo->regInfo, sipMonitor, count);
      }  
      count = (count & SST_SIPROCESS_PCI_CNTR) >> SST_SIPROCESS_PCI_CNTR_SHIFT;
    }
	 
    // read back actual SIP nand count
    if(chipNum > 0) {
      HWC_IO_LOAD_SLAVE(chipNum, bInfo->regInfo, sipMonitor, val);
    } else {
      HWC_IO_LOAD(bInfo->regInfo, sipMonitor, val);
    }    
    val &= SST_SIPROCESS_OSC_CNTR;

    // some statistics here
    if (val < min) { min = val; }
    if (val > max) { max = val; }
	 
    valarray[val]++;   // Do you have to initialize the array first? I forget...
  }

  // Please change the formatting of any text to however you see fit
  //printf("\n\n--- Process Values (NOR chain) ---\n");
  //printf("\n sip value: occurences (probability)\n");
  total = 0;
  for (i = 0; i <= max; i++) {
    if (valarray[i] != 0) {
      //printf("  %4d: %4d (%f)\n", i, valarray[i], ((float)valarray[i])/iters);
      total += i * valarray[i];
    }
  }

  E = total / iters;
  total = 0;
  for (i = 0; i <= max; i++) {
    if (valarray[i] != 0) {
      total += (i - E)*(i - E) * valarray[i];
    }
  }

  VAR = total / iters;
  SD  = sqrt(VAR);    // how do you do sqrt??

  total = 0; N = 0;
  for (i = 0; i <= max; i++) {
    if ((valarray[i] != 0) && (i >= (E - SD)) && (i <= (E + SD))) {
      total += i * valarray[i];
      N += valarray[i];
    }
  }
  avg = total / N;

  //printf("E   = %g\n", E);
  //printf("VAR = %g\n", VAR);
  //printf("SD  = %g\n", SD);
  //printf("NOR avg = %d (using %d values about E)\n", (FxU32)avg, N);

  *norChain = (FxU32)avg;

  // Restore clock speed
  HWC_IO_STORE(bInfo->regInfo, pllCtrl1, pllCtrl1_save);

}
