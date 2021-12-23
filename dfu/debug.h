#pragma once

#include <QDebug>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(CATEGORY_DEBUG)

#define check_return_bool(condition, failMsg) \
    if(!(condition)) { \
        qCDebug(CATEGORY_DEBUG).noquote() << (failMsg); \
        return false; \
    }

#define check_return_void(condition, failMsg) \
    if(!(condition)) { \
        qCDebug(CATEGORY_DEBUG).noquote() << (failMsg); \
        return; \
    }

#define check_return_val(condition, failMsg, val) \
    if(!(condition)) { \
        qCDebug(CATEGORY_DEBUG).noquote() << (failMsg); \
        return val; \
    }

#define check_continue(condition, failMsg) \
    if(!(condition)) { \
        qCDebug(CATEGORY_DEBUG).noquote() << (failMsg); \
    }

#define error_msg(msg) \
    qCDebug(CATEGORY_DEBUG).noquote() << (msg);

#define debug_msg(msg) \
    qCDebug(CATEGORY_DEBUG).noquote() << (msg);

#define begin_ignore_block() \
    qCDebug(CATEGORY_DEBUG).noquote() << "vvv ATTENTION: Ignore errors below until matching closing message. vvv";

#define end_ignore_block() \
    qCDebug(CATEGORY_DEBUG).noquote() << "^^^ ATTENTION: Ignore errors above until matching opening message. ^^^";
