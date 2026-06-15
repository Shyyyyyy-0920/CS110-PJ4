.section .text
.globl draw
.type draw, @function
draw:
	li a0,20
	li a1,15
	li a2,100
	li a3,55
	li a4,0xF800
	addi sp,sp,-4
	sw ra,0(sp)
	jal LCD_DrawRectangle
	lw ra,0(sp)
	addi sp,sp,4
	ret
