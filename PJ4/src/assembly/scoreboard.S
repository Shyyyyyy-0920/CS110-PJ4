/*
 * PJ4 Scoreboard 汇编实现。
 *
 * 排行榜保存前三名：
 *   scoreboard_scores[0..2]
 *   scoreboard_levels[0..2]
 *
 * 排序规则：
 *   1. 分数高的排前面
 *   2. 分数相同，关卡难度高的排前面，即 1-3 > 1-2 > 1-1
 *
 * C 调用接口：
 *   void scoreboard_update_asm(int level, int score);
 *   int scoreboard_get_level_asm(int index);
 *   int scoreboard_get_score_asm(int index);
 */

.section .data
.align 2

/*
 * 当前排行榜分数。
 * 初始为 0。
 */
.globl scoreboard_scores
scoreboard_scores:
    .word 0, 0, 0

/*
 * 当前排行榜关卡。
 * 初始为 0，表示空记录。
 */
.globl scoreboard_levels
scoreboard_levels:
    .word 0, 0, 0


.section .text
.align 2

/*
 * int scoreboard_get_score_asm(int index)
 *
 * 参数：
 *   a0 = index
 *
 * 返回：
 *   a0 = scoreboard_scores[index]
 */
.globl scoreboard_get_score_asm
.type scoreboard_get_score_asm, @function
scoreboard_get_score_asm:
    la t0, scoreboard_scores
    slli t1, a0, 2
    add t0, t0, t1
    lw a0, 0(t0)
    ret


/*
 * int scoreboard_get_level_asm(int index)
 *
 * 参数：
 *   a0 = index
 *
 * 返回：
 *   a0 = scoreboard_levels[index]
 */
.globl scoreboard_get_level_asm
.type scoreboard_get_level_asm, @function
scoreboard_get_level_asm:
    la t0, scoreboard_levels
    slli t1, a0, 2
    add t0, t0, t1
    lw a0, 0(t0)
    ret


/*
 * void scoreboard_update_asm(int level, int score)
 *
 * 参数：
 *   a0 = level
 *   a1 = score
 *
 * 判断 new_record 是否优于 old_record：
 *   new_score > old_score
 *   或者 new_score == old_score 且 new_level > old_level
 *
 * 插入逻辑：
 *   如果优于第 0 名：
 *      第 1 名移动到第 2 名
 *      第 0 名移动到第 1 名
 *      新记录放到第 0 名
 *
 *   如果优于第 1 名：
 *      第 1 名移动到第 2 名
 *      新记录放到第 1 名
 *
 *   如果优于第 2 名：
 *      新记录放到第 2 名
 *
 *   否则不更新。
 */
.globl scoreboard_update_asm
.type scoreboard_update_asm, @function
scoreboard_update_asm:
    /*
     * 保存需要用到的 s 寄存器。
     */
    addi sp, sp, -24
    sw ra, 20(sp)
    sw s0, 16(sp)
    sw s1, 12(sp)
    sw s2, 8(sp)
    sw s3, 4(sp)
    sw s4, 0(sp)

    /*
     * s0 = new_level
     * s1 = new_score
     * s2 = scores 基地址
     * s3 = levels 基地址
     */
    mv s0, a0
    mv s1, a1
    la s2, scoreboard_scores
    la s3, scoreboard_levels

    /*
     * 检查是否应该插入第 0 名。
     */
    lw t0, 0(s2)        /* old_score = scores[0] */
    lw t1, 0(s3)        /* old_level = levels[0] */

    blt t0, s1, insert_pos0
    bne t0, s1, check_pos1
    blt t1, s0, insert_pos0

check_pos1:
    /*
     * 检查是否应该插入第 1 名。
     */
    lw t0, 4(s2)        /* old_score = scores[1] */
    lw t1, 4(s3)        /* old_level = levels[1] */

    blt t0, s1, insert_pos1
    bne t0, s1, check_pos2
    blt t1, s0, insert_pos1

check_pos2:
    /*
     * 检查是否应该插入第 2 名。
     */
    lw t0, 8(s2)        /* old_score = scores[2] */
    lw t1, 8(s3)        /* old_level = levels[2] */

    blt t0, s1, insert_pos2
    bne t0, s1, finish_update
    blt t1, s0, insert_pos2
    j finish_update


insert_pos0:
    /*
     * scores[2] = scores[1]
     * levels[2] = levels[1]
     */
    lw t0, 4(s2)
    sw t0, 8(s2)
    lw t1, 4(s3)
    sw t1, 8(s3)

    /*
     * scores[1] = scores[0]
     * levels[1] = levels[0]
     */
    lw t0, 0(s2)
    sw t0, 4(s2)
    lw t1, 0(s3)
    sw t1, 4(s3)

    /*
     * scores[0] = new_score
     * levels[0] = new_level
     */
    sw s1, 0(s2)
    sw s0, 0(s3)

    j finish_update


insert_pos1:
    /*
     * scores[2] = scores[1]
     * levels[2] = levels[1]
     */
    lw t0, 4(s2)
    sw t0, 8(s2)
    lw t1, 4(s3)
    sw t1, 8(s3)

    /*
     * scores[1] = new_score
     * levels[1] = new_level
     */
    sw s1, 4(s2)
    sw s0, 4(s3)

    j finish_update


insert_pos2:
    /*
     * scores[2] = new_score
     * levels[2] = new_level
     */
    sw s1, 8(s2)
    sw s0, 8(s3)

    j finish_update


finish_update:
    /*
     * 恢复寄存器并返回。
     */
    lw s4, 0(sp)
    lw s3, 4(sp)
    lw s2, 8(sp)
    lw s1, 12(sp)
    lw s0, 16(sp)
    lw ra, 20(sp)
    addi sp, sp, 24
    ret