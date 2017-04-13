#ifndef DEBUGCONFIG_H
#define DEBUGCONFIG_H
//_VDebug is used to choose a verbose debug
//which reports almost everything
#define _VDebug 0
#if _VDebug
#endif
//_AssDebug is used to choose a default c++ assertion debug
//which reports if the respective given conditions failed
#define _AssDebug 0
#if _AssDebug
#endif
#endif // DEBUGCONFIG_H
