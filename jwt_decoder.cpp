// jwt_decoder.cpp
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <nlohmann/json.hpp>
#include <cctype>

using json = nlohmann::json;
using namespace std;

const string RESET = "\033[0m";
const string KEY = "\033[94m";
const string STR = "\033[92m";
const string NUM = "\033[93m";
const string BOOL = "\033[95m";
const string NULL_ = "\033[90m";
const string HEADER = "\033[96m";
const string SIG = "\033[90m";

string base64url_decode(const string& data) {
    string d = data;
    // Добавляем padding
    int pad = 4 - (d.size() % 4);
    if (pad != 4) d.append(pad, '=');
    // Заменяем '-' на '+' и '_' на '/'
    for (char& c : d) {
        if (c == '-') c = '+';
        else if (c == '_') c = '/';
    }
    // Декодируем
    string result;
    size_t in_len = d.size();
    if (in_len % 4) return "";
    vector<unsigned char> out(in_len * 3 / 4);
    size_t out_len = 0;
    for (size_t i = 0; i < in_len; i += 4) {
        int val = 0;
        int bits = 0;
        for (size_t j = 0; j < 4; ++j) {
            char c = d[i+j];
            if (c == '=') break;
            int v;
            if (c >= 'A' && c <= 'Z') v = c - 'A';
            else if (c >= 'a' && c <= 'z') v = c - 'a' + 26;
            else if (c >= '0' && c <= '9') v = c - '0' + 52;
            else if (c == '+') v = 62;
            else if (c == '/') v = 63;
            else v = 0;
            val = (val << 6) | v;
            bits += 6;
        }
        int bytes = bits / 8;
        for (int j = bytes-1; j >= 0; --j) {
            out[out_len + j] = (unsigned char)(val & 0xFF);
            val >>= 8;
        }
        out_len += bytes;
    }
    return string(out.begin(), out.begin()+out_len);
}

string colorize_json(const json& j, int indent = 2, int level = 0) {
    string spaces(level * indent, ' ');
    if (j.is_object()) {
        if (j.empty()) return "{}";
        vector<string> lines;
        lines.push_back("{");
        int i = 0;
        for (auto it = j.begin(); it != j.end(); ++it, ++i) {
            string key = it.key();
            const json& val = it.value();
            string val_str = colorize_json(val, indent, level+1);
            string line = spaces + string(indent, ' ') + KEY + "\"" + key + "\"" + RESET + ": " + val_str;
            if (i < (int)j.size()-1) line += ",";
            lines.push_back(line);
        }
        lines.push_back(spaces + "}");
        string result;
        for (const auto& l : lines) result += l + "\n";
        return result;
    } else if (j.is_array()) {
        if (j.empty()) return "[]";
        vector<string> lines;
        lines.push_back("[");
        for (size_t i=0; i<j.size(); ++i) {
            string line = spaces + string(indent, ' ') + colorize_json(j[i], indent, level+1);
            if (i < j.size()-1) line += ",";
            lines.push_back(line);
        }
        lines.push_back(spaces + "]");
        string result;
        for (const auto& l : lines) result += l + "\n";
        return result;
    } else if (j.is_string()) {
        return STR + "\"" + j.get<string>() + "\"" + RESET;
    } else if (j.is_boolean()) {
        return BOOL + (j.get<bool>() ? "true" : "false") + RESET;
    } else if (j.is_null()) {
        return NULL_ + "null" + RESET;
    } else if (j.is_number()) {
        return NUM + to_string(j.get<double>()) + RESET;
    }
    return j.dump();
}

bool decode_jwt(const string& token, json& header, json& payload, string& sig) {
    vector<string> parts;
    stringstream ss(token);
    string part;
    while (getline(ss, part, '.')) parts.push_back(part);
    if (parts.size() != 3) return false;
    string header_b64 = parts[0], payload_b64 = parts[1];
    sig = parts[2];
    string header_str = base64url_decode(header_b64);
    string payload_str = base64url_decode(payload_b64);
    try {
        header = json::parse(header_str);
        payload = json::parse(payload_str);
    } catch (...) {
        return false;
    }
    return true;
}

int main(int argc, char* argv[]) {
    string token;
    bool compact = false;
    string file_path;
    for (int i=1; i<argc; ++i) {
        string arg = argv[i];
        if (arg == "--file" && i+1 < argc) {
            file_path = argv[++i];
        } else if (arg == "--compact") {
            compact = true;
        } else {
            token = arg;
        }
    }
    if (!file_path.empty()) {
        ifstream f(file_path);
        if (f.is_open()) {
            getline(f, token, '\0');
            f.close();
        } else {
            cerr << "Ошибка чтения файла." << endl;
            return 1;
        }
    }
    if (token.empty()) {
        cerr << "Не указан JWT-токен." << endl;
        return 1;
    }
    json header, payload;
    string sig;
    if (!decode_jwt(token, header, payload, sig)) {
        cerr << "Ошибка декодирования JWT." << endl;
        return 1;
    }
    int indent = compact ? 0 : 2;
    cout << HEADER << "--- Header ---" << RESET << endl;
    cout << colorize_json(header, indent);
    cout << HEADER << "--- Payload ---" << RESET << endl;
    cout << colorize_json(payload, indent);
    cout << SIG << "--- Signature ---" << RESET << endl;
    cout << SIG << sig << RESET << endl;
    return 0;
}
