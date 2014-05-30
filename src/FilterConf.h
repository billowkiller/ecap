#include <string>
using namespace std;
class FilterConf{
    public:
        int type; // 1，r, relpace; 2, d, delete;
        string s_str;
        string d_str;

        FilterConf(){}
        FilterConf(int t, string s):type(t),s_str(s){}
        FilterConf(int t, string s,string d):type(t),s_str(s),d_str(d){}
};
