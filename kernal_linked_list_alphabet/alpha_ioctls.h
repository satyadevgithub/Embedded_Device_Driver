#ifndef ALPHA_IOCTLS_H
#define ALPHA_IOCTLS_H

#include <linux/ioctl.h>  // For ioctl definitions

#define SHOW_LIST_ALPHA _IOR('a', 1, int)  // Read from kernel to user
#define SHOW_LIST_VOWEL _IOR('a', 2, int)  // Read from kernel to user
#define SHOW_LIST_CONSO _IOR('a', 3, int)  // Read from kernel to user

#define DEL_LIST_ALPHA  _IOW('a', 4, int)  // Write to kernel from user
#define DEL_LIST_VOWEL  _IOW('a', 5, int)  // Write to kernel from user
#define DEL_LIST_CONSO  _IOW('a', 6, int)  // Write to kernel from user

#endif // ALPHA_IOCTLS_H
