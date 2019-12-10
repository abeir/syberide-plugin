%{Cpp:LicenseTemplate}\
#include <QString>
#include <QtTest>
@if %{requiresQApplication}
#include <QCoreApplication>
@endif

class %{CN} : public QObject
{
  Q_OBJECT

  public:
  %{CN}();

private Q_SLOTS:
@if %{generateICCode}
   void initTestCase();
   void cleanupTestCase();
@endif
@if %{useTestDatabase}
   void %{testSlotName}_data();
@endif
   void %{testSlotName}();

};

%{CN}::%{CN}()
{
}

@if %{generateICCode}
void %{CN}::initTestCase()
{

}

void %{CN}::cleanupTestCase()
{

}

@endif
@if %{useTestDatabase}
void %{CN}::%{testSlotName}_data()
{
    QTest::addColumn<QString>("data");
    QTest::newRow("0") << QString();
}

@endif
void %{CN}::%{testSlotName}()
{
@if %{useTestDatabase}
   QFETCH(QString, data);
@endif
@if '%{testType}' === 'Test'
   QVERIFY2(true, "Failure");
@else
   QBENCHMARK {
   }
@endif
}

@if %{requiresQApplication}
QTEST_MAIN(%{CN})
@else
QTEST_APPLESS_MAIN(%{CN})
@endif


