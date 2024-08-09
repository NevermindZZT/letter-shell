#include "ringbuffer.h"
#include <string.h>

#undef USER_ASSERT
#define USER_ASSERT(exp) (void)0 // 开启此宏关闭此文件用户自定义断言

// clang-format off
#ifndef USER_ASSERT
    #include <stdio.h>
    #define USER_ASSERT(exp)                                                                \
        if (!(exp))                                                                         \
        {                                                                                   \
            printf("\"" #exp "\" assert failed at file: %s, line: %d", __FILE__, __LINE__); \
            for (;;)                                                                        \
                ;                                                                           \
        }
#endif // ! USER_ASSERT
// clang-format on

void ringbuffer_init(ringbuffer_t *rb, uint8_t *buffer, uint16_t size)
{
    USER_ASSERT(rb != NULL);

    rb->buffer = buffer;
    rb->buffer_size = size;
    rb->busy = false;

    ringbuffer_reset(rb);
}

void ringbuffer_reset(ringbuffer_t *rb)
{
    USER_ASSERT(rb != NULL);
    rb->head = 0;
    rb->tail = 0;
    rb->data_count = 0;
}

uint16_t ringbuffer_get_free(ringbuffer_t *rb)
{
    USER_ASSERT(rb != NULL);
    return rb->buffer_size - rb->data_count;
}

uint16_t ringbuffer_get_used(ringbuffer_t *rb)
{
    USER_ASSERT(rb != NULL);
    return rb->data_count;
}

uint16_t ringbuffer_get_size(ringbuffer_t *rb)
{
    USER_ASSERT(rb != NULL);
    return rb->buffer_size;
}

uint16_t ringbuffer_get_last_series_size(ringbuffer_t *rb)
{
    USER_ASSERT(rb != NULL);
    return rb->last_series_count;
}

uint16_t ringbuffer_write(ringbuffer_t *rb, const uint8_t *data, uint16_t size)
{
    USER_ASSERT(rb != NULL);

    uint16_t free_size = ringbuffer_get_free(rb);

    if (size > free_size)
    {
        size = free_size; // 多余的数据不写入(丢掉)
    }

    // 判断超过尾部没有
    if (rb->tail + size > rb->buffer_size)
    {
        uint16_t first_size = rb->buffer_size - rb->tail;
        memcpy((uint8_t *)(rb->buffer) + rb->tail, data, first_size);
        memcpy(rb->buffer, data + first_size, size - first_size);
    }
    else
    {
        memcpy((uint8_t *)(rb->buffer) + rb->tail, data, size);
    }

    // 限制尾部位置
    if (rb->tail + size >= rb->buffer_size)
    {
        rb->tail = size - (rb->buffer_size - rb->tail);
    }
    else
    {
        rb->tail += size;
    }

    rb->data_count += size;

    return size;
}

uint16_t ringbuffer_read(ringbuffer_t *rb, uint8_t *data, uint16_t size)
{
    USER_ASSERT(rb != NULL);

    uint16_t used_size = ringbuffer_get_used(rb);

    if (size > used_size)
    {
        size = used_size; // 读取的数据超过了已有数据量
    }

    // 判断超过尾部没有
    if (rb->head + size > rb->buffer_size)
    {
        uint16_t first_size = rb->buffer_size - rb->head;
        memcpy(data, (uint8_t *)(rb->buffer) + rb->head, first_size);
        memcpy(data + first_size, rb->buffer, size - first_size);
    }
    else
    {
        memcpy(data, (uint8_t *)(rb->buffer) + rb->head, size);
    }

    // 限制头部位置
    if (rb->head + size >= rb->buffer_size)
    {
        rb->head = size - (rb->buffer_size - rb->head);
    }
    else
    {
        rb->head += size;
    }

    rb->data_count -= size;

    return size;
}

uint8_t *ringbuffer_linear_write_setup(ringbuffer_t *rb, uint16_t *size)
{
    USER_ASSERT(rb != NULL);
    USER_ASSERT(size != NULL);
    uint16_t max_size = ringbuffer_get_free(rb);

    if (max_size + rb->tail > rb->buffer_size)
    {
        max_size = rb->buffer_size - rb->tail;
    }

    *size = max_size;
    rb->last_series_count = max_size;

    return (uint8_t *)(rb->buffer) + rb->tail;
}

uint8_t *ringbuffer_linear_read_setup(ringbuffer_t *rb, uint16_t *size)
{
    USER_ASSERT(rb != NULL);
    USER_ASSERT(size != NULL);
    uint16_t max_size = ringbuffer_get_used(rb);

    if (max_size + rb->head > rb->buffer_size)
    {
        max_size = rb->buffer_size - rb->head;
    }

    *size = max_size;
    rb->last_series_count = max_size;

    return (uint8_t *)(rb->buffer) + rb->head;
}

uint16_t ringbuffer_linear_write_done(ringbuffer_t *rb, uint16_t size)
{
    USER_ASSERT(rb != NULL);

    // 限制尾部位置
    if (rb->tail + size >= rb->buffer_size)
    {
        rb->tail = size - (rb->buffer_size - rb->tail);
    }
    else
    {
        rb->tail += size;
    }

    rb->data_count += size;

    return size;
}

uint16_t ringbuffer_linear_read_done(ringbuffer_t *rb, uint16_t size)
{
    USER_ASSERT(rb != NULL);

    // 限制头部位置
    if (rb->head + size >= rb->buffer_size)
    {
        rb->head = size - (rb->buffer_size - rb->head);
    }
    else
    {
        rb->head += size;
    }

    rb->data_count -= size;

    return size;
}

void ringbuffer_mark_busy(ringbuffer_t *rb)
{
    USER_ASSERT(rb != NULL);
    rb->busy = true;
}

void ringbuffer_mark_idle(ringbuffer_t *rb)
{
    USER_ASSERT(rb != NULL);
    rb->busy = false;
}

bool ringbuffer_is_busy(ringbuffer_t *rb)
{
    USER_ASSERT(rb != NULL);
    return rb->busy;
}
