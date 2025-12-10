#ifndef NOTIFICATIONS_H
#define NOTIFICATIONS_H

#include "structures.h"

void notifications_init(NotificationQueue *queue);
void notifications_push(NotificationQueue *queue, NotificationType type, const char *message, double ttl_seconds);
void notifications_tick(NotificationQueue *queue, double delta_seconds);

#endif // NOTIFICATIONS_H
