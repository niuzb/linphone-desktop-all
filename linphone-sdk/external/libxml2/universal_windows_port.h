
#ifndef LIBXML2_PORT_H
#define LIBXML2_PORT_H

#include <windows.h>

#define GetVersionEx(osvi)		(((osvi)->dwPlatformId = 0) != 0)
#define getcwd(a, b) NULL

#endif /* LIBXML2_PORT_H */
