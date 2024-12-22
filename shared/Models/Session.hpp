
#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"

#include <string>
using namespace std;

enum LoginStatus
{
    Error,
    Success,
};

class Session {
    std::string playerKey;
    std::string serverKey;
    LoginStatus loginStatus;
    static Session session;

    public:
    Session(rapidjson::Value const &&session);
    Session();
    static Session GetSession();
};