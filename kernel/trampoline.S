	#
        # code to switch between user and kernel space.
        #
        # this code is mapped at the same virtual address
        # (TRAMPOLINE) in user and kernel space so that
        # it continues to work when it switches page tables.
	#
	# kernel.ld causes this to be aligned
        # to a page boundary.
        #
	.section trampsec
.globl trampoline
trampoline:
.align 4

# 作用：导出全局符号 uservec。
# 为什么需要全局：trap.c/trapinit() 会把 stvec（陷入入口寄存器）设置为 uservec 的地址
# （准确说是在 trampoline 页里的那个入口地址），
# 所以必须能从别的编译单元引用到这个符号。

.globl uservec
uservec:    
	#
        # trap.c sets stvec to point here, so
        # traps from user space start here,
        # in supervisor mode, but with a
        # user page table.
        #
        # sscratch points to where the process's p->trapframe is
        # mapped into user space, at TRAPFRAME.
        #
        
	# swap a0 and sscratch
        # so that a0 is TRAPFRAME
        # csr(read and write) | 将 sscratch（控制状态寄存器） 的值读到 a0 寄存器中，并将 a0 寄存器的值写入 sscratch 寄存器
        # a0 ← sscratch（a0 现在变成 trapframe 指针）
        # sscratch ← 原来的 a0（把用户态的 a0 暂存到 sscratch，避免丢）
        csrrw a0, sscratch, a0

        # save the user registers in TRAPFRAME
        sd ra, 40(a0)
        sd sp, 48(a0)
        sd gp, 56(a0)
        sd tp, 64(a0)
        sd t0, 72(a0)
        sd t1, 80(a0)
        sd t2, 88(a0)
        sd s0, 96(a0)
        sd s1, 104(a0)
        sd a1, 120(a0)
        sd a2, 128(a0)
        sd a3, 136(a0)
        sd a4, 144(a0)
        sd a5, 152(a0)
        sd a6, 160(a0)
        sd a7, 168(a0)
        sd s2, 176(a0)
        sd s3, 184(a0)
        sd s4, 192(a0)
        sd s5, 200(a0)
        sd s6, 208(a0)
        sd s7, 216(a0)
        sd s8, 224(a0)
        sd s9, 232(a0)
        sd s10, 240(a0)
        sd s11, 248(a0)
        sd t3, 256(a0)
        sd t4, 264(a0)
        sd t5, 272(a0)
        sd t6, 280(a0)

	# save the user a0 in p->trapframe->a0
        # csr(control and status register) r(read) | 将 sscratch（控制状态寄存器） 的值读到 t0 寄存器中
        csrr t0, sscratch 
        sd t0, 112(a0)

        # restore kernel stack pointer from p->trapframe->kernel_sp
        # load doubleword | 将 a0 偏移 8 字节处的值加载到 sp 寄存器中
        # 内核栈顶
        ld sp, 8(a0)

        # make tp hold the current hartid, from p->trapframe->kernel_hartid
        # 当前 hart id（多核用）
        ld tp, 32(a0)

        # load the address of usertrap(), p->trapframe->kernel_trap
        # C 入口函数地址（比如 usertrap）
        ld t0, 16(a0)

        # restore kernel page table from p->trapframe->kernel_satp
        # 内核页表（satp 值）
        ld t1, 0(a0)

        # csr(write) | 将 t1 寄存器的值写入 satp（地址空间标识符寄存器）
        csrw satp, t1

        # flush TLB
        sfence.vma zero, zero

        # a0 is no longer valid, since the kernel page
        # table does not specially map p->tf.

        # jump to usertrap(), which does not return
        jr t0

.globl userret
userret:
        # userret(TRAPFRAME, pagetable)
        # switch from kernel to user.
        # usertrapret() calls here.
        # a0: TRAPFRAME, in user page table.
        # a1: user page table, for satp.

        # switch to the user page table.
        csrw satp, a1
        sfence.vma zero, zero

        # put the saved user a0 in sscratch, so we
        # can swap it with our a0 (TRAPFRAME) in the last step.
        ld t0, 112(a0)
        csrw sscratch, t0

        # restore all but a0 from TRAPFRAME
        ld ra, 40(a0)
        ld sp, 48(a0)
        ld gp, 56(a0)
        ld tp, 64(a0)
        ld t0, 72(a0)
        ld t1, 80(a0)
        ld t2, 88(a0)
        ld s0, 96(a0)
        ld s1, 104(a0)
        ld a1, 120(a0)
        ld a2, 128(a0)
        ld a3, 136(a0)
        ld a4, 144(a0)
        ld a5, 152(a0)
        ld a6, 160(a0)
        ld a7, 168(a0)
        ld s2, 176(a0)
        ld s3, 184(a0)
        ld s4, 192(a0)
        ld s5, 200(a0)
        ld s6, 208(a0)
        ld s7, 216(a0)
        ld s8, 224(a0)
        ld s9, 232(a0)
        ld s10, 240(a0)
        ld s11, 248(a0)
        ld t3, 256(a0)
        ld t4, 264(a0)
        ld t5, 272(a0)
        ld t6, 280(a0)

	# restore user a0, and save TRAPFRAME in sscratch
        csrrw a0, sscratch, a0
        
        # return to user mode and user pc.
        # usertrapret() set up sstatus and sepc.
        sret
