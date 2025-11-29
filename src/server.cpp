#include "nlohmann/httplib.h"
#include "functions.h"

using namespace httplib;

int main() {
    httplib::Server svr;

    // connecting HTML files
    svr.Get("/", handle_index);
    svr.Get("/about", [&](auto&, auto& res){res.set_content(read_file("pages/about.html"), "text/html"); });
    svr.Get("/tashkent", [&](auto&, auto& res){res.set_content(read_file("pages/tashkent.html"), "text/html"); });
    svr.Get("/samarkand", [&](auto&, auto& res){res.set_content(read_file("pages/samarkand.html"), "text/html"); });
    svr.Get("/bukhara", [&](auto&, auto& res){res.set_content(read_file("pages/bukhara.html"), "text/html"); });
    svr.Get("/khiva", [&](auto&, auto& res){res.set_content(read_file("pages/khiva.html"), "text/html"); });
    svr.Get("/signup", [&](auto&, auto& res){ res.set_content(read_file("pages/signup.html"), "text/html"); });
    svr.Get("/signin", [&](auto&, auto& res){ res.set_content(read_file("pages/signin.html"), "text/html"); });
    svr.Get("/quiz", handle_quiz);
    svr.Get("/results", handle_results);

    // Serve CSS & JS
    svr.Get("/styles/(.*)", [](auto& req, auto& res){res.set_content(read_file("styles/" + req.matches[1].str()), "text/css"); });
    svr.Get("/javaScript/(.*)", [](auto& req, auto& res){res.set_content(read_file("javaScript/" + req.matches[1].str()), "application/javascript");});
    svr.Get("/images/(.*)", [](auto& req, auto& res){res.set_content(read_file("images/" + req.matches[1].str()), "image/jpeg");});

    svr.Get("/data/(.*)", [&](auto& req, auto& res){
        string filepath = "data/" + req.matches[1].str();
        string content = read_file(filepath);
        res.set_content(content, "application/json");     
    });

    svr.Post("/signup", handle_signup);
    svr.Post("/signin", handle_signin);
    svr.Get("/logout", handle_logout);
    svr.Post("/quiz_submit", handle_quiz_submit);

    // Get port from environment variable (for Render deployment)
    const char* port_env = getenv("PORT");
    int port = port_env ? atoi(port_env) : 5500;

    svr.listen("0.0.0.0", 5500);
}