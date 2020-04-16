! This program executes pow as a test program using the LC 2200 calling convention
! Check your registers ($v0) and memory to see if it is consistent with this program

        ! vector table
vector0:
        .fill 0x00000000                        ! device ID 0
        .fill 0x00000000                        ! device ID 1
        .fill 0x00000000                        ! ...
        .fill 0x00000000
        .fill 0x00000000
        .fill 0x00000000
        .fill 0x00000000
        .fill 0x00000000
        .fill 0x00000000
        .fill 0x00000000
        .fill 0x00000000
        .fill 0x00000000
        .fill 0x00000000
        .fill 0x00000000
        .fill 0x00000000
        .fill 0x00000000                        ! device ID 15
        ! end vector table

main:	lea $sp, initsp                         ! initialize the stack pointer
        lw $sp, 0($sp)                          ! finish initialization

        lea $s0, vector0                        ! Install timer interrupt handler into vector table
        lea $s1, timer_handler
        sw $s1, 1($s0)                          ! Install keyboard interrupt handler into vector table

        
        lea $s1, keyboard_handler
        sw $s1, 2($s0)

        EI                                      ! Enable interrupts

        lea $a0, BASE                           ! load base for pow
        lw $a0, 0($a0)
        lea $a1, EXP                            ! load power for pow
        lw $a1, 0($a1)
        lea $at, POW                            ! load address of pow
        jalr $ra, $at                           ! run pow
        lea $a0, ANS                            ! load base for pow
        sw $v0, 0($a0)

        halt                                    ! stop the program here
        addi $v0, $zero, -1                     ! load a bad value on failure to halt

BASE:   .fill 2
EXP:    .fill 8
ANS:	.fill 0                                 ! should come out to 256 (BASE^EXP)

POW:    addi $sp, $sp, -1                       ! allocate space for old frame pointer
        sw $fp, 0($sp)

        addi $fp, $sp, 0                        ! set new frame pinter
        
        skplt $zero, $a1                        ! check if $a1 is zero (if not, skip the goto)
        goto RET1                               ! if the power is 0 return 1
        skplt $zero, $a0
        goto RET0                               ! if the base is 0 return 0 (otherwise, the goto was skipped)

        addi $a1, $a1, -1                       ! decrement the power

        lea $at, POW                            ! load the address of POW
        addi $sp, $sp, -2                       ! push 2 slots onto the stack
        sw $ra, -1($fp)                         ! save RA to stack
        sw $a0, -2($fp)                         ! save arg 0 to stack
        jalr $ra, $at                           ! recursively call POW
        add $a1, $v0, $zero                     ! store return value in arg 1
        lw $a0, -2($fp)                         ! load the base into arg 0
        lea $at, MULT                           ! load the address of MULT
        jalr $ra, $at                           ! multiply arg 0 (base) and arg 1 (running product)
        lw $ra, -1($fp)                         ! load RA from the stack
        addi $sp, $sp, 2

        goto FIN                                ! return

RET1:   addi $v0, $zero, 1                      ! return a value of 1
        skplt $zero, $v0                        ! convenient use of skplt to get to FIN (0 < 1)

RET0:   add $v0, $zero, $zero                   ! return a value of 0

FIN:	lw $fp, 0($fp)                          ! restore old frame pointer
        addi $sp, $sp, 1                        ! pop off the stack
        jalr $zero, $ra

MULT:   add $v0, $zero, $zero                   ! return value = 0
        addi $t0, $zero, 1                      ! sentinel = 0
AGAIN:  add $v0, $v0, $a0                       ! return value += argument0
        addi $t0, $t0, 1                        ! increment sentinel
        skplt $a1, $t0                          ! if $t0 <= $a1, keep looping
        goto AGAIN                              ! loop again

        jalr $zero, $ra                         ! return from mult

timer_handler:

        addi $sp, $sp, -1                       !saving the current value of $k0 
        sw $k0, 0($sp)                          !and enabling the interrupt
        EI
        
        addi $sp, $sp, -15                       !save the states of the interuptted program                          
        sw $t0, 0($sp)
        sw $t1, 1($sp)
        sw $t2, 2($sp)
        #store all registers
        sw $zero, 3($sp)
        sw $at, 4($sp)
        sw $v0, 5($sp)
        sw $a0, 6($sp)
        sw $a1, 7($sp)
        sw $a2, 8($sp)
        sw $s0, 9($sp)
        sw $s1, 10($sp)
        sw $s2, 11($sp)
        sw $sp, 12($sp)
        sw $fp, 13($sp)
        sw $ra, 14($sp)

        lea $t0, ticks                          !Incrementing the counter variable provided in memory                                      
        lw $t1, 0($t0)
        lw $t2, 0($t1)
        addi $t2, $t2, 1
        sw $t2, 0($t1)

        lw $t0, 0($sp)                          !Restore the state of the original program and return using RETI.
        lw $t1, 1($sp)
        lw $t2, 2($sp)

        #restore
        lw $zero, 3($sp)
        lw $at, 4($sp)
        lw $v0, 5($sp)
        lw $a0, 6($sp)
        lw $a1, 7($sp)
        lw $a2, 8($sp)
        lw $s0, 9($sp)
        lw $s1, 10($sp)
        lw $s2, 11($sp)
        lw $sp, 12($sp)
        lw $fp, 13($sp)
        lw $ra, 14($sp)
        addi $sp, $sp, 15

        DI
        lw $k0, 0($sp)
        addi $sp, $sp, 1
        RETI

keyboard_handler:

        addi $sp, $sp, -1                       !saving current val of $k0 and enabling interupt 
        sw $k0, 0($sp)
        EI

        addi $sp, $sp, -15                       ! saving states of program                                         
        sw $t0, 0($sp)
        sw $t1, 1($sp)
        sw $t2, 2($sp)

        sw $zero, 3($sp)
        sw $at, 4($sp)
        sw $v0, 5($sp)
        sw $a0, 6($sp)
        sw $a1, 7($sp)
        sw $a2, 8($sp)
        sw $s0, 9($sp)
        sw $s1, 10($sp)
        sw $s2, 11($sp)
        sw $sp, 12($sp)
        sw $fp, 13($sp)
        sw $ra, 14($sp)

        lea $t0, key_buf_ptr                    ! same as timer, incremementing the variable for keyboard (tail pointer)
        lw $t1, 0($t0)                          ! and also using the IN instruction to get the recently pressed key
        in $t2, 0x2                             ! and storing that value to the memory location of tail pointer which 

        sw $t2, 0($t1)                          ! started at key_buf_ptr aka 0xFFE0
        addi $t1, $t1, 1
        sw $t1, 0($t0)

        lw $t0, 0($sp)                                               
        lw $t1, 1($sp)
        sw $t2, 2($sp)

        lw $zero, 3($sp)
        lw $at, 4($sp)
        lw $v0, 5($sp)
        lw $a0, 6($sp)
        lw $a1, 7($sp)
        lw $a2, 8($sp)
        lw $s0, 9($sp)
        lw $s1, 10($sp)
        lw $s2, 11($sp)
        lw $sp, 12($sp)
        lw $fp, 13($sp)
        lw $ra, 14($sp)
        addi $sp, $sp, 15
        #addi $sp, $sp, 3

        DI
        lw $k0, 0($sp)
        addi $sp, $sp, 1
        RETI

initsp: .fill 0xA000

ticks:  .fill 0xFFFF
key_buf_ptr: .fill 0xFFE0
