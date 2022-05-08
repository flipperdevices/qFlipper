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

    // Parse release-candidate suffix first
    if(tokens.size() == 2) {
        const auto &suffix = tokens[1];
        const auto correctSuffix = QStringLiteral("rc");

        if(!suffix.startsWith(correctSuffix)) {
            qCDebug(CATEGORY_DEBUG) << "Malformed release candidate suffix";
            return;

        } else if(suffix == correctSuffix) {
            m_rc = 0;

        } else {
            const auto rc = suffix.midRef(2).toInt(&canConvert);
            if(canConvert) {
                m_rc = rc;
            } else {
                qCDebug(CATEGORY_DEBUG) << "Illegal characters in release candidate suffix";
                return;
            }
        }
    }

    // Parse version
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
    return m_version[VersionMajor];
}

int VersionInfo::minor() const
{
    return m_version[VersionMinor];
}

int VersionInfo::sub() const
{
    return m_version[VersionSub];
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

bool VersionInfo::isVersionGreaterThan(const VersionInfo &other) const
{
    if(major() != other.major()) {
        return major() > other.major();
    } else if(minor() != other.minor()) {
        return minor() > other.minor();
    } else {
        return sub() > other.sub();
    }
}

bool VersionInfo::isRCNumGreaterThan(const VersionInfo &other) const
{
    return rc() > other.rc();
}

bool VersionInfo::isDateGreaterThan(const VersionInfo &other) const
{
    return other.date().daysTo(date()) > 0;
}

bool VersionInfo::isCommitDifferent(const VersionInfo &other) const
{
    return commit() != other.commit();
}

bool VersionInfo::operator >(const VersionInfo &other) const
{
    Q_UNUSED(other)
//    if(!isValid() || !other.isValid()) {
//        return false;

//    } else if(other.isDevelopment()) {
//        if(isDevelopment()) {
//            return isCommitDifferent(other) || isDateGreaterThan(other);
//        } else {
//            return isDateGreaterThan(other);
//        }

//    } else if(other.isReleaseCandidate()) {
//        if(isDevelopment()) {
//            return isDateGreaterThan(other);
//        } else if(isReleaseCandidate()) {
//            return isVersionGreaterThan(other) || isRCNumGreaterThan(other);
//        } else {
//            return isVersionGreaterThan(other);
//        }

//    } else {
//        if(isDevelopment()) {
//            return isDateGreaterThan(other);
//        } else if(isReleaseCandidate()) {
//            return isVersionGreaterThan(other);
//        } else {
//            return isVersionGreaterThan(other);
//        }
//    }
    return false;
}

bool VersionInfo::operator ==(const VersionInfo &other) const
{
    if(!isValid() || !other.isValid()) {
        return false;

    } else if(other.isDevelopment()) {
        if(isDevelopment()) {
            return (m_commit == other.m_commit) && (m_branch == other.m_branch) && (m_date == other.m_date);
        } else {
            return false;
        }

    } else if(other.isReleaseCandidate()) {
        if(isReleaseCandidate()) {
            return (m_version == other.m_version) && (m_rc == other.m_rc);
        } else {
            return false;
        }

    } else {
        if(!isDevelopment() && !isReleaseCandidate()) {
            return m_version == other.m_version;
        } else {
            return false;
        }
    }
}
