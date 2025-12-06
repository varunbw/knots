// Project
#include "FileHandler.hpp"
#include "HttpServer.hpp"
#include "Utils.hpp"

int main(void) {

    HttpServerConfiguration config = ParseConfigurationFile("config/config.yaml");

    Router router;

    // Example route
    router.Get("/users/{userId}/orders/{orderId}/details/{detailId}/yetAnotherPage",
        [] (const HttpRequest& req, HttpResponse& res) {
            // FileHandler::ReadFileIntoBody("static/index.html", res);
            const std::string userId = req.routeParams.at("userId");
            const std::string orderId = req.routeParams.at("orderId");
            const std::string detailId = req.routeParams.at("detailId");

            const std::string body = std::format(
                "<!DOCTYPE html>\n"
                "<html>\n"
                    "<body style='background-color: black'>\n"
                        "<h1 align='center'>\n"
                            "<div style='color: white;'>Hello World</div>\n"
                            "<div style='color: white;'>userId   : {}</div>\n"
                            "<div style='color: white;'>orderId  : {}</div>\n"
                            "<div style='color: white;'>detailsId: {}</div>\n"
                        "</h1>\n"
                    "</body>\n"
                "</html>",
                userId,
                orderId,
                detailId
            );

            res.body = body;
            res.headers["Content-Length"] = std::to_string(res.body.size());

            return;
        }
    );

    HttpServer server(config, router);

    server.AddErrorRoute(404, [] (const HttpRequest& req, HttpResponse& res) {
        FileHandler::ReadFileIntoBody("static/not-found.html", res);
        
        return;
    });

    server.AcceptConnections();

    Log::Info("Stopping server...");
    return 0;
}