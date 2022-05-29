#pragma once
#include <Arduino.h>

#include <esp_event.h>
#include "labpass_event/labpass_event.h"

struct AuthReqResp
{
    bool isError;
    bool isAuthorized;
    bool isLoggedOut;
    bool isManager;
    bool inService;
    String badgeId;
    String managerId;
    String stationId;
};

struct EnrollReqResp
{
    bool isError;
    bool isEnrolled;
    bool inService;
    String managerId;
    String badgeId;
    String stationId;
};

struct PingReqResp
{
    bool isError;
    bool inService;
    String stationId;
};

class LabpassClient
{
public:
    LabpassClient(String &baseURL, String &stationId, String &token);

    void authorize(AuthReqResp * authReqResp);
    void enroll(EnrollReqResp * enrollReqResp);
    void ping(PingReqResp * pingReqResp);

private:
    String &baseURL_;
    String &token_;
};
