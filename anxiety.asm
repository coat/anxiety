.macro buf_cmd
    ld          a, 23
    rst.lil     $10
    ld          a, 0
    rst.lil     $10
    ld          a, $a0
    rst.lil     $10
    .endmacro

.macro vdu end
    push        hl
    push        bc
    ld          hl, @vdu_start
    ld          bc, end - @vdu_start
    rst.lil     $18
    pop         bc
    pop         hl
    jr          end
@vdu_start:
    .endmacro

font_buf:       .equ 1000

    .include "mos_api.inc"

    .assume   adl = 1
    .org      $40000

    jr        start
    .asciz    "anxiety.bin"
    .align    64                       ; fill with 0 up to MOS header
    .db       "MOS"                    ; MOS header
    .db       0                        ; version 0
    .db       1                        ; ADL mode

start:
    ei

    ld        hl, mode                 ; location of screen mode restore
    mos       mos_sysvars              ; load pointer to sysvars in IX
    ld        a, (ix + sysvar_scrMode) ; get current screen mode
    ld        (hl), a                  ; save it in command stream

vdu_init:
    vdu @n
    .db 12                          ; CLS
    .db 16                          ; CLG
    .db 23, 0, $c0, 0               ; logical coords off
    .db 22, 18                      ; select screenmode, 1024x768x2
    .db 23, 1, 0                    ; hide cursor
@@:
    ld          de, 0
    ld          hl, font            ; first 256 bytes are character widths
    ld          ix, font + $100     ; font icn data starts here
    ld          b, -1
create_char:
    call        create_bitmap
    

    inc         hl
    inc         d
    djnz        create_char

main:
    ld          hl, font
    ld          (font_ptr), hl

    ld          hl, 40
    ld          (x), hl

    ld          hl, text
    call        print_text

    ld          hl, 0
    ld          (x), hl

    ld          hl, (y)
    ld          de, 32
    add         hl, de
    ld          (y), hl
    ld          hl, small_text
    call        print_text

loop:
    mos    mos_sysvars
    ld     a, (ix + sysvar_keyascii)
    cp	    '\e'
    jr	    nz, loop                ; Loop until key is pressed

end:
    vdu     @n
    .db     16                      ; CLG
    .db     23, 1, 1                ; show cursor
    .db     22                      ; restore screen mode
mode:
    .db     0
@@:

     ld     hl, 0                   ; return success
     ret

create_bitmap:
    ld          e, (hl)             ; get width

    buf_cmd
    ld          a, d                ; buffer id
    rst.lil     $10
    ld          a, 0                ; buffer id is 16-bit, send msb
    rst.lil     $10 
    ld          a, 2                ; clear buffer
    rst.lil     $10 

    buf_cmd
    ld          a, d                ; buffer id
    rst.lil     $10
    ld          a, 0                ; buffer id is 16-bit, send msb
    rst.lil     $10
    ld          a, 0                ; write block
    rst.lil     $10
    ld          a, 32               ; block length
    rst.lil     $10
    ld          a, 0                ; length msb
    rst.lil     $10

    push        bc

    ld          b, 16
@loop:
    ld          a, (ix)
    rst.lil     $10
    ld          a, (ix + 16)
    rst.lil     $10
    inc         ix
    djnz        @loop

    ld          b, 16
@@:
    inc         ix
    djnz        @p

    pop         bc

    buf_cmd
    ld          a, d                ; buffer id
    rst.lil     $10
    ld          a, 0                ; buffer id is 16-bit, send msb
    rst.lil     $10
    ld          a, 14               ; consolidate blocks
    rst.lil     $10

    ld          a, 23               ; bitmap api
    rst.lil     $10
    ld          a, 27               ; bitmap api
    rst.lil     $10
    ld          a, $20              ; select bitmap
    rst.lil     $10
    ld          a, d                ; buffer id
    rst.lil     $10
    ld          a, 0                ; buffer id is 16-bit, send msb
    rst.lil     $10

    ld          a, 23               ; bitmap api
    rst.lil     $10
    ld          a, 27               ; bitmap api
    rst.lil     $10
    ld          a, $21              ; create bitmap
    rst.lil     $10
    ld          a, 16               ; width id
    rst.lil     $10
    ld          a, 0                ; width is 16-bit, send msb
    rst.lil     $10
    ld          a, 16               ; height
    rst.lil     $10
    ld          a, 0                ; height is 16-bit, send msb
    rst.lil     $10
    ld          a, 2                ; 1-bpp
    rst.lil     $10
    ret

plot:
    ld      a, 25
    rst.lil $10
    ld      a, $ed
    rst.lil $10
    ld      a, l
    rst.lil $10
    ld      a, h
    rst.lil $10
    ld      a, e
    rst.lil $10
    ld      a, d
    rst.lil $10
    ret

draw_char:
    push    hl

    ld          de, 0
    ld          e, a

    ld          a, 23               ; bitmap api
    rst.lil     $10
    ld          a, 27               ; bitmap api
    rst.lil     $10
    ld          a, $20              ; select bitmap
    rst.lil     $10
    ld          a, e                ; select bitmap id based on ascii code
    rst.lil     $10
    ld          a, 0                ; buffer id is 16-bit, send msb
    rst.lil     $10

    push        de
    ld          hl, (x)
    ld          de, (y)
    call        plot
    pop         de

    ld          hl, font
    add         hl, de              ; add width pointer offset

    ld          a, (hl)             ; get width
    ld          d, 0
    ld          e, a

    ld          hl, (x)
    add         hl, de              ; update cursor pointer by width
    ld          (x), hl

    pop         hl
    ret

print_text:
    ld      a, (hl)
    cp      0
    ret     z
    inc     hl
    call    draw_char
    jr      print_text

font:
    .incbin "times15.uf2"
text:
    .asciz "Tomorrow will be different, I swear."
small_text:
    .asciz "You are surrounded by a thick fog."
font_ptr:
    .dw 0
x:
    .dw 0
y:
    .dw 0
