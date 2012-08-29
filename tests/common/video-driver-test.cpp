#if HAVE_CONFIG_H
#include <config.h>
#endif
#include <stdexcept>
#include <fstream>
#include <xorg/gtest/xorg-gtest.h>

#include "video-driver-test.h"

void VideoDriverTest::StartServer() {
    server.Start();
    server.WaitForConnections();
    xorg::testing::Test::SetDisplayString(server.GetDisplayString());

    ASSERT_NO_FATAL_FAILURE(xorg::testing::Test::SetUp());
}

void VideoDriverTest::SetUpConfigAndLog(const std::string& param) {
    std::stringstream s;
    s << "/tmp/Xorg-" << param << ".log";
    server.SetOption("-logfile", s.str());

    s.str(std::string());
    s << "/tmp/" << param << ".conf";
    config.SetPath(s.str());

    config.AddDefaultScreenWithDriver(param, param);
    config.WriteConfig();
    server.SetOption("-config", config.GetPath());
}

void VideoDriverTest::SetUpEventListener() {
    failed = false;

    testing::TestEventListeners &listeners = ::testing::UnitTest::GetInstance()->listeners();
    listeners.Append(this);
}

void VideoDriverTest::SetUp() {
    SetUp("");
}

void VideoDriverTest::SetUp(const std::string &param) {
    SetUpEventListener();
    SetUpConfigAndLog(param);
    StartServer();
}

void VideoDriverTest::TearDown() {
    if (server.Pid() != -1)
        if (!server.Terminate(3000))
            server.Kill(3000);

    if (!Failed()) {
        config.RemoveConfig();
        server.RemoveLogFile();
    }

    testing::TestEventListeners &listeners = ::testing::UnitTest::GetInstance()->listeners();
    listeners.Release(this);
}

void VideoDriverTest::OnTestPartResult(const ::testing::TestPartResult &test_part_result) {
    failed = test_part_result.failed();
}

bool VideoDriverTest::Failed() {
    return failed;
}