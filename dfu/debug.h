#pragma once

#include <QDebug>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(CATEGORY_DEBUG)

#define check_return_bool(condition, failMsg) \
    if(!(condition)) { \
        qCCritical(CATEGORY_DEBUG) << Q_FUNC_INFO << ":" << failMsg; \
        return false; \
    }

#define check_return_void(condition, failMsg) \
    if(!(condition)) { \
        qCCritical(CATEGORY_DEBUG) << Q_FUNC_INFO << ":" << failMsg; \
        return; \
    }

#define check_return_val(condition, failMsg, val) \
    if(!(condition)) { \
        qCCritical(CATEGORY_DEBUG) << Q_FUNC_INFO << ":" << failMsg; \
        return val; \
    }

#define check_continue(condition, failMsg) \
    if(!(condition)) { \
        qCCritical(CATEGORY_DEBUG) << Q_FUNC_INFO << ":" << failMsg; \
    }

#define error_msg(msg) \
    qCCritical(CATEGORY_DEBUG) << Q_FUNC_INFO << ":" << (msg);

#define debug_msg(msg) \
    qCDebug(CATEGORY_DEBUG) << Q_FUNC_INFO << ":" << (msg);

#define begin_ignore_block() \
    qCDebug(CATEGORY_DEBUG) << "vvv ATTENTION: Ignore errors below until matching closing message. vvv";

#define end_ignore_block() \
    qCDebug(CATEGORY_DEBUG) << "^^^ ATTENTION: Ignore errors above until matching opening message. ^^^";
