#ifndef LINUX_INIT_H
#define LINUX_INIT_H

#define __init __attribute__ ((__section__ (".init.text")))
#define __initdata __attribute__ ((__section__ ("init.data")))

#endif
