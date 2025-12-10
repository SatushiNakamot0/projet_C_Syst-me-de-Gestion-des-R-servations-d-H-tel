#include "notifications.h"

#include <string.h>

void notifications_init(NotificationQueue *queue) {
    if (!queue) {
        return;
    }
    memset(queue, 0, sizeof(NotificationQueue));
}

void notifications_push(NotificationQueue *queue, NotificationType type, const char *message, double ttl_seconds) {
    if (!queue || !message) {
        return;
    }
    if (queue->count == MAX_TOASTS_DISPLAYED) {
        memmove(&queue->items[0], &queue->items[1], sizeof(Notification) * (MAX_TOASTS_DISPLAYED - 1));
        queue->count -= 1;
    }
    Notification *notif = &queue->items[queue->count++];
    notif->type = type;
    strncpy(notif->message, message, sizeof(notif->message) - 1);
    notif->message[sizeof(notif->message) - 1] = '\0';
    notif->ttl_seconds = ttl_seconds;
}

void notifications_tick(NotificationQueue *queue, double delta_seconds) {
    if (!queue || queue->count == 0) {
        return;
    }
    size_t write_index = 0;
    for (size_t i = 0; i < queue->count; ++i) {
        Notification *notif = &queue->items[i];
        notif->ttl_seconds -= delta_seconds;
        if (notif->ttl_seconds > 0.0) {
            if (write_index != i) {
                queue->items[write_index] = *notif;
            }
            write_index++;
        }
    }
    queue->count = write_index;
}
