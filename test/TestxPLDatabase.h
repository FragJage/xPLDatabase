#include <cassert>
#include <thread>
#include "xPLLib/Schemas/SchemaObject.h"
#include "Plateforms/Plateforms.h"
#include "UnitTest/UnitTest.h"
#ifdef WIN32
    #include "Thread/mingw.thread.h"
#else
    #include <thread>
#endif
#include "../src/xPLDatabase.h"
#include "ControlSockMock.h"
#include "LogWriterMock.h"


class TestxPLDatabase : public TestClass<TestxPLDatabase>
{
public:
    TestxPLDatabase();
    ~TestxPLDatabase();
    static void ThreadStart(xPLDatabase* pxPLDev);
    bool Start();
    bool StdConfig();
    bool SetAdvConfig();
    bool GetAdvConfig();
    bool ModifyAdvConfig();
    bool CoverageCache();
    bool GetDevices();
    bool GetValuesNum();
    bool GetValuesBool();
    bool GetValuesAlpha();
    bool Stop();
    bool ReStart();
    bool MySQLConfig();
    bool MySQLAddValues();
    bool MySQLGetDevices();
    bool MySQLGetValues();
    bool WrongDbType();
    bool WrongSQLiteDb();
    bool WrongMySQLDb();
    bool DirectInsert();
    bool DelAdvConfig();
    bool ReStop();
    xPLDatabase xPLDev;
    LogWriterMock logWriterMock;
private:
    void RemoveSqliteFile();
};
