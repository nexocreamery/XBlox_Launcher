#include <iostream>
#include <string>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <cctype>

std::string urlEncode(const std::string& value)
{
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;

    for (unsigned char c : value)
    {
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
            escaped << c;
        else
            escaped << '%' << std::setw(2) << int(c);
    }

    return escaped.str();
}

std::string runCurl(const std::string& command)
{
    std::string result;
    char buffer[4096];

    FILE* pipe = _popen(command.c_str(), "r");
    if (!pipe) return "";

    while (fgets(buffer, sizeof(buffer), pipe))
        result += buffer;

    _pclose(pipe);
    return result;
}

std::string extractCookie(const std::string& headers)
{
    std::istringstream stream(headers);
    std::string line;

    while (std::getline(stream, line))
    {
        if (line.find("Set-Cookie: ROBLOSECURITY=") != std::string::npos)
        {
            size_t start = line.find("ROBLOSECURITY=") + 14;
            size_t end = line.find(';', start);
            return line.substr(start, end - start);
        }
    }

    return "";
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cout << "Usage:\n";
        std::cout << "CLI Cookie <cookie> <id>\n";
        std::cout << "CLI Login <username> <password> <id>\n";
        return 1;
    }

    std::string mode = argv[1];

    if (mode == "Cookie")
    {
        if (argc < 4)
        {
            std::cout << "Usage: CLI Cookie <cookie> <id>\n";
            return 1;
        }

        std::string cookie = argv[2];
        std::string id = argv[3];

        std::string command =
            "XBloxPlayerBeta -c content "
            "-a https://www.xspy.dev/Login/Negotiate.ashx "
            "-t ROBLOSECURITY_COOKIE "
            "-j https://www.xspy.dev/game/join.ashx?session=" + cookie +
            " --id " + id;

        system(command.c_str());
    }

    else if (mode == "Login")
    {
        if (argc < 5)
        {
            std::cout << "Usage: CLI Login <username> <password> <id>\n";
            return 1;
        }

        std::string username = urlEncode(argv[2]);
        std::string password = urlEncode(argv[3]);
        std::string id = argv[4];

        std::cout << "Sending login request...\n";

        std::string curlCommand =
            "curl -i \"https://www.xspy.dev/Login/NewLogin.aspx\" "
            "-X POST "
            "-H \"User-Agent: Mozilla/5.0\" "
            "-H \"Content-Type: application/x-www-form-urlencoded\" "
            "-H \"Origin: https://www.xspy.dev\" "
            "-H \"Referer: https://www.xspy.dev/Login/NewLogin.aspx\" "
            "--data \"username=" + username +
            "&password=" + password +
            "&submitLogin=login\"";

        std::string response = runCurl(curlCommand);

        std::string cookie = extractCookie(response);

        if (cookie.empty())
        {
            std::cout << "Failed to get cookie\n";
            return 1;
        }

        std::cout << "Cookie obtained!\n";

        std::string command =
            "XBloxPlayerBeta -c content "
            "-a https://www.xspy.dev/Login/Negotiate.ashx "
            "-t ROBLOSECURITY_COOKIE "
            "-j https://www.xspy.dev/game/join.ashx?session=" + cookie +
            " --id " + id;

        system(command.c_str());
    }

    else
    {
        std::cout << "Unknown mode\n";
    }

    return 0;
}