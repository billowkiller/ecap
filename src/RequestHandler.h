#include <string>
#include "parser.h"

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
        void _init_c_info();
        void writeLog_kw(std::string);
    private:
        struct connection_info c_info;
        RequestInfo &request_info;
        
        
        
};