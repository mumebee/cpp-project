#include "httplib.h"
#include <fstream>
#include <sstream>
using namespace std;

string read_file(const string& path) {
    ifstream f(path, ios::binary);
    stringstream s;
    s << f.rdbuf();
    return s.str();
}
int main() {
    httplib::Server svr;
    svr.Get("/", [](auto&, auto& res) {
        res.set_content(read_file("pages/index.html"), "text/html");
    });
    svr.Get("/about", [](auto&, auto& res) {
        res.set_content(read_file("pages/about.html"), "text/html");
    });
    svr.Get("/destinations", [](auto&, auto& res) {
        res.set_content(read_file("pages/destinations.html"), "text/html");
    });
    svr.Get("/styles/(.*)", [](auto& req, auto& res) {
        res.set_content(read_file("styles/" + req.matches[1].str()), "text/css");
    });
    svr.Get("/images/(.*)", [](auto& req, auto& res) {
        res.set_content(read_file("images/" + req.matches[1].str()), "image/jpeg");
    });
    svr.Get(R"(/javaScript/(.*))", [](auto& req, auto& res) {
        res.set_content(read_file("javaScript/" + req.matches[1].str()), "application/javascript");
    });
    svr.listen("0.0.0.0", 8080);
}