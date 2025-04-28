#ifndef KNOTS_ROUTER_HPP
#define KNOTS_ROUTER_HPP

#include <map>
#include <string>

struct Route {
    std::string filePath;
    std::string contentType;

    Route() : filePath{}, contentType{} {};
    Route(const std::string& filePath, const std::string& contentType)
        : filePath(filePath), contentType(contentType) {};

    bool IsValid() {
        return filePath.size() && contentType.size();
    }
};

class Router {
private:
    std::map<std::string, Route> routes;

    void LoadRoutesFromConfig(const std::string& configFilePath);

public:
    explicit Router(const std::string& configFilePath) {
        LoadRoutesFromConfig(configFilePath);
    }

    Route GetRoute(const std::string& requestUrl) const;
};


#endif