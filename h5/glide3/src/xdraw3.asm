;; THIS SOFTWARE IS SUBJECT TO COPYRIGHT PROTECTION AND IS OFFERED ONLY
;; PURSUANT TO THE 3DFX GLIDE GENERAL PUBLIC LICENSE. THERE IS NO RIGHT
;; TO USE THE GLIDE TRADEMARK WITHOUT PRIOR WRITTEN PERMISSION OF 3DFX
;; INTERACTIVE, INC. A COPY OF THIS LICENSE MAY BE OBTAINED FROM THE 
;; DISTRIBUTOR OR BY CONTACTING 3DFX INTERACTIVE INC(info@3dfx.com). 
;; THIS PROGRAM IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
;; EXPRESSED OR IMPLIED. SEE THE 3DFX GLIDE GENERAL PUBLIC LICENSE FOR A
;; FULL TEXT OF THE NON-WARRANTY PROVISIONS.  
;; 
;; USE, DUPLICATION OR DISCLOSURE BY THE GOVERNMENT IS SUBJECT TO
;; RESTRICTIONS AS SET FORTH IN SUBDIVISION (C)(1)(II) OF THE RIGHTS IN
;; TECHNICAL DATA AND COMPUTER SOFTWARE CLAUSE AT DFARS 252.227-7013,
;; AND/OR IN SIMILAR OR SUCCESSOR CLAUSES IN THE FAR, DOD OR NASA FAR
;; SUPPLEMENT. UNPUBLISHED RIGHTS RESERVED UNDER THE COPYRIGHT LAWS OF
;; THE UNITED STATES.  
;; 
;; COPYRIGHT 3DFX INTERACTIVE, INC. 1999, ALL RIGHTS RESERVED

TITLE   xdraw3.asm
.686P
ifdef GL_AMD3D
.MMX
.K3D
endif
ifdef GL_MMX
.MMX
endif
ifdef GL_SSE
.XMM
endif

EXTRN   __GlideRoot:DWORD
EXTRN   __grValidateState:NEAR
EXTRN   __grCommandTransportMakeRoom@12:NEAR
EXTRN	__fltused:BYTE

ifdef GL_AMD3D

;;--------------------------------------------------------------------------
;; start AMD3D version
;;--------------------------------------------------------------------------

;;; include listing.inc
INCLUDE fxgasm.h
    
CONST   SEGMENT
        ALIGN 8
_F256_F256      DQ    04380000043800000h ; 256 | 256
CONST   ENDS

_DATA   SEGMENT
        ALIGN   8
btab            DD    8 DUP(0)
atab            DD    8 DUP(0)
vSize           DD    0
strideinbytes   DD    0
vertices        DD    0
_DATA    ENDS


_TEXT         SEGMENT PAGE PUBLIC USE32 'CODE'
              ASSUME DS: FLAT, SS: FLAT

            ALIGN 32
PUBLIC  __grDrawTriangles_3DNow@12
_mode     = 20
_count    = 24
_pointers = 28
__grDrawTriangles_3DNow@12 PROC NEAR

; 930  : {
; 931  : #define FN_NAME "_grDrawTriangles"
; 932  : 
; 933  :   FxI32
; 934  : #ifdef GLIDE_DEBUG
; 935  :     vSize,
; 936  : #endif
; 937  :     k;
; 938  :   FxI32 stride = mode;
; 939  :   float *vPtr;
; 940  : 
; 941  :   GR_BEGIN_NOFIFOCHECK(FN_NAME, 90);
; 942  : 
; 943  :   GDBG_INFO_MORE(gc->myLevel, "(count = %d, pointers = 0x%x)\n",
; 944  :                  count, pointers);
; 945  : 
; 946  :   GR_FLUSH_STATE();

gc            TEXTEQU  <edi>             ; points to graphics context
fifo          TEXTEQU  <ecx>             ; points to next entry in fifo
dlp           TEXTEQU  <ebp>             ; points to dataList structure
vertexCount   TEXTEQU  <esi>             ; Current vertex counter in the packet
vertexPtr     TEXTEQU  <ebx>             ; Current vertex pointer (in deref mode)
vertex        TEXTEQU  <ebx>             ; Current vertex (in non-deref mode)
dlpStart      TEXTEQU  <edx>             ; Pointer to start of offset list

    push      edi                        ; save caller's register variable
    mov       eax, DWORD PTR fs:[18h]    ; get thread local storage base pointer

    push      esi                        ; save caller's register variable
    mov       edx, [__GlideRoot+tlsOffset]; offset of GC into tls

    push      ebx                        ; save caller's register variable
    mov       vertexCount, [esp+_count-4]; number of vertices in triangles

    mov       gc, [eax+edx]              ; get GC for current thread 
    mov       vertexPtr, [esp+_pointers-4]; get current vertex pointer (deref mode)

    push      ebp                        ; save frame pointer
    mov       edx, [gc + invalid]        ; state needs validation ?

    test      vertexCount, vertexCount   ; number of vertices <= 0 ?
    jle       $tris_done                 ; yup, triangles are done

    test      edx, edx                   ; do we need to validate state ?
    je        $no_validation             ; nope, it's valid

    call      __grValidateState          ; validate state

$no_validation:

; 947  : 
; 948  : #ifdef GLIDE_DEBUG
; 949  :   GDBG_INFO(110, "%s:  vSize = %d\n", FN_NAME, vSize);
; 950  : 
; 951  :   GDBG_INFO(110, "%s:  paramMask = 0x%x\n", FN_NAME, gc->cmdTransportInfo.paramMask);
; 952  : #endif
; 953  : 
; 954  :   if (stride == 0)
; 955  :     stride = gc->state.vData.vStride;
; 956  : 
; 957  : 
; 958  :   _GlideRoot.stats.trisProcessed+=(count/3);
; 959  : 
; 960  :   if (gc->state.grCoordinateSpaceArgs.coordinate_space_mode == GR_WINDOW_COORDS) {

;; We can operate in one of two modes:
;;
;; 0. We are stepping through an array of vertices, in which case
;; the stridesize is equal to the size of the vertex data, and
;; always > 4, since vertex data must a least contain x,y (ie 8 bytes).
;; vertexPtr is pointing to the array of vertices.
;;
;; 1. We are stepping through an array of pointers to vertices
;; in which case the stride is 4 bytes and we need to dereference
;; the pointers to get at the vertex data. vertexPtr is pointing
;; to the array of pointers to vertices.

    mov       eax, [esp + _count]        ; count
    mov       ebp, 0AAAAAAABh            ; 1/3*2^32*2

    femms                                ; we'll use MMX; clear MMX/3DX state      

    mul       ebp                        ; edx:eax = 1/3*2*2^32*count; edx = 1/3*2*count
    nop                                  ; filler

    mov       eax, [gc + trisProcessed]  ; trisProcessed
    shr       edx, 1                     ; count/3

    add       eax, edx                   ; trisProcessed += count/3
    mov       edx, [esp + _mode]         ; get mode (0 or 1)

    mov       ecx, [gc + CoordinateSpace]; coordinates space (window/clip)
    mov       [gc + trisProcessed], eax  ; trisProcessed

    test      edx, edx                   ; mode 0 (array of vertices) ?
    jnz       $deref_mode                ; nope, it's mode 1 (array of pointers to vertices)

    mov       edx, [gc + vertexStride]   ; get stride in DWORDs

    shl       edx, 2                     ; stride in bytes
    test      ecx, ecx                   ; coordinate space == 0 (window) ?

    mov       [strideinbytes], edx       ; save off stride (in bytes)
    jnz       $clip_coordinates_ND       ; nope, coordinate space != window  

; 961  :     while (count > 0) {
; 962  :       FxI32 vcount = count >=15 ? 15 : count;
; 963  :       GR_SET_EXPECTED_SIZE(vcount * gc->state.vData.vSize, 1);
; 964  :       TRI_STRIP_BEGIN(kSetupStrip, vcount, gc->state.vData.vSize, SSTCP_PKT3_BDDBDD);
; 965  :       

$win_coords_loop_ND:

    sub       vertexCount, 15            ; vertexCount >= 15 ? CF=0 : CF=1
    mov       ecx, [gc + vertexSize]     ; bytes of data for each vertex 

    sbb       eax, eax                   ; vertexCount >= 15 ? 00000000:ffffffff

    and       vertexCount, eax           ; vertexCount >= 15 ? 0 : vertexcount-15
    add       vertexCount, 15            ; vertexcount >= 15 ? 15 :vertexcount

    imul      ecx, vertexCount           ; total amount of vertex data we'll send

    mov       eax, [gc + fifoRoom]       ; fifo space available
    add       ecx, 4                     ; add header size ==> total packet size

    cmp       eax, ecx                   ; fifo space avail >= packet size ?
    jge       $win_tri_begin_ND          ; yup, start writing triangle data

    push      @Line                      ; line number inside this function
    push      0h                         ; pointer to function name = NULL

    push      ecx                        ; fifo space needed
    call      __grCommandTransportMakeRoom@12 ; note: updates fifoPtr

        align   32
$win_tri_begin_ND:

    mov       eax, vertexCount           ; number of vertices in triangles
    mov       fifo, [gc + fifoPtr]       ; get fifoPtr

    mov       ebp, [gc + cullStripHdr]   ; <2:0> = type
    shl       eax, 6                     ; <9:6> = vertex count (max 15)

    lea       dlpStart, [gc+tsuDataList] ; pointer to start of offset list
    or        eax, ebp                   ; setup vertex count and type

    test      fifo, 4                    ; fifoPtr QWORD aligned ?
    jz        $fifo_aligned_ND           ; yup

    mov       [fifo], eax                ; PCI write packet type
    add       fifo, 4                    ; fifo pointer now QWORD aligned

; 966  :       for (k = 0; k < vcount; k++) {
; 967  :         FxI32 i;
; 968  :         FxU32 dataElem = 0;
; 969  :         
; 970  :         vPtr = pointers;
; 971  :         if (mode)
; 972  :           vPtr = *(float **)vPtr;
; 973  :         (float *)pointers += stride;
; 974  :         
; 975  :         i = gc->tsuDataList[dataElem];
; 976  :         
; 977  :         TRI_SETF(FARRAY(vPtr, 0));
; 978  :         TRI_SETF(FARRAY(vPtr, 4));
; 979  :         while (i != GR_DLIST_END) {
; 980  :           TRI_SETF(FARRAY(vPtr, i));
; 981  :           dataElem++;
; 982  :           i = gc->tsuDataList[dataElem];
; 983  :         }
; 984  :       }
; 985  :       TRI_END;
; 986  :       GR_CHECK_SIZE();
; 987  :       count -= 15;
; 988  :     }

$win_vertex_loop_ND_WB0:                 ; nothing in "write buffer"

    mov       eax, [dlpStart]            ; get first offset from offset list
    mov       dlp, dlpStart              ; point to start of offset list

    movq      mm1, [vertex]              ; get vertex x,y
    add       fifo, 8                    ; fifoPtr += 2*sizeof(FxU32)

    add       dlp, 4                     ; dlp++
    test      eax, eax                   ; if offset == 0, end of list

    movq      [fifo-8], mm1              ; PCI write x, y
    jz        $win_datalist_end_ND_WB0   ; no more vertex data, nothing in "write buffer" 

$win_datalist_loop_ND_WB0:               ; nothing in "write buffer"

    movd      mm1, [vertex + eax]        ; get next parameter
    mov       eax, [dlp]                 ; get next offset from offset list

    test      eax, eax                   ; at end of offset list (offset == 0) ?
    jz        $win_datalist_end_ND_WB1   ; exit, write buffer contains one DWORD

    movd      mm2, [vertex + eax]        ; get next parameter
    add       dlp, 8                     ; dlp++

    mov       eax, [dlp-4]               ; get next offset from offset list
    add       fifo, 8                    ; fifoPtr += 2*sizeof(FxU32)

    test      eax, eax                   ; at end of offset list (offset == 0) ?
    punpckldq mm1, mm2                   ; current param | previous param

    movq      [fifo-8], mm1              ; PCI write current param | previous param
    jnz       $win_datalist_loop_ND_WB0  ; nope, copy next parameter

$win_datalist_end_ND_WB0:

    mov       eax, [strideinbytes]       ; get offset to next vertex
    dec       vertexCount                ; another vertex done. Any left?

    lea       vertex, [vertex + eax]     ; points to next vertex
    jnz       $win_vertex_loop_ND_WB0    ; yup, output next vertex

$win_vertex_end_ND_WB0:

    mov       eax, [gc + fifoPtr]        ; old fifoPtr
    mov       ebp, [gc + fifoRoom]       ; old number of bytes available in fifo

    mov       vertexCount, [esp + _count]; remaining vertices before previous loop
    sub       eax, fifo                  ; old fifoPtr - new fifoPtr (fifo room used)

    mov       [gc + fifoPtr], fifo       ; save current fifoPtr 
    add       ebp, eax                   ; new number of bytes available in fifo

    sub       vertexCount, 15            ; remaining number of vertices to process
    mov       [gc + fifoRoom], ebp       ; save current number of bytes available in fifo

    mov       [esp + _count], vertexCount; remaining number of vertices to process 
    test      vertexCount, vertexCount   ; any vertices left to process ?

    nop                                  ; filler
    jg        $win_coords_loop_ND        ; loop if number of vertices to process >= 0

    femms                                ; no more MMX code; clear MMX/FPU state

    pop       ebp                        ; restore frame pointer
    pop       ebx                        ; restore caller's register variable

    pop       esi                        ; restore caller's register variable
    pop       edi                        ; restore caller's register variable

    ret       12                         ; return, pop 3 DWORD parameters off stack

$fifo_aligned_ND:

    movd      mm1, eax                   ; move header into "write buffer"

$win_vertex_loop_ND_WB1:                 ; one DWORD in "write buffer"

    movd      mm2, [vertex]              ; 0 | x of vertex
    add       fifo, 8                    ; fifoPtr += 2*sizeof(FxU32)

    lea       dlp, [dlpStart + 4]        ; point to start of offset list
    nop                                  ; filler

    punpckldq mm1, mm2                   ; packet header | x of vertex
    mov       eax, [dlp-4]               ; first offset in offset list

    movq      [fifo-8], mm1              ; PCI write packet header | x of vertex
    movd      mm1, [vertex+4]            ; 0 | y of vertex

    cmp       eax, 0                     ; offset == 0 (list empty) ?
    jz        $win_datalist_end_ND_WB1   ; yup, no more vertex data, one DWORD in "write buffer"

$win_datalist_loop_ND_WB1:               ; one DWORD in "write buffer" 

    movd      mm2, [vertex + eax]        ; get next parameter
    add       fifo, 8                    ; fifoPtr += 2*sizeof(FxU32)

    mov       eax, [dlp]                 ; get next offset from offset list
    add       dlp, 8                     ; dlp += 2

    punpckldq mm1, mm2                   ; current param | previous param
    cmp       eax, 0                     ; at end of offset list (offset == 0) ?

    movq      [fifo-8], mm1              ; PCI write current param | previous param
    jz        $win_datalist_end_ND_WB0   ; yes, exit, "write buffer" empty

    movd      mm1, [vertex + eax]        ; get next parameter
    mov       eax, [dlp-4]               ; get next offset from offset list

    cmp       eax, 0                     ; at end of offset list (offset == 0) ?
    jnz       $win_datalist_loop_ND_WB1  ; nope, copy next parameter

$win_datalist_end_ND_WB1:

    mov       eax, [strideinbytes]       ; get offset to next vertex
    dec       vertexCount                ; another vertex done. Any left?

    lea       vertex, [vertex + eax]     ; points to next vertex
    jnz       $win_vertex_loop_ND_WB1    ; yup, output next vertex

$win_vertex_end_ND_WB1:

    movd      [fifo], mm1                ; flush "write buffer"
    mov       eax, [gc + fifoPtr]        ; old fifoPtr

    add       fifo, 4                    ; fifoPtr += sizeof(FxU32)
    mov       ebp, [gc + fifoRoom]       ; old number of bytes available in fifo

    mov       vertexCount, [esp + _count]; remaining vertices before previous loop
    sub       eax, fifo                  ; old fifoPtr - new fifoPtr (fifo room used)

    mov       [gc + fifoPtr], fifo       ; save current fifoPtr 
    add       ebp, eax                   ; new number of bytes available in fifo

    mov       [gc + fifoRoom], ebp       ; save current number of bytes available in fifo
    sub       vertexCount, 15            ; remaining number of vertices to process

    mov       [esp + _count], vertexCount; remaining number of vertices to process 
    test      vertexCount, vertexCount   ; any vertices left to process ?

    nop                                  ; filler
    jg        $win_coords_loop_ND        ; loop if number of vertices to process >= 0

    femms                                ; no more MMX code; clear MMX/FPU state

    pop       ebp                        ; restore frame pointer
    pop       ebx                        ; restore caller's register variable

    pop       esi                        ; restore caller's register variable
    pop       edi                        ; restore caller's register variable

    ret       12                         ; return, pop 3 DWORD parameters off stack

    align 32

$deref_mode:

    prefetch  [vertexPtr]                ; pre-load first group of pointers

    test      ecx, ecx                   ; coordinate space == 0 (window) ?
    jnz       $clip_coordinates_D        ; nope, coordinate space != window

$win_coords_loop_D:

    sub       vertexCount, 15            ; vertexCount >= 15 ? CF=0 : CF=1
    mov       ecx, [gc + vertexSize]     ; bytes of data for each vertex 

    sbb       eax, eax                   ; vertexCount >= 15 ? 00000000:ffffffff

    and       vertexCount, eax           ; vertexCount >= 15 ? 0 : vertexcount-15
    add       vertexCount, 15            ; vertexcount >= 15 ? 15 :vertexcount

    imul      ecx, vertexCount           ; total amount of vertex data we'll send

    mov       eax, [gc + fifoRoom]       ; fifo space available
    add       ecx, 4                     ; add header size ==> total packet size

    cmp       eax, ecx                   ; fifo space avail >= packet size ?
    jge       $win_tri_begin_D           ; yup, start writing triangle data

    push      @Line                      ; line number inside this function
    push      0h                         ; pointer to function name = NULL

    push      ecx                        ; fifo space needed
    call      __grCommandTransportMakeRoom@12 ; note: updates fifoPtr

        align 32
$win_tri_begin_D:

    mov       eax, vertexCount           ; number of vertices in triangles
    mov       fifo, [gc + fifoPtr]       ; get fifoPtr

    mov       ebp, [gc + cullStripHdr]   ; <2:0> = type
    shl       eax, 6                     ; <9:6> = vertex count (max 15)

    or        eax, ebp                   ; setup mode, vertex count, and type
    lea       dlpStart, [gc+tsuDataList] ; pointer to start of offset list

    test      fifo, 4                    ; fifoPtr QWORD aligned ?
    jz        $fifo_aligned_D            ; yup

    mov       [fifo], eax                ; PCI write packet type
    add       fifo, 4                    ; fifo pointer now QWORD aligned

$win_vertex_loop_D_WB0:                  ; nothing in "write buffer"

    mov       edx, [vertexPtr]           ; dereference pointer, edx points to vertex
    add       vertexPtr, 4               ; next pointer

    lea       dlp, [gc + tsuDataList]    ; get pointer to offset list
    movq      mm1, [edx]                 ; get vertex x,y

    mov       eax, [dlp]                 ; get first offset from offset list
    add       dlp, 4                     ; dlp++

    movq      [fifo], mm1                ; PCI write x, y
    add       fifo, 8                    ; fifo += 2

    test      eax, eax                   ; if offset == 0, end of offset list
    je        $win_datalist_end_D_WB0    ; no more vertex data, nothing in "write buffer" 

$win_datalist_loop_D_WB0:                ; nothing in "write buffer"

    movd      mm1, [edx + eax]           ; get next parameter
    mov       eax, [dlp]                 ; get next offset from offset list

    test      eax, eax                   ; at end of offset list (offset == 0) ?
    jz        $win_datalist_end_D_WB1    ; exit, write buffer contains one DWORD

    movd      mm2, [edx + eax]           ; get next parameter
    add       dlp, 8                     ; dlp++

    mov       eax, [dlp-4]               ; get next offset from offset list
    add       fifo, 8                    ; fifoPtr += 2*sizeof(FxU32)

    punpckldq mm1, mm2                   ; current param | previous param
    cmp       eax, 0                     ; at end of offset list (offset == 0) ?

    movq      [fifo-8], mm1              ; PCI write current param | previous param
    jnz       $win_datalist_loop_D_WB0   ; nope, copy next parameter

$win_datalist_end_D_WB0:

    dec       vertexCount                ; another vertex done. Any left?
    jnz       $win_vertex_loop_D_WB0     ; yup, output next vertex

$win_vertex_end_D_WB0:

    mov       eax, [gc + fifoPtr]        ; old fifoPtr
    nop                                  ; filler

    mov       ebp, [gc + fifoRoom]       ; old number of bytes available in fifo
    nop                                  ; filler

    mov       vertexCount, [esp + _count]; remaining vertices before previous loop
    sub       eax, fifo                  ; old fifoPtr - new fifoPtr (fifo room used)

    add       ebp, eax                   ; new number of bytes available in fifo
    mov       [gc + fifoPtr], fifo       ; save current fifoPtr 

    sub       vertexCount, 15            ; remaining number of vertices to process

    mov       [gc + fifoRoom], ebp       ; save current number of bytes available in fifo
    test      vertexCount, vertexCount   ; any vertices left to process ?

    mov       [esp + _count], vertexCount; remaining number of vertices to process
    jg        $win_coords_loop_D         ; loop if number of vertices to process >= 0

    femms                                ; no more MMX code; clear MMX/FPU state

    pop       ebp                        ; restore frame pointer
    pop       ebx                        ; restore caller's register variable

    pop       esi                        ; restore caller's register variable
    pop       edi                        ; restore caller's register variable

    ret       12                         ; return, pop 3 DWORD parameters off stack

$fifo_aligned_D:

    movd      mm1, eax                   ; move header into "write buffer"

$win_vertex_loop_D_WB1:                  ; one DWORD in "write buffer"

    mov       edx, [vertexPtr]           ; dereference pointer, edx points to vertex
    add       vertexPtr, 4               ; next pointer

    add       fifo, 8                    ; fifoPtr += 2*sizeof(FxU32)
    lea       dlp, [gc + tsuDataList]    ; get pointer to start of offset list

    movd      mm2, [edx]                 ; 0 | x of vertex
    add       dlp, 4                     ; dlp++
 
    mov       eax, [dlp-4]               ; first offset in offset list
    punpckldq mm1, mm2                   ; packet header | x of vertex

    movq      [fifo-8], mm1              ; PCI write packet header | x of vertex
    movd      mm1, [edx + 4]             ; 0 | y of vertex

    cmp       eax, 0                     ; offset == 0 (list empty) ?
    je        $win_datalist_end_D_WB1    ; yup, no more vertex data, one DWORD in "write buffer"

$win_datalist_loop_D_WB1:                ; one DWORD in "write buffer" = MM1

    movd      mm2, [edx + eax]           ; get next parameter
    add       fifo, 8                    ; fifoPtr += 2*sizeof(FxU32)

    mov       eax, [dlp]                 ; get next offset from offset list
    add       dlp, 8                     ; dlp += 2

    punpckldq mm1, mm2                   ; current param | previous param
    test      eax, eax                   ; at end of offset list (offset == 0) ?

    movq      [fifo-8], mm1              ; PCI write current param | previous param
    jz        $win_datalist_end_D_WB0    ; yes, exit, "write buffer" empty

    movd      mm1, [edx + eax]           ; get next parameter
    mov       eax, [dlp-4]               ; get next offset from offset list

    test      eax,  eax                  ; at end of offset list (offset == 0) ?
    jnz       $win_datalist_loop_D_WB1   ; nope, copy next parameter

$win_datalist_end_D_WB1:

    dec       vertexCount                ; another vertex done. Any left?
    jnz       $win_vertex_loop_D_WB1     ; yup, output next vertex

$win_vertex_end_D_WB1:

    movd      [fifo], mm1                ; flush "write buffer"
    mov       eax, [gc + fifoPtr]        ; old fifoPtr

    add       fifo, 4                    ; fifoPtr++
    mov       ebp, [gc + fifoRoom]       ; old number of bytes available in fifo

    mov       vertexCount, [esp + _count]; remaining vertices before previous loop
    sub       eax, fifo                  ; old fifoPtr - new fifoPtr (fifo room used)

    mov       [gc + fifoPtr], fifo       ; save current fifoPtr 
    sub       vertexCount, 15            ; remaining number of vertices to process

    add       ebp, eax                   ; new number of bytes available in fifo
    mov       [gc + fifoRoom], ebp       ; save current number of bytes available in fifo

    mov       [esp + _count], vertexCount; remaining number of vertices to process 
    cmp       vertexCount, 0             ; any vertices left to process ?

    nop                                  ; filler
    jg        $win_coords_loop_D         ; loop if number of vertices to process >= 0

    femms                                ; no more MMX code; clear MMX/FPU state

    pop       ebp                        ; restore frame pointer
    pop       ebx                        ; restore caller's register variable

    pop       esi                        ; restore caller's register variable
    pop       edi                        ; restore caller's register variable

    ret       12                         ; return, pop 3 DWORD parameters off stack

    ALIGN     32

; 989  :   }
; 990  :   else {
; 991  :     /*
; 992  :      * first cut of clip space coordinate code, no optimization.
; 993  :      */
; 994  :     float oow;
; 995  :     
; 996  :     while (count > 0) {
; 997  :       FxI32 vcount = count >= 15 ? 15 : count;
; 998  :       
; 999  :       GR_SET_EXPECTED_SIZE(vcount * gc->state.vData.vSize, 1);
; 1000 :       TRI_STRIP_BEGIN(kSetupStrip, vcount, gc->state.vData.vSize, SSTCP_PKT3_BDDBDD);
; 1001 :       
; 1002 :       for (k = 0; k < vcount; k++) {
; 1003 :         vPtr = pointers;
; 1004 :         if (mode)
; 1005 :           vPtr = *(float **)pointers;
; 1006 :         oow = 1.0f / FARRAY(vPtr, gc->state.vData.wInfo.offset);
; 1007 :         
; 1008 :         /* x, y */
; 1009 :         TRI_SETF(FARRAY(vPtr, 0)
; 1010 :                  *oow*gc->state.Viewport.hwidth + gc->state.Viewport.ox);
; 1011 :         TRI_SETF(FARRAY(vPtr, 4)
; 1012 :                  *oow*gc->state.Viewport.hheight + gc->state.Viewport.oy);
; 1013 :         (float *)pointers += stride;
; 1014 :         
; 1015 :         TRI_VP_SETFS(vPtr,oow);
; 1016 :       }
; 1017 :       TRI_END;
; 1018 :       GR_CHECK_SIZE();
; 1019 :       count -= 15;
; 1020 :   }
; 1021 : }

$clip_coordinates_D:

    mov       [strideinbytes], 4         ; unit stride for array of pointers to vertices

$clip_coordinates_ND:

dataElem      textequ <ebp>              ; number of vertex components processed

    movd      mm6,[__GlideRoot+pool_f255]; GlideRoot.pool.f255 

$clip_coords_begin:

    sub       vertexCount, 15            ; vertexCount >= 15 ? CF=0 : CF=1
    mov       ecx, [gc + vertexSize]     ; bytes of data for each vertex 

    sbb       eax, eax                   ; vertexCount >= 15 ? 00000000:ffffffff

    and       vertexCount, eax           ; vertexCount >= 15 ? 0 : vertexcount-15
    add       vertexCount, 15            ; vertexcount >= 15 ? 15 :vertexcount

    imul      ecx, vertexCount           ; total amount of vertex data we'll send

    mov       eax, [gc + fifoRoom]       ; fifo space available
    add       ecx, 4                     ; add header size ==> total packet size

    cmp       eax, ecx                   ; fifo space avail >= packet size ?
    jge       $clip_tri_begin            ; yup, start writing triangle data

    push      @Line                      ; line number inside this function
    push      0h                         ; pointer to function name = NULL

    push      ecx                        ; fifo space needed
    call      __grCommandTransportMakeRoom@12 ; note: updates fifoPtr

        align 32
$clip_tri_begin:
    mov       edx, vertexCount           ; number of vertices in triangles
    mov       fifo, [gc + fifoPtr]       ; get fifoPtr

    mov       ebp, [gc + cullStripHdr]   ; <2:0> = type
    shl       edx, 6                     ; <9:6> = vertex count (max 15)

    or        edx, ebp                   ; setup mode, vertex count, and type

    mov       [fifo], edx                ; PCI write packet type
    add       fifo, 4                    ; fifo pointer now QWORD aligned

$clip_for_begin:

    mov       edx, vertexPtr             ; vertex = vertexPtr (assume no-deref mode)
    mov       eax, [esp+_mode]           ; mode 0 = no deref, mode 1 = deref

    mov       [vertices], vertexCount    ; save numnber of vertices
    test      eax, eax                   ; deref mode ?

    mov       eax, [gc + wInfo_offset]   ; get offset of W into vertex struct
    jz        $clip_noderef              ; yup, no-deref mode

    mov       edx, [vertexPtr]           ; vertex = *vertexPtr

$clip_noderef:

    movd      mm0, [edx + eax]           ; 0 | W of current vertex
    pfrcp     mm1, mm0                   ; 0 | 1/W approx

    mov       ebp, [strideinbytes]       ; offset to next vertex/vertexPtr
    movq      mm2, [edx]                 ; y | x of current vertex

    pfrcpit1  mm0, mm1                   ; 0 | 1/W refine
    movq      mm3, [gc + vp_hwidth]      ; gc->state.Viewport.hheight | gc->state.Viewport.hwidth

    movq      mm4, [gc + vp_ox]          ; gc->state.Viewport.oy | gc->state.Viewport.ox
    add       vertexPtr, ebp             ; point to next vertex/VertexPtr

    pfrcpit2  mm0, mm1                   ; oow = 1.0f / FARRAY(vPtr, gc->state.vData.wInfo.offset
    mov       esi, [gc + paramIndex]     ; gc->state.paramIndex

    pfmul     mm2, mm3                   ; TRI_SETF(FARRAY(vPtr,0)*state.Viewport.hheight | TRI_SETF(FARRAY(vPtr,4)*state.Viewport.hwidth
    xor       dataElem, dataElem         ; dataElem = 0

    add       fifo, 8                    ; fifoPtr += 2*sizeof(FxFloat)
    punpckldq mm0, mm0                   ; oow | oow

    pfmul     mm2, mm0                   ; TRI_SETF(FARRAY(vPtr, 4)*oow*gc->state.Viewport.height | TRI_SETF(FARRAY(vPtr, 0)*oow*gc->state.Viewport.hwidth
    pfadd     mm2, mm4                   ; TRI_SETF(FARRAY(vPtr, 4)*oow*gc->state.Viewport.hheight + gc->state.Viewport.oy) |

;;;  FxI32 i, dataElem=0; \
;;;  i = gc->tsuDataList[dataElem]; \
;;;  if (gc->state.paramIndex & (STATE_REQUIRES_IT_DRGB | STATE_REQUIRES_IT_ALPHA)) { \
;;;    if (gc->state.vData.colorType == GR_FLOAT) { \
;;;      if (gc->state.paramIndex & STATE_REQUIRES_IT_DRGB) { \
;;;        DA_SETF_SCALE_ADVANCE(_s,_GlideRoot.pool.f255); \
;;;        DA_SETF_SCALE_ADVANCE(_s,_GlideRoot.pool.f255); \
;;;        DA_SETF_SCALE_ADVANCE(_s,_GlideRoot.pool.f255); \
;;;      } \
;;;      if (gc->state.paramIndex & STATE_REQUIRES_IT_ALPHA) { \
;;;        DA_SETF_SCALE_ADVANCE(_s,_GlideRoot.pool.f255); \
;;;      } \
;;;    } \
;;;    else { \
;;;      DA_SETF(FARRAY(_s, i)); \
;;;      dataElem++; \
;;;      i = gc->tsuDataList[dataElem]; \
;;;    } \
;;;  } \

    test      esi, 3                     ; STATE_REQUIRES_IT_DRGB | STATE_REQUIRES_IT_ALPHA ?
    mov       eax, [gc + tsuDataList]    ; first entry from offset list

    movq      [fifo-8], mm2              ; PCI write transformed x, y
    jz        $clip_setup_ooz            ; nope, no color at all needed
  
    cmp       DWORD PTR [gc+colorType], 0; gc->state.vData.colorType == GR_FLOAT ?
    jne       $clip_setup_pargb          ; nope, packed ARGB format
  
    test      esi, 1                     ; STATE_REQUIRES_IT_DRGB ?
    jz        $clip_setup_a              ; no, but definitely A

    movd      mm2, [edx + eax]           ; 0 | r
    mov       eax, [gc + tsuDataList+4]  ; offset of g part of vertex data

    pfmul     mm2, mm6                   ; 0 | r * 255.0f
    movd      mm3, [edx + eax]           ; 0 | g

    mov       eax, [gc + tsuDataList + 8]; offset of b part of vertex data
    movd      [fifo], mm2                ; PCI write r*255

    pfmul     mm3, mm6                   ; 0 | g * 255.0f
    movd      mm2, [edx + eax]           ; 0 | b

    movd      [fifo+4], mm3              ; PCI write g*255
    mov       dataElem, 12               ; dataElem = 3

    pfmul     mm2, mm6                   ; 0 | b * 255.0f
    mov       eax, [gc + tsuDataList+12] ; offset of A part of vertex data

    test      esi, 2                     ; STATE_REQUIRES_IT_ALPHA ?
    lea       fifo, [fifo+12]            ; fifoPtr += 3*sizeof(FxFloat)

    movd      [fifo-4], mm2              ; PCI write b*255
    jz        $clip_setup_ooz            ; nope, no alpha, proceeed with ooz

$clip_setup_a:
    movd      mm2, [eax+edx]             ; 0 | a
    add       fifo, 4                    ; fifoPtr += sizeof(FxFloat)

    mov       esp, esp                   ; filler
    add       dataElem, 4                ; dataElem++ 

    pfmul     mm2, mm6                   ; 0 | a * 255.0f
    mov       eax, [gc+dataElem+tsuDataList]; offset of next part of vertex data

    movd      [fifo-4], mm2              ; PCI write a*255
    jmp       $clip_setup_ooz            ; check whether we need to push out z

$clip_setup_pargb:
    movd      mm2, [eax+edx]             ; get packed ARGB data
    add       fifo, 4                    ; fifoPtr += sizeof(FxU32)

    mov       dataElem, 4                ; dataElem = 1 (namely pargb)
    mov       eax, [gc+tsuDataList+4]    ; offset of next part of vertex data

    movd      [fifo-4], mm2              ; PCI write packed ARGB

;;;  if (gc->state.paramIndex & STATE_REQUIRES_OOZ) { \
;;;    if (gc->state.fbi_config.fbzMode & SST_DEPTH_FLOAT_SEL) { \
;;;      if (gc->state.vData.qInfo.mode == GR_PARAM_ENABLE) { \
;;;        DA_SETF(FARRAY(_s, gc->state.vData.qInfo.offset)*_oow); \
;;;      } else { \
;;;        DA_SETF(_oow); \
;;;      } \
;;;      dataElem++; \
;;;      i = gc->tsuDataList[dataElem]; \
;;;    } \
;;;    else { \
;;;      DA_SETF(FARRAY(_s, i)*_oow*gc->state.Viewport.hdepth + gc->state.Viewport.oz); \
;;;      dataElem++; \
;;;      i = gc->tsuDataList[dataElem]; \
;;;    } \
;;;  } \

$clip_setup_ooz:
  
    test      esi, 4                     ; STATE_REQUIRES_OOZ ?
    jz        $clip_setup_qow            ; nope

    test      DWORD PTR[gc+fbi_fbzMode],200000h ; gc->state.fbi_config.fbzMode & SST_DEPTH_FLOAT_SEL != 0 ?
    je        $clip_setup_ooz_nofog      ; nope

    cmp       DWORD PTR[gc+qInfo_mode], 0; gc->state.vData.qInfo.mode == GR_PARAM_ENABLE ?
    jz        $clip_setup_fog_oow        ; nope

    mov       eax, [gc + qInfo_offset]   ; offset of Q component of vertex
    add       fifo, 4                    ; fifoPtr += sizeof(FxFloat)

    add       dataElem, 4                ; dataElem++
    movd      mm2, [edx + eax]           ; 0 | q of vertex

    mov       eax,[gc+dataElem+tsuDataList]; pointer to next vertex component
    pfmul     mm2, mm0                   ; 0 | q*oow

    movd      [fifo-4], mm2              ; PCI write transformed Q
    jmp       $clip_setup_qow            ; check whether we need to write Q or W

$clip_setup_fog_oow:

    add       fifo, 4                    ; fifoPtr += sizeof(FxFloat) 
    add       dataElem, 4                ; dataElem++

    movd      mm3, [gc + depth_range]    ;
    pfmul     mm3, mm0                   ; 

    movd      mm4, [gc + depth_range]    ; depth range
    pfsub     mm4, mm3                   ; 

    movd      [fifo-4], mm4              ; PCI write oow
    mov       eax,[gc+dataElem+tsuDataList]; pointer to next vertex component

    jmp       $clip_setup_qow            ; check whether we need to write Q or W

$clip_setup_ooz_nofog:

    movd      mm2, [eax + edx]           ; 0 | z component of vertex
    add       fifo, 4                    ; fifoPtr += sizeof(FxFloat)

    add       dataElem, 4                ; dataElem += 1
    movd      mm3, [gc + vp_hdepth]      ; 0 | gc->state.Viewport.hdepth

    pfmul     mm2, mm0                   ; TRI_SETF(FARRAY(_s, i)*_oow
    movd      mm4, [gc + vp_oz]          ; 0 | gc->state.Viewport.oz

    pfmul     mm2, mm3                   ; 0 | TRI_SETF(FARRAY(_s, i)*_oow*gc->state.Viewport.hdepth
    mov       eax, [gc+dataElem+tsuDataList]; offset of next vertex component

    pfadd     mm2, mm4                   ; 0 | TRI_SETF(FARRAY(_s, i)*_oow*gc->state.Viewport.hdepth+gc->state.Viewport.oz
    movd      [fifo-4], mm2              ; PCI write transformed Z

;;;  if (gc->state.paramIndex & STATE_REQUIRES_OOW_FBI) { \
;;;    if (gc->state.vData.fogInfo.mode == GR_PARAM_ENABLE) { \
;;;      DA_SETF(FARRAY(_s, gc->state.vData.fogInfo.offset)*_oow); \
;;;    } \
;;;    else if (gc->state.vData.qInfo.mode == GR_PARAM_ENABLE) { \
;;;      DA_SETF(FARRAY(_s, gc->state.vData.qInfo.offset)*_oow); \
;;;    } else { \
;;;      DA_SETF(_oow); \
;;;    } \
;;;    dataElem++; \
;;;    i = gc->tsuDataList[dataElem]; \
;;;  } \

$clip_setup_qow:
    test      esi, 8                     ; STATE_REQUIRES_OOW_FBI ?
    jz        $clip_setup_qow0           ; nope

    cmp       DWORD PTR[gc+fogInfo_mode],0; gc->state.vData.fogInfo.mode == GR_PARAM_ENABLE ?
    jz        $clip_setup_oow_nofog      ; nope, no fog

    mov       eax, [gc + fogInfo_offset] ; offset of fogInfo component of vertex
    add       fifo, 4                    ; fifoPtr += sizeof(FxFloat)

    add       dataElem, 4                ; dataElem++
    movd      mm2, [edx + eax]           ; 0 | fogInfo of vertex

    mov       eax,[gc+dataElem+tsuDataList]; pointer to next vertex component
    pfmul     mm2, mm0                   ; fogInfo*oow

    movd      [fifo-4], mm2              ; PCI write transformed Q
    jmp       $clip_setup_qow0           ; continue with q0

$clip_setup_oow_nofog:

    cmp       DWORD PTR [gc+qInfo_mode],0; gc->state.vData.qInfo.mode == GR_PARAM_ENABLE ?
    je        $clip_setup_oow            ; nope, write oow, not Q

    mov       eax, [gc + qInfo_offset]   ; offset of Q component of vertex
    add       fifo, 4                    ; fifoPtr += sizeof(FxFloat)

    add       dataElem, 4                ; dataElem++
    movd      mm2, [edx + eax]           ; 0 | q of vertex

    mov       eax,[gc+dataElem+tsuDataList]; pointer to next vertex component
    pfmul     mm2, mm0                   ; q*oow

    movd      [fifo-4], mm2              ; PCI write transformed Q
    jmp       $clip_setup_qow0           ; continue with q0

$clip_setup_oow:
    add       fifo, 4                    ; fifoPtr += sizeof(FxFloat) 
    add       dataElem, 4                ; dataElem++

    movd      [fifo-4], mm0              ; PCI write oow
    mov       eax,[gc+dataElem+tsuDataList]; pointer to next vertex component

;;;  if (gc->state.paramIndex & STATE_REQUIRES_W_TMU0) { \
;;;    if (gc->state.vData.q0Info.mode == GR_PARAM_ENABLE) { \
;;;      DA_SETF(FARRAY(_s, gc->state.vData.q0Info.offset)*_oow); \
;;;    } \
;;;    else { \
;;;      DA_SETF(_oow); \
;;;    } \
;;;    dataElem++; \
;;;    i = gc->tsuDataList[dataElem]; \
;;;  } \

$clip_setup_qow0:
    test      esi, 16                    ; STATE_REQUIRES_W_TMU0 ?
    jz        $clip_setup_stow0          ; nope 

    cmp       DWORD PTR [gc+q0Info_mode],0; does vertex have Q component ?
    je        $clip_setup_oow0           ; nope, not Q but W

    mov       eax, [gc+q0Info_offset]    ; offset of Q component of vertex
    add       fifo, 4                    ; fifoPtr += sizeof(FxFloat)

    add       dataElem, 4                ; dataElem++
    movd      mm2, [edx+eax]             ; 0 | q0 of vertex

    mov       eax,[gc+dataElem+tsuDataList]; pointer to next vertex component
    pfmul     mm2, mm0                   ; q0*oow

    movd      [fifo-4], mm2              ; PCI write transformed q0
    jmp       $clip_setup_stow0          ; continue with stow0

    nop                                  ; filler

$clip_setup_oow0:
    add       fifo, 4                    ; fifoPtr += sizeof(FxFloat) 
    add       dataElem, 4                ; dataElem++

    movd      [fifo-4], mm0              ; PCI write oow
    mov       eax,[gc+dataElem+tsuDataList]; pointer to next vertex component

;;;  if (gc->state.paramIndex & STATE_REQUIRES_ST_TMU0) { \
;;;    DA_SETF_SCALE_ADVANCE(_s,_oow*gc->state.tmu_config[0].s_scale); \
;;;    DA_SETF_SCALE_ADVANCE(_s,_oow*gc->state.tmu_config[0].t_scale); \
;;;  } \

$clip_setup_stow0:

    test      esi, 32                    ; STATE_REQUIRES_ST_TMU0 ?
    jz        $clip_setup_qow1           ; nope

    movq      mm7, [gc + tmu0_s_scale]   ; state.tmu_config[0].t_scale | state.tmu_config[0].s_scale
    add       fifo, 8                    ; fifoPtr += 2*sizeof(FxFloat)

    movd      mm2, [edx + eax]           ; param1
    mov       eax,[gc+dataElem+tsuDataList+4];pointer to next vertex component

    pfmul     mm7, mm0                   ; oow*tmu0_t_scale | oow*tmu0_s_scale
    add       dataElem, 8                ; dataElem += 2

    movd      mm3, [edx + eax]           ; param2
    punpckldq mm2, mm3                   ; param2 | param1

    pfmul     mm2, mm7                   ; param2*oow*tmu0_t_scale | param1*oow*tmu0_s_scale
    nop                                  ; filler

    movq      [fifo-8], mm2              ; PCI write param2*oow*tmu0_t_scale | param1*oow*tmu0_s_scale
    mov       eax, [gc+dataElem+tsuDataList]; pointer to next vertex component

;;;  if (gc->state.paramIndex & STATE_REQUIRES_W_TMU1) { \
;;;    if (gc->state.vData.q1Info.mode == GR_PARAM_ENABLE) { \
;;;      DA_SETF(FARRAY(_s, gc->state.vData.q1Info.offset)*_oow); \
;;;    } \
;;;    else { \
;;;      DA_SETF(_oow); \
;;;    } \
;;;    dataElem++; \
;;;    i = gc->tsuDataList[dataElem]; \
;;;  } \

$clip_setup_qow1:
    test      esi, 64                    ; STATE_REQUIRES_W_TMU1 ?
    jz        $clip_setup_stow1          ; nope

    cmp       DWORD PTR [gc+q1Info_mode],0; does vertex have Q component ?
    je        $clip_setup_oow1           ; nope, not Q but W

    mov       eax, [gc+q1Info_offset]    ; offset of Q component of vertex
    add       fifo, 4                    ; fifoPtr += sizeof(FxFloat)

    add       dataElem, 4                ; dataElem++
    movd      mm2, [edx + eax]           ; 0 | q1 of vertex

    mov       eax,[gc+dataElem+tsuDataList]; pointer to next vertex component
    pfmul     mm2, mm0                  ; q1*oow

    movd      [fifo-4], mm2              ; PCI write transformed q1
    jmp       $clip_setup_stow1          ; continue with stow1

$clip_setup_oow1:
    add       fifo, 4                    ; fifoPtr += sizeof(FxFloat) 
    add       dataElem, 4                ; dataElem++

    movd      [fifo-4], mm0              ; PCI write oow
    mov       eax,[gc+dataElem+tsuDataList]; pointer to next vertex component

;;;  if (gc->state.paramIndex & STATE_REQUIRES_ST_TMU1) { \
;;;    DA_SETF_SCALE_ADVANCE(_s,_oow*gc->state.tmu_config[1].s_scale); \
;;;    DA_SETF_SCALE_ADVANCE(_s,_oow*gc->state.tmu_config[1].t_scale); \
;;;  } \

$clip_setup_stow1:

    test      esi, 128                   ; STATE_REQUIRES_ST_TMU1 ?
    mov       vertexCount, [vertices]    ; get number of vertices

    movq      mm7, [gc + tmu1_s_scale]   ; state.tmu_config[1].t_scale | state.tmu_config[1].s_scale
    jz        $clip_setup_end            ; nope

    movd      mm2, [edx + eax]           ; param1
    add       fifo, 8                    ; fifoPtr += 2*sizeof(FxFloat)

    mov       eax,[gc+dataElem+tsuDataList+4]; pointer to next vertex component
    pfmul     mm7, mm0                   ; oow*state.tmu_config[1].t_scale | oow*state.tmu_config[1].s_scale

    movd      mm3, [edx + eax]           ; param2
    punpckldq mm2, mm3                   ; param2 | param1

    pfmul     mm2, mm7                   ; param2*oow*state.tmu_config[1].t_scale | param1*oow*state.tmu_config[1].s_scale
    movq      [fifo-8], mm2              ; PCI write param2*oow*state.tmu_config[1].t_scale | param1*oow*state.tmu_config[1].s_scale

$clip_setup_end:

    dec       vertexCount                ; vcount--
    jnz       $clip_for_begin            ; until 

$clip_for_end:

    mov       eax, [gc + fifoPtr]        ; old fifoPtr
    mov       ebp, [gc + fifoRoom]       ; old number of bytes available in fifo

    mov       vertexCount, [esp + _count]; remaining vertices before previous loop
    sub       eax, fifo                  ; old fifoPtr - new fifoPtr (fifo room used)

    mov       [gc + fifoPtr], fifo       ; save current fifoPtr 
    add       ebp, eax                   ; new number of bytes available in fifo

    sub       vertexCount, 15            ; remaining number of vertices to process
    mov       [gc + fifoRoom], ebp       ; save current number of bytes available in fifo

    mov       [esp + _count], vertexCount; remaining number of vertices to process 
    cmp       vertexCount, 0             ; any vertices left to process ?

    jg        $clip_coords_begin         ; loop if number of vertices to process >= 0

    femms                                ; no more MMX code; clear MMX/FPU state

$tris_done:
    pop       ebp                        ; restore frame pointer
    pop       ebx                        ; restore caller's register variable

    pop       esi                        ; restore caller's register variable
    pop       edi                        ; restore caller's register variable

    ret       12                         ; return, pop 3 DWORD parameters
__grDrawTriangles_3DNow@12 ENDP

_pktype   = 20
_type     = 24
_mode     = 28
_count    = 32
_pointers = 36

gc            TEXTEQU  <edi>             ; points to graphics context
fifo          TEXTEQU  <ecx>             ; points to next entry in fifo
dlp           TEXTEQU  <ebp>             ; points to dataList structure
vertexCount   TEXTEQU  <esi>             ; Current vertex counter in the packet
vertexPtr     TEXTEQU  <ebx>             ; Current vertex pointer (in deref mode)
vertex        TEXTEQU  <ebx>             ; Current vertex (in non-deref mode)
dlpStart      TEXTEQU  <edx>             ; Pointer to start of offset list

X TEXTEQU     <0>
Y TEXTEQU     <4>

                  ALIGN  32

    PUBLIC  __grDrawVertexList_3DNow_Window@20
__grDrawVertexList_3DNow_Window@20 PROC NEAR
; 132  : {

    mov       edx, DWORD PTR fs:[18h]    ; get thread local storage base pointer        
    push      edi                        ; save caller's register variable

    push      esi                        ; save caller's register variable
    mov       vertexCount, [esp+_count-8]; number of vertices in strip/fan

    push      ebp                        ; save frame pointer
    mov       ebp, [__GlideRoot + tlsOffset]; GC position relative to tls base    

    push      ebx                        ; save caller's register variable        
    mov       vertexPtr, [esp+_pointers] ; get current vertex pointer (deref mode)
                                         ; get current vertex (non-deref mode)
    
    mov       gc, [edx + ebp]            ; get current graphics context from tls
    test      vertexCount, vertexCount   ; number of vertices <= 0 ?

    nop                                  ; filler
    jle       strip_done                 ; yup, the strip/fan is done
  
;;;     vSize = gc->state.vData.vSize
;;;     if (stride == 0)
;;;       stride = gc->state.vData.vStride;

;; We can operate in one of two modes:
;;
;; 0. We are stepping through an array of vertices, in which case
;; the stridesize is equal to the size of the vertex data, and
;; always > 4, since vertex data must a least contain x,y (ie 8 bytes).
;; vertexPtr is pointing to the array of vertices.
;;
;; 1. We are stepping through an array of pointers to vertices
;; in which case the stride is 4 bytes and we need to dereference
;; the pointers to get at the vertex data. vertexPtr is pointing
;; to the array of pointers to vertices.

    mov       edx, [esp + _mode]         ; get mode (0 or 1)
    mov       eax, [gc + vertexSize]     ; size of vertex data in bytes

    test      edx, edx                   ; mode 0 (array of vertices) ?
    mov       edx, [gc + vertexStride]   ; get stride in DWORDs
    
    jnz       deref_mode                 ; nope, it's mode 1 (array of pointers to vertices)

    femms                                ; we'll use MMX; clear MMX/3DX state      

    shl       edx, 2                     ; stride in bytes
    mov       [strideinbytes], edx       ; save off stride (in bytes)

;;;     Draw the first (or possibly only) set.  This is necessary because
;;;     the packet is 3_BDDDDDD, and in the next set, the packet is 3_DDDDDD
;;;     We try to make tstrip code simple to read. We combine the original code
;;;     into a single loop by adding an extra packet type assignment at the end of the loop.
;;; 
;;;     if (gc->state.grCoordinateSpaceArgs.coordinate_space_mode == GR_WINDOW_COORDS) {
;;;       while (count > 0) {
;;;         FxI32 k, vcount = count >= 15 ? 15 : count;
;;;         GR_SET_EXPECTED_SIZE(vcount * vSize, 1);
;;;         TRI_STRIP_BEGIN(type, vcount, vSize, pktype);


win_coords_loop_ND:

    sub       vertexCount, 15            ; vertexCount >= 15 ? CF=0 : CF=1
    mov       ecx, [gc + vertexSize]     ; bytes of data for each vertex 

    sbb       eax, eax                   ; vertexCount >= 15 ? 00000000:ffffffff

    and       vertexCount, eax           ; vertexCount >= 15 ? 0 : vertexcount-15
    add       vertexCount, 15            ; vertexcount >= 15 ? 15 :vertexcount

    imul      ecx, vertexCount           ; total amount of vertex data we'll send

    mov       eax, [gc + fifoRoom]       ; fifo space available
    add       ecx, 4                     ; add header size ==> total packet size

    cmp       eax, ecx                   ; fifo space avail >= packet size ?
    jge       win_strip_begin_ND         ; yup, start writing strip data

    push      @Line                      ; line number inside this function
    push      0h                         ; pointer to function name = NULL

    push      ecx                        ; fifo space needed
    call      __grCommandTransportMakeRoom@12 ; note: updates fifoPtr

        align 32
win_strip_begin_ND:
;;;     Setup packet header
;;;
    mov       eax, vertexCount           ; number of vertices in strip/fan
    mov       edx, [esp + _type]         ; setup mode

    mov       fifo, [gc + fifoPtr]       ; get fifoPtr
    shl       edx, 22                    ; <27:22> = setup mode (kSetupStrip or kSetupFan)

    mov       ebp, [gc + cullStripHdr]   ; <2:0> = type
    shl       eax, 6                     ; <9:6> = vertex count (max 15)

    or        eax, edx                   ; setup mode and vertex count
    mov       edx, [esp + _pktype]       ; <5:3> = command (SSTCP_PKT3_BDDBDD, SSTCP_PKT3_BDDDDD, or SSTCP_PKT3_DDDDDD)

    or        eax, ebp                   ; setup mode, vertex count, and type
    nop                                  ; filler

    or        eax, edx                   ; setup mode, vertex count, type, and command
    lea       dlpStart, [gc+tsuDataList] ; pointer to start of offset list

    test      fifo, 4                    ; fifoPtr QWORD aligned ?
    jz        fifo_aligned_ND            ; yup

    mov       [fifo], eax                ; PCI write packet type
    add       fifo, 4                    ; fifo pointer now QWORD aligned

;;;     for (k = 0; k < vcount; k++) {
;;;       FxI32 i;
;;;       FxU32 dataElem;
;;;       float *vPtr;
;;;       vPtr = pointers;
;;;       if (mode)
;;;         vPtr = *(float **)vPtr
;;;       (float *)pointers += stride;
;;;       TRI_SETF(FARRAY(vPtr, 0));
;;;       dataElem = 0;
;;;       TRI_SETF(FARRAY(vPtr, 4));
;;;       i = gc->tsuDataList[dataElem];

win_vertex_loop_ND_WB0:                  ; nothing in "write buffer"

    mov       eax, [dlpStart]            ; get first offset from offset list
    lea       dlp, [dlpStart+4]          ; point to start of offset list

    movq      mm1, [vertex+X]            ; get vertex x,y
    add       fifo, 8                    ; fifoPtr += 2*sizeof(FxU32)

    nop                                  ; filler
    test      eax, eax                   ; if offset == 0, end of list

    movq      [fifo-8], mm1              ; PCI write x, y
    jz        win_datalist_end_ND_WB0    ; no more vertex data, nothing in "write buffer" 

;;;       while (i != GR_DLIST_END) {
;;;         TRI_SETF(FARRAY(vPtr, i));
;;;         dataElem++;
;;;         i = gc->tsuDataList[dataElem];
;;;       }

win_datalist_loop_ND_WB0:                ; nothing in "write buffer"

    movd      mm1, [vertex + eax]        ; get next parameter
    mov       eax, [dlp]                 ; get next offset from offset list

    test      eax, eax                   ; at end of offset list (offset == 0) ?
    jz        win_datalist_end_ND_WB1    ; exit, write buffer contains one DWORD

    movd      mm2, [vertex + eax]        ; get next parameter
    add       dlp, 8                     ; dlp++

    mov       eax, [dlp-4]               ; get next offset from offset list
    add       fifo, 8                    ; fifoPtr += 2*sizeof(FxU32)

    test      eax, eax                   ; at end of offset list (offset == 0) ?
    punpckldq mm1, mm2                   ; current param | previous param

    movq      [fifo-8], mm1              ; PCI write current param | previous param
    jnz       win_datalist_loop_ND_WB0   ; nope, copy next parameter

win_datalist_end_ND_WB0:

    mov       eax, [strideinbytes]       ; get offset to next vertex
    sub       vertexCount, 1             ; another vertex done. Any left?

    lea       vertex, [vertex + eax]     ; points to next vertex
    jnz       win_vertex_loop_ND_WB0     ; yup, output next vertex

win_vertex_end_ND_WB0:

;;;       TRI_END;
;;;     Prepare for the next packet (if the strip size is longer than 15)
;;;       GR_CHECK_SIZE();
;;;       count -= 15;
;;;       pktype = SSTCP_PKT3_DDDDDD;
;;;     }
  
    mov       ebp, 16                    ; pktype = SSTCP_PKT3_DDDDDD (strip continuation)
    mov       eax, [gc + fifoPtr]        ; old fifoPtr

    mov       [esp + _pktype], ebp       ; pktype = SSTCP_PKT3_DDDDDD (strip continuation)
    mov       ebp, [gc + fifoRoom]       ; old number of bytes available in fifo

    mov       vertexCount, [esp + _count]; remaining vertices before previous loop
    sub       eax, fifo                  ; old fifoPtr - new fifoPtr (fifo room used)

    mov       [gc + fifoPtr], fifo       ; save current fifoPtr 
    add       ebp, eax                   ; new number of bytes available in fifo

    nop                                  ; filler
    sub       vertexCount, 15            ; remaining number of vertices to process

    mov       [gc + fifoRoom], ebp       ; save current number of bytes available in fifo
    mov       [esp + _count], vertexCount; remaining number of vertices to process 

    test      vertexCount, vertexCount   ; any vertices left to process ?
    jg        win_coords_loop_ND         ; loop if number of vertices to process >= 0

    femms                                ; no more MMX code; clear MMX/FPU state

    pop       ebx                        ; restore caller's register variable
    pop       ebp                        ; restore frame pointer

    pop       esi                        ; restore caller's register variable
    pop       edi                        ; restire caller's register variable

    ret       20                         ; return, pop 5 DWORD parameters off stack

    ALIGN 32

fifo_aligned_ND:

    movd      mm1, eax                   ; move header into "write buffer"

win_vertex_loop_ND_WB1:                  ; one DWORD in "write buffer"

    movd      mm2, [vertex + X]          ; 0 | x of vertex
    add       fifo, 8                    ; fifoPtr += 2*sizeof(FxU32)

    lea       dlp, [dlpStart + 4]        ; point to start of offset list
    nop                                  ; filler

    punpckldq mm1, mm2                   ; packet header | x of vertex
    mov       eax, [dlp-4]               ; first offset in offset list

    movq      [fifo-8], mm1              ; PCI write packet header | x of vertex
    movd      mm1, [vertex + Y]          ; 0 | y of vertex

    cmp       eax, 0                     ; offset == 0 (list empty) ?
    jz        win_datalist_end_ND_WB1    ; yup, no more vertex data, one DWORD in "write buffer"

;;;       while (i != GR_DLIST_END) {
;;;         TRI_SETF(FARRAY(vPtr, i));
;;;         dataElem++;
;;;         i = gc->tsuDataList[dataElem];
;;;       }

win_datalist_loop_ND_WB1:                ; one DWORD in "write buffer" 

    movd      mm2, [vertex + eax]        ; get next parameter
    add       fifo, 8                    ; fifoPtr += 2*sizeof(FxU32)

    mov       eax, [dlp]                 ; get next offset from offset list
    add       dlp, 8                     ; dlp += 2

    punpckldq mm1, mm2                   ; current param | previous param
    cmp       eax, 0                     ; at end of offset list (offset == 0) ?

    movq      [fifo-8], mm1              ; PCI write current param | previous param
    jz        win_datalist_end_ND_WB0    ; yes, exit, "write buffer" empty

    movd      mm1, [vertex+eax]          ; get next parameter
    mov       eax, [dlp-4]               ; get next offset from offset list

    test      eax, eax                   ; at end of offset list (offset == 0) ?
    jnz       win_datalist_loop_ND_WB1   ; nope, copy next parameter

win_datalist_end_ND_WB1:

    mov       eax, [strideinbytes]       ; get offset to next vertex
    sub       vertexCount, 1             ; another vertex done. Any left?

    lea       vertex, [vertex + eax]     ; points to next vertex
    jnz       win_vertex_loop_ND_WB1     ; yup, output next vertex

win_vertex_end_ND_WB1:

    movd      [fifo], mm1                ; flush "write buffer"
    add       fifo, 4                    ; fifoPtr += sizeof(FxU32)

;;;       TRI_END;
;;;     Prepare for the next packet (if the strip size is longer than 15)
;;;       GR_CHECK_SIZE();
;;;       count -= 15;
;;;       pktype = SSTCP_PKT3_DDDDDD;
;;;     }

    mov       ebp, 16                    ; pktype = SSTCP_PKT3_DDDDDD (strip continuation)
    mov       eax, [gc + fifoPtr]        ; old fifoPtr

    mov       [esp+_pktype], ebp         ; pktype = SSTCP_PKT3_DDDDDD (strip continuation)
    mov       ebp, [gc + fifoRoom]       ; old number of bytes available in fifo

    mov       vertexCount, [esp + _count]; remaining vertices before previous loop
    sub       eax, fifo                  ; old fifoPtr - new fifoPtr (fifo room used)

    mov       [gc + fifoPtr], fifo       ; save current fifoPtr 
    add       ebp, eax                   ; new number of bytes available in fifo

    mov       [gc + fifoRoom], ebp       ; save current number of bytes available in fifo
    sub       vertexCount, 15            ; remaining number of vertices to process

    mov       [esp + _count], vertexCount; remaining number of vertices to process 
    test      vertexCount, vertexCount   ; any vertices left to process ?

    nop                                  ; filler
    jg        win_coords_loop_ND         ; loop if number of vertices to process >= 0

    femms                                ; no more MMX code; clear MMX/FPU state

    pop       ebx                        ; restore caller's register variable
    pop       ebp                        ; restore frame pointer

    pop       esi                        ; restore caller's register variable
    pop       edi                        ; restire caller's register variable

    ret       20                         ; return, pop 5 DWORD parameters off stack

    ALIGN 32
    
deref_mode:

    femms                                ; we'll use MMX; clear FPU/MMX state

    prefetch  [vertexPtr]                ; pre-load first group of pointers

win_coords_loop_D:

    sub       vertexCount, 15            ; vertexCount >= 15 ? CF=0 : CF=1
    mov       ecx, [gc + vertexSize]     ; bytes of data for each vertex 

    sbb       eax, eax                   ; vertexCount >= 15 ? 00000000:ffffffff

    and       vertexCount, eax           ; vertexCount >= 15 ? 0 : vertexcount-15
    add       vertexCount, 15            ; vertexcount >= 15 ? 15 :vertexcount

    imul      ecx, vertexCount           ; total amount of vertex data we'll send

    mov       eax, [gc + fifoRoom]       ; fifo space available
    add       ecx, 4                     ; add header size ==> total packet size

    cmp       eax, ecx                   ; fifo space avail >= packet size ?
    jge       win_strip_begin_D          ; yup, start writing strip data

    push      @Line                      ; line number inside this function
    push      0h                         ; pointer to function name = NULL

    push      ecx                        ; fifo space needed
    call      __grCommandTransportMakeRoom@12 ; note: updates fifoPtr

        align 32
win_strip_begin_D:
;;;     Setup packet header
;;;
    mov       eax, vertexCount           ; number of vertices in strip/fan
    mov       edx, [esp + _type]         ; setup mode

    mov       fifo, [gc + fifoPtr]       ; get fifoPtr
    shl       edx, 22                    ; <27:22> = setup mode (kSetupStrip or kSetupFan)

    mov       ebp, [gc + cullStripHdr]   ; <2:0> = type
    shl       eax, 6                     ; <9:6> = vertex count (max 15)

    or        eax, edx                   ; setup mode and vertex count
    mov       edx, [esp + _pktype]       ; <5:3> = command (SSTCP_PKT3_BDDBDD, SSTCP_PKT3_BDDDDD, or SSTCP_PKT3_DDDDDD)

    or        eax, ebp                   ; setup mode, vertex count, and type
    mov       ebp, 4                     ; test bit 2

    or        eax, edx                   ; setup mode, vertex count, type, and command
    lea       dlpStart, [gc+tsuDataList] ; pointer to start of offset list

    test      fifo, ebp                  ; fifoPtr QWORD aligned ?
    jz        fifo_aligned_D             ; yup

    mov       [fifo], eax                ; PCI write packet type
    add       fifo, 4                    ; fifo pointer now QWORD aligned

;;;     for (k = 0; k < vcount; k++) {
;;;       FxI32 i;
;;;       FxU32 dataElem;
;;;       float *vPtr;
;;;       vPtr = pointers;
;;;       if (mode)
;;;         vPtr = *(float **)vPtr
;;;       (float *)pointers += stride;
;;;       TRI_SETF(FARRAY(vPtr, 0));
;;;       dataElem = 0;
;;;       TRI_SETF(FARRAY(vPtr, 4));
;;;       i = gc->tsuDataList[dataElem];


win_vertex_loop_D_WB0:                   ; nothing in "write buffer"

    mov       edx, [vertexPtr]           ; dereference pointer, edx points to vertex
    add       vertexPtr, 4               ; next pointer

    lea       dlp, [gc + tsuDataList]    ; get pointer to offset list; dlp ++
    add       dlp, 4                     ; dlp ++

    movq      mm1, [edx + X]             ; get vertex x,y
    add       fifo, 8                    ; fifo += 2

    mov       eax, [dlp - 4]             ; get first offset from offset list
    movq      [fifo-8], mm1              ; PCI write x, y

    test      eax, eax                   ; if offset == 0, end of offset list
    je        win_datalist_end_D_WB0     ; no more vertex data, nothing in "write buffer" 

;;;       while (i != GR_DLIST_END) {
;;;         TRI_SETF(FARRAY(vPtr, i));
;;;         dataElem++;
;;;         i = gc->tsuDataList[dataElem];
;;;       }

win_datalist_loop_D_WB0:                 ; nothing in "write buffer"

    movd      mm1, [edx + eax]           ; get next parameter
    mov       eax, [dlp]                 ; get next offset from offset list

    test      eax, eax                   ; at end of offset list (offset == 0) ?
    jz        win_datalist_end_D_WB1     ; exit, write buffer contains one DWORD

    add       dlp, 8                     ; dlp++
    movd      mm2, [edx + eax]           ; get next parameter

    mov       eax, [dlp-4]               ; get next offset from offset list
    add       fifo, 8                    ; fifoPtr += 2*sizeof(FxU32)

    punpckldq mm1, mm2                   ; current param | previous param
    test      eax, eax                   ; at end of offset list (offset == 0) ?

    movq      [fifo-8], mm1              ; PCI write current param | previous param
    jnz       win_datalist_loop_D_WB0    ; nope, copy next parameter

win_datalist_end_D_WB0:

    dec       vertexCount                ; another vertex done. Any left?
    jnz       win_vertex_loop_D_WB0      ; yup, output next vertex

win_vertex_end_D_WB0:

;;;       TRI_END;
;;;     Prepare for the next packet (if the strip size is longer than 15)
;;;       GR_CHECK_SIZE();
;;;       count -= 15;
;;;       pktype = SSTCP_PKT3_DDDDDD;
;;;     }

    mov       ebp, 16                    ; pktype = SSTCP_PKT3_DDDDDD (strip continuation)
    mov       eax, [gc + fifoPtr]        ; old fifoPtr

    mov       [esp + _pktype], ebp       ; pktype = SSTCP_PKT3_DDDDDD (strip continuation)
    mov       ebp, [gc + fifoRoom]       ; old number of bytes available in fifo

    mov       vertexCount, [esp + _count]; remaining vertices before previous loop
    sub       eax, fifo                  ; old fifoPtr - new fifoPtr (fifo room used)

    add       ebp, eax                   ; new number of bytes available in fifo
    mov       [gc + fifoPtr], fifo       ; save current fifoPtr 

    sub       vertexCount, 15            ; remaining number of vertices to process
    mov       [gc + fifoRoom], ebp       ; save current number of bytes available in fifo

    mov       [esp + _count], vertexCount; remaining number of vertices to process 
    test      vertexCount, vertexCount   ; any vertices left to process ?

    nop                                  ; filler
    jg        win_coords_loop_D          ; loop if number of vertices to process >= 0

    femms                                ; no more MMX code; clear MMX/FPU state

    pop       ebx                        ; restore caller's register variable
    pop       ebp                        ; restore frame pointer

    pop       esi                        ; restore caller's register variable
    pop       edi                        ; restire caller's register variable

    ret       20                         ; return, pop 5 DWORD parameters off stack

    ALIGN 32

fifo_aligned_D:

    movd      mm1, eax                   ; move header into "write buffer"

win_vertex_loop_D_WB1:                   ; one DWORD in "write buffer"

    mov       edx, [vertexPtr]           ; dereference pointer, edx points to vertex
    add       vertexPtr, 4               ; next pointer

    add       fifo, 8                    ; fifoPtr += 2*sizeof(FxU32)
    lea       dlp, [gc + tsuDataList]    ; get pointer to start of offset list

    movd      mm2, [edx + X]             ; 0 | x of vertex
    add       dlp, 4                     ; dlp++
 
    mov       eax, [dlp-4]               ; first offset in offset list
    punpckldq mm1, mm2                   ; packet header | x of vertex

    movq      [fifo-8], mm1              ; PCI write packet header | x of vertex
    movd      mm1, [edx + Y]             ; 0 | y of vertex

    test      eax, eax                   ; offset == 0 (list empty) ?
    je        win_datalist_end_D_WB1     ; yup, no more vertex data, one DWORD in "write buffer"

;;;       while (i != GR_DLIST_END) {
;;;         TRI_SETF(FARRAY(vPtr, i));
;;;         dataElem++;
;;;         i = gc->tsuDataList[dataElem];
;;;       }

win_datalist_loop_D_WB1:                ; one DWORD in "write buffer" = MM1

    movd      mm2, [edx + eax]          ; get next parameter
    add       fifo, 8                   ; fifoPtr += 2*sizeof(FxU32)

    mov       eax, [dlp]                ; get next offset from offset list
    add       dlp, 8                    ; dlp += 2

    punpckldq mm1, mm2                  ; current param | previous param
    cmp       eax, 0                    ; at end of offset list (offset == 0) ?

    movq      [fifo-8], mm1             ; PCI write current param | previous param
    jz        win_datalist_end_D_WB0    ; yes, exit, "write buffer" empty

    movd      mm1, [edx + eax]          ; get next parameter
    mov       eax, [dlp-4]              ; get next offset from offset list

    cmp       eax, 0                    ; at end of offset list (offset == 0) ?
    jnz       win_datalist_loop_D_WB1   ; nope, copy next parameter

win_datalist_end_D_WB1:

    dec       vertexCount               ; another vertex done. Any left?
    jnz       win_vertex_loop_D_WB1     ; yup, output next vertex

win_vertex_end_D_WB1:

    movd      [fifo], mm1               ; flush "write buffer"
    add       fifo, 4                   ; fifoPtr++

;;;       TRI_END;
;;;     Prepare for the next packet (if the strip size is longer than 15)
;;;       GR_CHECK_SIZE();
;;;       count -= 15;
;;;       pktype = SSTCP_PKT3_DDDDDD;
;;;     }

    mov       ebp, 16                    ; pktype = SSTCP_PKT3_DDDDDD (strip continuation)
    mov       eax, [gc + fifoPtr]        ; old fifoPtr

    mov       [esp+_pktype], ebp         ; pktype = SSTCP_PKT3_DDDDDD (strip continuation)
    mov       ebp, [gc + fifoRoom]       ; old number of bytes available in fifo

    mov       vertexCount, [esp + _count]; remaining vertices before previous loop
    nop                                  ; filler

    sub       eax, fifo                  ; old fifoPtr - new fifoPtr (fifo room used)
    mov       [gc + fifoPtr], fifo       ; save current fifoPtr 

    sub       vertexCount, 15            ; remaining number of vertices to process
    add       ebp, eax                   ; new number of bytes available in fifo

    mov       [gc + fifoRoom], ebp       ; save current number of bytes available in fifo
    cmp       vertexCount, 0             ; any vertices left to process ?

    mov       [esp + _count], vertexCount; remaining number of vertices to process 
    jg        win_coords_loop_D          ; loop if number of vertices to process >= 0

    femms                                ; no more MMX code; clear MMX/FPU state

strip_done: 
    pop       ebx                        ; restore caller's register variable
    pop       ebp                        ; restore frame pointer

    pop       esi                        ; restore caller's register variable
    pop       edi                        ; restire caller's register variable

    ret       20                         ; return, pop 5 DWORD parameters off stack

__grDrawVertexList_3DNow_Window@20 ENDP


   

    PUBLIC  __grDrawVertexList_3DNow_Clip@20
__grDrawVertexList_3DNow_Clip@20 PROC NEAR
; 132  : {

    ALIGN 32

    mov       edx, DWORD PTR fs:[18h]    ; get thread local storage base pointer        
    push      edi                        ; save caller's register variable

    push      esi                        ; save caller's register variable
    mov       vertexCount, [esp+_count-8]; number of vertices in strip/fan

    push      ebp                        ; save frame pointer
    mov       ebp, [__GlideRoot + tlsOffset]; GC position relative to tls base    

    push      ebx                        ; save caller's register variable        
    mov       vertexPtr, [esp+_pointers] ; get current vertex pointer (deref mode)
                                         ; get current vertex (non-deref mode)
    
    mov       gc, [edx + ebp]            ; get current graphics context from tls
    test      vertexCount, vertexCount   ; number of vertices <= 0 ?

    jle       strip_done                 ; yup, the strip/fan is done
  
;;;     vSize = gc->state.vData.vSize
;;;     if (stride == 0)
;;;       stride = gc->state.vData.vStride;

;; We can operate in one of two modes:
;;
;; 0. We are stepping through an array of vertices, in which case
;; the stridesize is equal to the size of the vertex data, and
;; always > 4, since vertex data must a least contain x,y (ie 8 bytes).
;; vertexPtr is pointing to the array of vertices.
;;
;; 1. We are stepping through an array of pointers to vertices
;; in which case the stride is 4 bytes and we need to dereference
;; the pointers to get at the vertex data. vertexPtr is pointing
;; to the array of pointers to vertices.

    mov       edx, [esp + _mode]         ; get mode (0 or 1)
    mov       eax, [gc + vertexSize]     ; size of vertex data in bytes

    test      edx, edx                   ; mode 0 (array of vertices) ?
    mov       edx, [gc + vertexStride]   ; get stride in DWORDs

    movd      mm6, [__GlideRoot+pool_f255]; GlideRoot.pool.f255     
    mov       [strideinbytes], 4         ; array of pointers    
        
    jnz       clip_coords_begin          ; nope, it's mode 1

clip_coordinates_ND:

    shl       edx, 2                     ; stride in bytes
    mov       [strideinbytes], edx       ; save off stride (in bytes)

    align   32
clip_coords_begin:

dataElem      textequ <ebp>              ; number of vertex components processed    

;;;   {
;;;     float oow;
;;;       while (count > 0) {
;;;       FxI32 k, vcount = count >= 15 ? 15 : count;

    sub       vertexCount, 15            ; vertexCount >= 15 ? CF=0 : CF=1
    mov       ecx, [gc + vertexSize]     ; bytes of data for each vertex 

    sbb       eax, eax                   ; vertexCount >= 15 ? 00000000:ffffffff
    and       vertexCount, eax           ; vertexCount >= 15 ? 0 : vertexcount-15

    mov       eax, [gc+fifoRoom]         ; fifo space available
    add       vertexCount, 15            ; vertexcount >= 15 ? 15 :vertexcount

    imul      ecx, vertexCount           ; total amount of vertex data we'll send

    add       ecx, 4                     ; add header size ==> total packet size
    nop                                  ; filler

    cmp       eax, ecx                   ; fifo space avail >= packet size ?
    jge       clip_strip_begin           ; yup, start writing strip data

    push      @Line                      ; line number inside this function
    push      0h                         ; pointer to function name = NULL

    push      ecx                        ; fifo space needed
    call      __grCommandTransportMakeRoom@12 ; note: updates fifoPtr

        align 32
clip_strip_begin:
;;;     TRI_STRIP_BEGIN(type, vcount, vSize, pktype)

    mov       edx, [esp + _type]         ; setup mode
    mov       eax, vertexCount           ; number of vertices in strip/fan

    mov       fifo, [gc + fifoPtr]       ; get fifoPtr
    shl       eax, 6                     ; <9:6> = vertex count (max 15)

    mov       ebp, [gc + cullStripHdr]   ; <2:0> = type
    shl       edx, 22                    ; <27:22> = setup mode (kSetupStrip or kSetupFan)

    or        eax, edx                   ; setup mode and vertex count
    mov       edx, [esp + _pktype]       ; <5:3> = command (SSTCP_PKT3_BDDBDD, SSTCP_PKT3_BDDDDD, or SSTCP_PKT3_DDDDDD)

    or        eax, ebp                   ; setup mode, vertex count, and type
    add       fifo, 4                    ; fifoPtr += sizeof(FxU32)

    or        eax, edx                   ; setup mode, vertex count, type, and command
    mov       [fifo-4], eax              ; PCI write header

;;;     for (k = 0; k < vcount; k++) {
;;;       float *vPtr
;;;       vPtr = pointers
  
clip_for_begin:

;;;       if (mode)
;;;         vPtr = *(float **)vPtr

    mov       edx, vertexPtr             ; vertex = vertexPtr (assume no-deref mode)
    mov       eax, [esp+_mode]           ; mode 0 = no deref, mode 1 = deref

    mov       [vertices], vertexCount    ; save numnber of vertices
    test      eax, eax                   ; deref mode ?

    mov       eax, [gc+wInfo_offset]     ; get offset of W into vertex struct
    jz        clip_noderef               ; yup, no-deref mode

    mov       edx, [vertexPtr]           ; vertex = *vertexPtr
    lea       esp, [esp]                 ; filler

clip_noderef:

;;;       oow = 1.0f / FARRAY(vPtr, gc->state.vData.wInfo.offset)

    movd      mm0, [edx+eax]             ; 0 | W of current vertex
    pfrcp     mm1, mm0                   ; 0 | 1/W approx

    mov       ebp, [strideinbytes]       ; offset to next vertex/vertexPtr
    movq      mm2, [edx]                 ; y | x of current vertex

    pfrcpit1  mm0, mm1                   ; 0 | 1/W refine
    movq      mm3, [gc+vp_hwidth]        ; gc->state.Viewport.hheight | gc->state.Viewport.hwidth

    movq      mm4, [gc+vp_ox]            ; gc->state.Viewport.oy | gc->state.Viewport.ox
    add       vertexPtr, ebp             ; point to next vertex/VertexPtr

    pfrcpit2  mm0, mm1                   ; oow = 1.0f / FARRAY(vPtr, gc->state.vData.wInfo.offset
    mov       esi, [gc+paramIndex]       ; gc->state.paramIndex

;;;       /* x, y */
;;;       TRI_SETF(FARRAY(vPtr, 0)
;;;         *oow*gc->state.Viewport.hwidth + gc->state.Viewport.ox)
;;;       TRI_SETF(FARRAY(vPtr, 4)
;;;         *oow*gc->state.Viewport.hheight + gc->state.Viewport.oy)

    pfmul     mm2, mm3                   ; TRI_SETF(FARRAY(vPtr,0)*state.Viewport.hheight | TRI_SETF(FARRAY(vPtr,4)*state.Viewport.hwidth
    xor       dataElem, dataElem         ; dataElem = 0

    add       fifo, 8                    ; fifoPtr += 2*sizeof(FxFloat)
    punpckldq mm0, mm0                   ; oow | oow

    pfmul     mm2, mm0                   ; TRI_SETF(FARRAY(vPtr, 4)*oow*gc->state.Viewport.height | TRI_SETF(FARRAY(vPtr, 0)*oow*gc->state.Viewport.hwidth
    pfadd     mm2, mm4                   ; TRI_SETF(FARRAY(vPtr, 4)*oow*gc->state.Viewport.hheight + gc->state.Viewport.oy) |

    test      esi, 3                     ; STATE_REQUIRES_IT_DRGB | STATE_REQUIRES_IT_ALPHA ?
    mov       eax, [gc+tsuDataList]      ; first entry from offset list

;;;       (float *)pointers += stride
;;;       TRI_VP_SETFS(vPtr, oow);

    movq      [fifo-8], mm2              ; PCI write transformed x, y
    jz        clip_setup_ooz             ; nope, no color at all needed
  
    cmp       DWORD PTR [gc+colorType], 0; gc->state.vData.colorType == GR_FLOAT ?
    jne       clip_setup_pargb           ; nope, packed ARGB format
  
    test      esi, 1                     ; STATE_REQUIRES_IT_DRGB ?
    jz        clip_setup_a               ; no, but definitely A

    movd      mm2, [edx + eax]           ; 0 | r
    mov       eax, [gc+tsuDataList+4]    ; offset of g part of vertex data

    pfmul     mm2, mm6                   ; 0 | r * 255.0f
    movd      mm3, [edx + eax]           ; 0 | g

    mov       eax, [gc+tsuDataList+8]    ; offset of b part of vertex data
    movd      [fifo], mm2                ; PCI write r*255

    pfmul     mm3, mm6                   ; 0 | g * 255.0f
    movd      mm2, [edx + eax]           ; 0 | b

    movd      [fifo+4], mm3              ; PCI write g*255
    mov       dataElem, 12               ; dataElem = 3

    pfmul     mm2, mm6                   ; 0 | b * 255.0f
    mov       eax, [gc+tsuDataList+12]   ; offset of A part of vertex data

    test      esi, 2                     ; STATE_REQUIRES_IT_ALPHA ?
    lea       fifo, [fifo+12]            ; fifoPtr += 3*sizeof(FxFloat)

    movd      [fifo-4], mm2              ; PCI write b*255
    jz        clip_setup_ooz             ; nope, no alpha, proceeed with ooz

clip_setup_a:
    movd      mm2, [eax+edx]             ; 0 | a
    add       fifo, 4                    ; fifoPtr += sizeof(FxFloat)

    mov       esp, esp                   ; filler
    add       dataElem, 4                ; dataElem++ 

    pfmul     mm2, mm6                   ; 0 | a * 255.0f
    mov       eax, [gc+dataElem+tsuDataList]; offset of next part of vertex data

    movd      [fifo-4], mm2              ; PCI write a*255
    jmp       clip_setup_ooz             ; check whether we need to push out z

    ALIGN     32

clip_setup_pargb:
    movd      mm2, [eax+edx]             ; get packed ARGB data
    add       fifo, 4                    ; fifoPtr += sizeof(FxU32)

    mov       dataElem, 4                ; dataElem = 1 (namely pargb)
    mov       eax, [gc+tsuDataList+4]    ; offset of next part of vertex data

    movd      [fifo-4], mm2              ; PCI write packed ARGB

clip_setup_ooz:
  
    test      esi, 4                     ; STATE_REQUIRES_OOZ ?
    jz        clip_setup_qow             ; nope

    test      DWORD PTR[gc+fbi_fbzMode],200000h ; gc->state.fbi_config.fbzMode & SST_DEPTH_FLOAT_SEL != 0 ?
    je        clip_setup_ooz_nofog       ; nope

    cmp       DWORD PTR[gc+qInfo_mode], 0; gc->state.vData.qInfo.mode == GR_PARAM_ENABLE ?
    jz        clip_setup_fog_oow         ; nope

    mov       eax, [gc + qInfo_offset]   ; offset of Q component of vertex
    add       fifo, 4                    ; fifoPtr += sizeof(FxFloat)

    add       dataElem, 4                ; dataElem++
    movd      mm2, [edx + eax]           ; 0 | q of vertex

    mov       eax,[gc+dataElem+tsuDataList]; pointer to next vertex component
    pfmul     mm2, mm0                   ; 0 | q*oow

    movd      [fifo-4], mm2              ; PCI write transformed Q
    jmp       clip_setup_qow             ; check whether we need to write Q or W

clip_setup_fog_oow:

    add       fifo, 4                    ; fifoPtr += sizeof(FxFloat) 
    add       dataElem, 4                ; dataElem++

    movd      mm3, [gc + depth_range]    ;
    pfmul     mm3, mm0                   ; 

    movd      mm4, [gc + depth_range]    ; depth range
    pfsub     mm4, mm3                   ; 

    movd      [fifo-4], mm4              ; PCI write oow
    mov       eax,[gc+dataElem+tsuDataList]; pointer to next vertex component

    jmp       clip_setup_qow             ; check whether we need to write Q or W

clip_setup_ooz_nofog:

    movd      mm2, [eax + edx]           ; 0 | z component of vertex
    add       fifo, 4                    ; fifoPtr += sizeof(FxFloat)

    add       dataElem, 4                ; dataElem += 1
    movd      mm3, [gc + vp_hdepth]      ; 0 | gc->state.Viewport.hdepth

    pfmul     mm2, mm0                   ; TRI_SETF(FARRAY(_s, i)*_oow
    movd      mm4, [gc + vp_oz]          ; 0 | gc->state.Viewport.oz

    pfmul     mm2, mm3                   ; 0 | TRI_SETF(FARRAY(_s, i)*_oow*gc->state.Viewport.hdepth
    mov       eax, [gc+dataElem+tsuDataList]; offset of next vertex component

    pfadd     mm2, mm4                   ; 0 | TRI_SETF(FARRAY(_s, i)*_oow*gc->state.Viewport.hdepth+gc->state.Viewport.oz
    movd      [fifo-4], mm2              ; PCI write transformed Z

clip_setup_qow:
    test      esi, 8                     ; STATE_REQUIRES_OOW_FBI ?
    jz        clip_setup_qow0            ; nope

    cmp       DWORD PTR[gc+fogInfo_mode],0; gc->state.vData.fogInfo.mode == GR_PARAM_ENABLE ?
    jz        clip_setup_oow_nofog       ; nope, no fog

    mov       eax, [gc + fogInfo_offset] ; offset of fogInfo component of vertex
    add       fifo, 4                    ; fifoPtr += sizeof(FxFloat)

    add       dataElem, 4                ; dataElem++
    movd      mm2, [edx + eax]           ; 0 | fogInfo of vertex

    mov       eax,[gc+dataElem+tsuDataList]; pointer to next vertex component
    pfmul     mm2, mm0                   ; fogInfo*oow

    movd      [fifo-4], mm2              ; PCI write transformed Q
    jmp       clip_setup_qow0            ; continue with q0

clip_setup_oow_nofog:

    cmp       DWORD PTR [gc+qInfo_mode],0; does vertex have Q component ?
    je        clip_setup_oow             ; nope, not Q but W

    add       fifo, 4                    ; fifoPtr += sizeof(FxFloat)
    mov       eax, [gc+qInfo_offset]     ; offset of Q component of vertex

    add       dataElem, 4                ; dataElem++
    movd      mm2, [edx+eax]             ; 0 | q of vertex

    mov       eax,[gc+dataElem+tsuDataList]; pointer to next vertex component
    pfmul     mm2, mm0                   ; q*oow

    movd      [fifo-4], mm2              ; PCI write transformed Q
    jmp       clip_setup_qow0            ; continue with q0

    ALIGN     32

clip_setup_oow:
    add       fifo, 4                    ; fifoPtr += sizeof(FxFloat) 
    add       dataElem, 4                ; dataElem++

    movd      [fifo-4], mm0              ; PCI write oow
    mov       eax,[gc+dataElem+tsuDataList]; pointer to next vertex component

clip_setup_qow0:
    test      esi, 16                    ; STATE_REQUIRES_W_TMU0 ?
    jz        clip_setup_stow0           ; nope 

    cmp       DWORD PTR [gc+q0Info_mode],0; does vertex have Q component ?
    je        clip_setup_oow0            ; nope, not Q but W

    mov       eax, [gc+q0Info_offset]    ; offset of Q component of vertex
    add       fifo, 4                    ; fifoPtr += sizeof(FxFloat)

    add       dataElem, 4                ; dataElem++
    movd      mm2, [edx+eax]             ; 0 | q0 of vertex

    mov       eax,[gc+dataElem+tsuDataList]; pointer to next vertex component
    pfmul     mm2, mm0                   ; q0*oow

    movd      [fifo-4], mm2              ; PCI write transformed q0
    jmp       clip_setup_stow0           ; continue with stow0

    ALIGN     32

clip_setup_oow0:
    add       fifo, 4                    ; fifoPtr += sizeof(FxFloat) 
    add       dataElem, 4                ; dataElem++

    movd      [fifo-4], mm0              ; PCI write oow
    mov       eax,[gc+dataElem+tsuDataList]; pointer to next vertex component

clip_setup_stow0:

    test      esi, 32                    ; STATE_REQUIRES_ST_TMU0 ?
    jz        clip_setup_qow1            ; nope

    movq      mm7, [gc + tmu0_s_scale]   ; state.tmu_config[0].t_scale | state.tmu_config[0].s_scale
    add       fifo, 8                    ; fifoPtr += 2*sizeof(FxFloat)

    movd      mm2, [edx+eax]             ; param1
    mov       eax,[gc+dataElem+tsuDataList+4];pointer to next vertex component

    pfmul     mm7, mm0                   ; oow*tmu0_t_scale | oow*tmu0_s_scale
    add       dataElem, 8                ; dataElem += 2

    movd      mm3, [edx+eax]             ; param2
    punpckldq mm2, mm3                   ; param2 | param1

    pfmul     mm2, mm7                   ; param2*oow*tmu0_t_scale | param1*oow*tmu0_s_scale

    movq      [fifo-8], mm2              ; PCI write param2*oow*tmu0_t_scale | param1*oow*tmu0_s_scale 
    mov       eax, [gc+dataElem+tsuDataList]; pointer to next vertex component

clip_setup_qow1:
    test      esi, 64                    ; STATE_REQUIRES_W_TMU1 ?
    jz        clip_setup_stow1           ; nope

    cmp       DWORD PTR [gc+q1Info_mode],0; does vertex have Q component ?
    je        clip_setup_oow1            ; nope, not Q but W

    mov       eax, [gc+q1Info_offset]    ; offset of Q component of vertex
    add       fifo, 4                    ; fifoPtr += sizeof(FxFloat)

    add       dataElem, 4                ; dataElem++
    movd      mm2, [edx+eax]             ; 0 | q1 of vertex

    mov       eax,[gc+dataElem+tsuDataList]; pointer to next vertex component
    pfmul     mm2, mm0                   ; q1*oow

    movd      [fifo-4], mm2              ; PCI write transformed q1
    jmp       clip_setup_stow1           ; continue with stow1

    ALIGN     32

clip_setup_oow1:
    add       fifo, 4                    ; fifoPtr += sizeof(FxFloat) 
    add       dataElem, 4                ; dataElem++

    movd      [fifo-4], mm0              ; PCI write oow
    mov       eax,[gc+dataElem+tsuDataList]; pointer to next vertex component

clip_setup_stow1:

    test      esi, 128                   ; STATE_REQUIRES_ST_TMU1 ?
    mov       vertexCount, [vertices]    ; get number of vertices

    movq      mm7, [gc + tmu1_s_scale]   ; state.tmu_config[1].t_scale | state.tmu_config[1].s_scale
    jz        clip_setup_end             ; nope

    movd      mm2, [edx+eax]             ; param1
    add       fifo, 8                    ; fifoPtr += 2*sizeof(FxFloat)

    mov       eax,[gc+dataElem+tsuDataList+4]; pointer to next vertex component
    pfmul     mm7, mm0                   ; oow*state.tmu_config[1].t_scale | oow*state.tmu_config[1].s_scale

    movd      mm3, [edx+eax]             ; param2
    punpckldq mm2, mm3                   ; param2 | param1

    pfmul     mm2, mm7                   ; param2*oow*state.tmu_config[1].t_scale | param1*oow*state.tmu_config[1].s_scale
    movq      [fifo-8], mm2              ; PCI write param2*oow*state.tmu_config[1].t_scale | param1*oow*state.tmu_config[1].s_scale

clip_setup_end:

; 206  :       for (k = 0; k < vcount; k++) {

    dec       vertexCount                ; vcount--
    jnz       clip_for_begin             ; until 
clip_for_end:

; 221  :       }
; 222  :       TRI_END;

    mov       eax, [gc + fifoPtr]        ; old fifoPtr
    mov       esp, esp                   ; filler

    nop                                  ; filler
    mov       ebp, [gc + fifoRoom]       ; old number of bytes available in fifo

    mov       vertexCount, [esp + _count]; remaining vertices before previous loop
    sub       eax, fifo                  ; old fifoPtr - new fifoPtr (fifo room used)

    mov       [gc + fifoPtr], fifo       ; save current fifoPtr 
    add       ebp, eax                   ; new number of bytes available in fifo

    sub       vertexCount, 15            ; remaining number of vertices to process
    mov       [gc + fifoRoom], ebp       ; save current number of bytes available in fifo

    mov       [esp + _count], vertexCount; remaining number of vertices to process 
    cmp       vertexCount, 0             ; any vertices left to process ?

    mov       DWORD PTR [esp+_pktype], 16; pktype = SSTCP_PKT3_DDDDDD (strip continuation)
    jg        clip_coords_begin          ; loop if number of vertices to process >= 0

    femms                                ; no more MMX code; clear MMX/FPU state    

strip_done:
;;;    }
;;;  #undef FN_NAME
;;;  } /* _grDrawVertexList */

    pop       ebx                        ; restore caller's register variable
    pop       ebp                        ; restore frame pointer 

    pop       esi                        ; restore caller's register variable
    pop       edi                        ; restore caller's register variable

    ret       20                         ; return, pop 5 DWORD parameters off stack

__grDrawVertexList_3DNow_Clip@20 ENDP

_TEXT    ENDS

;;--------------------------------------------------------------------------
;; end AMD3D version
;;--------------------------------------------------------------------------
endif ; GL_AMD3D

ifdef GL_SSE

;;--------------------------------------------------------------------------
;; start SSE version
;;--------------------------------------------------------------------------

;;; include listing.inc
INCLUDE fxgasm.h
    
CONST   SEGMENT
        ALIGN 8
_F256_F256      DQ    04380000043800000h ; 256 | 256
CONST   ENDS

_DATA   SEGMENT
        ALIGN   8
btab            DD    8 DUP(0)
atab            DD    8 DUP(0)
vSize           DD    0
strideinbytes   DD    0
vertices        DD    0
_DATA    ENDS


_TEXT         SEGMENT PAGE PUBLIC USE32 'CODE'
              ASSUME DS: FLAT, SS: FLAT

            ALIGN 32
PUBLIC  __grDrawTriangles_SSE@12
_mode     = 20
_count    = 24
_pointers = 28
__grDrawTriangles_SSE@12 PROC NEAR

; 930  : {
; 931  : #define FN_NAME "_grDrawTriangles"
; 932  : 
; 933  :   FxI32
; 934  : #ifdef GLIDE_DEBUG
; 935  :     vSize,
; 936  : #endif
; 937  :     k;
; 938  :   FxI32 stride = mode;
; 939  :   float *vPtr;
; 940  : 
; 941  :   GR_BEGIN_NOFIFOCHECK(FN_NAME, 90);
; 942  : 
; 943  :   GDBG_INFO_MORE(gc->myLevel, "(count = %d, pointers = 0x%x)\n",
; 944  :                  count, pointers);
; 945  : 
; 946  :   GR_FLUSH_STATE();

gc            TEXTEQU  <edi>             ; points to graphics context
fifo          TEXTEQU  <ecx>             ; points to next entry in fifo
dlp           TEXTEQU  <ebp>             ; points to dataList structure
vertexCount   TEXTEQU  <esi>             ; Current vertex counter in the packet
vertexPtr     TEXTEQU  <ebx>             ; Current vertex pointer (in deref mode)
vertex        TEXTEQU  <ebx>             ; Current vertex (in non-deref mode)
dlpStart      TEXTEQU  <edx>             ; Pointer to start of offset list

    push      edi                        ; save caller's register variable
    mov       eax, DWORD PTR fs:[18h]    ; get thread local storage base pointer

    push      esi                        ; save caller's register variable
    mov       edx, [__GlideRoot+tlsOffset]; offset of GC into tls

    push      ebx                        ; save caller's register variable
    mov       vertexCount, [esp+_count-4]; number of vertices in triangles

    mov       gc, [eax+edx]              ; get GC for current thread 
    mov       vertexPtr, [esp+_pointers-4]; get current vertex pointer (deref mode)

    push      ebp                        ; save frame pointer
    mov       edx, [gc + invalid]        ; state needs validation ?

    test      vertexCount, vertexCount   ; number of vertices <= 0 ?
    jle       $tris_done                 ; yup, triangles are done

    test      edx, edx                   ; do we need to validate state ?
    je        $no_validation             ; nope, it's valid

    call      __grValidateState          ; validate state

$no_validation:

; 947  : 
; 948  : #ifdef GLIDE_DEBUG
; 949  :   GDBG_INFO(110, "%s:  vSize = %d\n", FN_NAME, vSize);
; 950  : 
; 951  :   GDBG_INFO(110, "%s:  paramMask = 0x%x\n", FN_NAME, gc->cmdTransportInfo.paramMask);
; 952  : #endif
; 953  : 
; 954  :   if (stride == 0)
; 955  :     stride = gc->state.vData.vStride;
; 956  : 
; 957  : 
; 958  :   _GlideRoot.stats.trisProcessed+=(count/3);
; 959  : 
; 960  :   if (gc->state.grCoordinateSpaceArgs.coordinate_space_mode == GR_WINDOW_COORDS) {

;; We can operate in one of two modes:
;;
;; 0. We are stepping through an array of vertices, in which case
;; the stridesize is equal to the size of the vertex data, and
;; always > 4, since vertex data must a least contain x,y (ie 8 bytes).
;; vertexPtr is pointing to the array of vertices.
;;
;; 1. We are stepping through an array of pointers to vertices
;; in which case the stride is 4 bytes and we need to dereference
;; the pointers to get at the vertex data. vertexPtr is pointing
;; to the array of pointers to vertices.

    mov       eax, [esp + _count]        ; count
    mov       ebp, 0AAAAAAABh            ; 1/3*2^32*2

    xorps     xmm0,xmm0                  ; clear SIMD register
    xorps     xmm1,xmm1
    xorps     xmm2,xmm2
    xorps     xmm3,xmm3
    xorps     xmm4,xmm4
    xorps     xmm5,xmm5
    xorps     xmm6,xmm6
    xorps     xmm7,xmm7

    mul       ebp                        ; edx:eax = 1/3*2*2^32*count; edx = 1/3*2*count
    nop                                  ; filler

    mov       eax, [gc + trisProcessed]  ; trisProcessed
    shr       edx, 1                     ; count/3

    add       eax, edx                   ; trisProcessed += count/3
    mov       edx, [esp + _mode]         ; get mode (0 or 1)

    mov       ecx, [gc + CoordinateSpace]; coordinates space (window/clip)
    mov       [gc + trisProcessed], eax  ; trisProcessed

    test      edx, edx                   ; mode 0 (array of vertices) ?
    jnz       $deref_mode                ; nope, it's mode 1 (array of pointers to vertices)

    mov       edx, [gc + vertexStride]   ; get stride in DWORDs

    shl       edx, 2                     ; stride in bytes
    test      ecx, ecx                   ; coordinate space == 0 (window) ?

    mov       [strideinbytes], edx       ; save off stride (in bytes)
    jnz       $clip_coordinates_ND       ; nope, coordinate space != window  

; 961  :     while (count > 0) {
; 962  :       FxI32 vcount = count >=15 ? 15 : count;
; 963  :       GR_SET_EXPECTED_SIZE(vcount * gc->state.vData.vSize, 1);
; 964  :       TRI_STRIP_BEGIN(kSetupStrip, vcount, gc->state.vData.vSize, SSTCP_PKT3_BDDBDD);
; 965  :       

$win_coords_loop_ND:

    sub       vertexCount, 15            ; vertexCount >= 15 ? CF=0 : CF=1
    mov       ecx, [gc + vertexSize]     ; bytes of data for each vertex 

    sbb       eax, eax                   ; vertexCount >= 15 ? 00000000:ffffffff

    and       vertexCount, eax           ; vertexCount >= 15 ? 0 : vertexcount-15
    add       vertexCount, 15            ; vertexcount >= 15 ? 15 :vertexcount

    imul      ecx, vertexCount           ; total amount of vertex data we'll send

    mov       eax, [gc + fifoRoom]       ; fifo space available
    add       ecx, 4                     ; add header size ==> total packet size

    cmp       eax, ecx                   ; fifo space avail >= packet size ?
    jge       $win_tri_begin_ND          ; yup, start writing triangle data

    push      @Line                      ; line number inside this function
    push      0h                         ; pointer to function name = NULL

    push      ecx                        ; fifo space needed
    call      __grCommandTransportMakeRoom@12 ; note: updates fifoPtr

        align   32
$win_tri_begin_ND:

    mov       eax, vertexCount           ; number of vertices in triangles
    mov       fifo, [gc + fifoPtr]       ; get fifoPtr

    mov       ebp, [gc + cullStripHdr]   ; <2:0> = type
    shl       eax, 6                     ; <9:6> = vertex count (max 15)

    lea       dlpStart, [gc+tsuDataList] ; pointer to start of offset list
    or        eax, ebp                   ; setup vertex count and type

    test      fifo, 4                    ; fifoPtr QWORD aligned ?
    jz        $fifo_aligned_ND           ; yup

    mov       [fifo], eax                ; PCI write packet type
    add       fifo, 4                    ; fifo pointer now QWORD aligned

; 966  :       for (k = 0; k < vcount; k++) {
; 967  :         FxI32 i;
; 968  :         FxU32 dataElem = 0;
; 969  :         
; 970  :         vPtr = pointers;
; 971  :         if (mode)
; 972  :           vPtr = *(float **)vPtr;
; 973  :         (float *)pointers += stride;
; 974  :         
; 975  :         i = gc->tsuDataList[dataElem];
; 976  :         
; 977  :         TRI_SETF(FARRAY(vPtr, 0));
; 978  :         TRI_SETF(FARRAY(vPtr, 4));
; 979  :         while (i != GR_DLIST_END) {
; 980  :           TRI_SETF(FARRAY(vPtr, i));
; 981  :           dataElem++;
; 982  :           i = gc->tsuDataList[dataElem];
; 983  :         }
; 984  :       }
; 985  :       TRI_END;
; 986  :       GR_CHECK_SIZE();
; 987  :       count -= 15;
; 988  :     }

$win_vertex_loop_ND_WB0:                 ; nothing in "write buffer"

    mov       eax, [dlpStart]            ; get first offset from offset list
    mov       dlp, dlpStart              ; point to start of offset list

    movlps    xmm1, [vertex]             ; 0 | 0 | x | y  get vertex x,y
    add       fifo, 8                    ; fifoPtr += 2*sizeof(FxU32)

    add       dlp, 4                     ; dlp++
    test      eax, eax                   ; if offset == 0, end of list

    movlps    [fifo-8], xmm1             ; PCI write x, y
    jz        $win_datalist_end_ND_WB0   ; no more vertex data, nothing in "write buffer" 

$win_datalist_loop_ND_WB0:               ; nothing in "write buffer"

    movss     xmm1,[vertex + eax]        ; get next parameter
    mov       eax, [dlp]                 ; get next offset from offset list

    test      eax, eax                   ; at end of offset list (offset == 0) ?
    jz        $win_datalist_end_ND_WB1   ; exit, write buffer contains one DWORD

    movss     xmm2,[vertex + eax]        ; get next parameter
    add       dlp, 8                     ; dlp++

    mov       eax, [dlp-4]               ; get next offset from offset list
    add       fifo, 8                    ; fifoPtr += 2*sizeof(FxU32)

    test      eax, eax                   ; at end of offset list (offset == 0) ?
    unpcklps  xmm1, xmm2                 ; current param | previous param

    movlps    [fifo-8],xmm1              ; PCI write current param | previous param
    jnz       $win_datalist_loop_ND_WB0  ; nope, copy next parameter

$win_datalist_end_ND_WB0:

    mov       eax, [strideinbytes]       ; get offset to next vertex
    dec       vertexCount                ; another vertex done. Any left?

    lea       vertex, [vertex + eax]     ; points to next vertex
    jnz       $win_vertex_loop_ND_WB0    ; yup, output next vertex

$win_vertex_end_ND_WB0:

    mov       eax, [gc + fifoPtr]        ; old fifoPtr
    mov       ebp, [gc + fifoRoom]       ; old number of bytes available in fifo

    mov       vertexCount, [esp + _count]; remaining vertices before previous loop
    sub       eax, fifo                  ; old fifoPtr - new fifoPtr (fifo room used)

    mov       [gc + fifoPtr], fifo       ; save current fifoPtr 
    add       ebp, eax                   ; new number of bytes available in fifo

    sub       vertexCount, 15            ; remaining number of vertices to process
    mov       [gc + fifoRoom], ebp       ; save current number of bytes available in fifo

    mov       [esp + _count], vertexCount; remaining number of vertices to process 
    test      vertexCount, vertexCount   ; any vertices left to process ?

    nop                                  ; filler
    jg        $win_coords_loop_ND        ; loop if number of vertices to process >= 0

    pop       ebp                        ; restore frame pointer
    pop       ebx                        ; restore caller's register variable

    pop       esi                        ; restore caller's register variable
    pop       edi                        ; restore caller's register variable

    ret       12                         ; return, pop 3 DWORD parameters off stack

$fifo_aligned_ND:

    mov       [fifo], eax                ; PCI write packet header
    movss     xmm2, [vertex]             ; 0 | x of vertex
    add       fifo, 8                    ; fifoPtr += 2*sizeof(FxU32)

    lea       dlp, [dlpStart + 4]        ; point to start of offset list
    nop                                  ; filler

    mov       eax, [dlp-4]               ; first offset in offset list

    movss     [fifo-4], xmm2             ; PCI write x of vertex
    movss     xmm1,[vertex+4]            ; 0 | y of vertex

    cmp       eax, 0                     ; offset == 0 (list empty) ?
    jz        $win_datalist_end_ND_WB1   ; yup, no more vertex data, one DWORD in "write buffer"
    jmp       $win_datalist_loop_ND_WB1

$win_vertex_loop_ND_WB1:                 ; one DWORD in "write buffer"

    movss     xmm2,[vertex]              ; 0 | x of vertex
    add       fifo, 8                    ; fifoPtr += 2*sizeof(FxU32)

    lea       dlp, [dlpStart + 4]        ; point to start of offset list
    nop                                  ; filler

    unpcklps  xmm1, xmm2                 ; packet header | x of vertex ?reversed?
    mov       eax, [dlp-4]               ; first offset in offset list

    movlps    [fifo-8], xmm1             ; PCI write packet header | x of vertex
    movss     xmm1,[vertex+4]            ; 0 | y of vertex

    cmp       eax, 0                     ; offset == 0 (list empty) ?
    jz        $win_datalist_end_ND_WB1   ; yup, no more vertex data, one DWORD in "write buffer"

$win_datalist_loop_ND_WB1:               ; one DWORD in "write buffer" 

    movss     xmm2,[vertex + eax]        ; get next parameter
    add       fifo, 8                    ; fifoPtr += 2*sizeof(FxU32)

    mov       eax, [dlp]                 ; get next offset from offset list
    add       dlp, 8                     ; dlp += 2

    unpcklps  xmm1,xmm2                  ; current param | previous param
    cmp       eax, 0                     ; at end of offset list (offset == 0) ?

    movlps    [fifo-8],xmm1              ; PCI write current param | previous param
    jz        $win_datalist_end_ND_WB0   ; yes, exit, "write buffer" empty

    movss     xmm1,[vertex + eax]        ; get next parameter
    mov       eax, [dlp-4]               ; get next offset from offset list

    cmp       eax, 0                     ; at end of offset list (offset == 0) ?
    jnz       $win_datalist_loop_ND_WB1  ; nope, copy next parameter

$win_datalist_end_ND_WB1:

    mov       eax, [strideinbytes]       ; get offset to next vertex
    dec       vertexCount                ; another vertex done. Any left?

    lea       vertex, [vertex + eax]     ; points to next vertex
    jnz       $win_vertex_loop_ND_WB1    ; yup, output next vertex

$win_vertex_end_ND_WB1:

    movss     [fifo],xmm1                ; flush "write buffer"
    mov       eax, [gc + fifoPtr]        ; old fifoPtr

    add       fifo, 4                    ; fifoPtr += sizeof(FxU32)
    mov       ebp, [gc + fifoRoom]       ; old number of bytes available in fifo

    mov       vertexCount, [esp + _count]; remaining vertices before previous loop
    sub       eax, fifo                  ; old fifoPtr - new fifoPtr (fifo room used)

    mov       [gc + fifoPtr], fifo       ; save current fifoPtr 
    add       ebp, eax                   ; new number of bytes available in fifo

    mov       [gc + fifoRoom], ebp       ; save current number of bytes available in fifo
    sub       vertexCount, 15            ; remaining number of vertices to process

    mov       [esp + _count], vertexCount; remaining number of vertices to process 
    test      vertexCount, vertexCount   ; any vertices left to process ?

    nop                                  ; filler
    jg        $win_coords_loop_ND        ; loop if number of vertices to process >= 0

    pop       ebp                        ; restore frame pointer
    pop       ebx                        ; restore caller's register variable

    pop       esi                        ; restore caller's register variable
    pop       edi                        ; restore caller's register variable

    ret       12                         ; return, pop 3 DWORD parameters off stack

    align 32

$deref_mode:

    prefetchnta [vertexPtr]               ; pre-load first group of pointers

    test      ecx, ecx                   ; coordinate space == 0 (window) ?
    jnz       $clip_coordinates_D        ; nope, coordinate space != window

$win_coords_loop_D:

    sub       vertexCount, 15            ; vertexCount >= 15 ? CF=0 : CF=1
    mov       ecx, [gc + vertexSize]     ; bytes of data for each vertex 

    sbb       eax, eax                   ; vertexCount >= 15 ? 00000000:ffffffff

    and       vertexCount, eax           ; vertexCount >= 15 ? 0 : vertexcount-15
    add       vertexCount, 15            ; vertexcount >= 15 ? 15 :vertexcount

    imul      ecx, vertexCount           ; total amount of vertex data we'll send

    mov       eax, [gc + fifoRoom]       ; fifo space available
    add       ecx, 4                     ; add header size ==> total packet size

    cmp       eax, ecx                   ; fifo space avail >= packet size ?
    jge       $win_tri_begin_D           ; yup, start writing triangle data

    push      @Line                      ; line number inside this function
    push      0h                         ; pointer to function name = NULL

    push      ecx                        ; fifo space needed
    call      __grCommandTransportMakeRoom@12 ; note: updates fifoPtr

        align 32
$win_tri_begin_D:

    mov       eax, vertexCount           ; number of vertices in triangles
    mov       fifo, [gc + fifoPtr]       ; get fifoPtr

    mov       ebp, [gc + cullStripHdr]   ; <2:0> = type
    shl       eax, 6                     ; <9:6> = vertex count (max 15)

    or        eax, ebp                   ; setup mode, vertex count, and type
    lea       dlpStart, [gc+tsuDataList] ; pointer to start of offset list

    test      fifo, 4                    ; fifoPtr QWORD aligned ?
    jz        $fifo_aligned_D            ; yup

    mov       [fifo], eax                ; PCI write packet type
    add       fifo, 4                    ; fifo pointer now QWORD aligned

$win_vertex_loop_D_WB0:                  ; nothing in "write buffer"

    mov       edx, [vertexPtr]           ; dereference pointer, edx points to vertex
    add       vertexPtr, 4               ; next pointer

    lea       dlp, [gc + tsuDataList]    ; get pointer to offset list
    movlps    xmm1,[edx]                 ; get vertex x,y

    mov       eax, [dlp]                 ; get first offset from offset list
    add       dlp, 4                     ; dlp++

    movlps    [fifo],xmm1                ; PCI write x, y
    add       fifo, 8                    ; fifo += 2

    test      eax, eax                   ; if offset == 0, end of offset list
    je        $win_datalist_end_D_WB0    ; no more vertex data, nothing in "write buffer" 

$win_datalist_loop_D_WB0:                ; nothing in "write buffer"

    movss     xmm1,[edx + eax]           ; get next parameter
    mov       eax, [dlp]                 ; get next offset from offset list

    test      eax, eax                   ; at end of offset list (offset == 0) ?
    jz        $win_datalist_end_D_WB1    ; exit, write buffer contains one DWORD

    movss     xmm2,[edx + eax]           ; get next parameter
    add       dlp, 8                     ; dlp++

    mov       eax, [dlp-4]               ; get next offset from offset list
    add       fifo, 8                    ; fifoPtr += 2*sizeof(FxU32)

    unpcklps  xmm1,xmm2                  ; current param | previous param
    cmp       eax, 0                     ; at end of offset list (offset == 0) ?

    movlps    [fifo-8],xmm1              ; PCI write current param | previous param
    jnz       $win_datalist_loop_D_WB0   ; nope, copy next parameter

$win_datalist_end_D_WB0:

    dec       vertexCount                ; another vertex done. Any left?
    jnz       $win_vertex_loop_D_WB0     ; yup, output next vertex

$win_vertex_end_D_WB0:

    mov       eax, [gc + fifoPtr]        ; old fifoPtr
    nop                                  ; filler

    mov       ebp, [gc + fifoRoom]       ; old number of bytes available in fifo
    nop                                  ; filler

    mov       vertexCount, [esp + _count]; remaining vertices before previous loop
    sub       eax, fifo                  ; old fifoPtr - new fifoPtr (fifo room used)

    add       ebp, eax                   ; new number of bytes available in fifo
    mov       [gc + fifoPtr], fifo       ; save current fifoPtr 

    sub       vertexCount, 15            ; remaining number of vertices to process

    mov       [gc + fifoRoom], ebp       ; save current number of bytes available in fifo
    test      vertexCount, vertexCount   ; any vertices left to process ?

    mov       [esp + _count], vertexCount; remaining number of vertices to process
    jg        $win_coords_loop_D         ; loop if number of vertices to process >= 0

    pop       ebp                        ; restore frame pointer
    pop       ebx                        ; restore caller's register variable

    pop       esi                        ; restore caller's register variable
    pop       edi                        ; restore caller's register variable

    ret       12                         ; return, pop 3 DWORD parameters off stack

$fifo_aligned_D:

    mov       [fifo], eax                ; PCI write hacket header
    mov       edx, [vertexPtr]           ; dereference pointer, edx points to vertex
    add       vertexPtr, 4               ; next pointer

    add       fifo, 8                    ; fifoPtr += 2*sizeof(FxU32)
    lea       dlp, [gc + tsuDataList]    ; get pointer to start of offset list

    movss     xmm2,[edx]                 ; 0 | x of vertex
    add       dlp, 4                     ; dlp++
 
    mov       eax, [dlp-4]               ; first offset in offset list

    movss     [fifo-4], xmm2             ; PCI write x of vertex
    movss     xmm1,[edx + 4]             ; 0 | y of vertex

    cmp       eax, 0                     ; offset == 0 (list empty) ?
    je        $win_datalist_end_D_WB1    ; yup, no more vertex data, one DWORD in "write buffer"
    jmp       $win_datalist_loop_D_WB1

$win_vertex_loop_D_WB1:                  ; one DWORD in "write buffer"

    mov       edx, [vertexPtr]           ; dereference pointer, edx points to vertex
    add       vertexPtr, 4               ; next pointer

    add       fifo, 8                    ; fifoPtr += 2*sizeof(FxU32)
    lea       dlp, [gc + tsuDataList]    ; get pointer to start of offset list

    movss     xmm2,[edx]                 ; 0 | x of vertex
    add       dlp, 4                     ; dlp++
 
    mov       eax, [dlp-4]               ; first offset in offset list
    unpcklps  xmm1,xmm2                  ; packet header | x of vertex ?reversed?

    movlps    [fifo-8],xmm1              ; PCI write packet header | x of vertex
    movss     xmm1,[edx + 4]             ; 0 | y of vertex

    cmp       eax, 0                     ; offset == 0 (list empty) ?
    je        $win_datalist_end_D_WB1    ; yup, no more vertex data, one DWORD in "write buffer"

$win_datalist_loop_D_WB1:                ; one DWORD in "write buffer" = MM1

    movss     xmm2,[edx + eax]           ; get next parameter
    add       fifo, 8                    ; fifoPtr += 2*sizeof(FxU32)

    mov       eax, [dlp]                 ; get next offset from offset list
    add       dlp, 8                     ; dlp += 2

    unpcklps  xmm1,xmm2                  ; current param | previous param
    test      eax, eax                   ; at end of offset list (offset == 0) ?

    movlps    [fifo-8],xmm1              ; PCI write current param | previous param
    jz        $win_datalist_end_D_WB0    ; yes, exit, "write buffer" empty

    movss     xmm1,[edx + eax]           ; get next parameter
    mov       eax, [dlp-4]               ; get next offset from offset list

    test      eax,  eax                  ; at end of offset list (offset == 0) ?
    jnz       $win_datalist_loop_D_WB1   ; nope, copy next parameter

$win_datalist_end_D_WB1:

    dec       vertexCount                ; another vertex done. Any left?
    jnz       $win_vertex_loop_D_WB1     ; yup, output next vertex

$win_vertex_end_D_WB1:

    movss     [fifo],xmm1                ; flush "write buffer"
    mov       eax, [gc + fifoPtr]        ; old fifoPtr

    add       fifo, 4                    ; fifoPtr++
    mov       ebp, [gc + fifoRoom]       ; old number of bytes available in fifo

    mov       vertexCount, [esp + _count]; remaining vertices before previous loop
    sub       eax, fifo                  ; old fifoPtr - new fifoPtr (fifo room used)

    mov       [gc + fifoPtr], fifo       ; save current fifoPtr 
    sub       vertexCount, 15            ; remaining number of vertices to process

    add       ebp, eax                   ; new number of bytes available in fifo
    mov       [gc + fifoRoom], ebp       ; save current number of bytes available in fifo

    mov       [esp + _count], vertexCount; remaining number of vertices to process 
    cmp       vertexCount, 0             ; any vertices left to process ?

    nop                                  ; filler
    jg        $win_coords_loop_D         ; loop if number of vertices to process >= 0

    pop       ebp                        ; restore frame pointer
    pop       ebx                        ; restore caller's register variable

    pop       esi                        ; restore caller's register variable
    pop       edi                        ; restore caller's register variable

    ret       12                         ; return, pop 3 DWORD parameters off stack

    ALIGN     32

; 989  :   }
; 990  :   else {
; 991  :     /*
; 992  :      * first cut of clip space coordinate code, no optimization.
; 993  :      */
; 994  :     float oow;
; 995  :     
; 996  :     while (count > 0) {
; 997  :       FxI32 vcount = count >= 15 ? 15 : count;
; 998  :       
; 999  :       GR_SET_EXPECTED_SIZE(vcount * gc->state.vData.vSize, 1);
; 1000 :       TRI_STRIP_BEGIN(kSetupStrip, vcount, gc->state.vData.vSize, SSTCP_PKT3_BDDBDD);
; 1001 :       
; 1002 :       for (k = 0; k < vcount; k++) {
; 1003 :         vPtr = pointers;
; 1004 :         if (mode)
; 1005 :           vPtr = *(float **)pointers;
; 1006 :         oow = 1.0f / FARRAY(vPtr, gc->state.vData.wInfo.offset);
; 1007 :         
; 1008 :         /* x, y */
; 1009 :         TRI_SETF(FARRAY(vPtr, 0)
; 1010 :                  *oow*gc->state.Viewport.hwidth + gc->state.Viewport.ox);
; 1011 :         TRI_SETF(FARRAY(vPtr, 4)
; 1012 :                  *oow*gc->state.Viewport.hheight + gc->state.Viewport.oy);
; 1013 :         (float *)pointers += stride;
; 1014 :         
; 1015 :         TRI_VP_SETFS(vPtr,oow);
; 1016 :       }
; 1017 :       TRI_END;
; 1018 :       GR_CHECK_SIZE();
; 1019 :       count -= 15;
; 1020 :   }
; 1021 : }

$clip_coordinates_D:

    mov       [strideinbytes], 4         ; unit stride for array of pointers to vertices

$clip_coordinates_ND:

dataElem      textequ <ebp>              ; number of vertex components processed

    movss     xmm6,[__GlideRoot+pool_f255]; GlideRoot.pool.f255 

$clip_coords_begin:

    sub       vertexCount, 15            ; vertexCount >= 15 ? CF=0 : CF=1
    mov       ecx, [gc + vertexSize]     ; bytes of data for each vertex 

    sbb       eax, eax                   ; vertexCount >= 15 ? 00000000:ffffffff

    and       vertexCount, eax           ; vertexCount >= 15 ? 0 : vertexcount-15
    add       vertexCount, 15            ; vertexcount >= 15 ? 15 :vertexcount

    imul      ecx, vertexCount           ; total amount of vertex data we'll send

    mov       eax, [gc + fifoRoom]       ; fifo space available
    add       ecx, 4                     ; add header size ==> total packet size

    cmp       eax, ecx                   ; fifo space avail >= packet size ?
    jge       $clip_tri_begin            ; yup, start writing triangle data

    push      @Line                      ; line number inside this function
    push      0h                         ; pointer to function name = NULL

    push      ecx                        ; fifo space needed
    call      __grCommandTransportMakeRoom@12 ; note: updates fifoPtr

        align 32
$clip_tri_begin:
    mov       edx, vertexCount           ; number of vertices in triangles
    mov       fifo, [gc + fifoPtr]       ; get fifoPtr

    mov       ebp, [gc + cullStripHdr]   ; <2:0> = type
    shl       edx, 6                     ; <9:6> = vertex count (max 15)

    or        edx, ebp                   ; setup mode, vertex count, and type

    mov       [fifo], edx                ; PCI write packet type
    add       fifo, 4                    ; fifo pointer now QWORD aligned

$clip_for_begin:

    mov       edx, vertexPtr             ; vertex = vertexPtr (assume no-deref mode)
    mov       eax, [esp+_mode]           ; mode 0 = no deref, mode 1 = deref

    mov       [vertices], vertexCount    ; save numnber of vertices
    test      eax, eax                   ; deref mode ?

    mov       eax, [gc + wInfo_offset]   ; get offset of W into vertex struct
    jz        $clip_noderef              ; yup, no-deref mode

    mov       edx, [vertexPtr]           ; vertex = *vertexPtr

$clip_noderef:

    movss     xmm1,[edx + eax]           ; 0 | W of current vertex
    rcpss     xmm0,xmm1                  ; 0 | 1/W approx

    mov       ebp, [strideinbytes]       ; offset to next vertex/vertexPtr
    movlps    xmm2,[edx]                 ; y | x of current vertex

    movlps    xmm3,[gc + vp_hwidth]      ; gc->state.Viewport.hheight | gc->state.Viewport.hwidth

    movlps    xmm4,[gc + vp_ox]          ; gc->state.Viewport.oy | gc->state.Viewport.ox
    add       vertexPtr, ebp             ; point to next vertex/VertexPtr

    mulss     xmm1,xmm0                  ; 0 | 1/W refine
    mulss     xmm1,xmm0
    addss     xmm0,xmm0
    subss     xmm0,xmm1                  ; oow = 1.0f / FARRAY(vPtr, gc->state.vData.wInfo.offset
    mov       esi, [gc + paramIndex]     ; gc->state.paramIndex

    mulps     xmm2,xmm3                  ; TRI_SETF(FARRAY(vPtr,0)*state.Viewport.hheight | TRI_SETF(FARRAY(vPtr,4)*state.Viewport.hwidth
    xor       dataElem, dataElem         ; dataElem = 0

    add       fifo, 8                    ; fifoPtr += 2*sizeof(FxFloat)
    unpcklps  xmm0,xmm0                  ; oow | oow

    mulps     xmm2,xmm0                  ; TRI_SETF(FARRAY(vPtr, 4)*oow*gc->state.Viewport.height | TRI_SETF(FARRAY(vPtr, 0)*oow*gc->state.Viewport.hwidth
    addps     xmm2,xmm4                  ; TRI_SETF(FARRAY(vPtr, 4)*oow*gc->state.Viewport.hheight + gc->state.Viewport.oy) |

;;;  FxI32 i, dataElem=0; \
;;;  i = gc->tsuDataList[dataElem]; \
;;;  if (gc->state.paramIndex & (STATE_REQUIRES_IT_DRGB | STATE_REQUIRES_IT_ALPHA)) { \
;;;    if (gc->state.vData.colorType == GR_FLOAT) { \
;;;      if (gc->state.paramIndex & STATE_REQUIRES_IT_DRGB) { \
;;;        DA_SETF_SCALE_ADVANCE(_s,_GlideRoot.pool.f255); \
;;;        DA_SETF_SCALE_ADVANCE(_s,_GlideRoot.pool.f255); \
;;;        DA_SETF_SCALE_ADVANCE(_s,_GlideRoot.pool.f255); \
;;;      } \
;;;      if (gc->state.paramIndex & STATE_REQUIRES_IT_ALPHA) { \
;;;        DA_SETF_SCALE_ADVANCE(_s,_GlideRoot.pool.f255); \
;;;      } \
;;;    } \
;;;    else { \
;;;      DA_SETF(FARRAY(_s, i)); \
;;;      dataElem++; \
;;;      i = gc->tsuDataList[dataElem]; \
;;;    } \
;;;  } \

    test      esi, 3                     ; STATE_REQUIRES_IT_DRGB | STATE_REQUIRES_IT_ALPHA ?
    mov       eax, [gc + tsuDataList]    ; first entry from offset list

    movlps    [fifo-8],xmm2              ; PCI write transformed x, y
    jz        $clip_setup_ooz            ; nope, no color at all needed
  
    cmp       DWORD PTR [gc+colorType], 0; gc->state.vData.colorType == GR_FLOAT ?
    jne       $clip_setup_pargb          ; nope, packed ARGB format
  
    test      esi, 1                     ; STATE_REQUIRES_IT_DRGB ?
    jz        $clip_setup_a              ; no, but definitely A

    movss     xmm2,[edx + eax]           ; 0 | r
    mov       eax, [gc + tsuDataList+4]  ; offset of g part of vertex data

    mulss     xmm2,xmm6                  ; 0 | r * 255.0f
    movss     xmm3,[edx + eax]           ; 0 | g

    mov       eax, [gc + tsuDataList + 8]; offset of b part of vertex data
    movss     [fifo],xmm2                ; PCI write r*255

    mulss     xmm3,xmm6                  ; 0 | g * 255.0f
    movss     xmm2,[edx + eax]           ; 0 | b

    movss     [fifo+4],xmm3              ; PCI write g*255
    mov       dataElem, 12               ; dataElem = 3

    mulss     xmm2,xmm6                  ; 0 | b * 255.0f
    mov       eax, [gc + tsuDataList+12] ; offset of A part of vertex data

    test      esi, 2                     ; STATE_REQUIRES_IT_ALPHA ?
    lea       fifo, [fifo+12]            ; fifoPtr += 3*sizeof(FxFloat)

    movss     [fifo-4],xmm2              ; PCI write b*255
    jz        $clip_setup_ooz            ; nope, no alpha, proceeed with ooz

$clip_setup_a:
    movss     xmm2,[eax+edx]             ; 0 | a
    add       fifo, 4                    ; fifoPtr += sizeof(FxFloat)

    mov       esp, esp                   ; filler
    add       dataElem, 4                ; dataElem++ 

    mulss     xmm2,xmm6                  ; 0 | a * 255.0f
    mov       eax, [gc+dataElem+tsuDataList]; offset of next part of vertex data

    movss     [fifo-4],xmm2              ; PCI write a*255
    jmp       $clip_setup_ooz            ; check whether we need to push out z

$clip_setup_pargb:
    mov       dataElem,[eax+edx]         ; get packed ARGB data
    add       fifo, 4                    ; fifoPtr += sizeof(FxU32)
    mov       [fifo-4],dataElem          ; PCI write packed ARGB

    mov       dataElem, 4                ; dataElem = 1 (namely pargb)
    mov       eax, [gc+tsuDataList+4]    ; offset of next part of vertex data

;;;  if (gc->state.paramIndex & STATE_REQUIRES_OOZ) { \
;;;    if (gc->state.fbi_config.fbzMode & SST_DEPTH_FLOAT_SEL) { \
;;;      if (gc->state.vData.qInfo.mode == GR_PARAM_ENABLE) { \
;;;        DA_SETF(FARRAY(_s, gc->state.vData.qInfo.offset)*_oow); \
;;;      } else { \
;;;        DA_SETF(_oow); \
;;;      } \
;;;      dataElem++; \
;;;      i = gc->tsuDataList[dataElem]; \
;;;    } \
;;;    else { \
;;;      DA_SETF(FARRAY(_s, i)*_oow*gc->state.Viewport.hdepth + gc->state.Viewport.oz); \
;;;      dataElem++; \
;;;      i = gc->tsuDataList[dataElem]; \
;;;    } \
;;;  } \

$clip_setup_ooz:
  
    test      esi, 4                     ; STATE_REQUIRES_OOZ ?
    jz        $clip_setup_qow            ; nope

    test      DWORD PTR[gc+fbi_fbzMode],200000h ; gc->state.fbi_config.fbzMode & SST_DEPTH_FLOAT_SEL != 0 ?
    je        $clip_setup_ooz_nofog      ; nope

    cmp       DWORD PTR[gc+qInfo_mode], 0; gc->state.vData.qInfo.mode == GR_PARAM_ENABLE ?
    jz        $clip_setup_fog_oow        ; nope

    mov       eax, [gc + qInfo_offset]   ; offset of Q component of vertex
    add       fifo, 4                    ; fifoPtr += sizeof(FxFloat)

    add       dataElem, 4                ; dataElem++
    movss     xmm2,[edx + eax]           ; 0 | q of vertex

    mov       eax,[gc+dataElem+tsuDataList]; pointer to next vertex component
    mulss     xmm2, xmm0                 ; 0 | q*oow

    movss     [fifo-4],xmm2              ; PCI write transformed Q
    jmp       $clip_setup_qow            ; check whether we need to write Q or W

$clip_setup_fog_oow:

    add       fifo, 4                    ; fifoPtr += sizeof(FxFloat) 
    add       dataElem, 4                ; dataElem++

    movss     xmm3,[gc + depth_range]    ;
    pfmul     xmm3,xmm0                  ; 

    movss     xmm4,[gc + depth_range]    ; depth range
    subss     xmm4,xmm3                  ; 

    movss     [fifo-4],xmm4              ; PCI write oow
    mov       eax,[gc+dataElem+tsuDataList]; pointer to next vertex component

    jmp       $clip_setup_qow            ; check whether we need to write Q or W

$clip_setup_ooz_nofog:

    movss     xmm2,[eax + edx]           ; 0 | z component of vertex
    add       fifo, 4                    ; fifoPtr += sizeof(FxFloat)

    add       dataElem, 4                ; dataElem += 1
    movss     xmm3,[gc + vp_hdepth]      ; 0 | gc->state.Viewport.hdepth

    mulss     xmm2,xmm0                  ; TRI_SETF(FARRAY(_s, i)*_oow
    movss     xmm4,[gc + vp_oz]          ; 0 | gc->state.Viewport.oz

    mulss     xmm2,xmm3                  ; 0 | TRI_SETF(FARRAY(_s, i)*_oow*gc->state.Viewport.hdepth
    mov       eax, [gc+dataElem+tsuDataList]; offset of next vertex component

    addss     xmm2,xmm4                  ; 0 | TRI_SETF(FARRAY(_s, i)*_oow*gc->state.Viewport.hdepth+gc->state.Viewport.oz
    movss     [fifo-4],xmm2              ; PCI write transformed Z

;;;  if (gc->state.paramIndex & STATE_REQUIRES_OOW_FBI) { \
;;;    if (gc->state.vData.fogInfo.mode == GR_PARAM_ENABLE) { \
;;;      DA_SETF(FARRAY(_s, gc->state.vData.fogInfo.offset)*_oow); \
;;;    } \
;;;    else if (gc->state.vData.qInfo.mode == GR_PARAM_ENABLE) { \
;;;      DA_SETF(FARRAY(_s, gc->state.vData.qInfo.offset)*_oow); \
;;;    } else { \
;;;      DA_SETF(_oow); \
;;;    } \
;;;    dataElem++; \
;;;    i = gc->tsuDataList[dataElem]; \
;;;  } \

$clip_setup_qow:
    test      esi, 8                     ; STATE_REQUIRES_OOW_FBI ?
    jz        $clip_setup_qow0           ; nope

    cmp       DWORD PTR[gc+fogInfo_mode],0; gc->state.vData.fogInfo.mode == GR_PARAM_ENABLE ?
    jz        $clip_setup_oow_nofog      ; nope, no fog

    mov       eax, [gc + fogInfo_offset] ; offset of fogInfo component of vertex
    add       fifo, 4                    ; fifoPtr += sizeof(FxFloat)

    add       dataElem, 4                ; dataElem++
    movss     xmm2,[edx + eax]           ; 0 | fogInfo of vertex

    mov       eax,[gc+dataElem+tsuDataList]; pointer to next vertex component
    mulss     xmm2,xmm0                  ; fogInfo*oow

    movss     [fifo-4],xmm2              ; PCI write transformed Q
    jmp       $clip_setup_qow0           ; continue with q0

$clip_setup_oow_nofog:

    cmp       DWORD PTR [gc+qInfo_mode],0; gc->state.vData.qInfo.mode == GR_PARAM_ENABLE ?
    je        $clip_setup_oow            ; nope, write oow, not Q

    mov       eax, [gc + qInfo_offset]   ; offset of Q component of vertex
    add       fifo, 4                    ; fifoPtr += sizeof(FxFloat)

    add       dataElem, 4                ; dataElem++
    movss     xmm2,[edx + eax]           ; 0 | q of vertex

    mov       eax,[gc+dataElem+tsuDataList]; pointer to next vertex component
    mulss     xmm2,xmm0                  ; q*oow

    movss     [fifo-4],xmm2              ; PCI write transformed Q
    jmp       $clip_setup_qow0           ; continue with q0

$clip_setup_oow:
    add       fifo, 4                    ; fifoPtr += sizeof(FxFloat) 
    add       dataElem, 4                ; dataElem++

    movss     [fifo-4],xmm0              ; PCI write oow
    mov       eax,[gc+dataElem+tsuDataList]; pointer to next vertex component

;;;  if (gc->state.paramIndex & STATE_REQUIRES_W_TMU0) { \
;;;    if (gc->state.vData.q0Info.mode == GR_PARAM_ENABLE) { \
;;;      DA_SETF(FARRAY(_s, gc->state.vData.q0Info.offset)*_oow); \
;;;    } \
;;;    else { \
;;;      DA_SETF(_oow); \
;;;    } \
;;;    dataElem++; \
;;;    i = gc->tsuDataList[dataElem]; \
;;;  } \

$clip_setup_qow0:
    test      esi, 16                    ; STATE_REQUIRES_W_TMU0 ?
    jz        $clip_setup_stow0          ; nope 

    cmp       DWORD PTR [gc+q0Info_mode],0; does vertex have Q component ?
    je        $clip_setup_oow0           ; nope, not Q but W

    mov       eax, [gc+q0Info_offset]    ; offset of Q component of vertex
    add       fifo, 4                    ; fifoPtr += sizeof(FxFloat)

    add       dataElem, 4                ; dataElem++
    movss     xmm2,[edx+eax]             ; 0 | q0 of vertex

    mov       eax,[gc+dataElem+tsuDataList]; pointer to next vertex component
    mulss     xmm2,xmm0                  ; q0*oow

    movss     [fifo-4],xmm2              ; PCI write transformed q0
    jmp       $clip_setup_stow0          ; continue with stow0

    nop                                  ; filler

$clip_setup_oow0:
    add       fifo, 4                    ; fifoPtr += sizeof(FxFloat) 
    add       dataElem, 4                ; dataElem++

    movss     [fifo-4],xmm0              ; PCI write oow
    mov       eax,[gc+dataElem+tsuDataList]; pointer to next vertex component

;;;  if (gc->state.paramIndex & STATE_REQUIRES_ST_TMU0) { \
;;;    DA_SETF_SCALE_ADVANCE(_s,_oow*gc->state.tmu_config[0].s_scale); \
;;;    DA_SETF_SCALE_ADVANCE(_s,_oow*gc->state.tmu_config[0].t_scale); \
;;;  } \

$clip_setup_stow0:

    test      esi, 32                    ; STATE_REQUIRES_ST_TMU0 ?
    jz        $clip_setup_qow1           ; nope

    movlps    xmm7,[gc + tmu0_s_scale]   ; state.tmu_config[0].t_scale | state.tmu_config[0].s_scale
    add       fifo, 8                    ; fifoPtr += 2*sizeof(FxFloat)

    movss     xmm2,[edx + eax]           ; param1
    mov       eax,[gc+dataElem+tsuDataList+4];pointer to next vertex component

    mulps     xmm7,xmm0                  ; oow*tmu0_t_scale | oow*tmu0_s_scale
    add       dataElem, 8                ; dataElem += 2

    movss     xmm3,[edx + eax]           ; param2
    unpcklps  xmm2,xmm3                  ; param2 | param1

    mulps     xmm2,xmm7                  ; param2*oow*tmu0_t_scale | param1*oow*tmu0_s_scale
    nop                                  ; filler

    movlps    [fifo-8],xmm2              ; PCI write param2*oow*tmu0_t_scale | param1*oow*tmu0_s_scale
    mov       eax, [gc+dataElem+tsuDataList]; pointer to next vertex component

;;;  if (gc->state.paramIndex & STATE_REQUIRES_W_TMU1) { \
;;;    if (gc->state.vData.q1Info.mode == GR_PARAM_ENABLE) { \
;;;      DA_SETF(FARRAY(_s, gc->state.vData.q1Info.offset)*_oow); \
;;;    } \
;;;    else { \
;;;      DA_SETF(_oow); \
;;;    } \
;;;    dataElem++; \
;;;    i = gc->tsuDataList[dataElem]; \
;;;  } \

$clip_setup_qow1:
    test      esi, 64                    ; STATE_REQUIRES_W_TMU1 ?
    jz        $clip_setup_stow1          ; nope

    cmp       DWORD PTR [gc+q1Info_mode],0; does vertex have Q component ?
    je        $clip_setup_oow1           ; nope, not Q but W

    mov       eax, [gc+q1Info_offset]    ; offset of Q component of vertex
    add       fifo, 4                    ; fifoPtr += sizeof(FxFloat)

    add       dataElem, 4                ; dataElem++
    movss     xmm2,[edx + eax]           ; 0 | q1 of vertex

    mov       eax,[gc+dataElem+tsuDataList]; pointer to next vertex component
    mulss     xmm2,xmm0                  ; q1*oow

    movss     [fifo-4],xmm2              ; PCI write transformed q1
    jmp       $clip_setup_stow1          ; continue with stow1

$clip_setup_oow1:
    add       fifo, 4                    ; fifoPtr += sizeof(FxFloat) 
    add       dataElem, 4                ; dataElem++

    movss     [fifo-4],xmm0              ; PCI write oow
    mov       eax,[gc+dataElem+tsuDataList]; pointer to next vertex component

;;;  if (gc->state.paramIndex & STATE_REQUIRES_ST_TMU1) { \
;;;    DA_SETF_SCALE_ADVANCE(_s,_oow*gc->state.tmu_config[1].s_scale); \
;;;    DA_SETF_SCALE_ADVANCE(_s,_oow*gc->state.tmu_config[1].t_scale); \
;;;  } \

$clip_setup_stow1:

    test      esi, 128                   ; STATE_REQUIRES_ST_TMU1 ?
    mov       vertexCount, [vertices]    ; get number of vertices

    movlps    xmm7,[gc + tmu1_s_scale]   ; state.tmu_config[1].t_scale | state.tmu_config[1].s_scale
    jz        $clip_setup_end            ; nope

    movss     xmm2,[edx + eax]           ; param1
    add       fifo, 8                    ; fifoPtr += 2*sizeof(FxFloat)

    mov       eax,[gc+dataElem+tsuDataList+4]; pointer to next vertex component
    mulps     xmm7,xmm0                  ; oow*state.tmu_config[1].t_scale | oow*state.tmu_config[1].s_scale

    movss     xmm3,[edx + eax]           ; param2
    unpcklps  xmm2,xmm3                  ; param2 | param1

    mulps     xmm2,xmm7                  ; param2*oow*state.tmu_config[1].t_scale | param1*oow*state.tmu_config[1].s_scale
    movlps    [fifo-8],xmm2              ; PCI write param2*oow*state.tmu_config[1].t_scale | param1*oow*state.tmu_config[1].s_scale

$clip_setup_end:

    dec       vertexCount                ; vcount--
    jnz       $clip_for_begin            ; until 

$clip_for_end:

    mov       eax, [gc + fifoPtr]        ; old fifoPtr
    mov       ebp, [gc + fifoRoom]       ; old number of bytes available in fifo

    mov       vertexCount, [esp + _count]; remaining vertices before previous loop
    sub       eax, fifo                  ; old fifoPtr - new fifoPtr (fifo room used)

    mov       [gc + fifoPtr], fifo       ; save current fifoPtr 
    add       ebp, eax                   ; new number of bytes available in fifo

    sub       vertexCount, 15            ; remaining number of vertices to process
    mov       [gc + fifoRoom], ebp       ; save current number of bytes available in fifo

    mov       [esp + _count], vertexCount; remaining number of vertices to process 
    cmp       vertexCount, 0             ; any vertices left to process ?

    jg        $clip_coords_begin         ; loop if number of vertices to process >= 0

$tris_done:
    pop       ebp                        ; restore frame pointer
    pop       ebx                        ; restore caller's register variable

    pop       esi                        ; restore caller's register variable
    pop       edi                        ; restore caller's register variable

    ret       12                         ; return, pop 3 DWORD parameters
__grDrawTriangles_SSE@12 ENDP

_pktype   = 20
_type     = 24
_mode     = 28
_count    = 32
_pointers = 36

gc            TEXTEQU  <edi>             ; points to graphics context
fifo          TEXTEQU  <ecx>             ; points to next entry in fifo
dlp           TEXTEQU  <ebp>             ; points to dataList structure
vertexCount   TEXTEQU  <esi>             ; Current vertex counter in the packet
vertexPtr     TEXTEQU  <ebx>             ; Current vertex pointer (in deref mode)
vertex        TEXTEQU  <ebx>             ; Current vertex (in non-deref mode)
dlpStart      TEXTEQU  <edx>             ; Pointer to start of offset list

X TEXTEQU     <0>
Y TEXTEQU     <4>

                  ALIGN  32

    PUBLIC  __grDrawVertexList_SSE_Window@20
__grDrawVertexList_SSE_Window@20 PROC NEAR
; 132  : {

    mov       edx, DWORD PTR fs:[18h]    ; get thread local storage base pointer        
    push      edi                        ; save caller's register variable

    push      esi                        ; save caller's register variable
    mov       vertexCount, [esp+_count-8]; number of vertices in strip/fan

    push      ebp                        ; save frame pointer
    mov       ebp, [__GlideRoot + tlsOffset]; GC position relative to tls base    

    push      ebx                        ; save caller's register variable        
    mov       vertexPtr, [esp+_pointers] ; get current vertex pointer (deref mode)
                                         ; get current vertex (non-deref mode)
    
    mov       gc, [edx + ebp]            ; get current graphics context from tls
    test      vertexCount, vertexCount   ; number of vertices <= 0 ?

    nop                                  ; filler
    jle       strip_done                 ; yup, the strip/fan is done
  
;;;     vSize = gc->state.vData.vSize
;;;     if (stride == 0)
;;;       stride = gc->state.vData.vStride;

;; We can operate in one of two modes:
;;
;; 0. We are stepping through an array of vertices, in which case
;; the stridesize is equal to the size of the vertex data, and
;; always > 4, since vertex data must a least contain x,y (ie 8 bytes).
;; vertexPtr is pointing to the array of vertices.
;;
;; 1. We are stepping through an array of pointers to vertices
;; in which case the stride is 4 bytes and we need to dereference
;; the pointers to get at the vertex data. vertexPtr is pointing
;; to the array of pointers to vertices.

    mov       edx, [esp + _mode]         ; get mode (0 or 1)
    mov       eax, [gc + vertexSize]     ; size of vertex data in bytes

    test      edx, edx                   ; mode 0 (array of vertices) ?
    mov       edx, [gc + vertexStride]   ; get stride in DWORDs
    
    jnz       deref_mode                 ; nope, it's mode 1 (array of pointers to vertices)

    xorps     xmm0,xmm0                  ; clear SIMD register
    xorps     xmm1,xmm1
    xorps     xmm2,xmm2
    xorps     xmm3,xmm3
    xorps     xmm4,xmm4
    xorps     xmm5,xmm5
    xorps     xmm6,xmm6
    xorps     xmm7,xmm7

    shl       edx, 2                     ; stride in bytes
    mov       [strideinbytes], edx       ; save off stride (in bytes)

;;;     Draw the first (or possibly only) set.  This is necessary because
;;;     the packet is 3_BDDDDDD, and in the next set, the packet is 3_DDDDDD
;;;     We try to make tstrip code simple to read. We combine the original code
;;;     into a single loop by adding an extra packet type assignment at the end of the loop.
;;; 
;;;     if (gc->state.grCoordinateSpaceArgs.coordinate_space_mode == GR_WINDOW_COORDS) {
;;;       while (count > 0) {
;;;         FxI32 k, vcount = count >= 15 ? 15 : count;
;;;         GR_SET_EXPECTED_SIZE(vcount * vSize, 1);
;;;         TRI_STRIP_BEGIN(type, vcount, vSize, pktype);


win_coords_loop_ND:

    sub       vertexCount, 15            ; vertexCount >= 15 ? CF=0 : CF=1
    mov       ecx, [gc + vertexSize]     ; bytes of data for each vertex 

    sbb       eax, eax                   ; vertexCount >= 15 ? 00000000:ffffffff

    and       vertexCount, eax           ; vertexCount >= 15 ? 0 : vertexcount-15
    add       vertexCount, 15            ; vertexcount >= 15 ? 15 :vertexcount

    imul      ecx, vertexCount           ; total amount of vertex data we'll send

    mov       eax, [gc + fifoRoom]       ; fifo space available
    add       ecx, 4                     ; add header size ==> total packet size

    cmp       eax, ecx                   ; fifo space avail >= packet size ?
    jge       win_strip_begin_ND         ; yup, start writing strip data

    push      @Line                      ; line number inside this function
    push      0h                         ; pointer to function name = NULL

    push      ecx                        ; fifo space needed
    call      __grCommandTransportMakeRoom@12 ; note: updates fifoPtr

        align 32
win_strip_begin_ND:
;;;     Setup packet header
;;;
    mov       eax, vertexCount           ; number of vertices in strip/fan
    mov       edx, [esp + _type]         ; setup mode

    mov       fifo, [gc + fifoPtr]       ; get fifoPtr
    shl       edx, 22                    ; <27:22> = setup mode (kSetupStrip or kSetupFan)

    mov       ebp, [gc + cullStripHdr]   ; <2:0> = type
    shl       eax, 6                     ; <9:6> = vertex count (max 15)

    or        eax, edx                   ; setup mode and vertex count
    mov       edx, [esp + _pktype]       ; <5:3> = command (SSTCP_PKT3_BDDBDD, SSTCP_PKT3_BDDDDD, or SSTCP_PKT3_DDDDDD)

    or        eax, ebp                   ; setup mode, vertex count, and type
    nop                                  ; filler

    or        eax, edx                   ; setup mode, vertex count, type, and command
    lea       dlpStart, [gc+tsuDataList] ; pointer to start of offset list

    test      fifo, 4                    ; fifoPtr QWORD aligned ?
    jz        fifo_aligned_ND            ; yup

    mov       [fifo], eax                ; PCI write packet type
    add       fifo, 4                    ; fifo pointer now QWORD aligned

;;;     for (k = 0; k < vcount; k++) {
;;;       FxI32 i;
;;;       FxU32 dataElem;
;;;       float *vPtr;
;;;       vPtr = pointers;
;;;       if (mode)
;;;         vPtr = *(float **)vPtr
;;;       (float *)pointers += stride;
;;;       TRI_SETF(FARRAY(vPtr, 0));
;;;       dataElem = 0;
;;;       TRI_SETF(FARRAY(vPtr, 4));
;;;       i = gc->tsuDataList[dataElem];

win_vertex_loop_ND_WB0:                  ; nothing in "write buffer"

    mov       eax, [dlpStart]            ; get first offset from offset list
    lea       dlp, [dlpStart+4]          ; point to start of offset list

    movlps    xmm1,[vertex+X]            ; get vertex x,y
    add       fifo, 8                    ; fifoPtr += 2*sizeof(FxU32)

    nop                                  ; filler
    test      eax, eax                   ; if offset == 0, end of list

    movlps    [fifo-8],xmm1              ; PCI write x, y
    jz        win_datalist_end_ND_WB0    ; no more vertex data, nothing in "write buffer" 

;;;       while (i != GR_DLIST_END) {
;;;         TRI_SETF(FARRAY(vPtr, i));
;;;         dataElem++;
;;;         i = gc->tsuDataList[dataElem];
;;;       }

win_datalist_loop_ND_WB0:                ; nothing in "write buffer"

    movss     xmm1,[vertex + eax]        ; get next parameter
    mov       eax, [dlp]                 ; get next offset from offset list

    test      eax, eax                   ; at end of offset list (offset == 0) ?
    jz        win_datalist_end_ND_WB1    ; exit, write buffer contains one DWORD

    movss     xmm2,[vertex + eax]        ; get next parameter
    add       dlp, 8                     ; dlp++

    mov       eax, [dlp-4]               ; get next offset from offset list
    add       fifo, 8                    ; fifoPtr += 2*sizeof(FxU32)

    test      eax, eax                   ; at end of offset list (offset == 0) ?
    unpcklps  xmm1,xmm2                  ; current param | previous param

    movlps    [fifo-8],xmm1              ; PCI write current param | previous param
    jnz       win_datalist_loop_ND_WB0   ; nope, copy next parameter

win_datalist_end_ND_WB0:

    mov       eax, [strideinbytes]       ; get offset to next vertex
    sub       vertexCount, 1             ; another vertex done. Any left?

    lea       vertex, [vertex + eax]     ; points to next vertex
    jnz       win_vertex_loop_ND_WB0     ; yup, output next vertex

win_vertex_end_ND_WB0:

;;;       TRI_END;
;;;     Prepare for the next packet (if the strip size is longer than 15)
;;;       GR_CHECK_SIZE();
;;;       count -= 15;
;;;       pktype = SSTCP_PKT3_DDDDDD;
;;;     }
  
    mov       ebp, 16                    ; pktype = SSTCP_PKT3_DDDDDD (strip continuation)
    mov       eax, [gc + fifoPtr]        ; old fifoPtr

    mov       [esp + _pktype], ebp       ; pktype = SSTCP_PKT3_DDDDDD (strip continuation)
    mov       ebp, [gc + fifoRoom]       ; old number of bytes available in fifo

    mov       vertexCount, [esp + _count]; remaining vertices before previous loop
    sub       eax, fifo                  ; old fifoPtr - new fifoPtr (fifo room used)

    mov       [gc + fifoPtr], fifo       ; save current fifoPtr 
    add       ebp, eax                   ; new number of bytes available in fifo

    nop                                  ; filler
    sub       vertexCount, 15            ; remaining number of vertices to process

    mov       [gc + fifoRoom], ebp       ; save current number of bytes available in fifo
    mov       [esp + _count], vertexCount; remaining number of vertices to process 

    test      vertexCount, vertexCount   ; any vertices left to process ?
    jg        win_coords_loop_ND         ; loop if number of vertices to process >= 0

    pop       ebx                        ; restore caller's register variable
    pop       ebp                        ; restore frame pointer

    pop       esi                        ; restore caller's register variable
    pop       edi                        ; restire caller's register variable

    ret       20                         ; return, pop 5 DWORD parameters off stack

    ALIGN 32

fifo_aligned_ND:

    mov       [fifo], eax                ; PCI write packet header
    movss     xmm2,[vertex + X]          ; 0 | x of vertex
    add       fifo, 8                    ; fifoPtr += 2*sizeof(FxU32)

    lea       dlp, [dlpStart + 4]        ; point to start of offset list
    nop                                  ; filler

    mov       eax, [dlp-4]               ; first offset in offset list

    movss     [fifo-4], xmm2             ; PCI write x of vertex
    movss     xmm1,[vertex + Y]          ; 0 | y of vertex

    cmp       eax, 0                     ; offset == 0 (list empty) ?
    jz        win_datalist_end_ND_WB1    ; yup, no more vertex data, one DWORD in "write buffer"
    jmp       win_datalist_loop_ND_WB1

win_vertex_loop_ND_WB1:                  ; one DWORD in "write buffer"

    movss     xmm2,[vertex + X]          ; 0 | x of vertex
    add       fifo, 8                    ; fifoPtr += 2*sizeof(FxU32)

    lea       dlp, [dlpStart + 4]        ; point to start of offset list
    nop                                  ; filler

    unpcklps  xmm1,xmm2                  ; packet header | x of vertex ?reversed?
    mov       eax, [dlp-4]               ; first offset in offset list

    movlps    [fifo-8],xmm1              ; PCI write packet header | x of vertex
    movss     xmm1,[vertex + Y]          ; 0 | y of vertex

    cmp       eax, 0                     ; offset == 0 (list empty) ?
    jz        win_datalist_end_ND_WB1    ; yup, no more vertex data, one DWORD in "write buffer"

;;;       while (i != GR_DLIST_END) {
;;;         TRI_SETF(FARRAY(vPtr, i));
;;;         dataElem++;
;;;         i = gc->tsuDataList[dataElem];
;;;       }

win_datalist_loop_ND_WB1:                ; one DWORD in "write buffer" 

    movss     xmm2,[vertex + eax]        ; get next parameter
    add       fifo, 8                    ; fifoPtr += 2*sizeof(FxU32)

    mov       eax, [dlp]                 ; get next offset from offset list
    add       dlp, 8                     ; dlp += 2

    unpcklps  xmm1,xmm2                  ; current param | previous param
    cmp       eax, 0                     ; at end of offset list (offset == 0) ?

    movlps    [fifo-8],xmm1              ; PCI write current param | previous param
    jz        win_datalist_end_ND_WB0    ; yes, exit, "write buffer" empty

    movss     xmm1,[vertex+eax]          ; get next parameter
    mov       eax, [dlp-4]               ; get next offset from offset list

    test      eax, eax                   ; at end of offset list (offset == 0) ?
    jnz       win_datalist_loop_ND_WB1   ; nope, copy next parameter

win_datalist_end_ND_WB1:

    mov       eax, [strideinbytes]       ; get offset to next vertex
    sub       vertexCount, 1             ; another vertex done. Any left?

    lea       vertex, [vertex + eax]     ; points to next vertex
    jnz       win_vertex_loop_ND_WB1     ; yup, output next vertex

win_vertex_end_ND_WB1:

    movss     [fifo],xmm1                ; flush "write buffer"
    add       fifo, 4                    ; fifoPtr += sizeof(FxU32)

;;;       TRI_END;
;;;     Prepare for the next packet (if the strip size is longer than 15)
;;;       GR_CHECK_SIZE();
;;;       count -= 15;
;;;       pktype = SSTCP_PKT3_DDDDDD;
;;;     }

    mov       ebp, 16                    ; pktype = SSTCP_PKT3_DDDDDD (strip continuation)
    mov       eax, [gc + fifoPtr]        ; old fifoPtr

    mov       [esp+_pktype], ebp         ; pktype = SSTCP_PKT3_DDDDDD (strip continuation)
    mov       ebp, [gc + fifoRoom]       ; old number of bytes available in fifo

    mov       vertexCount, [esp + _count]; remaining vertices before previous loop
    sub       eax, fifo                  ; old fifoPtr - new fifoPtr (fifo room used)

    mov       [gc + fifoPtr], fifo       ; save current fifoPtr 
    add       ebp, eax                   ; new number of bytes available in fifo

    mov       [gc + fifoRoom], ebp       ; save current number of bytes available in fifo
    sub       vertexCount, 15            ; remaining number of vertices to process

    mov       [esp + _count], vertexCount; remaining number of vertices to process 
    test      vertexCount, vertexCount   ; any vertices left to process ?

    nop                                  ; filler
    jg        win_coords_loop_ND         ; loop if number of vertices to process >= 0

    pop       ebx                        ; restore caller's register variable
    pop       ebp                        ; restore frame pointer

    pop       esi                        ; restore caller's register variable
    pop       edi                        ; restire caller's register variable

    ret       20                         ; return, pop 5 DWORD parameters off stack

    ALIGN 32
    
deref_mode:

    xorps     xmm0,xmm0                  ; clear SIMD register
    xorps     xmm1,xmm1
    xorps     xmm2,xmm2
    xorps     xmm3,xmm3
    xorps     xmm4,xmm4
    xorps     xmm5,xmm5
    xorps     xmm6,xmm6
    xorps     xmm7,xmm7

    prefetchnta [vertexPtr]               ; pre-load first group of pointers

win_coords_loop_D:

    sub       vertexCount, 15            ; vertexCount >= 15 ? CF=0 : CF=1
    mov       ecx, [gc + vertexSize]     ; bytes of data for each vertex 

    sbb       eax, eax                   ; vertexCount >= 15 ? 00000000:ffffffff

    and       vertexCount, eax           ; vertexCount >= 15 ? 0 : vertexcount-15
    add       vertexCount, 15            ; vertexcount >= 15 ? 15 :vertexcount

    imul      ecx, vertexCount           ; total amount of vertex data we'll send

    mov       eax, [gc + fifoRoom]       ; fifo space available
    add       ecx, 4                     ; add header size ==> total packet size

    cmp       eax, ecx                   ; fifo space avail >= packet size ?
    jge       win_strip_begin_D          ; yup, start writing strip data

    push      @Line                      ; line number inside this function
    push      0h                         ; pointer to function name = NULL

    push      ecx                        ; fifo space needed
    call      __grCommandTransportMakeRoom@12 ; note: updates fifoPtr

        align 32
win_strip_begin_D:
;;;     Setup packet header
;;;
    mov       eax, vertexCount           ; number of vertices in strip/fan
    mov       edx, [esp + _type]         ; setup mode

    mov       fifo, [gc + fifoPtr]       ; get fifoPtr
    shl       edx, 22                    ; <27:22> = setup mode (kSetupStrip or kSetupFan)

    mov       ebp, [gc + cullStripHdr]   ; <2:0> = type
    shl       eax, 6                     ; <9:6> = vertex count (max 15)

    or        eax, edx                   ; setup mode and vertex count
    mov       edx, [esp + _pktype]       ; <5:3> = command (SSTCP_PKT3_BDDBDD, SSTCP_PKT3_BDDDDD, or SSTCP_PKT3_DDDDDD)

    or        eax, ebp                   ; setup mode, vertex count, and type
    mov       ebp, 4                     ; test bit 2

    or        eax, edx                   ; setup mode, vertex count, type, and command
    lea       dlpStart, [gc+tsuDataList] ; pointer to start of offset list

    test      fifo, ebp                  ; fifoPtr QWORD aligned ?
    jz        fifo_aligned_D             ; yup

    mov       [fifo], eax                ; PCI write packet type
    add       fifo, 4                    ; fifo pointer now QWORD aligned

;;;     for (k = 0; k < vcount; k++) {
;;;       FxI32 i;
;;;       FxU32 dataElem;
;;;       float *vPtr;
;;;       vPtr = pointers;
;;;       if (mode)
;;;         vPtr = *(float **)vPtr
;;;       (float *)pointers += stride;
;;;       TRI_SETF(FARRAY(vPtr, 0));
;;;       dataElem = 0;
;;;       TRI_SETF(FARRAY(vPtr, 4));
;;;       i = gc->tsuDataList[dataElem];


win_vertex_loop_D_WB0:                   ; nothing in "write buffer"

    mov       edx, [vertexPtr]           ; dereference pointer, edx points to vertex
    add       vertexPtr, 4               ; next pointer

    lea       dlp, [gc + tsuDataList]    ; get pointer to offset list; dlp ++
    add       dlp, 4                     ; dlp ++

    movlps    xmm1,[edx + X]             ; get vertex x,y
    add       fifo, 8                    ; fifo += 2

    mov       eax, [dlp - 4]             ; get first offset from offset list
    movlps    [fifo-8],xmm1              ; PCI write x, y

    test      eax, eax                   ; if offset == 0, end of offset list
    je        win_datalist_end_D_WB0     ; no more vertex data, nothing in "write buffer" 

;;;       while (i != GR_DLIST_END) {
;;;         TRI_SETF(FARRAY(vPtr, i));
;;;         dataElem++;
;;;         i = gc->tsuDataList[dataElem];
;;;       }

win_datalist_loop_D_WB0:                 ; nothing in "write buffer"

    movss     xmm1,[edx + eax]           ; get next parameter
    mov       eax, [dlp]                 ; get next offset from offset list

    test      eax, eax                   ; at end of offset list (offset == 0) ?
    jz        win_datalist_end_D_WB1     ; exit, write buffer contains one DWORD

    add       dlp, 8                     ; dlp++
    movss     xmm2,[edx + eax]           ; get next parameter

    mov       eax, [dlp-4]               ; get next offset from offset list
    add       fifo, 8                    ; fifoPtr += 2*sizeof(FxU32)

    unpcklps  xmm1,xmm2                  ; current param | previous param
    test      eax, eax                   ; at end of offset list (offset == 0) ?

    movlps    [fifo-8],xmm1              ; PCI write current param | previous param
    jnz       win_datalist_loop_D_WB0    ; nope, copy next parameter

win_datalist_end_D_WB0:

    dec       vertexCount                ; another vertex done. Any left?
    jnz       win_vertex_loop_D_WB0      ; yup, output next vertex

win_vertex_end_D_WB0:

;;;       TRI_END;
;;;     Prepare for the next packet (if the strip size is longer than 15)
;;;       GR_CHECK_SIZE();
;;;       count -= 15;
;;;       pktype = SSTCP_PKT3_DDDDDD;
;;;     }

    mov       ebp, 16                    ; pktype = SSTCP_PKT3_DDDDDD (strip continuation)
    mov       eax, [gc + fifoPtr]        ; old fifoPtr

    mov       [esp + _pktype], ebp       ; pktype = SSTCP_PKT3_DDDDDD (strip continuation)
    mov       ebp, [gc + fifoRoom]       ; old number of bytes available in fifo

    mov       vertexCount, [esp + _count]; remaining vertices before previous loop
    sub       eax, fifo                  ; old fifoPtr - new fifoPtr (fifo room used)

    add       ebp, eax                   ; new number of bytes available in fifo
    mov       [gc + fifoPtr], fifo       ; save current fifoPtr 

    sub       vertexCount, 15            ; remaining number of vertices to process
    mov       [gc + fifoRoom], ebp       ; save current number of bytes available in fifo

    mov       [esp + _count], vertexCount; remaining number of vertices to process 
    test      vertexCount, vertexCount   ; any vertices left to process ?

    nop                                  ; filler
    jg        win_coords_loop_D          ; loop if number of vertices to process >= 0

    pop       ebx                        ; restore caller's register variable
    pop       ebp                        ; restore frame pointer

    pop       esi                        ; restore caller's register variable
    pop       edi                        ; restire caller's register variable

    ret       20                         ; return, pop 5 DWORD parameters off stack

    ALIGN 32

fifo_aligned_D:

    mov       [fifo], eax                ; PCI write packet header
    mov       edx, [vertexPtr]           ; dereference pointer, edx points to vertex
    add       vertexPtr, 4               ; next pointer

    add       fifo, 8                    ; fifoPtr += 2*sizeof(FxU32)
    lea       dlp, [gc + tsuDataList]    ; get pointer to start of offset list

    movss     xmm2,[edx + X]             ; 0 | x of vertex
    add       dlp, 4                     ; dlp++
 
    mov       eax, [dlp-4]               ; first offset in offset list

    movss     [fifo-4], xmm2             ; PCI write x of vertex
    movss     xmm1,[edx + Y]             ; 0 | y of vertex

    test      eax, eax                   ; offset == 0 (list empty) ?
    je        win_datalist_end_D_WB1     ; yup, no more vertex data, one DWORD in "write buffer"
    jmp       win_datalist_loop_D_WB1

win_vertex_loop_D_WB1:                   ; one DWORD in "write buffer"

    mov       edx, [vertexPtr]           ; dereference pointer, edx points to vertex
    add       vertexPtr, 4               ; next pointer

    add       fifo, 8                    ; fifoPtr += 2*sizeof(FxU32)
    lea       dlp, [gc + tsuDataList]    ; get pointer to start of offset list

    movss     xmm2,[edx + X]             ; 0 | x of vertex
    add       dlp, 4                     ; dlp++
 
    mov       eax, [dlp-4]               ; first offset in offset list
    unpcklps  xmm1,xmm2                  ; packet header | x of vertex ?reversed?

    movlps    [fifo-8],xmm1              ; PCI write packet header | x of vertex
    movss     xmm1,[edx + Y]             ; 0 | y of vertex

    test      eax, eax                   ; offset == 0 (list empty) ?
    je        win_datalist_end_D_WB1     ; yup, no more vertex data, one DWORD in "write buffer"

;;;       while (i != GR_DLIST_END) {
;;;         TRI_SETF(FARRAY(vPtr, i));
;;;         dataElem++;
;;;         i = gc->tsuDataList[dataElem];
;;;       }

win_datalist_loop_D_WB1:                ; one DWORD in "write buffer" = MM1

    movss     xmm2,[edx + eax]          ; get next parameter
    add       fifo, 8                   ; fifoPtr += 2*sizeof(FxU32)

    mov       eax, [dlp]                ; get next offset from offset list
    add       dlp, 8                    ; dlp += 2

    unpcklps  xmm1,xmm2                 ; current param | previous param
    cmp       eax, 0                    ; at end of offset list (offset == 0) ?

    movlps    [fifo-8],xmm1             ; PCI write current param | previous param
    jz        win_datalist_end_D_WB0    ; yes, exit, "write buffer" empty

    movss     xmm1,[edx + eax]          ; get next parameter
    mov       eax, [dlp-4]              ; get next offset from offset list

    cmp       eax, 0                    ; at end of offset list (offset == 0) ?
    jnz       win_datalist_loop_D_WB1   ; nope, copy next parameter

win_datalist_end_D_WB1:

    dec       vertexCount               ; another vertex done. Any left?
    jnz       win_vertex_loop_D_WB1     ; yup, output next vertex

win_vertex_end_D_WB1:

    movss     [fifo],xmm1               ; flush "write buffer"
    add       fifo, 4                   ; fifoPtr++

;;;       TRI_END;
;;;     Prepare for the next packet (if the strip size is longer than 15)
;;;       GR_CHECK_SIZE();
;;;       count -= 15;
;;;       pktype = SSTCP_PKT3_DDDDDD;
;;;     }

    mov       ebp, 16                    ; pktype = SSTCP_PKT3_DDDDDD (strip continuation)
    mov       eax, [gc + fifoPtr]        ; old fifoPtr

    mov       [esp+_pktype], ebp         ; pktype = SSTCP_PKT3_DDDDDD (strip continuation)
    mov       ebp, [gc + fifoRoom]       ; old number of bytes available in fifo

    mov       vertexCount, [esp + _count]; remaining vertices before previous loop
    nop                                  ; filler

    sub       eax, fifo                  ; old fifoPtr - new fifoPtr (fifo room used)
    mov       [gc + fifoPtr], fifo       ; save current fifoPtr 

    sub       vertexCount, 15            ; remaining number of vertices to process
    add       ebp, eax                   ; new number of bytes available in fifo

    mov       [gc + fifoRoom], ebp       ; save current number of bytes available in fifo
    cmp       vertexCount, 0             ; any vertices left to process ?

    mov       [esp + _count], vertexCount; remaining number of vertices to process 
    jg        win_coords_loop_D          ; loop if number of vertices to process >= 0

strip_done: 
    pop       ebx                        ; restore caller's register variable
    pop       ebp                        ; restore frame pointer

    pop       esi                        ; restore caller's register variable
    pop       edi                        ; restire caller's register variable

    ret       20                         ; return, pop 5 DWORD parameters off stack

__grDrawVertexList_SSE_Window@20 ENDP


   

    PUBLIC  __grDrawVertexList_SSE_Clip@20
__grDrawVertexList_SSE_Clip@20 PROC NEAR
; 132  : {

    ALIGN 32

    mov       edx, DWORD PTR fs:[18h]    ; get thread local storage base pointer        
    push      edi                        ; save caller's register variable

    push      esi                        ; save caller's register variable
    mov       vertexCount, [esp+_count-8]; number of vertices in strip/fan

    push      ebp                        ; save frame pointer
    mov       ebp, [__GlideRoot + tlsOffset]; GC position relative to tls base    

    push      ebx                        ; save caller's register variable        
    mov       vertexPtr, [esp+_pointers] ; get current vertex pointer (deref mode)
                                         ; get current vertex (non-deref mode)
    
    mov       gc, [edx + ebp]            ; get current graphics context from tls
    test      vertexCount, vertexCount   ; number of vertices <= 0 ?

    jle       strip_done                 ; yup, the strip/fan is done
  
;;;     vSize = gc->state.vData.vSize
;;;     if (stride == 0)
;;;       stride = gc->state.vData.vStride;

;; We can operate in one of two modes:
;;
;; 0. We are stepping through an array of vertices, in which case
;; the stridesize is equal to the size of the vertex data, and
;; always > 4, since vertex data must a least contain x,y (ie 8 bytes).
;; vertexPtr is pointing to the array of vertices.
;;
;; 1. We are stepping through an array of pointers to vertices
;; in which case the stride is 4 bytes and we need to dereference
;; the pointers to get at the vertex data. vertexPtr is pointing
;; to the array of pointers to vertices.

    mov       edx, [esp + _mode]         ; get mode (0 or 1)
    mov       eax, [gc + vertexSize]     ; size of vertex data in bytes

    test      edx, edx                   ; mode 0 (array of vertices) ?
    mov       edx, [gc + vertexStride]   ; get stride in DWORDs

    movss     xmm6,[__GlideRoot+pool_f255]; GlideRoot.pool.f255     
    mov       [strideinbytes], 4         ; array of pointers    
        
    jnz       clip_coords_begin          ; nope, it's mode 1

clip_coordinates_ND:

    shl       edx, 2                     ; stride in bytes
    mov       [strideinbytes], edx       ; save off stride (in bytes)

    align   32
clip_coords_begin:

dataElem      textequ <ebp>              ; number of vertex components processed    

;;;   {
;;;     float oow;
;;;       while (count > 0) {
;;;       FxI32 k, vcount = count >= 15 ? 15 : count;

    sub       vertexCount, 15            ; vertexCount >= 15 ? CF=0 : CF=1
    mov       ecx, [gc + vertexSize]     ; bytes of data for each vertex 

    sbb       eax, eax                   ; vertexCount >= 15 ? 00000000:ffffffff
    and       vertexCount, eax           ; vertexCount >= 15 ? 0 : vertexcount-15

    mov       eax, [gc+fifoRoom]         ; fifo space available
    add       vertexCount, 15            ; vertexcount >= 15 ? 15 :vertexcount

    imul      ecx, vertexCount           ; total amount of vertex data we'll send

    add       ecx, 4                     ; add header size ==> total packet size
    nop                                  ; filler

    cmp       eax, ecx                   ; fifo space avail >= packet size ?
    jge       clip_strip_begin           ; yup, start writing strip data

    push      @Line                      ; line number inside this function
    push      0h                         ; pointer to function name = NULL

    push      ecx                        ; fifo space needed
    call      __grCommandTransportMakeRoom@12 ; note: updates fifoPtr

        align 32
clip_strip_begin:
;;;     TRI_STRIP_BEGIN(type, vcount, vSize, pktype)

    mov       edx, [esp + _type]         ; setup mode
    mov       eax, vertexCount           ; number of vertices in strip/fan

    mov       fifo, [gc + fifoPtr]       ; get fifoPtr
    shl       eax, 6                     ; <9:6> = vertex count (max 15)

    mov       ebp, [gc + cullStripHdr]   ; <2:0> = type
    shl       edx, 22                    ; <27:22> = setup mode (kSetupStrip or kSetupFan)

    or        eax, edx                   ; setup mode and vertex count
    mov       edx, [esp + _pktype]       ; <5:3> = command (SSTCP_PKT3_BDDBDD, SSTCP_PKT3_BDDDDD, or SSTCP_PKT3_DDDDDD)

    or        eax, ebp                   ; setup mode, vertex count, and type
    add       fifo, 4                    ; fifoPtr += sizeof(FxU32)

    or        eax, edx                   ; setup mode, vertex count, type, and command
    mov       [fifo-4], eax              ; PCI write header

;;;     for (k = 0; k < vcount; k++) {
;;;       float *vPtr
;;;       vPtr = pointers
  
clip_for_begin:

;;;       if (mode)
;;;         vPtr = *(float **)vPtr

    mov       edx, vertexPtr             ; vertex = vertexPtr (assume no-deref mode)
    mov       eax, [esp+_mode]           ; mode 0 = no deref, mode 1 = deref

    mov       [vertices], vertexCount    ; save numnber of vertices
    test      eax, eax                   ; deref mode ?

    mov       eax, [gc+wInfo_offset]     ; get offset of W into vertex struct
    jz        clip_noderef               ; yup, no-deref mode

    mov       edx, [vertexPtr]           ; vertex = *vertexPtr
    lea       esp, [esp]                 ; filler

clip_noderef:

;;;       oow = 1.0f / FARRAY(vPtr, gc->state.vData.wInfo.offset)

    movss     xmm1,[edx+eax]             ; 0 | W of current vertex
    rcpss     xmm0,xmm1                  ; 0 | 1/W approx

    mov       ebp, [strideinbytes]       ; offset to next vertex/vertexPtr
    movlps    xmm2,[edx]                 ; y | x of current vertex

    movlps    xmm3,[gc+vp_hwidth]        ; gc->state.Viewport.hheight | gc->state.Viewport.hwidth

    movlps    xmm4,[gc+vp_ox]            ; gc->state.Viewport.oy | gc->state.Viewport.ox
    add       vertexPtr, ebp             ; point to next vertex/VertexPtr

    mulss     xmm1,xmm0                  ; 0 | 1/W refine
    mulss     xmm1,xmm0
    addss     xmm0,xmm0
    subss     xmm0,xmm1                  ; oow = 1.0f / FARRAY(vPtr, gc->state.vData.wInfo.offset
    mov       esi, [gc+paramIndex]       ; gc->state.paramIndex

;;;       /* x, y */
;;;       TRI_SETF(FARRAY(vPtr, 0)
;;;         *oow*gc->state.Viewport.hwidth + gc->state.Viewport.ox)
;;;       TRI_SETF(FARRAY(vPtr, 4)
;;;         *oow*gc->state.Viewport.hheight + gc->state.Viewport.oy)

    mulps     xmm2,xmm3                  ; TRI_SETF(FARRAY(vPtr,0)*state.Viewport.hheight | TRI_SETF(FARRAY(vPtr,4)*state.Viewport.hwidth
    xor       dataElem, dataElem         ; dataElem = 0

    add       fifo, 8                    ; fifoPtr += 2*sizeof(FxFloat)
    unpcklps  xmm0,xmm0                  ; oow | oow

    mulps     xmm2,xmm0                  ; TRI_SETF(FARRAY(vPtr, 4)*oow*gc->state.Viewport.height | TRI_SETF(FARRAY(vPtr, 0)*oow*gc->state.Viewport.hwidth
    addps     xmm2,xmm4                  ; TRI_SETF(FARRAY(vPtr, 4)*oow*gc->state.Viewport.hheight + gc->state.Viewport.oy) |

    test      esi, 3                     ; STATE_REQUIRES_IT_DRGB | STATE_REQUIRES_IT_ALPHA ?
    mov       eax, [gc+tsuDataList]      ; first entry from offset list

;;;       (float *)pointers += stride
;;;       TRI_VP_SETFS(vPtr, oow);

    movlps    [fifo-8],xmm2              ; PCI write transformed x, y
    jz        clip_setup_ooz             ; nope, no color at all needed
  
    cmp       DWORD PTR [gc+colorType], 0; gc->state.vData.colorType == GR_FLOAT ?
    jne       clip_setup_pargb           ; nope, packed ARGB format
  
    test      esi, 1                     ; STATE_REQUIRES_IT_DRGB ?
    jz        clip_setup_a               ; no, but definitely A

    movss     xmm2,[edx + eax]           ; 0 | r
    mov       eax, [gc+tsuDataList+4]    ; offset of g part of vertex data

    mulss     xmm2,xmm6                  ; 0 | r * 255.0f
    movss     xmm3,[edx + eax]           ; 0 | g

    mov       eax, [gc+tsuDataList+8]    ; offset of b part of vertex data
    movss     [fifo],xmm2                ; PCI write r*255

    mulss     xmm3,xmm6                  ; 0 | g * 255.0f
    movss     xmm2,[edx + eax]           ; 0 | b

    movss     [fifo+4],xmm3              ; PCI write g*255
    mov       dataElem, 12               ; dataElem = 3

    mulss     xmm2,xmm6                  ; 0 | b * 255.0f
    mov       eax, [gc+tsuDataList+12]   ; offset of A part of vertex data

    test      esi, 2                     ; STATE_REQUIRES_IT_ALPHA ?
    lea       fifo, [fifo+12]            ; fifoPtr += 3*sizeof(FxFloat)

    movss     [fifo-4],xmm2              ; PCI write b*255
    jz        clip_setup_ooz             ; nope, no alpha, proceeed with ooz

clip_setup_a:
    movss     xmm2,[eax+edx]             ; 0 | a
    add       fifo, 4                    ; fifoPtr += sizeof(FxFloat)

    mov       esp, esp                   ; filler
    add       dataElem, 4                ; dataElem++ 

    mulss     xmm2,xmm6                  ; 0 | a * 255.0f
    mov       eax, [gc+dataElem+tsuDataList]; offset of next part of vertex data

    movss     [fifo-4],xmm2              ; PCI write a*255
    jmp       clip_setup_ooz             ; check whether we need to push out z

    ALIGN     32

clip_setup_pargb:
    mov       dataElem,[eax+edx]         ; get packed ARGB data
    add       fifo, 4                    ; fifoPtr += sizeof(FxU32)
    mov       [fifo-4],dataElem          ; PCI write packed ARGB

    mov       dataElem, 4                ; dataElem = 1 (namely pargb)
    mov       eax, [gc+tsuDataList+4]    ; offset of next part of vertex data

clip_setup_ooz:
  
    test      esi, 4                     ; STATE_REQUIRES_OOZ ?
    jz        clip_setup_qow             ; nope

    test      DWORD PTR[gc+fbi_fbzMode],200000h ; gc->state.fbi_config.fbzMode & SST_DEPTH_FLOAT_SEL != 0 ?
    je        clip_setup_ooz_nofog       ; nope

    cmp       DWORD PTR[gc+qInfo_mode], 0; gc->state.vData.qInfo.mode == GR_PARAM_ENABLE ?
    jz        clip_setup_fog_oow         ; nope

    mov       eax, [gc + qInfo_offset]   ; offset of Q component of vertex
    add       fifo, 4                    ; fifoPtr += sizeof(FxFloat)

    add       dataElem, 4                ; dataElem++
    movss     xmm2,[edx + eax]           ; 0 | q of vertex

    mov       eax,[gc+dataElem+tsuDataList]; pointer to next vertex component
    mulss     xmm2,xmm0                  ; 0 | q*oow

    movss     [fifo-4],xmm2              ; PCI write transformed Q
    jmp       clip_setup_qow             ; check whether we need to write Q or W

clip_setup_fog_oow:

    add       fifo, 4                    ; fifoPtr += sizeof(FxFloat) 
    add       dataElem, 4                ; dataElem++

    movss     xmm3,[gc + depth_range]    ;
    mulss     xmm3,xmm0                  ; 

    movss     xmm4,[gc + depth_range]    ; depth range
    subss     xmm4,xmm3                  ; 

    movss     [fifo-4],xmm4              ; PCI write oow
    mov       eax,[gc+dataElem+tsuDataList]; pointer to next vertex component

    jmp       clip_setup_qow             ; check whether we need to write Q or W

clip_setup_ooz_nofog:

    movss     xmm2,[eax + edx]           ; 0 | z component of vertex
    add       fifo, 4                    ; fifoPtr += sizeof(FxFloat)

    add       dataElem, 4                ; dataElem += 1
    movss     xmm3,[gc + vp_hdepth]      ; 0 | gc->state.Viewport.hdepth

    mulss     xmm2,xmm0                  ; TRI_SETF(FARRAY(_s, i)*_oow
    movss     xmm4,[gc + vp_oz]          ; 0 | gc->state.Viewport.oz

    mulss     xmm2,xmm3                  ; 0 | TRI_SETF(FARRAY(_s, i)*_oow*gc->state.Viewport.hdepth
    mov       eax, [gc+dataElem+tsuDataList]; offset of next vertex component

    addss     xmm2,xmm4                  ; 0 | TRI_SETF(FARRAY(_s, i)*_oow*gc->state.Viewport.hdepth+gc->state.Viewport.oz
    movss     [fifo-4],xmm2              ; PCI write transformed Z

clip_setup_qow:
    test      esi, 8                     ; STATE_REQUIRES_OOW_FBI ?
    jz        clip_setup_qow0            ; nope

    cmp       DWORD PTR[gc+fogInfo_mode],0; gc->state.vData.fogInfo.mode == GR_PARAM_ENABLE ?
    jz        clip_setup_oow_nofog       ; nope, no fog

    mov       eax, [gc + fogInfo_offset] ; offset of fogInfo component of vertex
    add       fifo, 4                    ; fifoPtr += sizeof(FxFloat)

    add       dataElem, 4                ; dataElem++
    movss     xmm2,[edx + eax]           ; 0 | fogInfo of vertex

    mov       eax,[gc+dataElem+tsuDataList]; pointer to next vertex component
    mulss     xmm2,xmm0                  ; fogInfo*oow

    movss     [fifo-4],xmm2              ; PCI write transformed Q
    jmp       clip_setup_qow0            ; continue with q0

clip_setup_oow_nofog:

    cmp       DWORD PTR [gc+qInfo_mode],0; does vertex have Q component ?
    je        clip_setup_oow             ; nope, not Q but W

    add       fifo, 4                    ; fifoPtr += sizeof(FxFloat)
    mov       eax, [gc+qInfo_offset]     ; offset of Q component of vertex

    add       dataElem, 4                ; dataElem++
    movss     xmm2,[edx+eax]             ; 0 | q of vertex

    mov       eax,[gc+dataElem+tsuDataList]; pointer to next vertex component
    mulss     xmm2,xmm0                  ; q*oow

    movss     [fifo-4],xmm2              ; PCI write transformed Q
    jmp       clip_setup_qow0            ; continue with q0

    ALIGN     32

clip_setup_oow:
    add       fifo, 4                    ; fifoPtr += sizeof(FxFloat) 
    add       dataElem, 4                ; dataElem++

    movss     [fifo-4],xmm0              ; PCI write oow
    mov       eax,[gc+dataElem+tsuDataList]; pointer to next vertex component

clip_setup_qow0:
    test      esi, 16                    ; STATE_REQUIRES_W_TMU0 ?
    jz        clip_setup_stow0           ; nope 

    cmp       DWORD PTR [gc+q0Info_mode],0; does vertex have Q component ?
    je        clip_setup_oow0            ; nope, not Q but W

    mov       eax, [gc+q0Info_offset]    ; offset of Q component of vertex
    add       fifo, 4                    ; fifoPtr += sizeof(FxFloat)

    add       dataElem, 4                ; dataElem++
    movss     xmm2,[edx+eax]             ; 0 | q0 of vertex

    mov       eax,[gc+dataElem+tsuDataList]; pointer to next vertex component
    mulss     xmm2,xmm0                  ; q0*oow

    movss     [fifo-4],xmm2              ; PCI write transformed q0
    jmp       clip_setup_stow0           ; continue with stow0

    ALIGN     32

clip_setup_oow0:
    add       fifo, 4                    ; fifoPtr += sizeof(FxFloat) 
    add       dataElem, 4                ; dataElem++

    movss     [fifo-4],xmm0              ; PCI write oow
    mov       eax,[gc+dataElem+tsuDataList]; pointer to next vertex component

clip_setup_stow0:

    test      esi, 32                    ; STATE_REQUIRES_ST_TMU0 ?
    jz        clip_setup_qow1            ; nope

    movlps    xmm7,[gc + tmu0_s_scale]   ; state.tmu_config[0].t_scale | state.tmu_config[0].s_scale
    add       fifo, 8                    ; fifoPtr += 2*sizeof(FxFloat)

    movss     xmm2,[edx+eax]             ; param1
    mov       eax,[gc+dataElem+tsuDataList+4];pointer to next vertex component

    mulps     xmm7,xmm0                  ; oow*tmu0_t_scale | oow*tmu0_s_scale
    add       dataElem, 8                ; dataElem += 2

    movss     xmm3,[edx+eax]             ; param2
    unpcklps  xmm2,xmm3                  ; param2 | param1

    mulps     xmm2,xmm7                  ; param2*oow*tmu0_t_scale | param1*oow*tmu0_s_scale

    movlps    [fifo-8],xmm2              ; PCI write param2*oow*tmu0_t_scale | param1*oow*tmu0_s_scale 
    mov       eax, [gc+dataElem+tsuDataList]; pointer to next vertex component

clip_setup_qow1:
    test      esi, 64                    ; STATE_REQUIRES_W_TMU1 ?
    jz        clip_setup_stow1           ; nope

    cmp       DWORD PTR [gc+q1Info_mode],0; does vertex have Q component ?
    je        clip_setup_oow1            ; nope, not Q but W

    mov       eax, [gc+q1Info_offset]    ; offset of Q component of vertex
    add       fifo, 4                    ; fifoPtr += sizeof(FxFloat)

    add       dataElem, 4                ; dataElem++
    movss     xmm2,[edx+eax]             ; 0 | q1 of vertex

    mov       eax,[gc+dataElem+tsuDataList]; pointer to next vertex component
    mulss     xmm2,xmm0                  ; q1*oow

    movss     [fifo-4],xmm2              ; PCI write transformed q1
    jmp       clip_setup_stow1           ; continue with stow1

    ALIGN     32

clip_setup_oow1:
    add       fifo, 4                    ; fifoPtr += sizeof(FxFloat) 
    add       dataElem, 4                ; dataElem++

    movss     [fifo-4],xmm0              ; PCI write oow
    mov       eax,[gc+dataElem+tsuDataList]; pointer to next vertex component

clip_setup_stow1:

    test      esi, 128                   ; STATE_REQUIRES_ST_TMU1 ?
    mov       vertexCount, [vertices]    ; get number of vertices

    movlps    xmm7,[gc + tmu1_s_scale]   ; state.tmu_config[1].t_scale | state.tmu_config[1].s_scale
    jz        clip_setup_end             ; nope

    movss     xmm2,[edx+eax]             ; param1
    add       fifo, 8                    ; fifoPtr += 2*sizeof(FxFloat)

    mov       eax,[gc+dataElem+tsuDataList+4]; pointer to next vertex component
    mulps     xmm7,xmm0                  ; oow*state.tmu_config[1].t_scale | oow*state.tmu_config[1].s_scale

    movss     xmm3,[edx+eax]             ; param2
    unpcklps  xmm2,xmm3                  ; param2 | param1

    mulps     xmm2,xmm7                  ; param2*oow*state.tmu_config[1].t_scale | param1*oow*state.tmu_config[1].s_scale
    movlps    [fifo-8],xmm2              ; PCI write param2*oow*state.tmu_config[1].t_scale | param1*oow*state.tmu_config[1].s_scale

clip_setup_end:

; 206  :       for (k = 0; k < vcount; k++) {

    dec       vertexCount                ; vcount--
    jnz       clip_for_begin             ; until 
clip_for_end:

; 221  :       }
; 222  :       TRI_END;

    mov       eax, [gc + fifoPtr]        ; old fifoPtr
    mov       esp, esp                   ; filler

    nop                                  ; filler
    mov       ebp, [gc + fifoRoom]       ; old number of bytes available in fifo

    mov       vertexCount, [esp + _count]; remaining vertices before previous loop
    sub       eax, fifo                  ; old fifoPtr - new fifoPtr (fifo room used)

    mov       [gc + fifoPtr], fifo       ; save current fifoPtr 
    add       ebp, eax                   ; new number of bytes available in fifo

    sub       vertexCount, 15            ; remaining number of vertices to process
    mov       [gc + fifoRoom], ebp       ; save current number of bytes available in fifo

    mov       [esp + _count], vertexCount; remaining number of vertices to process 
    cmp       vertexCount, 0             ; any vertices left to process ?

    mov       DWORD PTR [esp+_pktype], 16; pktype = SSTCP_PKT3_DDDDDD (strip continuation)
    jg        clip_coords_begin          ; loop if number of vertices to process >= 0

strip_done:
;;;    }
;;;  #undef FN_NAME
;;;  } /* _grDrawVertexList */

    pop       ebx                        ; restore caller's register variable
    pop       ebp                        ; restore frame pointer 

    pop       esi                        ; restore caller's register variable
    pop       edi                        ; restore caller's register variable

    ret       20                         ; return, pop 5 DWORD parameters off stack

__grDrawVertexList_SSE_Clip@20 ENDP

_TEXT    ENDS

;;--------------------------------------------------------------------------
;; end SSE version
;;--------------------------------------------------------------------------
endif ; GL_SSE

;;--------------------------------------------------------------------------
;; start original code
;;--------------------------------------------------------------------------

ifndef GL_AMD3D
ifndef GL_MMX
ifndef GL_SSE

TITLE   xdraw3.asm
.586P
;;; include listing.inc
INCLUDE fxgasm.h

CONST   SEGMENT
_F1    DD      03f800000r                      ; 1
_F256  DD      043800000r                      ; 256

_VPF1    DD      03f800000r                      ; 1
_VPF256  DD      043800000r                      ; 256    
CONST   ENDS

_DATA   SEGMENT
vSize           DD    0
ccoow           DD    0
packetVal       DD    0
strideinbytes   DD    0


oowa            DD    0
vPtr0           DD    0
vPtr1           DD    0
vPtr2           DD    0
_DATA    ENDS

_TEXT       SEGMENT PAGE PUBLIC USE32 'CODE'
            ASSUME DS: FLAT, SS: FLAT
    
_pktype = 20
_type = 24
_mode = 28
_count = 32
_pointers = 36

gc                 TEXTEQU     <esi>       ; points to graphics context
fifo               TEXTEQU     <ecx>       ; points to next entry in fifo
dlp                TEXTEQU     <ebp>       ; points to dataList structure
vertexCount        TEXTEQU     <ebx>       ; Current vertex counter in the packet
vertexPtr          TEXTEQU     <edi>       ; Current vertex pointer

        ALIGN 32

        PUBLIC __drawvertexlist@20
__drawvertexlist@20 PROC NEAR
; 132  : {

        mov     eax, DWORD PTR fs:[18h]; get thread local storage base pointer        
        push    esi

        mov     esi, [__GlideRoot + tlsOffset]; GC position relative to tls base
        push    edi
    
        mov     gc, DWORD PTR [eax + esi]    
        push    ebx

;;;     GR_DCL_GC
;;;     vSize = gc->state.vData.vSize
;;;     if (stride == 0)
;;;       stride = gc->state.vData.vStride;
        push    ebp            
        mov     ecx, DWORD PTR [gc+vertexSize]
    
        mov     edx, DWORD PTR [esp+_mode]
        mov     vertexCount, DWORD PTR [esp+_count]
    
        mov     vertexPtr, DWORD PTR [esp+_pointers]
        mov     DWORD PTR vSize, ecx

        shl     edx, 2
;;;     mov     ecx, DWORD PTR [gc+CoordinateSpace]
          test    edx, edx
        jne     SHORT no_stride
        mov     edx, DWORD PTR [gc+vertexStride]
        shl     edx, 2

        align 4
no_stride:

;;;     Draw the first (or possibly only) set.  This is necessary because
;;;     the packet is 3_BDDDDDD, and in the next set, the packet is 3_DDDDDD
;;;     We try to make tstrip code simple to read. We combine the original code
;;;     into a single loop by adding an extra packet type assignment at the end of the loop.
;;; 
;;;     if (gc->state.grCoordinateSpaceArgs.coordinate_space_mode == GR_WINDOW_COORDS) {

;;;     test    ecx, ecx
          mov     DWORD PTR strideinbytes, edx

;;;       while (count > 0) {
;;;         FxI32 k, vcount = count >= 15 ? 15 : count;
;;;         GR_SET_EXPECTED_SIZE(vcount * vSize, 1);
;;;         TRI_STRIP_BEGIN(type, vcount, vSize, pktype);

        mov     eax, DWORD PTR [esp+_count]
;;;       jne     clip_coordinates

        test    eax, eax
          jle     strip_done

        align 4
window_coords_begin:

        cmp     vertexCount, 15                 ; 0000000fH
          jl      SHORT win_partial_packet
        mov     vertexCount, 15                 ; 0000000fH

        align 4
win_partial_packet:

        mov     eax, DWORD PTR vSize
        mov     ecx, DWORD PTR [gc+fifoRoom]
        imul    eax, vertexCount
        add     eax, 4
        cmp     ecx, eax
        jge     SHORT win_strip_begin
        push    @Line
        push    0h
        push    eax
        call    __grCommandTransportMakeRoom@12
        
        align 4
win_strip_begin:

;;;     Setup pacet header
;;;
        mov     fifo, DWORD PTR [gc+fifoPtr]
          mov     eax, vertexCount
        mov     edx, DWORD PTR [esp+_type]
          mov     ebp, DWORD PTR [gc+cullStripHdr]
        shl     edx, 22                 ; 00000010H
          add     fifo, 4
        shl     eax, 6
          or    ebp, edx
        or      eax, ebp
          mov     edx, DWORD PTR [esp+_pktype]
        or      eax, edx
          nop
        mov     DWORD PTR [fifo-4], eax

;;;     for (k = 0; k < vcount; k++) {
;;;       FxI32 i;
;;;       FxU32 dataElem;
;;;       float *vPtr;
;;;       vPtr = pointers;
;;;       if (mode)
;;;         vPtr = *(float **)vPtr
;;;       (float *)pointers += stride;
;;;       TRI_SETF(FARRAY(vPtr, 0));
;;;       dataElem = 0;
;;;       TRI_SETF(FARRAY(vPtr, 4));
;;;       i = gc->tsuDataList[dataElem];

        align 4
win_for_begin:

        mov     edx, vertexPtr
          mov     eax, DWORD PTR strideinbytes
        cmp     eax, 4
          jne     SHORT win_no_deref
        mov     edx, DWORD PTR [vertexPtr]

        align 4
win_no_deref:

        add     fifo, 8
          add     vertexPtr, eax

        mov     eax, DWORD PTR [edx]
          mov     ebp, DWORD PTR [edx+4]
        
        mov     DWORD PTR [fifo-8], eax
          mov     eax, DWORD PTR [gc+tsuDataList]

        mov     DWORD PTR [fifo-4], ebp
  
          test    eax, eax
        lea     dlp, DWORD PTR [gc+tsuDataList]
          je      SHORT win_datalist_end
  
        align 4

;;;       while (i != GR_DLIST_END) {
;;;         TRI_SETF(FARRAY(vPtr, i));
;;;         dataElem++;
;;;         i = gc->tsuDataList[dataElem];
;;;       }

win_datalist_begin:

        add     fifo, 4
          add     dlp, 4

        mov     eax, DWORD PTR [edx+eax]
          nop

        mov     DWORD PTR [fifo-4], eax
          mov     eax, DWORD PTR [dlp]


        test    eax, eax
          jne     SHORT win_datalist_begin
win_datalist_end:

        dec     vertexCount
          jne     SHORT win_for_begin
win_for_end:

;;;       TRI_END;
;;;     Prepare for the next packet (if the strip size is longer than 15)
;;;       GR_CHECK_SIZE();
;;;       count -= 15;
;;;       pktype = SSTCP_PKT3_DDDDDD;
;;;     }
  
        mov     eax, DWORD PTR [gc+fifoPtr]
          mov     edx, DWORD PTR [gc+fifoRoom]
        sub     eax, fifo
          mov     vertexCount, DWORD PTR [esp+_count]
        add     edx, eax
          sub     vertexCount, 15                 ; 0000000fH
  
        mov     DWORD PTR [gc+fifoRoom], edx
          mov     DWORD PTR [esp+_count], vertexCount
  
        mov     DWORD PTR [gc+fifoPtr], fifo
          test    vertexCount, vertexCount
  
        mov     DWORD PTR [esp+_pktype], 16 ; 00000010H
          jg      window_coords_begin

strip_done:
        pop     ebp
          pop     ebx
        pop     edi
          pop     esi
        ret     20                      ; 00000014H

__drawvertexlist@20 ENDP

_pktype = 20
_type = 24
_mode = 28
_count = 32
_pointers = 36

gc                 TEXTEQU     <esi>       ; points to graphics context
fifo               TEXTEQU     <ecx>       ; points to next entry in fifo
vertexPtr          TEXTEQU     <edx>       ; pointer to vertex or vertex array

        ALIGN 32    

        PUBLIC __vpdrawvertexlist@20
__vpdrawvertexlist@20 PROC NEAR

        mov     eax, DWORD PTR fs:  [18h]   ; tls base pointer
        push    esi

        mov     esi, [__GlideRoot + tlsOffset]; gc position relative to tls base
        push    edi

        push    ebx        
        mov     gc, [eax + esi]
        
        push    ebp
        mov     ecx, DWORD PTR [esp+_mode]
        
        mov     edi, DWORD PTR [esp+_pointers]
        mov     eax, DWORD PTR [gc+wInfo_offset]
    
        test    ecx, ecx
        je      w_no_dref
    
        mov     edi, DWORD PTR [edi]
        
        align   4
w_no_dref:
        
;;;     load first w
        
        fld     DWORD PTR [edi+eax]
        fdivr   DWORD PTR _F1
        
        mov     ecx, DWORD PTR [gc+vertexSize]
        mov     edx, DWORD PTR [esp+_mode]

        mov     edi, DWORD PTR [esp+_count]
;;;     mov     vertexArray, DWORD PTR [esp+_pointers]

        shl     edx, 2
        mov     DWORD PTR vSize, ecx

        test    edx, edx

        jne     SHORT no_stride

        mov     edx, DWORD PTR [gc+vertexStride]
        shl     edx, 2

        align 4
no_stride:

        mov     DWORD PTR strideinbytes, edx
        mov     eax, DWORD PTR [esp+_type]

        shl     eax, 16                 ; 00000010H
        mov     DWORD PTR packetVal, eax

clip_coords_begin:

        cmp     edi, 15
        jl      SHORT clip_partial_packet
        mov     edi, 15
clip_partial_packet:

;;;     GR_SET_EXPECTED_SIZE(vcount * vSize, 1)

        mov     eax, DWORD PTR vSize
        mov     ecx, DWORD PTR [gc+fifoRoom]
        
        imul    eax, edi
        add     eax, 4
        cmp     ecx, eax
        jge     SHORT clip_strip_begin
        push    @Line
        push    0h
        push    eax
        call    __grCommandTransportMakeRoom@12

            align 4
clip_strip_begin:
;;;     TRI_STRIP_BEGIN(type, vcount, vSize, pktype)

  
        mov     fifo, DWORD PTR [gc+fifoPtr]
        mov     eax, edi
        
        mov     edx, DWORD PTR packetVal
        mov     ebp, DWORD PTR [gc+cullStripHdr]
        
        or      eax, edx
        add     fifo, 4
        
        shl     eax, 6
        mov     edx, DWORD PTR [esp+_pktype]
        
        or      eax, ebp

        or      eax, edx
        mov     DWORD PTR [fifo-4], eax

  
        mov     vertexPtr, DWORD PTR [esp+_pointers]
        mov     eax, DWORD PTR [esp+_mode]

        test    eax, eax
        
        je      SHORT clip_for_begin
        mov     vertexPtr, DWORD PTR [vertexPtr]

        align   4
clip_for_begin:

        add     fifo, 8
        mov     ebp, DWORD PTR strideinbytes

        add     DWORD PTR [esp+_pointers], ebp
        mov     eax, DWORD PTR [gc+paramIndex]
        
        xor     ebp, ebp
        mov     ebx, DWORD PTR [gc+tsuDataList]

;;; ;   setup x and y

        fld     DWORD PTR [gc+vp_hwidth]
        fmul    DWORD PTR [vertexPtr]
        fmul    st, st(1)
        fadd    DWORD PTR [gc+vp_ox]
        fxch    st(1)

        fld     DWORD PTR [gc+vp_hheight]
        fmul    DWORD PTR [vertexPtr+4]
        test    al, 3
        fmul    st, st(1)
        fadd    DWORD PTR [gc+vp_oy]
        fxch    st(1)
        fstp    DWORD PTR ccoow
        fxch    st(1)
        fstp    DWORD PTR [fifo-8]
        fstp    DWORD PTR [fifo-4]
        
;;; ;   set up color

        je      clip_setup_ooz
  
        cmp     DWORD PTR [gc+colorType], ebp
        jne     SHORT clip_setup_pargb
  
        test    al, 1
        je      SHORT clip_setup_a
        
        add     fifo, 12
        mov     ebp, 3

        fld     DWORD PTR __GlideRoot+pool_f255
        fmul    DWORD PTR [ebx+vertexPtr]
        fld     DWORD PTR __GlideRoot+pool_f255
        fmul    DWORD PTR [ebx+vertexPtr+4]
        fld     DWORD PTR __GlideRoot+pool_f255
        fmul    DWORD PTR [ebx+vertexPtr+8]
        fxch    st(2)
        fstp    DWORD PTR [fifo-12]
        fstp    DWORD PTR [fifo-8]
        fstp    DWORD PTR [fifo-4]
        mov     ebx, DWORD PTR [gc+tsuDataList+12]
        
        align 4
clip_setup_a:
  
        test    al, 2
        je      SHORT clip_setup_ooz

        add     fifo, 4
        inc     ebp

        fld     DWORD PTR [ebx+vertexPtr]  
        fmul    DWORD PTR __GlideRoot+pool_f255
        fstp    DWORD PTR [fifo-4]
  
        mov     ebx, DWORD PTR [gc+ebp*4+tsuDataList]
        jmp     SHORT clip_setup_ooz
        
        align 4
clip_setup_pargb:
        add     fifo, 4
        mov     ebx, DWORD PTR [ebx+vertexPtr]

        mov     DWORD PTR [fifo-4], ebx
        nop
        
        mov     ebp, 1  
        mov     ebx, DWORD PTR [gc+tsuDataList+4]
        align 4
clip_setup_ooz:
  
        test    al, 4
        je      SHORT clip_setup_qow

        add     fifo, 4
        inc     ebp

        test    DWORD PTR [gc+fbi_fbzMode], 200000h
        je      clip_setup_ooz_nofog

        mov     ebx, DWORD PTR [gc+qInfo_mode]
        test    ebx, ebx
        je      SHORT clip_setup_fog_oow
        mov     ebx, DWORD PTR [gc+qInfo_offset]

        fld     DWORD PTR [vertexPtr+ebx]
        fmul    DWORD PTR ccoow
        fstp    DWORD PTR [fifo-4]
        
        mov     ebx, DWORD PTR [gc+ebp*4+tsuDataList]
        jmp     clip_setup_qow

        align 4
clip_setup_fog_oow:

        fld     DWORD PTR _F1
        fsub    DWORD PTR ccoow
        fmul    DWORD PTR [gc+depth_range]
        fstp    DWORD PTR [fifo-4]

        mov     ebx, DWORD PTR [gc+ebp*4+tsuDataList]
        jmp     clip_setup_qow

        align 4
clip_setup_ooz_nofog:   
        
        fld     DWORD PTR [ebx+vertexPtr]  
        fmul    DWORD PTR [gc+vp_hdepth]
        fmul    DWORD PTR ccoow  
        fadd    DWORD PTR [gc+vp_oz]
        fstp    DWORD PTR [fifo-4]
  
        mov     ebx, DWORD PTR [gc+ebp*4+tsuDataList]
        align 4
clip_setup_qow:
  
        test    al, 8
        je      SHORT clip_setup_qow0
  
        mov     ebx, DWORD PTR [gc+fogInfo_mode]
        test    ebx, ebx
        je      SHORT clip_setup_oow_nofog
        mov     ebx, DWORD PTR [gc+fogInfo_offset]

        fld     DWORD PTR [vertexPtr+ebx]
        fmul    DWORD PTR ccoow
        fstp    DWORD PTR [fifo]

        jmp     SHORT clip_setup_oow_inc
        
        align 4
clip_setup_oow_nofog:
        
        mov     ebx, DWORD PTR [gc+qInfo_mode]
        test    ebx, ebx
        je      SHORT clip_setup_oow
        mov     ebx, DWORD PTR [gc+qInfo_offset]

        fld     DWORD PTR [vertexPtr+ebx]
        fmul    DWORD PTR ccoow
        fstp    DWORD PTR [fifo]

        jmp     SHORT clip_setup_oow_inc
        
        align 4
clip_setup_oow:
        mov     ebx, DWORD PTR ccoow

        mov     DWORD PTR [fifo], ebx
        align 4
clip_setup_oow_inc:
  
        mov     ebx, DWORD PTR [gc+ebp*4+tsuDataList+4]
        add     fifo, 4
        
        inc     ebp
        align 4
clip_setup_qow0:
  
        test    al, 16
        je      SHORT clip_setup_stow0
  
        mov     ebx, DWORD PTR [gc+q0Info_mode]
        cmp     ebx, 1
        jne     SHORT clip_setup_oow0
  
        mov     ebx, DWORD PTR [gc+q0Info_offset]
        
        fld     DWORD PTR [ebx+vertexPtr]
        fmul    DWORD PTR ccoow
        fstp    DWORD PTR [fifo]
        
        jmp     SHORT clip_setup_oow0_inc
        align 4
clip_setup_oow0:
        mov     ebx, DWORD PTR ccoow
        
        mov     DWORD PTR [fifo], ebx
        align 4
clip_setup_oow0_inc:
        mov     ebx, DWORD PTR [gc+ebp*4+tsuDataList+4]
        add     fifo, 4
        
        inc     ebp
        align 4
clip_setup_stow0:
  
        test    al, 32
        je      SHORT clip_setup_qow1
        

        fld     DWORD PTR ccoow
        fmul    DWORD PTR [ebx+vertexPtr]

        add     fifo, 8
        add     ebp, 2

        fmul    DWORD PTR [gc+tmu0_s_scale]
        fld     DWORD PTR ccoow
        fmul    DWORD PTR [ebx+vertexPtr+4]
        mov     ebx, DWORD PTR [gc+ebp*4+tsuDataList]
        fmul    DWORD PTR [gc+tmu0_t_scale]
        fxch
        fstp    DWORD PTR [fifo-8]
        fstp    DWORD PTR [fifo-4]
        
        align 4
clip_setup_qow1:

        test    al, 64
        je      SHORT clip_setup_stow1

        mov     ebx, DWORD PTR [gc+q1Info_mode]
        cmp     ebx, 1
        jne     SHORT clip_setup_oow1

        mov     ebx, DWORD PTR [gc+q1Info_offset]
        
        fld     DWORD PTR [ebx+vertexPtr]
        fmul    DWORD PTR ccoow
        fstp    DWORD PTR [fifo]
        
        jmp     SHORT clip_setup_oow1_inc
        align 4
clip_setup_oow1:
        mov     ebx, DWORD PTR ccoow

        mov     DWORD PTR [fifo], ebx
        align 4
clip_setup_oow1_inc:
  
        mov     ebx, DWORD PTR [gc+ebp*4+tsuDataList+4]
        add     fifo, 4

        inc     ebp

        align 4
clip_setup_stow1:
  
        test    al, 128
        je      SHORT clip_setup_end

        fld     DWORD PTR ccoow
        fmul    DWORD PTR [ebx+vertexPtr]
        add     fifo, 8
        fmul    DWORD PTR [gc+tmu1_s_scale]
        fld     DWORD PTR ccoow
        fmul    DWORD PTR [ebx+vertexPtr+4]
        mov     ebx, DWORD PTR [gc+ebp*4+tsuDataList+4]
        fmul    DWORD PTR [gc+tmu1_t_scale]
        fxch        
        fstp    DWORD PTR [fifo-8]
        fstp    DWORD PTR [fifo-4]

        align 4
clip_setup_end:

        dec     edi        
        jz      clip_for_end

        mov     vertexPtr, DWORD PTR [esp+_pointers]
        mov     ebx, DWORD PTR [esp+_mode]

        test    ebx, ebx
        je      SHORT w_clip_no_deref


        mov     vertexPtr, DWORD PTR [vertexPtr]
        align 4
w_clip_no_deref:

        mov     ebx, DWORD PTR [gc+wInfo_offset]
        
        fld     DWORD PTR [ebx+vertexPtr]  
        fdivr   DWORD PTR _F1
        
        jmp     clip_for_begin
        align 4
clip_for_end:
  
        mov     ebx, DWORD PTR [gc+fifoPtr]
        mov     edx, DWORD PTR [gc+fifoRoom]
        
        sub     ebx, fifo
        mov     edi, DWORD PTR [esp+_count]
        
        add     edx, ebx
        sub     edi, 15                 ; 0000000fH
  
        mov     DWORD PTR [gc+fifoRoom], edx
        mov     DWORD PTR [esp+_count], edi
  
        mov     DWORD PTR [gc+fifoPtr], fifo
        mov     DWORD PTR [esp+_pktype], 16 ; 00000010H

        jle     strip_done
        mov     edx, DWORD PTR [esp+_pointers]

        mov     ebx, DWORD PTR [esp+_mode]
        test    ebx, ebx
        
        je      SHORT w1_clip_no_deref
        mov     edx, DWORD PTR [edx]
        
        align 4
w1_clip_no_deref:

        mov     ebx, DWORD PTR [gc+wInfo_offset]
        fld     DWORD PTR [ebx+edx]  
        fdivr   DWORD PTR _F1
  
        jmp     clip_coords_begin
        align 4
strip_done:

        pop     ebp
        pop     ebx
    
        pop     edi
        pop     esi
    
        ret     20                      ; 00000014H
__vpdrawvertexlist@20 ENDP

gc                 TEXTEQU     <esi>       ; points to graphics context
fifo               TEXTEQU     <ecx>       ; points to next entry in fifo
vertexPtr          TEXTEQU     <edi>       ; Current vertex pointer

    ;; NB:  All of the base triangle procs expect to have the gc
    ;;      passed from the caller in edx so that we can avoid
    ;;      the agi from the far pointer. Screw w/ this at your
    ;;      own peril.
    ;;
    ;;      YOU HAVE BEEN WARNED        

        ALIGN 32    

    PUBLIC  __vptrisetup_cull@12
__vptrisetup_cull@12 PROC NEAR
_va = 20
_vb = 24
_vc = 28
        push    ebx
        push    esi
        
        push    edi
        mov     gc, edx

        ;
        ; AJB: Clip Coord mode needs to call grValidateState
        ;
        mov       edx, [gc + invalid]        ; state needs validation ?

        test      edx, edx                   ; do we need to validate state ?
        je        $no_validation             ; nope, it's valid

        call      __grValidateState          ; validate state

$no_validation:
        
        mov     ecx, DWORD PTR [esp+_va-4]
        mov     eax, DWORD PTR [gc+wInfo_offset]
        
        push    ebp
        nop
        
;;; ;   oow[0] = 1.0f / FARRAY(va, gc->state.vData.wInfo.offset)

        fld     DWORD PTR [eax+ecx]

        fdivr   DWORD PTR _F1

        mov     ecx, DWORD PTR [esp+_vb]
        mov     ebx, DWORD PTR [esp+_vc]

        nop
        nop
        
        mov     ebp, DWORD PTR [eax+ecx]
        mov     edi, DWORD PTR [eax+ebx]

        mov     DWORD PTR vPtr1, ebp
        mov     DWORD PTR vPtr2, edi
        
;;; ;   GR_SET_EXPECTED_SIZE(_GlideRoot.curTriSize, 1)

        mov     eax, DWORD PTR [gc+curTriSize]
        mov     ecx, DWORD PTR [gc+fifoRoom]
        
        add     eax, 4
        nop
        
        cmp     ecx, eax
        jge     SHORT setup_pkt_hdr
        
        push    @Line                      ; line number inside this function
        push    0h                         ; pointer to function name = NULL

        push    eax
        call    __grCommandTransportMakeRoom@12

        align 4
setup_pkt_hdr:  

;;; ;   TRI_STRIP_BEGIN(kSetupStrip, 3, gc->state.vData.vSize, SSTCP_PKT3_BDDBDD)


        mov     fifo, DWORD PTR [gc+fifoPtr]
        mov     eax, DWORD PTR [gc+cullStripHdr]

        add     fifo, 4
        lea     ebp, DWORD PTR [esp+_va]

        or      eax, 192                ; 000000c0H
        mov     edx, 0                

        mov     DWORD PTR [fifo-4], eax        
        mov     vertexPtr, DWORD PTR [ebp]

        mov     eax, DWORD PTR [gc+paramIndex]
        nop

;;; Begin loop
        
        align 4
begin_for_loop:

        add     edx, 4
        add     fifo, 8
        
        xor     ebx, ebx
        mov     ebp, DWORD PTR [gc+tsuDataList]

;;; ;   setup x and y

        fld     DWORD PTR [gc+vp_hwidth]
        fmul    DWORD PTR [vertexPtr]
        fmul    st, st(1)
        fadd    DWORD PTR [gc+vp_ox]
        fxch    st(1)

        fld     DWORD PTR [vertexPtr+4]
        fmul    DWORD PTR [gc+vp_hheight]
        test    al, 3
        fmul    st, st(1)
        fadd    DWORD PTR [gc+vp_oy]
        fxch    st(1)
        fstp    DWORD PTR oowa    
        fxch    st(1)
        fstp    DWORD PTR [fifo-8]
        fstp    DWORD PTR [fifo-4]
        
;;; ;   set up color

        je      clip_setup_ooz

        cmp     DWORD PTR [gc+colorType], ebx
        jne     SHORT clip_setup_pargb
        
        test    al, 1
        je      SHORT clip_setup_a

        add     fifo, 12
        add     ebx, 3

        fld     DWORD PTR __GlideRoot+pool_f255
        fmul    DWORD PTR [vertexPtr+ebp]
        fld     DWORD PTR __GlideRoot+pool_f255
        fmul    DWORD PTR [vertexPtr+ebp+4]
        fld     DWORD PTR __GlideRoot+pool_f255
        fmul    DWORD PTR [vertexPtr+ebp+8]
        fxch    st(2)
        fstp    DWORD PTR [fifo-12]
        fstp    DWORD PTR [fifo-8]
        fstp    DWORD PTR [fifo-4]
        mov     ebp, DWORD PTR [gc+tsuDataList+12]
        
        align 4
clip_setup_a:          

        test    al, 2
        je      SHORT clip_setup_ooz
        
        add     fifo, 4
        inc     ebx
        
        fld     DWORD PTR [vertexPtr+ebp]
        fmul    DWORD PTR __GlideRoot+pool_f255
        fstp    DWORD PTR [fifo-4]
        
        mov     ebp, DWORD PTR [gc+tsuDataList+ebx*4]
        jmp     SHORT clip_setup_ooz
        align 4
clip_setup_pargb:
        add     fifo, 4
        mov     ebx, DWORD PTR [vertexPtr+ebp]
        
        mov     DWORD PTR [fifo-4], ebx
        nop
        
        mov     ebx, 1
        mov     ebp, DWORD PTR [gc+tsuDataList+4]
        align 4
clip_setup_ooz:

        test    al, 4
        je      SHORT clip_setup_qow
        
        add     fifo, 4
        inc     ebx
        
        test    DWORD PTR [gc+fbi_fbzMode], 200000h
        je      clip_setup_ooz_nofog

        mov     ebp, DWORD PTR [gc+qInfo_mode]
        test    ebp, ebp
        je      SHORT clip_setup_fog_oow
        mov     ebp, DWORD PTR [gc+qInfo_offset]

        fld     DWORD PTR [vertexPtr+ebp]
        fmul    DWORD PTR oowa
        fstp    DWORD PTR [fifo-4]

        mov     ebp, DWORD PTR [gc+tsuDataList+ebx*4]
        jmp     clip_setup_qow

        align 4
clip_setup_fog_oow:

        fld     DWORD PTR _F1
        fsub    DWORD PTR oowa
        fmul    DWORD PTR [gc+depth_range]
        fstp    DWORD PTR [fifo-4]

        mov     ebp, DWORD PTR [gc+tsuDataList+ebx*4]
        jmp     clip_setup_qow

        align 4
clip_setup_ooz_nofog:   

        fld     DWORD PTR [vertexPtr+ebp]
        fmul    DWORD PTR [gc+vp_hdepth]
        fmul    DWORD PTR oowa
        fadd    DWORD PTR [gc+vp_oz]
        fstp    DWORD PTR [fifo-4]
        
        mov     ebp, DWORD PTR [gc+tsuDataList+ebx*4]
        align 4
clip_setup_qow: 

        test    al, 8
        je      SHORT clip_setup_qow0

        cmp     DWORD PTR [gc+fogInfo_mode], 1
        jne     SHORT clip_setup_oow_nofog

        mov     ebp, DWORD PTR [gc+fogInfo_offset]
        fld     DWORD PTR oowa
        fmul    DWORD PTR [ebp+vertexPtr]
        fstp    DWORD PTR [fifo]
        
        jmp     SHORT clip_setup_oow_inc
        align 4
clip_setup_oow_nofog:
        cmp     DWORD PTR [gc+qInfo_mode], 1
        jne     SHORT clip_setup_oow

        mov     ebp, DWORD PTR [gc+qInfo_offset]
        fld     DWORD PTR oowa
        fmul    DWORD PTR [ebp+vertexPtr]
        fstp    DWORD PTR [fifo]
        
        jmp     SHORT clip_setup_oow_inc
        align 4
clip_setup_oow:
        
        mov     ebp, DWORD PTR oowa
        
        mov     DWORD PTR [fifo], ebp
        align 4
clip_setup_oow_inc:
        mov     ebp, DWORD PTR [gc+tsuDataList+ebx*4+4]   
        add     fifo, 4
        
        inc     ebx
        align 4
clip_setup_qow0:

        test    al, 16                  ; 00000010H
        je      SHORT clip_setup_stow0

        cmp     DWORD PTR [gc+q0Info_mode], 1        
        jne     SHORT clip_setup_oow0
                        
        mov     ebp, DWORD PTR [gc+q0Info_offset]
        
        fld     DWORD PTR oowa
        fmul    DWORD PTR [ebp+vertexPtr]
        fstp    DWORD PTR [fifo]
        
        jmp     SHORT clip_setup_oow0_inc
        align 4
clip_setup_oow0:
        mov     ebp, DWORD PTR oowa
        
        mov     DWORD PTR [fifo], ebp
        align 4
clip_setup_oow0_inc:
        mov     ebp, DWORD PTR [gc+tsuDataList+ebx*4+4]
        add     fifo, 4
        
        inc     ebx
        align 4
clip_setup_stow0:
                        
        test    al, 32
        je      SHORT clip_setup_qow1

        
        fld     DWORD PTR oowa
        fmul    DWORD PTR [vertexPtr+ebp]

        add     fifo, 8
        add     ebx, 2

        fmul    DWORD PTR [gc+tmu0_s_scale]
        fld     DWORD PTR oowa
        fmul    DWORD PTR [vertexPtr+ebp+4]
        mov     ebp, DWORD PTR [gc+tsuDataList+ebx*4]
        fmul    DWORD PTR [gc+tmu0_t_scale]
        fxch    
        fstp    DWORD PTR [fifo-8]
        fstp    DWORD PTR [fifo-4]
        
        align 4
clip_setup_qow1:

        test    al, 64
        je      SHORT clip_setup_stow1

        cmp     DWORD PTR [gc+q1Info_mode], 1
        jne     SHORT clip_setup_oow1

        mov     ebp, DWORD PTR [gc+q1Info_offset]
        
        fld     DWORD PTR [ebp+vertexPtr]
        fmul    DWORD PTR oowa
        fstp    DWORD PTR [fifo]
        
        jmp     SHORT clip_setup_oow1_inc
        align 4
clip_setup_oow1:
        mov     ebp, DWORD PTR oowa
        
        mov     DWORD PTR [fifo], ebp
        align 4
clip_setup_oow1_inc:
        mov     ebp, DWORD PTR [gc+tsuDataList+ebx*4+4]
        add     fifo, 4
        
        inc     ebx
        align 4
clip_setup_stow1:

        test    al, 128
        je      SHORT clip_setup_end
        

        fld     DWORD PTR oowa
        fmul    DWORD PTR [vertexPtr+ebp]
        add     fifo, 8  
        fmul    DWORD PTR [gc+tmu1_s_scale]
        fld     DWORD PTR oowa
        fmul    DWORD PTR [vertexPtr+ebp+4]
        fmul    DWORD PTR [gc+tmu1_t_scale]
        fxch    
        fstp    DWORD PTR [fifo-8]
        fstp    DWORD PTR [fifo-4]

        align 4
clip_setup_end:

        cmp     edx, 12
        je      update_fifo_ptr
        
        fld     DWORD PTR vPtr0[edx]
        fdivr   DWORD PTR _F1
        
        lea     ebx, DWORD PTR [esp+_va]
        mov     ebp, DWORD PTR [gc+wInfo_offset]
        
        mov     vertexPtr, DWORD PTR [ebx+edx]
        jmp     begin_for_loop
       
        align 4
update_fifo_ptr:        

        mov     ebx, DWORD PTR [gc+fifoPtr]; Move gcFifo->fifoPtr to ebx
        mov     edx, DWORD PTR [gc+fifoRoom]; move gcFifo->fifoRoom to edx
        
        sub     ebx, fifo       ; subtract local fifo copy from ebx
                                ; this yields the negative of what we
                                ; wrote to the fiof
        mov     eax, 1          ; place a 1 in eax for return

        add     edx, ebx        ; add (now negative) amount written to
                                ; the fifo to our local copy of the
                                ; fifoRoom variable 
        mov     DWORD PTR [gc+fifoRoom], edx; move the local fifoRoom
                                ; back to the gc
    
        mov     DWORD PTR [gc+fifoPtr], fifo; move local fifoPointer
                                ; back to gc
        mov     ebx, DWORD PTR [gc+trisProcessed]; load trisProcessed
                                ; into register

;;; ;   _GlideRoot.stats.trisProcessed++
        inc     ebx             ; increment trisProcessed

        mov     DWORD PTR [gc+trisProcessed], ebx; Store trisProcessed
                                ; back to GC
    
   ;; Clean up the stack    // FIXED by JHunter (wrong order, trashed edi/esi and ebx)
        pop     ebp

        pop     edi
        pop     esi
    
        pop     ebx
        
        ret     12                      ; 0000000cH
        
__vptrisetup_cull@12 ENDP
_TEXT   ENDS

endif  ; !GL_SSE
endif  ; !GL_MMX
endif  ; !GL_AMD3D

END
