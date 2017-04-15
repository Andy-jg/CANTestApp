/*********************USAGE**********************************/
/* I) Prerequisite Information                              */
/*    Add #include "wadbgconf.h" to where this file is used */
/*    There are three Flags which feature PARALLELY         */
/* 1) To Enable Debug, Set _IsDebugEnabled To Non-Zero      */
/*    To Disable Debug, Set _IsDebugEnabled To Zero         */
/* 2) To Enable Assertion, Set _IsAsserted To Non-Zero      */
/*    To Disable Assertion, Set _IsAsserted To Zero         */
/*    When _IsAsserted = 0, function wdbg() and adbg()      */
/*      do the same things no matter which given conditions */
/* 3) To Add Position Into Debug Printed-Out Messages       */
/*    To Enable This, Set _IsLocated To Non-Zero            */
/*    To Disable This, Set _IsLocated To Zero               */
/*II) THREE FUNCTIONS: dbgwrp(), wdbg() and adbg()          */
/*    FUNCTION dbgwrp stands for Debugging Wrapper          */
/*    Usage: dbgwrp(<logical expression>, <code>) acts like */
/*      a switch to execute <code>, just simple as follows, */
/*      when <logical expression> is TRUE, then run <code>  */
/*      when <logical expression> is FALSE, do nothing      */
/*    FUNCTION wdbg inherits from qDebug()                  */
/*    Usage: wdbg(<Input>) acts like qDebug() << <Input>;   */
/*      Ex: <Input> can be "ahjhj" << 5+8 << QBArr.toHex()  */
/*    FUNCTION adbg imitates standard method assert(),      */
/*      and also inherits from qDebug()                     */
/*    Usage: _With NON-ZERO _IsAsserted,                    */
/*           |adbg(<logical expression>, <Input>) acts like */
/*           |IF NOT <logical expression> THEN wdbg(<Input>)*/
/*           |,meaning only print <Input> if condition fails*/
/*           _With ZERO _IsAsserted(),                      */
/*           |adbg(<logical expression>, <Input>) equals to */
/*           |wdbg(<Input>), meaning always print <Input>   */
/*********************EXAMPLES*******************************/
/*    Notation <10x> means _IsDebugEnabled Is Non-Zero      */
/*                         _IsAsserted Is Zero              */
/*                         _IsLocated Is Not Cared          */
/* 1) wdbg("debug message"<<4<<"infinite << can follows");  */
/*    <1x0>=>debug message 4 infinite << can follows        */
/*    <1x1>=>debug message 4 infinite << can follows <- int */
/*                          qMain(int, char**) "main.cpp" 8 */
/*    After "<-" is the location of code wdbg(...) above in */
/*      a format : <function-name> <file-name> <line>       */
/* 2) adbg(1<10, "this msg should not be printed out");     */
/*    <11x>=>""                                             */
/*                    (No Debug Message Cause 1<10 Is True) */
/*    <100>=>this msg should not be printed out             */
/*    <101>=>this msg should not be printed out <- int qMa- */
/*                            -in(int, char**) "main.cpp" 8 */
/* 3) adbg((5>3)&&((1>2)||(4<3)),"debug msg"<<3<<"etc");    */
/*    <110>=>ASSERT Failed : (5>3)&&((1<2)||(4<3)) |  debug */
/*                                                msg 3 etc */
/*    <111>=>ASSERT Failed : (5>3)&&((1<2)||(4<3)) |  debug */
/*         msg 3 etc <- int qMain(int, char**) "main.cpp" 8 */
/*    <101>=>debug msg 3 etc <- int qMain(int, char**) "ma- */
/*                                               -in.cpp" 8 */
/*    <100>=>debug msg 3 etc                                */
/* 4) Providing a situation that debug messages from an in- */
/*      -dividual class (not all classes) are not necessary */
/*      , then here is a way to turn on/off those messages: */
/*      a) Below #define <CLASSNAME> in file <classname>.h, */
/*         insert #define UniqueKeyNameToTurnOn4ThisClass 1 */
/*                                 (Turn Off = Set To Zero) */
/*      b) Use dbgwrp() in combination with wdbg() or adbg()*/
/*         Ex: dbgwrp(UniqueKeyNameToTurnOn4ThisClass,      */
/*              adbg(4>9,"So Each Class Have Its Own Key"));*/
/************************************************************/
#ifndef WADBGCONF_H
#define WADBGCONF_H
/*********************DebugFlags*****************************/
//_IsDebugEnabled Is Zero => Debug Is Globally Disabled
#define _IsDebugEnabled 1
//_IsAsserted Is Non-Zero => Debug Is Additionally Conditioned
#define _IsAsserted 1
//_IsLocated Is Non-Zero => Debug Is Verbosely Located
#define _IsLocated 1
/*********************Function wdbg/adbg*********************/
#if _IsDebugEnabled
    #define dbgwrp( boolexpr, ...)                           \
        if (boolexpr) { __VA_ARGS__ ;}
    #define wdbg(...) qDebug() << __VA_ARGS__ << "" PosTail;
    #define adbg( boolexpr, ...)                             \
        IfNotHead(boolexpr)                                  \
            wdbg("" AssHead(boolexpr) << __VA_ARGS__)
#else
    #define wdbg(...)
    #define adbg( boolexpr, ...)
#endif
/*********************Miscellany*****************************/
#ifdef _WIN32
    #define __FILENAME__ QString(__FILE__).split('\\'        \
                        , QString::SkipEmptyParts).last()
#else
    #define __FILENAME__ QString(__FILE__).split('/'         \
                        , QString::SkipEmptyParts).last()
#endif
#if _IsAsserted
    #define IfNotHead(boolexpr) if (!(boolexpr))
    #define AssHead(boolexpr) "ASSERT Failed : " #boolexpr   \
                              " | "
#else
    #define IfNotHead(boolexpr)
    #define AssHead(boolexpr)
#endif
#if _IsLocated
    #define PosTail "<-"                                     \
        << __PRETTY_FUNCTION__ << __FILENAME__ << __LINE__
#else
    #define PosTail
#endif
/************************************************************/
#include <QDebug>
/************************************************************/
#endif // WADBGCONF_H
