#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <string>
#include <map>
#include <nlohmann/json.hpp>
#include <nlohmann/httplib.h>

using namespace std;
using namespace httplib;
using json = nlohmann::json;

//Constants
extern const string USER_DB_FILE;
extern const string ACTIVITIES_FILE;
extern map<string, string> active_sessions;

//Utilities
string read_file(const string& path);
json load_json(const string& path);
void save_json(const string& path, const json& j);
void replace_all(string& str, const string& from, const string& to);

//Auth
string generate_session_id();
string get_logged_in_user(const Request& req);

//Handlers
void handle_index(const Request& req, Response& res);
void handle_quiz(const Request& req, Response& res);
void handle_results(const Request& req, Response& res);
void handle_signup(const Request& req, Response& res);
void handle_signin(const Request& req, Response& res);
void handle_logout(const Request& req, Response& res);
void handle_quiz_submit(const Request& req, Response& res);

#endif