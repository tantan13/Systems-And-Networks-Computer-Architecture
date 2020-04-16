!============================================================
! CS 2200 Homework 2 Part 2: fib
!
! Apart from initializing the stack,
! please do not edit main's functionality.
!============================================================

main:
    lea     $sp, stack          ! load ADDRESS of stack label into $sp

    add     $zero, $zero, $zero ! TODO: Here, you need to initialize the stack
                                ! using the label below by loading its
                                ! VALUE into $sp (CHANGE THIS INSTRUCTION)

    lea     $at, fib            ! load address of fib label into $at
    addi    $a0, $zero, 5       ! $a0 = 5, the number a to compute fib(n)
    jalr    $ra, $at            ! jump to fib, set $ra to return addr
    halt                        ! when we return, just halt

fib:
    add     $zero, $zero, $zero ! TODO: perform post-call portion of
                                ! the calling convention. Make sure to
                                ! save any registers youll be using!
    addi    $sp, $sp, -1        ! making space to put $fp onto the stack
    sw      $fp, 0($sp)                            


    add     $zero, $zero, $zero ! TODO: Implement the following pseudocode in assembly:
                                ! IF (a0 <= 1) go to base  (so check if a0 > 1)
    addi	$t0,   $zero, $1	! t0 = 1                            
    skplt	$t1,   $a0			! if a0 > 1, then !a0 <= 1
    goto 	base
    goto 	else 				! ELSE
    							! GOTO ELSE

base:
    add     $zero, $zero, $zero ! TODO: If $a0 is less than 0, set $a0 to 0
    skplt	$a0, $zero		    !
    goto    $false				! else dont change value of a0 
    addi	$a0, $zero,	  0		! set $a0 to 0
false:
    addi    $v0, $a0, 	  0     ! return a
    goto    teardown            ! teardown the stack


else:
    add     $zero, $zero, $zero ! TODO: Save the value of the $a0 into a saved register
    addi    $s0,   $a0,	  0
    addi    $a0,   $a0,   -1    ! $a0 = $a0 - 1 (n - 1)

    add     $zero, $zero, $zero ! TODO: Implement the recursive call to fib
    lea     $at,   fib          ! You should not have to set any of the argument registers here.
    jalr    $ra,   $at          ! Per the PDF, do not save any temp registers!

    add     $zero, $zero, $zero ! TODO: Save the return value of the fib call into a register
    add     $v0, $zero, $a0     ! store return value in v0
    jalr    $zero, $ra          ! return to caller

    add     $zero, $zero, $zero ! TODO: Restore the old value of $a0 that we saved earlier
    addi	$a0,   $s0,   0		! 

    addi    $a0, $a0, -2        ! $a0 = $a0 - 2 (n - 2)

    add     $zero, $zero, $zero ! TODO: Implement the recursive call to fib
    lea     $at,   fib          ! If your previous recursive call worked correctly,
    jalr    $ra,   $at          ! you should be able to copy and paste it here :)

    add     $zero, $zero, $zero ! TODO: Compute fib(n - 1) [stored from earlier] + fib(n - 2) [just computed]
    add     $v0,   $v0,   $a0   ! Place the sum of those two values into $v0
    goto    teardown            ! return
    
teardown:
    add     $zero, $zero, $zero ! TODO: perform pre-return portion
    lw 	    $fp, 0x1($sp)       !       of the calling convention
    addi    $sp, $sp, 1			!
    add     $v0, $zero, $zero   ! clear return value register
    add     $v0, $zero, $a0     ! store return value in v0

    jalr    $zero, $ra          ! return to caller

stack: .word 0xFFFF             ! the stack begins here
