#include "httplib.h"
#include <fstream>
#include <sstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;

// Utility: Read file content
string read_file(const string& path) {
    ifstream f(path, ios::binary);
    stringstream s;
    s << f.rdbuf();
    return s.str();
}

// Load JSON file
json load_json(const string& path) {
    ifstream f(path);
    json j;
    f >> j;
    return j;
}

// Save JSON file
void save_json(const string& path, const json& j) {
    ofstream f(path);
    f << j.dump(4);
}

int main() {
    httplib::Server svr;

    // Serve static HTML pages
    svr.Get("/", [&](auto&, auto& res){ res.set_content(read_file("pages/index.html"), "text/html"); });
    svr.Get("/quiz", [&](auto&, auto& res){ res.set_content(read_file("pages/quiz.html"), "text/html"); });
    svr.Get("/results", [&](auto&, auto& res){ res.set_content(read_file("pages/results.html"), "text/html"); });

    // Serve CSS & JS
    svr.Get("/styles/(.*)", [](auto& req, auto& res){
        res.set_content(read_file("styles/" + req.matches[1].str()), "text/css");
    });
    svr.Get("/javaScript/(.*)", [](auto& req, auto& res){
        res.set_content(read_file("javaScript/" + req.matches[1].str()), "application/javascript");
    });
    svr.Get("/images/(.*)", [](auto& req, auto& res){
        res.set_content(read_file("images/" + req.matches[1].str()), "image/jpeg");
    });

    // ------------------ USER SIGN-UP ------------------
    svr.Post("/signup", [&](auto& req, auto& res){
        auto body = req.body;
        json data = json::parse(body);
        string username = data["username"];
        string password = data["password"];
        string email = data["email"];

        json users = load_json("data/users.json");

        // Check if username exists
        for (auto& u : users) {
            if (u["username"] == username) {
                res.set_content("{\"status\":\"error\",\"message\":\"Username exists\"}", "application/json");
                return;
            }
        }

        int new_id = users.size() + 1;
        json new_user = {
            {"id", new_id},
            {"username", username},
            {"email", email},
            {"password", password}, // plain for now
            {"quiz", json::object()}
        };
        users.push_back(new_user);
        save_json("data/users.json", users);

        res.set_content("{\"status\":\"ok\"}", "application/json");
    });

    // ------------------ USER SIGN-IN ------------------
    svr.Post("/signin", [&](auto& req, auto& res){
        auto body = req.body;
        json data = json::parse(body);
        string username = data["username"];
        string password = data["password"];

        json users = load_json("data/users.json");

        for (auto& u : users) {
            if (u["username"] == username && u["password"] == password) {
                res.set_content("{\"status\":\"ok\"}", "application/json");
                return;
            }
        }
        res.set_content("{\"status\":\"error\",\"message\":\"Invalid credentials\"}", "application/json");
    });

    // ------------------ QUIZ SUBMISSION ------------------
    svr.Post("/quiz_submit", [&](auto& req, auto& res){
        auto body = req.body;
        json quiz_data = json::parse(body);
        string username = quiz_data["username"];

        json users = load_json("data/users.json");

        for (auto& u : users) {
            if (u["username"] == username) {
                u["quiz"] = quiz_data["quiz"];
                break;
            }
        }

        save_json("data/users.json", users);
        res.set_content("{\"status\":\"ok\"}", "application/json");
    });

    // ------------------ FILTERED ACTIVITIES ------------------
    svr.Post("/filtered_activities", [&](auto& req, auto& res){
        auto body = req.body;
        json user_data = json::parse(body);
        auto user_quiz = user_data["quiz"];

        json activities = load_json("data/activities.json");
        json filtered = json::array();

        for (auto& act : activities) {
            // Filter by city
            if (!user_quiz["cities"].empty() &&
                find(user_quiz["cities"].begin(), user_quiz["cities"].end(), act["city"]) == user_quiz["cities"].end())
                continue;

            // Filter by budget
            // Assuming price is "from X$" -> extract number
            string price_str = act["price"];
            size_t pos = price_str.find('$');
            int price_val = (pos != string::npos) ? stoi(price_str.substr(5, pos - 5)) : 0;
            if (user_quiz.contains("budget") && price_val > user_quiz["budget"])
                continue;

            // Filter by category
            bool category_match = false;
            for (auto& c : act["category"]) {
                if (find(user_quiz["categories"].begin(), user_quiz["categories"].end(), c) != user_quiz["categories"].end()) {
                    category_match = true;
                    break;
                }
            }
            if (!category_match) continue;

            filtered.push_back(act);
        }

        res.set_content(filtered.dump(), "application/json");
    });

    svr.listen("0.0.0.0", 8080);
}