#include <string>

class RequestHandler
{
    public:
       typedef  struct{
            std::string url;
            std::string referrer;
            std::string cookie;
            std::string content;
        } RequestInfo;

        RequestHandler(RequestInfo info):request_info(info){}; 
        bool checkValidation();

    private:

        RequestInfo &request_info;
};
