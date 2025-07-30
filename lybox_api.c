/**
 * @file lybox_api.c
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief
 * @date 2025-07-30
 *
 * @copyright Copyright (c) 2024 Luyi365
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @note
 *
 *
 */

#include "lybox.h"

event_group_t *event_group_create(void)
{
    // 目前默认8bit位，后续考虑使用宏还是函数进行扩展
    return lybox_create(lyboxType_QUEUE, 1, sizeof(event_bits_t));
}

bool event_group_set(event_group_t *event_group, const event_bits_t bits)
{
    *(event_bits_t *)(event_group->box) |= bits;
}

bool event_group_clear(event_group_t *event_group, const event_bits_t bits)
{
    *(event_bits_t *)(event_group->box) &= ~bits;
}

event_bits_t event_group_get(event_group_t *event_group)
{
    return *(event_bits_t *)(event_group->box);
}

void event_group_delete(event_group_t *event_group)
{
    lybox_delete(event_group);
}