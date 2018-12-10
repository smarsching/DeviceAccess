#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE SubdeviceBackendTest
#include <boost/test/unit_test.hpp>
using namespace boost::unit_test_framework;

#include <thread>
#include "Device.h"

using namespace ChimeraTK;

#define CHECK_TIMEOUT(execPreCheck, condition, maxMilliseconds)                                                     \
    {                                                                                                               \
      std::chrono::steady_clock::time_point t0 = std::chrono::steady_clock::now();                                  \
      execPreCheck                                                                                                  \
      while(!(condition)) {                                                                                         \
        bool timeout_reached = (std::chrono::steady_clock::now()-t0) > std::chrono::milliseconds(maxMilliseconds);  \
        BOOST_CHECK( !timeout_reached );                                                                            \
        if(timeout_reached) break;                                                                                  \
        usleep(1000);                                                                                               \
        execPreCheck                                                                                                \
      }                                                                                                             \
    }

BOOST_AUTO_TEST_SUITE( SubdeviceBackendTestSuite )

/*********************************************************************************************************************/

BOOST_AUTO_TEST_CASE( testOpenClose ) {

    setDMapFilePath("subdeviceTest.dmap");

    Device dev;
    BOOST_CHECK(!dev.isOpened());
    dev.open("SUBDEV1");
    BOOST_CHECK(dev.isOpened());
    dev.close();
    BOOST_CHECK(!dev.isOpened());
    dev.open();
    BOOST_CHECK(dev.isOpened());
    dev.close();
    BOOST_CHECK(!dev.isOpened());

}

/// @todo Test exception handling!

/*********************************************************************************************************************/

BOOST_AUTO_TEST_CASE( testMayReplaceOther ) {

    setDMapFilePath("subdeviceTest.dmap");

    Device dev;
    dev.open("SUBDEV1");
    Device target;
    target.open("TARGET1");

    {
      auto acc1  = dev.getScalarRegisterAccessor<int32_t>("APP.0.MY_REGISTER1", 0, {AccessMode::raw});
      auto acc1_2  = dev.getScalarRegisterAccessor<int32_t>("APP.0.MY_REGISTER1", 0, {AccessMode::raw});
      BOOST_CHECK(acc1.getHighLevelImplElement()->mayReplaceOther(acc1_2.getHighLevelImplElement()));
      BOOST_CHECK(acc1_2.getHighLevelImplElement()->mayReplaceOther(acc1.getHighLevelImplElement()));
    }

    {
      auto acc1  = dev.getScalarRegisterAccessor<int32_t>("APP.0.MY_REGISTER1", 0, {AccessMode::raw});
      auto acc1_2  = dev.getScalarRegisterAccessor<int32_t>("APP.0.MY_REGISTER1");
      BOOST_CHECK(!acc1.getHighLevelImplElement()->mayReplaceOther(acc1_2.getHighLevelImplElement()));
      BOOST_CHECK(!acc1_2.getHighLevelImplElement()->mayReplaceOther(acc1.getHighLevelImplElement()));
    }

    {
      auto acc1  = dev.getScalarRegisterAccessor<int32_t>("APP.0.MY_REGISTER2");
      auto acc1_2  = dev.getScalarRegisterAccessor<int32_t>("APP.0.MY_REGISTER2");
      BOOST_CHECK(acc1.getHighLevelImplElement()->mayReplaceOther(acc1_2.getHighLevelImplElement()));
      BOOST_CHECK(acc1_2.getHighLevelImplElement()->mayReplaceOther(acc1.getHighLevelImplElement()));
    }

    {
      auto acc1  = dev.getScalarRegisterAccessor<int32_t>("APP.0.MY_REGISTER1");
      auto acc1_2  = dev.getScalarRegisterAccessor<int32_t>("APP.0.MY_REGISTER2");
      BOOST_CHECK(!acc1.getHighLevelImplElement()->mayReplaceOther(acc1_2.getHighLevelImplElement()));
      BOOST_CHECK(!acc1_2.getHighLevelImplElement()->mayReplaceOther(acc1.getHighLevelImplElement()));
    }

    {
      auto acc1  = dev.getScalarRegisterAccessor<int32_t>("APP.0.MY_REGISTER2");
      auto acc1_2  = dev.getScalarRegisterAccessor<int16_t>("APP.0.MY_REGISTER2");
      BOOST_CHECK(!acc1.getHighLevelImplElement()->mayReplaceOther(acc1_2.getHighLevelImplElement()));
      BOOST_CHECK(!acc1_2.getHighLevelImplElement()->mayReplaceOther(acc1.getHighLevelImplElement()));
    }

}

/*********************************************************************************************************************/

BOOST_AUTO_TEST_CASE( testWriteScalarRaw ) {

    setDMapFilePath("subdeviceTest.dmap");

    Device dev;
    dev.open("SUBDEV1");
    Device target;
    target.open("TARGET1");

    auto acc1  = dev.getScalarRegisterAccessor<int32_t>("APP.0.MY_REGISTER1", 0, {AccessMode::raw});
    auto acc1t = target.getScalarRegisterAccessor<int32_t>("APP.0.THE_AREA", 0, {AccessMode::raw});

    acc1 = 42;
    acc1.write();
    acc1t.read();
    BOOST_CHECK_EQUAL( (int32_t)acc1t, 42 );

    acc1 = -120;
    acc1.write();
    acc1t.read();
    BOOST_CHECK_EQUAL( (int32_t)acc1t, -120 );

    auto acc2  = dev.getScalarRegisterAccessor<int32_t>("APP.0.MY_REGISTER2", 0, {AccessMode::raw});
    auto acc2t = target.getScalarRegisterAccessor<int32_t>("APP.0.THE_AREA", 1, {AccessMode::raw});

    acc2 = 666;
    acc2.write();
    acc2t.read();
    BOOST_CHECK_EQUAL( (int32_t)acc2t, 666 );

    acc2 = -99999;
    acc2.write();
    acc2t.read();
    BOOST_CHECK_EQUAL( (int32_t)acc2t, -99999 );

    acc2.setAsCoocked<float>(42.5);
    BOOST_CHECK_EQUAL( (int32_t)acc2, 170 );    // 42.5*4, 2 fractional bits
    acc2 = 666*4;
    BOOST_CHECK_CLOSE( acc2.getAsCoocked<float>(), 666, 0.01 );

    dev.close();

}

/*********************************************************************************************************************/

BOOST_AUTO_TEST_CASE( testWriteScalarInAreaRaw ) {

    setDMapFilePath("subdeviceTest.dmap");

    Device dev;
    dev.open("SUBDEV1");
    Device target;
    target.open("TARGET1");

    auto acc1  = dev.getScalarRegisterAccessor<int32_t>("APP.0.MY_AREA1", 0, {AccessMode::raw});
    auto acc1t = target.getScalarRegisterAccessor<int32_t>("APP.0.THE_AREA", 2, {AccessMode::raw});

    acc1 = 42;
    acc1.write();
    acc1t.read();
    BOOST_CHECK_EQUAL( (int32_t)acc1t, 42 );

    acc1 = -120;
    acc1.write();
    acc1t.read();
    BOOST_CHECK_EQUAL( (int32_t)acc1t, -120 );

    auto acc2  = dev.getScalarRegisterAccessor<int32_t>("APP.0.MY_AREA1", 3, {AccessMode::raw});
    auto acc2t = target.getScalarRegisterAccessor<int32_t>("APP.0.THE_AREA", 5, {AccessMode::raw});

    acc2 = 666;
    acc2.write();
    acc2t.read();
    BOOST_CHECK_EQUAL( (int32_t)acc2t, 666 );

    acc2 = -99999;
    acc2.write();
    acc2t.read();
    BOOST_CHECK_EQUAL( (int32_t)acc2t, -99999 );

    dev.close();

}

/*********************************************************************************************************************/

BOOST_AUTO_TEST_CASE( testWriteArrayRaw ) {

    setDMapFilePath("subdeviceTest.dmap");

    Device dev;
    dev.open("SUBDEV1");
    Device target;
    target.open("TARGET1");

    auto acc1  = dev.getOneDRegisterAccessor<int32_t>("APP.0.MY_AREA1", 0, 0, {AccessMode::raw});
    auto acc1t = target.getOneDRegisterAccessor<int32_t>("APP.0.THE_AREA", 6, 2, {AccessMode::raw});

    acc1 = { 10, 20, 30, 40, 50, 60 };
    acc1.write();
    acc1t.read();
    BOOST_CHECK( (std::vector<int32_t>)acc1t == std::vector<int32_t>({ 10, 20, 30, 40, 50, 60 }) );


    acc1 = { 15, 25, 35, 45, 55, 65 };
    acc1.write();
    acc1t.read();
    BOOST_CHECK( (std::vector<int32_t>)acc1t == std::vector<int32_t>({ 15, 25, 35, 45, 55, 65 }) );

    dev.close();

}

/*********************************************************************************************************************/

BOOST_AUTO_TEST_CASE( testWriteScalarCooked ) {

    setDMapFilePath("subdeviceTest.dmap");

    Device dev;
    dev.open("SUBDEV1");
    Device target;
    target.open("TARGET1");

    auto acc1  = dev.getScalarRegisterAccessor<double>("APP.0.MY_REGISTER1");   // 0 fractional bits
    auto acc1t = target.getScalarRegisterAccessor<int32_t>("APP.0.THE_AREA", 0, {AccessMode::raw});

    acc1 = 42;
    acc1.write();
    acc1t.read();
    BOOST_CHECK_EQUAL( (int32_t)acc1t, 42 );

    acc1 = -120;
    acc1.write();
    acc1t.read();
    BOOST_CHECK_EQUAL( (int32_t)acc1t, -120 );

    auto acc2  = dev.getScalarRegisterAccessor<double>("APP.0.MY_REGISTER2");   // 2 fractional bits
    auto acc2t = target.getScalarRegisterAccessor<int32_t>("APP.0.THE_AREA", 1, {AccessMode::raw});

    acc2 = 666;
    acc2.write();
    acc2t.read();
    BOOST_CHECK_EQUAL( (int32_t)acc2t, 666*4 );

    acc2 = -333;
    acc2.write();
    acc2t.read();
    BOOST_CHECK_EQUAL( (int32_t)acc2t, (-333*4) & 0x3FFFF );   // the raw value does not get negative since we have 18 bits only

    acc2 = -99999;
    acc2.write();
    acc2t.read();
    BOOST_CHECK_EQUAL( (int32_t)acc2t, 131072 );                // negative overflow

    dev.close();

}

/*********************************************************************************************************************/

BOOST_AUTO_TEST_CASE( testWriteArrayCooked ) {

    setDMapFilePath("subdeviceTest.dmap");

    Device dev;
    dev.open("SUBDEV1");
    Device target;
    target.open("TARGET1");

    auto acc1  = dev.getOneDRegisterAccessor<int32_t>("APP.0.MY_AREA1");
    auto acc1t = target.getOneDRegisterAccessor<int32_t>("APP.0.THE_AREA", 6, 2, {AccessMode::raw});

    acc1 = { 10, 20, 30, 40, 50, 60 };
    acc1.write();
    acc1t.read();
    BOOST_CHECK( (std::vector<int32_t>)acc1t == std::vector<int32_t>({ 10 * 65536, 20 * 65536, 30 * 65536,
                                                                       40 * 65536, 50 * 65536, 60 * 65536 }) );


    acc1 = { 15, 25, 35, 45, 55, 65 };
    acc1.write();
    acc1t.read();
    BOOST_CHECK( (std::vector<int32_t>)acc1t == std::vector<int32_t>({ 15 * 65536, 25 * 65536, 35 * 65536,
                                                                       45 * 65536, 55 * 65536, 65  * 65536}) );

    dev.close();

}

/*********************************************************************************************************************/
/*********************************************************************************************************************/

BOOST_AUTO_TEST_CASE( testReadScalarRaw ) {

    setDMapFilePath("subdeviceTest.dmap");

    Device dev;
    dev.open("SUBDEV1");
    Device target;
    target.open("TARGET1");

    auto acc1  = dev.getScalarRegisterAccessor<int32_t>("APP.0.MY_REGISTER1", 0, {AccessMode::raw});
    auto acc1t = target.getScalarRegisterAccessor<int32_t>("APP.0.THE_AREA", 0, {AccessMode::raw});

    acc1t = 42;
    acc1t.write();
    acc1.read();
    BOOST_CHECK_EQUAL( (int32_t)acc1, 42 );

    acc1t = -120;
    acc1t.write();
    acc1.read();
    BOOST_CHECK_EQUAL( (int32_t)acc1, -120 );

    auto acc2  = dev.getScalarRegisterAccessor<int32_t>("APP.0.MY_REGISTER2", 0, {AccessMode::raw});
    auto acc2t = target.getScalarRegisterAccessor<int32_t>("APP.0.THE_AREA", 1, {AccessMode::raw});

    acc2t = 666;
    acc2t.write();
    acc2.read();
    BOOST_CHECK_EQUAL( (int32_t)acc2, 666 );

    acc2t = -99999;
    acc2t.write();
    acc2.read();
    BOOST_CHECK_EQUAL( (int32_t)acc2, -99999 );

    dev.close();

}

/*********************************************************************************************************************/

BOOST_AUTO_TEST_CASE( testReadScalarInAreaRaw ) {

    setDMapFilePath("subdeviceTest.dmap");

    Device dev;
    dev.open("SUBDEV1");
    Device target;
    target.open("TARGET1");

    auto acc1  = dev.getScalarRegisterAccessor<int32_t>("APP.0.MY_AREA1", 0, {AccessMode::raw});
    auto acc1t = target.getScalarRegisterAccessor<int32_t>("APP.0.THE_AREA", 2, {AccessMode::raw});

    acc1t = 42;
    acc1t.write();
    acc1.read();
    BOOST_CHECK_EQUAL( (int32_t)acc1, 42 );

    acc1t = -120;
    acc1t.write();
    acc1.read();
    BOOST_CHECK_EQUAL( (int32_t)acc1, -120 );

    auto acc2  = dev.getScalarRegisterAccessor<int32_t>("APP.0.MY_AREA1", 3, {AccessMode::raw});
    auto acc2t = target.getScalarRegisterAccessor<int32_t>("APP.0.THE_AREA", 5, {AccessMode::raw});

    acc2t = 666;
    acc2t.write();
    acc2.read();
    BOOST_CHECK_EQUAL( (int32_t)acc2, 666 );

    acc2t = -99999;
    acc2t.write();
    acc2.read();
    BOOST_CHECK_EQUAL( (int32_t)acc2, -99999 );

    dev.close();

}

/*********************************************************************************************************************/

BOOST_AUTO_TEST_CASE( testReadArrayRaw ) {

    setDMapFilePath("subdeviceTest.dmap");

    Device dev;
    dev.open("SUBDEV1");
    Device target;
    target.open("TARGET1");

    auto acc1  = dev.getOneDRegisterAccessor<int32_t>("APP.0.MY_AREA1", 0, 0, {AccessMode::raw});
    auto acc1t = target.getOneDRegisterAccessor<int32_t>("APP.0.THE_AREA", 6, 2, {AccessMode::raw});

    acc1t = { 10, 20, 30, 40, 50, 60 };
    acc1t.write();
    acc1.read();
    BOOST_CHECK( (std::vector<int32_t>)acc1 == std::vector<int32_t>({ 10, 20, 30, 40, 50, 60 }) );


    acc1t = { 15, 25, 35, 45, 55, 65 };
    acc1t.write();
    acc1.read();
    BOOST_CHECK( (std::vector<int32_t>)acc1 == std::vector<int32_t>({ 15, 25, 35, 45, 55, 65 }) );

    dev.close();

}

/*********************************************************************************************************************/

BOOST_AUTO_TEST_CASE( testReadScalarCooked ) {

    setDMapFilePath("subdeviceTest.dmap");

    Device dev;
    dev.open("SUBDEV1");
    Device target;
    target.open("TARGET1");

    auto acc1  = dev.getScalarRegisterAccessor<double>("APP.0.MY_REGISTER1");   // 0 fractional bits
    auto acc1t = target.getScalarRegisterAccessor<int32_t>("APP.0.THE_AREA", 0, {AccessMode::raw});

    acc1t = 42;
    acc1t.write();
    acc1.read();
    BOOST_CHECK_EQUAL( (int32_t)acc1, 42 );

    acc1t = -120;
    acc1t.write();
    acc1.read();
    BOOST_CHECK_EQUAL( (int32_t)acc1, -120 );

    auto acc2  = dev.getScalarRegisterAccessor<double>("APP.0.MY_REGISTER2");   // 2 fractional bits
    auto acc2t = target.getScalarRegisterAccessor<int32_t>("APP.0.THE_AREA", 1, {AccessMode::raw});

    acc2t = 666*4;
    acc2t.write();
    acc2.read();
    BOOST_CHECK_EQUAL( (int32_t)acc2, 666 );

    acc2t = -333*4;
    acc2t.write();
    acc2.read();
    BOOST_CHECK_EQUAL( (int32_t)acc2, -333 );   // the raw value does not get negative since we have 18 bits only

    acc2t = 131072;
    acc2t.write();
    acc2.read();
    BOOST_CHECK_EQUAL( (int32_t)acc2, -32768 );

    dev.close();

}

/*********************************************************************************************************************/

BOOST_AUTO_TEST_CASE( testReadArrayCooked ) {

    setDMapFilePath("subdeviceTest.dmap");

    Device dev;
    dev.open("SUBDEV1");
    Device target;
    target.open("TARGET1");

    auto acc1  = dev.getOneDRegisterAccessor<int32_t>("APP.0.MY_AREA1");
    auto acc1t = target.getOneDRegisterAccessor<int32_t>("APP.0.THE_AREA", 6, 2, {AccessMode::raw});

    acc1t = { 10 * 65536, 20 * 65536, 30 * 65536, 40 * 65536, 50 * 65536, 60 * 65536 };
    acc1t.write();
    acc1.read();
    BOOST_CHECK( (std::vector<int32_t>)acc1 == std::vector<int32_t>({ 10, 20, 30, 40, 50, 60 }) );


    acc1t = { 15 * 65536, 25 * 65536, 35 * 65536, 45 * 65536, 55 * 65536, 65 * 65536 };
    acc1t.write();
    acc1.read();
    BOOST_CHECK( (std::vector<int32_t>)acc1 == std::vector<int32_t>({ 15, 25, 35, 45, 55, 65 }) );

    dev.close();

}

/*********************************************************************************************************************/

BOOST_AUTO_TEST_CASE( test3regsScalarRead ) {

    setDMapFilePath("subdeviceTest.dmap");

    Device dev;
    dev.open("SUBDEV2");
    Device target;
    target.open("TARGET1");

    auto acc1  = dev.getScalarRegisterAccessor<double>("APP.0.MY_REGISTER1");
    auto acc2  = dev.getScalarRegisterAccessor<double>("APP.0.MY_REGISTER2");
    auto accA = target.getScalarRegisterAccessor<int32_t>("APP.1.ADDRESS");
    auto accD = target.getScalarRegisterAccessor<int32_t>("APP.1.DATA");
    auto accS = target.getScalarRegisterAccessor<int32_t>("APP.1.STATUS");
    std::atomic<bool> done;
    std::thread t;

    accS = 1;
    accS.write();
    done = false;
    t = std::thread([&]{
      acc2.read();
      done = true;
    });
    usleep(10000);
    BOOST_CHECK(done == false);
    accD = 123;
    accD.write();
    accS = 0;
    accS.write();
    CHECK_TIMEOUT( accA.read(); , (accA == 4) , 5000 );
    t.join();
    BOOST_CHECK_CLOSE(static_cast<double>(acc2), 123./4., 0.001);

    accA = 0;
    accA.write();

    accS = 1;
    accS.write();
    done = false;
    t = std::thread([&]{
      acc2.read();
      done = true;
    });
    usleep(10000);
    BOOST_CHECK(done == false);
    accD = 4000;
    accD.write();
    accS = 0;
    accS.write();
    CHECK_TIMEOUT( accA.read(); , (accA == 4) , 5000 );
    t.join();
    BOOST_CHECK_CLOSE(static_cast<double>(acc2), 4000./4., 0.001);

    accS = 1;
    accS.write();
    done = false;
    t = std::thread([&]{
      acc1.read();
      done = true;
    });
    usleep(10000);
    BOOST_CHECK(done == false);
    accD = 5432;
    accD.write();
    accS = 0;
    accS.write();
    CHECK_TIMEOUT( accA.read(); , (accA == 0) , 5000 );
    t.join();
    BOOST_CHECK_CLOSE(static_cast<double>(acc1), 5432., 0.001);

    accA = 222;
    accA.write();
    accD = 1897;
    accD.write();
    acc1.read();
    accA.read();
    BOOST_CHECK(accA == 0);
    BOOST_CHECK_CLOSE(static_cast<double>(acc1), 1897., 0.001);

    dev.close();

}

/*********************************************************************************************************************/

BOOST_AUTO_TEST_CASE( test3regsScalarWrite ) {

    setDMapFilePath("subdeviceTest.dmap");

    Device dev;
    dev.open("SUBDEV2");
    Device target;
    target.open("TARGET1");

    auto acc1  = dev.getScalarRegisterAccessor<double>("APP.0.MY_REGISTER1");
    auto acc2  = dev.getScalarRegisterAccessor<double>("APP.0.MY_REGISTER2");
    auto accA = target.getScalarRegisterAccessor<int32_t>("APP.1.ADDRESS");
    auto accD = target.getScalarRegisterAccessor<int32_t>("APP.1.DATA");
    auto accS = target.getScalarRegisterAccessor<int32_t>("APP.1.STATUS");
    std::atomic<bool> done;
    std::thread t;

    accS = 1;
    accS.write();
    done = false;
    t = std::thread([&]{
      acc2 = 42;
      acc2.write();
      done = true;
    });
    usleep(10000);
    BOOST_CHECK(done == false);
    accS = 0;
    accS.write();
    CHECK_TIMEOUT( accA.read(); , (accA == 4) , 5000 );
    accD.read();
    t.join();
    BOOST_CHECK_EQUAL(static_cast<int32_t>(accD), 42*4);

    acc1 = 120;
    acc1.write();
    accA.read();
    BOOST_CHECK_EQUAL(static_cast<int32_t>(accA), 0);
    accD.read();
    BOOST_CHECK_EQUAL(static_cast<int32_t>(accD), 120);

    dev.close();

}

/*********************************************************************************************************************/

BOOST_AUTO_TEST_CASE( test3regsArrayRead ) {

    setDMapFilePath("subdeviceTest.dmap");

    Device dev;
    dev.open("SUBDEV2");
    Device target;
    target.open("TARGET1");

    auto accArea  = dev.getOneDRegisterAccessor<double>("APP.0.MY_AREA2");
    auto accA = target.getScalarRegisterAccessor<int32_t>("APP.1.ADDRESS");
    auto accD = target.getScalarRegisterAccessor<int32_t>("APP.1.DATA");
    auto accS = target.getScalarRegisterAccessor<int32_t>("APP.1.STATUS");
    std::atomic<bool> done;
    std::thread t;

    accS = 1;
    accS.write();
    done = false;
    t = std::thread([&]{
      accArea.read();
      done = true;
    });
    usleep(10000);
    BOOST_CHECK(done == false);
    accD = 123;
    accD.write();
    accS = 0;
    accS.write();
    CHECK_TIMEOUT( accA.read(); , (accA == 36) , 5000 );
    t.join();
    BOOST_CHECK_CLOSE(accArea[0], 123., 0.001);
    BOOST_CHECK_CLOSE(accArea[1], 123., 0.001);

    /// @todo Make a proper test with a custom backend, to make sure all elements of the array are properly read

    dev.close();

}

/*********************************************************************************************************************/

BOOST_AUTO_TEST_CASE( test3regsArrayWrite ) {

    setDMapFilePath("subdeviceTest.dmap");

    Device dev;
    dev.open("SUBDEV2");
    Device target;
    target.open("TARGET1");

    auto accArea  = dev.getOneDRegisterAccessor<double>("APP.0.MY_AREA2");
    auto accA = target.getScalarRegisterAccessor<int32_t>("APP.1.ADDRESS");
    auto accD = target.getScalarRegisterAccessor<int32_t>("APP.1.DATA");
    auto accS = target.getScalarRegisterAccessor<int32_t>("APP.1.STATUS");
    std::atomic<bool> done;
    std::thread t;

    accS = 1;
    accS.write();
    done = false;
    t = std::thread([&]{
      accArea[0] = 123;
      accArea[1] = 456;
      accArea.write();
      done = true;
    });
    usleep(10000);
    BOOST_CHECK(done == false);
    accS = 0;
    accS.write();
    CHECK_TIMEOUT( accA.read(); , (accA == 36) , 5000 );
    t.join();
    accD.read();
    BOOST_CHECK_EQUAL(static_cast<int32_t>(accD), 456);

    /// @todo Make a proper test with a custom backend, to make sure all elements of the array are properly written

    dev.close();

}

/*********************************************************************************************************************/

BOOST_AUTO_TEST_SUITE_END()
