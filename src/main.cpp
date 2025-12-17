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
                        "<h1 align=\"center\">Hello world\n</h1>"
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
        res.SetBody(
            "<html>\n"
                "<body>\n"
                    "<h1 align=\"center\">404 Not Found\n</h1>"
                "<html>\n"
            "<body>\n"
        );
        res.SetStatus(404);
        return;
    });

    server.AcceptConnections();

    Log::Info("Stopping server...");
    return 0;
}