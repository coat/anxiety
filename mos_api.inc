;
; Title:	AGON MOS - API for user projects
; Author:	Dean Belfield
; Created:	03/08/2022
; Last Updated:	11/11/2023
;
; Modinfo:
; 05/08/2022:	Added mos_feof
; 09/08/2022:	Added system variables: cursorX, cursorY
; 18/08/2022:	Added system variables: scrchar, scrpixel, audioChannel, audioSuccess, vpd_pflags
; 05/09/2022:	Added mos_ren, vdp_pflag_mode
; 24/09/2022:	Added mos_getError, mos_mkdir
; 13/10/2022:	Added mos_oscli
; 23/02/2023:	Added more sysvars, fixed typo in sysvar_audioSuccess, offsets for sysvar_scrCols, sysvar_scrRows
; 04/03/2023:	Added sysvar_scrpixelIndex
; 08/03/2023:	Renamed sysvar_keycode to sysvar_keyascii, added sysvar_vkeycode
; 15/03/2023:	Added mos_copy, mos_getrtc, mos_setrtc, rtc, vdp_pflag_rtc
; 21/03/2023:	Added mos_setintvector, sysvars for keyboard status, vdu codes for vdp
; 22/03/2023:	The VDP commands are now indexed from 0x80
; 29/03/2023:	Added mos_uopen, mos_uclose, mos_ugetc, mos_uputc
; 13/04/2023:	Added FatFS file structures (FFOBJID, FIL, DIR, FILINFO)
; 15/04/2023:	Added mos_getfil, mos_fread, mos_fwrite and mos_flseek
; 19/05/2023:	Added sysvar_scrMode
; 05/06/2023:	Added sysvar_rtcEnable
; 03/08/2023:	Added mos_setkbvector
; 10/08/2023:	Added mos_getkbmap
; 11/11/2023:	Added mos_i2c_open, mos_i2c_close, mos_i2c_write and mos_i2c_read

; VDP control (VDU 23, 0, n)
;
vdp_gp:			equ 	80h
vdp_keycode:		equ 	81h
vdp_cursor:		equ	82h
vdp_scrchar:		equ	83h
vdp_scrpixel:		equ	84h
vdp_audio:		equ	85h
vdp_mode:		equ	86h
vdp_rtc:		equ	87h
vdp_keystate:		equ	88h
vdp_logicalcoords:	equ	C0h
vdp_terminalmode:	equ	FFh

; MOS high level functions
;
mos_getkey:		equ	00h
mos_load:		equ	01h
mos_save:		equ	02h
mos_cd:			equ	03h
mos_dir:		equ	04h
mos_del:		equ	05h
mos_ren:		equ	06h
mos_mkdir:		equ	07h
mos_sysvars:		equ	08h
mos_editline:		equ	09h
mos_fopen:		equ	0Ah
mos_fclose:		equ	0Bh
mos_fgetc:		equ	0Ch
mos_fputc:		equ	0Dh
mos_feof:		equ	0Eh
mos_getError:		equ	0Fh
mos_oscli:		equ	10h
mos_copy:		equ	11h
mos_getrtc:		equ	12h
mos_setrtc:		equ	13h
mos_setintvector:	equ	14h
mos_uopen:		equ	15h
mos_uclose:		equ	16h
mos_ugetc:		equ	17h
mos_uputc:		equ 	18h
mos_getfil:		equ	19h
mos_fread:		equ	1Ah
mos_fwrite:		equ	1Bh
mos_flseek:		equ	1Ch
mos_setkbvector:	equ	1Dh
mos_getkbmap:		equ	1Eh
mos_i2c_open:		equ	1Fh
mos_i2c_close:		equ	20h
mos_i2c_write:		equ	21h
mos_i2c_read:		equ	22h


; FatFS file access functions
;
ffs_fopen:		equ	80h
ffs_fclose:		equ	81h
ffs_fread:		equ	82h
ffs_fwrite:		equ	83h
ffs_flseek:		equ	84h
ffs_ftruncate:		equ	85h
ffs_fsync:		equ	86h
ffs_fforward:		equ	87h
ffs_fexpand:		equ	88h
ffs_fgets:		equ	89h
ffs_fputc:		equ	8Ah
ffs_fputs:		equ	8Bh
ffs_fprintf:		equ	8Ch
ffs_ftell:		equ	8Dh
ffs_feof:		equ	8Eh
ffs_fsize:		equ	8Fh
ffs_ferror:		equ	90h

; FatFS directory access functions
;
ffs_dopen:		equ	91h
ffs_dclose:		equ	92h
ffs_dread:		equ	93h
ffs_dfindfirst:		equ	94h
ffs_dfindnext:		equ	95h

; FatFS file and directory management functions
;
ffs_stat:		equ	96h
ffs_unlink:		equ	97h
ffs_rename:		equ	98h
ffs_chmod:		equ	99h
ffs_utime:		equ	9Ah
ffs_mkdir:		equ	9Bh
ffs_chdir:		equ	9Ch
ffs_chdrive:		equ	9Dh
ffs_getcwd:		equ	9Eh

; FatFS volume management and system configuration functions
;
ffs_mount:		equ	9Fh
ffs_mkfs:		equ	A0h
ffs_fdisk:		equ	A1h
ffs_getfree:		equ	A2h
ffs_getlabel:		equ	A3h
ffs_setlabel:		equ	A4h
ffs_setcp:		equ	A5h
	
; File access modes
;
fa_read:		equ	01h
fa_write:		equ	02h
fa_open_existing:	equ	00h
fa_create_new:		equ	04h
fa_create_always:	equ	08h
fa_open_always:		equ	10h
fa_open_append:		equ	30h
	
; System variable indexes for api_sysvars
; Index into _sysvars in globals.asm
;
sysvar_time:		equ	00h	; 4: Clock timer in centiseconds (incremented by 2 every VBLANK)
sysvar_vpd_pflags:	equ	04h	; 1: Flags to indicate completion of VDP commands
sysvar_keyascii:	equ	05h	; 1: ASCII keycode, or 0 if no key is pressed
sysvar_keymods:		equ	06h	; 1: Keycode modifiers
sysvar_cursorX:		equ	07h	; 1: Cursor X position
sysvar_cursorY:		equ	08h	; 1: Cursor Y position
sysvar_scrchar:		equ	09h	; 1: Character read from screen
sysvar_scrpixel:	equ	0Ah	; 3: Pixel data read from screen (R,B,G)
sysvar_audioChannel:	equ	0Dh	; 1: Audio channel 
sysvar_audioSuccess:	equ	0Eh	; 1: Audio channel note queued (0 = no, 1 = yes)
sysvar_scrWidth:	equ	0Fh	; 2: Screen width in pixels
sysvar_scrHeight:	equ	11h	; 2: Screen height in pixels
sysvar_scrCols:		equ	13h	; 1: Screen columns in characters
sysvar_scrRows:		equ	14h	; 1: Screen rows in characters
sysvar_scrColours:	equ	15h	; 1: Number of colours displayed
sysvar_scrpixelIndex:	equ	16h	; 1: Index of pixel data read from screen
sysvar_vkeycode:	equ	17h	; 1: Virtual key code from FabGL
sysvar_vkeydown:	equ	18h	; 1: Virtual key state from FabGL (0=up, 1=down)
sysvar_vkeycount:	equ	19h	; 1: Incremented every time a key packet is received
sysvar_rtc:		equ	1Ah	; 6: Real time clock data
sysvar_spare:		equ	20h	; 2: Spare, previously used by rtc
sysvar_keydelay:	equ	22h	; 2: Keyboard repeat delay
sysvar_keyrate:		equ	24h	; 2: Keyboard repeat reat
sysvar_keyled:		equ	26h	; 1: Keyboard LED status
sysvar_scrMode:		equ	27h	; 1: Screen mode
sysvar_rtcEnable:	equ	28h	; 1: RTC enable flag (0: disabled, 1: use ESP32 RTC)
sysvar_mouseX:		equ	29h	; 2: Mouse X position
sysvar_mouseY:		equ	2Bh	; 2: Mouse Y position
sysvar_mouseButtons:	equ	2Dh	; 1: Mouse button state
sysvar_mouseWheel:	equ	2Eh	; 1: Mouse wheel delta
sysvar_mouseXDelta:	equ	2Fh	; 2: Mouse X delta
sysvar_mouseYDelta:	equ	31h	; 2: Mouse Y delta
	
; Flags for the VPD protocol
;
vdp_pflag_cursor:	equ	00000001b
vdp_pflag_scrchar:	equ	00000010b
vdp_pflag_point:	equ	00000100b
vdp_pflag_audio:	equ	00001000b
vdp_pflag_mode:		equ	00010000b
vdp_pflag_rtc:		equ	00100000b
vdp_pflag_mouse:	equ	01000000b
; vdp_pflag_buffered:	equ	10000000b

;
; Macro for calling the API
; Parameters:
; - function: One of the function numbers listed above
;
    macro mos function
    ld a,function
    rst.lis $08
    endmacro
