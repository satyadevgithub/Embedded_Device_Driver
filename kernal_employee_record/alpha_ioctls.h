#ifndef ALPHA_IOCTLS_H
#define ALPHA_IOCTLS_H

#include <linux/ioctl.h>

#define SHOW_LIST _IO('a', 1) // Display employee list
#define DEL_LIST  _IO('a', 2) // Delete employee list

#endif // ALPHA_IOCTLS_H

