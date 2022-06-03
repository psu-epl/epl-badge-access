#include "client.h"
#include "ArduinoJson.h"
#include <WiFi.h>
#include <HTTPClient.h>

#define AUTHORIZE_PATH "/api/v1.0/auth"
#define ENROLL_PATH "/api/v1.0/enroll"
#define PING_PATH "/api/v1.0/ping"

LabpassClient::LabpassClient(String &baseURL, String &stationId, String &token) : baseURL_(baseURL),
                                                                                  token_(token)
{

}

void LabpassClient::authorize(AuthReqResp * authReqResp)
{
    authReqResp->isError = true;
    authReqResp->isAuthorized = false;
    authReqResp->isLoggedOut = false;
    authReqResp->inService = false;

    log_i("authorize backside, tag: %s", authReqResp->badgeId);

    HTTPClient http;

    String url = baseURL_ + AUTHORIZE_PATH;
    int err = http.begin(url);
    if (err != 1)
    {
        http.end();
        return;
    }

    StaticJsonDocument<256> body;
    body["badge_id"] = authReqResp->badgeId;
    body["station_id"] = authReqResp->stationId;

    String bodyString;
    serializeJson(body, bodyString);

    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", token_);

    log_i("sending: %s", bodyString.c_str());

    int httpResponseCode = http.POST(bodyString);

    log_i("response code: %d", httpResponseCode);

    if (httpResponseCode >= 200 && httpResponseCode < 300)
    {
        StaticJsonDocument<200> doc;
        DeserializationError error = deserializeJson(doc, http.getString());
        if (error) {
            http.end();
            return;
        }

        authReqResp->isError = false;
        authReqResp->isAuthorized = doc["is_authorized"].as<bool>();
        authReqResp->isLoggedOut = doc["is_logged_out"].as<bool>();
        authReqResp->isManager = doc["is_manager"].as<bool>();
        authReqResp->inService = doc["in_service"].as<bool>();

        authReqResp->badgeId = doc["badge_id"].as<String>();
        authReqResp->managerId = doc["maanger_id"].as<String>();
        authReqResp->stationId = doc["station_id"].as<String>();

    }

    http.end();
}

void LabpassClient::enroll(EnrollReqResp * enrollReqResp)
{
    enrollReqResp->isError = true;
    enrollReqResp->isEnrolled = false;
    enrollReqResp->inService = false;

    StaticJsonDocument<256> body;
    body["badge_id"] = enrollReqResp->badgeId;
    body["manager_badge_id"] = enrollReqResp->managerId;
    body["station_id"] = enrollReqResp->stationId;

    String bodyString;
    serializeJson(body, bodyString);

    HTTPClient http;

    String url = baseURL_ + ENROLL_PATH;
    if (!http.begin(url))
    {
        http.end();
        return;
    }

    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", token_);

    int httpResponseCode = http.POST(bodyString);

    if (httpResponseCode >= 200 && httpResponseCode < 300)
    {
        StaticJsonDocument<200> doc;
        DeserializationError error = deserializeJson(doc, http.getString());
        if (error)
        {
            http.end();
            return;
        }

        enrollReqResp->isError = false;
        enrollReqResp->isEnrolled = doc["is_enrolled"].as<String>();
        enrollReqResp->inService = doc["in_service"].as<String>();
        enrollReqResp->badgeId = doc["badge_id"].as<String>();
        enrollReqResp->managerId = doc["manager_id"].as<bool>();
        enrollReqResp->stationId = doc["station_id"].as<String>();
    }

    http.end();
}

void LabpassClient::ping(PingReqResp * pingReqResp)
{
    log_i("client ping request backside");

    pingReqResp->isError = true;
    pingReqResp->inService = false;
    String url = baseURL_ + PING_PATH;

    StaticJsonDocument<256> body;
    body["station_id"] = pingReqResp->stationId;
    String bodyString;
    serializeJson(body, bodyString);

    HTTPClient http;

    int error = http.begin(url);
    if (error != 1)
    {
        http.end();
        return;
    }

    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", token_);

    int httpResponseCode = http.POST(bodyString);

    if (httpResponseCode >= 200 && httpResponseCode < 300)
    {
        StaticJsonDocument<200> doc;
        DeserializationError error = deserializeJson(doc, http.getString());
        if (error)
        {
            http.end();
            return;
        }

        pingReqResp->isError = false;
        pingReqResp->inService = doc["in_service"].as<bool>();
        pingReqResp->stationId = doc["station_id"].as<String>();
        http.end();
        return;
    }

    http.end();

}
