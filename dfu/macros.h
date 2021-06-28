#ifndef MACROS_H
#define MACROS_H

#include <QDebug>

#define check_return_bool(condition, failMsg) \
    if(!(condition)) { \
        qCritical() << Q_FUNC_INFO << ":" << failMsg; \
        return false; \
    }

#define check_return_void(condition, failMsg) \
    if(!(condition)) { \
        qCritical() << Q_FUNC_INFO << ":" << failMsg; \
        return; \
    }

#define check_return_val(condition, failMsg, val) \
    if(!(condition)) { \
        qCritical() << Q_FUNC_INFO << ":" << failMsg; \
        return val; \
    }

#define check_continue(condition, failMsg) \
    if(!(condition)) { \
        qCritical() << Q_FUNC_INFO << ":" << failMsg; \
    }

#define error_msg(msg) \
    qCritical() << Q_FUNC_INFO << ":" << msg;

#define info_msg(msg) \
    qInfo() << Q_FUNC_INFO << ":" << msg;

#define debug_msg(msg) \
    qDebug() << Q_FUNC_INFO << ":" << msg;

#endif // MACROS_H
