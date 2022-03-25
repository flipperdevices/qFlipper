#pragma once

#include <QDate>
#include <QString>

class VersionInfo
{
    enum VersionIndex {
        VersionMajor = 0,
        VersionMinor,
        VersionSub
    };

public:
    VersionInfo();
    VersionInfo(int major, int minor, int sub);
    VersionInfo(const QString &versionString);

    VersionInfo withCommit(const QString &commit) const;
    VersionInfo withBranch(const QString &branch) const;
    VersionInfo withRcNumber(int candidateNum) const;
    VersionInfo withDate(const QDate &date) const;

    bool isValid() const;
    bool isVersionValid() const;
    bool isCommitValid() const;

    bool isDevelopment() const;
    bool isReleaseCandidate() const;

    int major() const;
    int minor() const;
    int sub() const;
    int rc() const;

    const QDate &date() const;
    const QString &commit() const;
    const QString &branch() const;

    QString toString() const;

    bool operator >(const VersionInfo &other) const;

private:
    bool isVersionGreaterThan(const VersionInfo &other) const;
    bool isRCNumGreaterThan(const VersionInfo &other) const;
    bool isDateGreaterThan(const VersionInfo &other) const;
    bool isCommitDifferent(const VersionInfo &other) const;

    QVector<int> m_version;
    int m_rc;

    QDate m_date;
    QString m_commit;
    QString m_branch;
};

