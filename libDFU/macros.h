#ifndef MACROS_H
#define MACROS_H

#define TRY_OR_ELSE(condition, failMsg) \
    if(!condition) { \
        qCritical() << dbgLabel << failMsg; \
        return false; \
    }

#define DEBUG_CRITICAL(msg) \
    qCritical() << dbgLabel << msg;

#endif // MACROS_H
