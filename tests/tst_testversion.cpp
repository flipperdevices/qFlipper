#include <QtTest>

#include "versioninfo.h"

class TestVersion : public QObject
{
    Q_OBJECT

public:
    TestVersion() {}
    ~TestVersion() {}

private slots:
    void testInvalid();
    void testBasic();
    void testDevelopment();
    void testReleaseCandidate();
};

void TestVersion::testInvalid()
{
    QVERIFY(!VersionInfo().isValid());
    QVERIFY(!VersionInfo(QString()).isValid());
    QVERIFY(!VersionInfo(QStringLiteral("0.34")).isValid());
    QVERIFY(!VersionInfo(QStringLiteral("0.69.0-rcx")).isValid());
    QVERIFY(!VersionInfo(QStringLiteral("Hello there!")).isValid());
}

void TestVersion::testBasic()
{
    const auto major = 0, minor = 43, sub = 1;
    const auto stringVersion = QStringLiteral("%1.%2.%3").arg(major).arg(minor).arg(sub);
    const auto release1 = VersionInfo(major, minor, sub);
    const auto release2 = VersionInfo(stringVersion);

    QVERIFY(release1.isValid());
    QVERIFY(release2.isValid());

    QCOMPARE(release1.toString(), stringVersion);
    QCOMPARE(release2.toString(), stringVersion);

    QVERIFY(release1 == release2);
}

void TestVersion::testDevelopment()
{
    const auto commitString = QStringLiteral("badf00dcafe");
    const auto version = VersionInfo().withCommit(commitString);

    QVERIFY(version.isValid());
    QVERIFY(version.isDevelopment());
    QVERIFY(!version.isReleaseCandidate());

    QCOMPARE(version.commit(), commitString);

    QVERIFY(version == version);
    QVERIFY(!(version == version.withDate(QDate::currentDate())));
    QVERIFY(!(version == version.withBranch(QStringLiteral("my_branch"))));
}

void TestVersion::testReleaseCandidate()
{
    const auto rcNumber = 3;
    const auto rcSimple = VersionInfo(QStringLiteral("0.42.3-rc"));
    const auto rcNumbered = VersionInfo(QStringLiteral("0.42.3")).withRcNumber(rcNumber);
    const auto rcNumberedAlt = VersionInfo(QStringLiteral("0.42.3-rc%1").arg(rcNumber));

    QVERIFY(!VersionInfo().withRcNumber(rcNumber).isValid());
    QVERIFY(rcSimple.isValid());
    QVERIFY(rcNumbered.isValid());
    QVERIFY(rcNumberedAlt.isValid());

    QVERIFY(rcSimple.isReleaseCandidate());
    QVERIFY(rcNumbered.isReleaseCandidate());
    QVERIFY(rcNumberedAlt.isReleaseCandidate());

    QCOMPARE(rcNumbered.rc(), rcNumber);
    QCOMPARE(rcNumberedAlt.rc(), rcNumber);

    QVERIFY(rcNumbered == rcNumberedAlt);
}

QTEST_APPLESS_MAIN(TestVersion)

#include "tst_testversion.moc"
