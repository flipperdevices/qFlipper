#include "versioninfo.h"

#include <QDebug>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(CATEGORY_DEBUG)

VersionInfo::VersionInfo():
    VersionInfo(-1, -1, -1)
{}

VersionInfo::VersionInfo(int major, int minor, int sub):
    m_version{major, minor, sub},
    m_rc(-1)
{}

VersionInfo::VersionInfo(const QString &versionString):
    VersionInfo()
{
    const auto tokens = versionString.toLower().split('-');
    const auto versionTokens = tokens[0].split('.');

    if(versionTokens.size() != 3) {
        qCDebug(CATEGORY_DEBUG) << "Malformed version string";
        return;
    }

    bool canConvert;

    for(auto it = versionTokens.cbegin(); it != versionTokens.cend(); ++it) {
        const auto num = (*it).toInt(&canConvert);

        if(canConvert) {
            const auto idx = std::distance(versionTokens.cbegin(), it);
            m_version[idx] = num;
        } else {
            qCDebug(CATEGORY_DEBUG) << "Illegal characters in version";
            return;
        }
    }

    if(tokens.size() == 2) {
        const auto &suffix = tokens[1];
        if(suffix.startsWith(QStringLiteral("rc"))) {
            if(suffix.length() == 2) {
                m_rc = 0;
                return;
            }

            const auto rc = suffix.midRef(2).toInt(&canConvert);
            if(canConvert) {
                m_rc = rc;
            } else {
                qCDebug(CATEGORY_DEBUG) << "Illegal characters in release candidate suffix";
            }
        }

    } else {
        qCDebug(CATEGORY_DEBUG) << "Malformed release candidate suffix";
    }
}

VersionInfo VersionInfo::withCommit(const QString &commit) const
{
    auto ret = *this;
    ret.m_commit = commit;
    return ret;
}

VersionInfo VersionInfo::withBranch(const QString &branch) const
{
    auto ret = *this;
    ret.m_branch = branch;
    return ret;
}

VersionInfo VersionInfo::withRcNumber(int candidateNum) const
{
    auto ret = *this;
    ret.m_rc = candidateNum;
    return ret;
}

VersionInfo VersionInfo::withDate(const QDate &date) const
{
    auto ret = *this;
    ret.m_date = date;
    return ret;
}

bool VersionInfo::isValid() const
{
    return isVersionValid() || isCommitValid();
}

bool VersionInfo::isVersionValid() const
{
    for(const auto num: m_version) {
        if(num < 0) {
            return false;
        }
    }

    return true;
}

bool VersionInfo::isCommitValid() const
{
    return !m_commit.isEmpty();
}

bool VersionInfo::isDevelopment() const
{
    return !isVersionValid() && isCommitValid();
}

bool VersionInfo::isReleaseCandidate() const
{
    return m_rc >= 0;
}

int VersionInfo::major() const
{
    return m_version[0];
}

int VersionInfo::minor() const
{
    return m_version[1];
}

int VersionInfo::sub() const
{
    return m_version[2];
}

int VersionInfo::rc() const
{
    return m_rc;
}

const QDate &VersionInfo::date() const
{
    return m_date;
}

const QString &VersionInfo::commit() const
{
    return m_commit;
}

const QString &VersionInfo::branch() const
{
    return m_branch;
}

QString VersionInfo::toString() const
{
    if(!isValid()) {
        return QStringLiteral("invalid");
    } else if(isDevelopment()) {
        return m_commit;
    }

    QStringList ver;

    for(const auto num: m_version) {
        ver.append(QString::number(num));
    }

    const auto ret = ver.join('.');

    if(isReleaseCandidate()) {
        return m_rc ? QStringLiteral("%1-rc%2").arg(ret, m_rc) :
                      QStringLiteral("%1-rc").arg(ret);
    } else {
        return ret;
    }
}

//bool VersionInfo::operator >(const VersionInfo &other) const
//{
//    if(!isValid() || !other.isValid()) {
//        return false;

//    } else if(other.isDevelopment()) {
//        if(isDevelopment()) {

//        } else if(isReleaseCandidate()) {

//        } else {

//        }

//    } else if(other.isReleaseCandidate()) {
//        if(isDevelopment()) {

//        } else if(isReleaseCandidate()) {

//        } else {

//        }

//    } else {
//        if(isDevelopment()) {

//        } else if(isReleaseCandidate()) {

//        } else {
//            if(major() != other.major()) {
//                return major() > other.major();
//            } else if(minor() != other.minor()) {
//                return minor() > other.minor();
//            } else {
//                return sub() > other.sub();
//            }
//        }
//    }

//    return false;
//}
