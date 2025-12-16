// Project
#include "FileHandler.hpp"
#include "HttpServer.hpp"
#include "Utils.hpp"

int main(void) {

    HttpServerConfiguration config = ParseConfigurationFile("config/config.yaml");

    Router router;

    // Example routes
    router.Get("/",
        [] (const HttpRequest& req, HttpResponse& res) {
            FileHandler::ReadFileIntoBody("static/index.html", res);
            res.SetHeader("Content-Length", std::to_string(res.body.size()));

            return;
        }
    );

    const std::string buffer(65536, '0');
    router.Get("/spam", 
        [buffer] (const HttpRequest& req, HttpResponse& res) {
            res.body = buffer;
            res.SetHeader("Content-Length", std::to_string(buffer.size()));
            res.SetHeader("Content-Type", "text/html");

            return;
        }
    );

    HttpServer server(config, router);

    server.AddErrorRoute(404, [] (const HttpRequest& req, HttpResponse& res) {
        FileHandler::ReadFileIntoBody("static/not-found.html", res);
        res.SetStatus(404);
        return;
    });

    server.AcceptConnections();

    Log::Info("Stopping server...");
    return 0;
}