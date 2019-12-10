#include <QString>
#include <QtTest>

class CTestExample : public QObject
{
public:
    CTestExample();

private Q_SLOTS:
    void testCase1();
};

CTestExample::CTestExample()
{
}

void CTestExample::testCase1()
{
    QVERIFY2(true, "Failure");
}

int main(int argc, char *argv[])
{
    CTestExample tc;
    return QTest::qExec(&tc, argc, argv);
}

