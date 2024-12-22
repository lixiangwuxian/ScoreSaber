#include "shared/Models/Session.hpp"

Session Session::session;

Session::Session() {
    playerKey = "";
    serverKey = "";
    loginStatus = LoginStatus::Error;
}

Session::Session(rapidjson::Value const &&value) {
    session.playerKey = value["a"].GetString();
    session.serverKey = value["e"].GetString();
    session.loginStatus = LoginStatus::Success;
}

Session Session::GetSession() {
    return session;
}