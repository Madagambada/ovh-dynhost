#include <iostream>
#include <string>

#include <curl/curl.h>
#include "strutil.h"
#include "cxxopts.hpp"

std::string host, user, pw, userAgent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/112.0.0.0 Safari/537.36";

static int my_trace(CURL* handle, curl_infotype type, char* data, size_t size, void* clientp) {
    std::string& ip = *static_cast<std::string*>(clientp);
    if (type != CURLINFO_TEXT) {
        return 0;
    }

    if (strutil::contains(data,"Trying")) {
        ip = strutil::split(strutil::split(data, "Trying ")[1], ":")[0];
    }
    return 0;
}

static size_t curlCallback(void* buffer, size_t size, size_t nmemb, void* param) {
    std::string& text = *static_cast<std::string*>(param);
    size_t totalsize = size * nmemb;
    text.append(static_cast<char*>(buffer), totalsize);
    return totalsize;
}

std::string curlGetIp(std::string url) {
    CURL* curl;
    CURLcode res;
    std::string data;
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_CAINFO, "/etc/ssl/certs/ca-certificates.crt");
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3L);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, userAgent.c_str());
        curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, my_trace);
        curl_easy_setopt(curl, CURLOPT_DEBUGDATA, &data);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(curl, CURLOPT_CONNECT_ONLY, 1L);
        curl_easy_setopt(curl, CURLOPT_DNS_SERVERS, "9.9.9.9:53,149.112.112.112:53");
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            
        }
        curl_easy_cleanup(curl);
    }
    return data;
}

std::string curlGetData(std::string url) {
    CURL* curl;
    CURLcode res;
    std::string data;
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_CAINFO, "/etc/ssl/certs/ca-certificates.crt");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 20L);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_DNS_SERVERS, "9.9.9.9:53,149.112.112.112:53");
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cout << "curlGet failed: " << res + ", " << curl_easy_strerror(res) << "\n";
            curl_easy_cleanup(curl);
            return "curlGet failed";
        }
        curl_easy_cleanup(curl);
    }
    return data;
}

int setIp(std::string newIp) {
    CURL* curl;
    CURLcode res;
    std::string data;
    std::string finalUrl = "https://www.ovh.com/nic/update?system=dyndns&hostname=" + host + "&myip=" + newIp;
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, finalUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_CAINFO, "/etc/ssl/certs/ca-certificates.crt");
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 20L);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_USERNAME, user.c_str());
        curl_easy_setopt(curl, CURLOPT_PASSWORD, pw.c_str());
        curl_easy_setopt(curl, CURLOPT_DNS_SERVERS, "9.9.9.9:53,149.112.112.112:53");
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cout << "curlGet failed: " << res + ", " << curl_easy_strerror(res) << "\n";
            curl_easy_cleanup(curl);
            return 1;
        }
        curl_easy_cleanup(curl);
    }
    return 0;
}


int main(int argc, char* argv[]) {
    std::cout << "DynDns v1.0.0\n\n";
    cxxopts::Options options("DynDns v1.0.0", "Update OVH DynHost");

    options.add_options()
        ("d", "DynHost host", cxxopts::value<std::string>())
        ("u", "username", cxxopts::value<std::string>())
        ("p", "password", cxxopts::value<std::string>())
        ("v,version", "Print version")
        ("h,help", "Print usage")
        ;
  
    auto result = options.parse(argc, argv);

    if (result.unmatched().size() != 0) {
        std::cout << "wrong argument(s):" << std::endl;
        for (int i = 0; i < result.unmatched().size(); i++) {
            std::cout << result.unmatched()[i] << std::endl;
        }
        return 1;
    }

    if (result.count("help")) {
        std::cout << options.help() << std::endl;
        return 0;
    }

    if (result.count("version")) {
        std::cout << "Build with: \n" + std::string(curl_version()) << std::endl;
        return 0;
    }

    if (!result.count("d") || !result.count("u") || !result.count("p")) {
        std::cout << "Error: -d, -u, -p are required arguments" << std::endl;
        return 1;
    }

    host = result["d"].as<std::string>();
    user = result["u"].as<std::string>();
    pw = result["p"].as<std::string>();

    curl_global_init(CURL_GLOBAL_DEFAULT);

    //std::cout << "Current " + host + " IP: ";
    std::string ovhIp = curlGetIp(host);
    //std::cout << ovhIp << std::endl;

    //std::cout << "Current IP: ";
    std::string currentIp = curlGetData("ifconfig.me");
    //std::cout << currentIp << std::endl;

    if (ovhIp == currentIp) {
        curl_global_cleanup();
        return 0;
    }

    //std::cout << "Set IP to: " + currentIp << std::endl;
    setIp(currentIp);

    curl_global_cleanup();
    return 0;
}
