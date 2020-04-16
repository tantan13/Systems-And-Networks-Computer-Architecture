.fill 0
.fill 0
.fill 0
.fill 0
.fill 0
.fill 0
.fill 0
.fill 0
.fill 0
.fill 0
.fill 0
.fill 0
.fill 0
.fill 0
.fill 0
.fill 0
lea $t0, handler
sw $t0, 1($zero)
ei
halt

handler:
addi $t0, $zero, 1