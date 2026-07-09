#include <QTest>

// add necessary includes here

class tst_linestest : public QObject
{
    Q_OBJECT

public:
    tst_linestest();
    ~tst_linestest();

private slots:
    void test_case1();
};

tst_linestest::tst_linestest() {}

tst_linestest::~tst_linestest() {}

void tst_linestest::test_case1() {}

QTEST_APPLESS_MAIN(tst_linestest)

#include "tst_tst_linestest.moc"
