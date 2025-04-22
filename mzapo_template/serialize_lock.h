#ifndef SERIALIZE_LOCK_H
#define SERIALIZE_LOCK_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int serialize_lock(int no_wait);

void serialize_unlock(void);

#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif  /*SERIALIZE_LOCK_H*/
