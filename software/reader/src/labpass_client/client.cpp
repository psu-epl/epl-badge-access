#include "client.h"
#include "ArduinoJson.h"
#include <WiFi.h>
#include <HTTPClient.h>

#define AUTHORIZE_PATH "/api/v1.0/auth"
#define ENROLL_PATH "/api/v1.0/enroll"
#define PING_PATH "/api/v1.0/ping"

LabpassClient::LabpassClient(String *baseURL, String *token) : baseURL_(*baseURL),
                                                               token_(*token)
{

}

void LabpassClient::authorize(AuthReqResp * authReqResp)
{
    authReqResp->isError = true;
    authReqResp->isAuthorized = false;
    authReqResp->isLoggedOut = false;
    authReqResp->inService = false;

    HTTPClient http;

    String url = baseURL_ + AUTHORIZE_PATH;
    int err = http.begin(url);
    if (err != 1)
    {
        http.end();
        return;
    }

    DynamicJsonDocument doc(512);
    doc["badge_id"] = authReqResp->badgeId;
    doc["station_id"] = authReqResp->stationId;

    String bodyString;
    serializeJson(doc, bodyString);

    http.addHeader("Content-Type", "application/json");
    http.setAuthorizationType("Bearer");
    http.setAuthorization(token_.c_str());
    http.setReuse(false);

    log_i("sending: %s", bodyString.c_str());

    int httpResponseCode = http.POST(bodyString);

    log_i("response code: %d", httpResponseCode);

    if (httpResponseCode == 200)
    {
        StaticJsonDocument<400> doc;
        DeserializationError error = deserializeJson(doc, http.getString());
        if (error) {
            log_e("deserialize error");
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
    else if (httpResponseCode == 401)
    {
        authReqResp->isError = false;
        authReqResp->isAuthorized = false;
    }

    http.end();
}

void LabpassClient::enroll(EnrollReqResp * enrollReqResp)
{
    enrollReqResp->isError = true;
    enrollReqResp->isEnrolled = false;
    enrollReqResp->inService = false;

    DynamicJsonDocument doc(512);
    doc["user_badge_id"] = enrollReqResp->userBadgeId;
    doc["manager_badge_id"] = enrollReqResp->managerBadgeId;
    doc["station_id"] = enrollReqResp->stationId;

    String bodyString;
    serializeJson(doc, bodyString);

    log_i("sending: %s", bodyString.c_str());

    HTTPClient http;

    String url = baseURL_ + ENROLL_PATH;
    if (!http.begin(url))
    {
        http.end();
        return;
    }

    http.addHeader("Content-Type", "application/json");
    http.setAuthorizationType("Bearer");
    http.setAuthorization(token_.c_str());
    http.setReuse(false);

    int httpResponseCode = http.POST(bodyString);

    if (httpResponseCode >= 201)
    {
        DynamicJsonDocument doc(512);
        DeserializationError error = deserializeJson(doc, http.getString());
        if (error)
        {
            http.end();
            return;
        }

        enrollReqResp->isError = false;
        enrollReqResp->isEnrolled = doc["is_enrolled"].as<String>();
        enrollReqResp->inService = doc["in_service"].as<String>();
        enrollReqResp->userBadgeId = doc["user_badge_id"].as<String>();
        enrollReqResp->managerBadgeId = doc["manager_badge_id"].as<bool>();
        enrollReqResp->stationId = doc["station_id"].as<String>();
    }
    http.end();
}

void LabpassClient::ping(PingReqResp * pingReqResp)
{
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
    //http.addHeader("Authorization", token_);
    http.setAuthorizationType("Bearer");
    http.setAuthorization(token_.c_str());
    http.setReuse(false);

    int httpResponseCode = http.POST(bodyString);
    if (httpResponseCode == 200)
    {
        DynamicJsonDocument doc(512);
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
