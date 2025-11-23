#include "functions.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <random>

const string USER_DB_FILE = "data/users.json";
const string ACTIVITIES_FILE = "data/activities.json";
map<string, string> active_sessions;

// ============= UTILITIES =============
string read_file(const string& path){
    ifstream f(path, ios::binary);
    if(!f.is_open()) return "";
    stringstream s;
    s << f.rdbuf();
    return s.str();
}
json load_json(const string& path){
    ifstream f(path);
    json j;
    if(f.is_open() && f.peek() !=EOF){
        try{ f >> j; }
        catch(const std::exception& e){
            cerr << "Error loading json: "<< e.what() << '\n';
            return json::array();
        }
    }
    return j.is_array() ? j : json::array();
}
void save_json(const string& path, const json& j){
    ofstream f(path);
    if (f.is_open()) f << j.dump(4);
}
void replace_all(string& str, const string& from, const string& to) {
    if (from.empty()) return;
    size_t pos = 0;  
    while ((pos = str.find(from, pos)) != string::npos) {
        str.replace(pos, from.length(), to);
        pos += to.length();
    }
}

// ============= AUTH =============
string generate_session_id(){
    static const char chars[] = "0123456789ABCDEFJHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0,61);

    string id;
    for(int i=0; i<32; i++) id+= chars[dis(gen)];
    return id;
}
string get_logged_in_user(const Request& req){
    if(!req.has_header("Cookie")) return "";

    string cookies = req.get_header_value("Cookie");
    size_t pos = cookies.find("session_id=");
    if (pos == string::npos) return "";

    // Start position: After "session_id=" (11 characters long)
    size_t start = pos + 11;

    // Find the next semicolon starting from 'start'
    size_t end = cookies.find(";", start);
    
    // Extract the session_id
    string session_id = (end == string::npos) ? 
        cookies.substr(start) : 
        cookies.substr(start, end - start);
    if (session_id.length() != 32) return ""; 

    return (active_sessions.count(session_id)) ? active_sessions[session_id] : "";
}
json find_user(const string& username){
    json users = load_json(USER_DB_FILE);
    for(const auto& u : users){
        if(u.value("username", "") == username) return u;
    }
    return json();
}
bool has_quiz_completed(const json& user){
    if(!user.contains("quiz") || user["quiz"].empty()) return false;
    json quiz = user["quiz"];
    return quiz.contains("preferred_name") || quiz.contains("cities") ||quiz.contains("activities");
}
string build_activity_card(const json& activity){
    string name = activity.value("name", "");
    int price = activity.value("price", 0);
    double duration = activity.value("duration_hours", 0.0);

    string style = "Activity";
    if(activity.contains("category") && !activity["category"].empty()){
        style = activity["category"][0].get<string>();
        if(!style.empty()) style[0] = toupper(style[0]);
    }
    stringstream duration_str;
    duration_str << fixed << setprecision(1) << duration;
    return "<li><div class='activity-info'><strong>NAME: " + name + "</strong>" +
    "<div class='activity-details'>" +
    "<div>STYLE: " + style + "</div>" +
    "<div>HOURS: " + duration_str.str() + "</div>" +
    "<div>PRICE: $" + to_string(price) + "</div>" +
    "</div></div></li>";
}
bool matches_category(const json& activity, const json& selected_categories){
    if(!activity.contains("category")) return false;
    for(const auto& cat : activity["category"]){
        for(const auto& sel : selected_categories){
            if(cat.get<string>() == sel.get<string>()) return true;
        }
    }
    return false;
}

// ============= HANDLERS =============
void handle_index(const Request& req, Response& res){
    string html = read_file("pages/index.html");
    string username = get_logged_in_user(req);
    string auth_section = username.empty() ? "" :
    "<button onclick=\"window.location.href='/results?username=" + username + "'\">MY TRIP</button>"
    "<button onclick=\"window.location.href='/logout'\">LOG OUT</button>";

    replace_all(html, "{{AUTH_SECTION}}", auth_section);
    res.set_content(html, "text/html");
}
void handle_quiz(const Request& req, Response& res){
    string username = req.get_param_value("username");
    string html = read_file("pages/quiz.html");
    replace_all(html, "{{USERNAME}}", username);
    res.set_content(html, "text/html");
}
void handle_results(const Request& req, Response& res){
    string username = req.get_param_value("username");
    cerr << "[RESULTS] username param: '" << username << "'" << endl;

    json user = find_user(username);
    cerr << "[RESULTS] user found: " << !user.is_null() << endl;

    if(user.is_null() || !user.contains("quiz")){
        cerr << "[RESULTS] ERROR: User not found or no quiz data!" << endl;
        res.set_content("User not found", "text/plain");
        return;
    }
    json quiz = user["quiz"];
    cerr << "[RESULTS] quiz data: " << quiz.dump() << endl;

    string preferred_name = quiz.value("preferred_name", "User");
    string budget = quiz.value("budget", "0");
    json cities = quiz.value("cities", json::array());
    json activities_filter = quiz.value("activities", json::array());
    cerr << "[RESULTS] preferred_name: " << preferred_name << endl;
    cerr << "[RESULTS] budget: " << budget << endl;
    cerr << "[RESULTS] cities: " << cities.dump() << endl;
    cerr << "[RESULTS] activities: " << activities_filter.dump() << endl;

    json all_activities = load_json(ACTIVITIES_FILE);
    map<string, string> city_results;
    int total_cost = 0;
    double total_hours = 0.0;
    int user_budget = budget.empty() ? 999999 : stoi(budget);
    
    for(const auto& activity : all_activities){
        string city = activity.value("city", "");
        string activity_name = activity.value("name", "");
        int price = activity.value("price", 0);
        double hours = activity.value("duration_hours", 0.0);

        // Filter by city
        bool city_match = false;
        for(const auto& c : cities){
            if(c.get<string>() == city){
                city_match = true;
                break;
            }
        }
        if(!city_match) continue;
        // Filter by category and budget
        bool cat_match = matches_category(activity, activities_filter);
        bool budget_ok = (price <= user_budget);
        bool total_budget_ok = (total_cost + price <= user_budget);  // NEW: Check if adding this would exceed budget
        
        if(cat_match && budget_ok && total_budget_ok){  // NEW: Added total_budget_ok
            cerr << "[ADDING] " << activity_name << " - $" << price << endl;
            total_cost += price;
            total_hours += hours;
            city_results[city] += build_activity_card(activity);
        } else if(cat_match && budget_ok && !total_budget_ok) {
            cerr << "[SKIPPED-TOTAL] " << activity_name << " - Would exceed budget" << endl;
        }
    }
    cerr << "[RESULTS] Final total_cost: " << total_cost << ", user_budget: " << user_budget << endl;
    
    string content = "";
    if(!city_results.empty()){
        cerr << "[RESULTS] city_results size: " << city_results.size() << endl;
        for(const auto& [city, cards] : city_results){
            cerr << "[RESULTS] City: " << city << ", cards length: " << cards.length() << endl;
            content += "<div class='city-card'><h3>Cities to travel: " + city + "</h3><ul>" + cards + "</ul></div>";
        }
    } else{
        content = "<div>No activities found matching your preferences</div>";
    }

    string html = read_file("pages/results.html");
    replace_all(html, "{{PREFERRED_NAME}}", preferred_name);
    replace_all(html, "{{USER_BUDGET}}", budget);
    replace_all(html, "{{TOTAL_COST}}", to_string(total_cost));

    stringstream hours_str;
    hours_str << fixed << setprecision(1) << total_hours;
    replace_all(html, "{{TOTAL_HOURS}}", hours_str.str());
    replace_all(html, "{{RESULTS_CONTENT}}", content);
    cerr << "[RESULTS] content length: " << content.length() << endl;
    cerr << "[RESULTS] html length: " << html.length() << endl;

    res.set_content(html, "text/html");
}

void handle_signup(const Request& req, Response& res){
    string username = req.get_param_value("username");
    string password = req.get_param_value("password");
    string email = req.get_param_value("email");

    json users = load_json(USER_DB_FILE);
    json new_user = {
        {"id", users.size()+1},
        {"username", username},
        {"email", email},
        {"password", password},
        {"quiz", json::object()}
    };
    users.push_back(new_user);
    save_json(USER_DB_FILE, users);
    string session_id = generate_session_id();
    active_sessions[session_id] = username;
    res.status = 303;
    res.set_header("Set-Cookie", "session_id=" + session_id + "; Path=/; Max-Age=86400");
    res.set_header("Location", "/quiz?username=" + username);
}
void handle_signin(const Request& req, Response& res){
    string username = req.get_param_value("username");
    string password = req.get_param_value("password");
    json users = load_json(USER_DB_FILE);
    json found_user;
    for(const auto& u : users){
        if(u.value("username", "") == username && u.value("password", "") == password){
            found_user = u;
            break;
        }
    }
    if(found_user.is_null()){
        res.status = 401;
        res.set_content("Invalid username or password", "text/plain");
        return;
    }
    string session_id = generate_session_id();
    active_sessions[session_id] = username;
    res.status = 303;
    res.set_header("Set-Cookie", "session_id=" + session_id + "; Path=/; Max-Age=86400");
    string redirect = has_quiz_completed(found_user) ?
        "/results?username=" + username : "/quiz?username=" + username;
    res.set_header("Location", redirect);
}
void handle_logout(const Request& req, Response& res){
    if(req.has_header("Cookie")){
        string cookies = req.get_header_value("Cookie");
        size_t pos = cookies.find("session_id=");
        if(pos != string::npos)
            active_sessions.erase(cookies.substr(pos + 11, 32));
    }
    res.status = 303;
    res.set_header("Set-Cookie", "session_id=; Path=/; Max-Age=0");
    res.set_header("Location", "/");
}
void handle_quiz_submit(const Request& req, Response& res){
    string username = req.get_param_value("username");
    json quiz_data = {
        {"preferred_name", req.get_param_value("preferred_name")},
        {"age", req.get_param_value("age")},
        {"budget", req.get_param_value("budget")},
        {"cities", json::array()},
        {"activities", json::array()}
    };
    //Collect cities
    for(const auto& param : req.params){
        if(param.first == "cities[]")
            quiz_data["cities"].push_back(param.second);
        if(param.first == "categories[]")
            quiz_data["activities"].push_back(param.second);
    }
    json users = load_json(USER_DB_FILE);
    bool updated = false;
    for(auto& u : users) {
        if(u.value("username", "") == username){
            u["quiz"] = quiz_data;
            updated = true;
            break;
        }
    }
    if(!updated){
        res.status = 404;
        res.set_content("User not found", "text/plain");
        return;
    }
    save_json(USER_DB_FILE, users);
    res.status = 303;
    res.set_header("Location", "/results?username=" + username);
}