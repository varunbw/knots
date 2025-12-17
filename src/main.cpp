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
            res.SetBody(
                "<html>\n"
                    "<body>\n"
                        "<div style=\"align: center\"><h1>Hello world\n<h1></div>"
                    "<html>\n"
                "<body>\n"
            );
            return;
        }
    );

    const std::string buffer(65536, '0');
    router.Get("/spam", 
        [buffer] (const HttpRequest& req, HttpResponse& res) {
            res.SetBody(buffer);
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