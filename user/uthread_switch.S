	.text

	/*
         * save the old thread's registers,
         * restore the new thread's registers.
         */

	.globl thread_switch
thread_switch:
	/* YOUR CODE HERE */
    # 保存ra（返回地址）、sp（栈指针）
    sd ra, 0(a0)    # ra -> 上下文偏移0
    sd sp, 8(a0)    # sp -> 上下文偏移8
    # 保存被调用者保存寄存器（s0-s11）
    sd s0, 16(a0)   # s0 -> 偏移16
    sd s1, 24(a0)   # s1 -> 偏移24
    sd s2, 32(a0)   # s2 -> 偏移32
    sd s3, 40(a0)   # s3 -> 偏移40
    sd s4, 48(a0)   # s4 -> 偏移48
    sd s5, 56(a0)   # s5 -> 偏移56
    sd s6, 64(a0)   # s6 -> 偏移64
    sd s7, 72(a0)   # s7 -> 偏移72
    sd s8, 80(a0)   # s8 -> 偏移80
    sd s9, 88(a0)   # s9 -> 偏移88
    sd s10, 96(a0)  # s10 -> 偏移96
    sd s11, 104(a0) # s11 -> 偏移104

    # 从a1指向的内存恢复目标线程的上下文
    ld ra, 0(a1)    # 恢复ra
    ld sp, 8(a1)    # 恢复sp（切换到目标线程栈）
    ld s0, 16(a1)   # 恢复s0
    ld s1, 24(a1)   # 恢复s1
    ld s2, 32(a1)   # 恢复s2
    ld s3, 40(a1)   # 恢复s3
    ld s4, 48(a1)   # 恢复s4
    ld s5, 56(a1)   # 恢复s5
    ld s6, 64(a1)   # 恢复s6
    ld s7, 72(a1)   # 恢复s7
    ld s8, 80(a1)   # 恢复s8
    ld s9, 88(a1)   # 恢复s9
    ld s10, 96(a1)  # 恢复s10
    ld s11, 104(a1) # 恢复s11

    ret  # 返回目标线程的ra（即目标线程的执行点）