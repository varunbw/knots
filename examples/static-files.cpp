#include "knots/HttpServer.hpp"
#include "knots/StaticRoutes.hpp"
#include "knots/Utils.hpp"

int main(void) {

    HttpServerConfiguration config = ParseConfigurationFile("config/config.yaml");

    Router router;

    // StaticRoutes::AddStaticDirectory("/home/varun/dev/knots/static", router, "/home/varun/dev/knots/static");
    StaticRoutes::AddStaticDirectory("/home/varun/dev/", router);

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
        res.SetHeader("Content-Type", "text/html");
        return;
    });

    server.AddErrorRoute(405, [] (const HttpRequest& req, HttpResponse& res) {
        res.SetBody(
            "<html>\n"
                "<body>\n"
                    "<h1 align=\"center\">405 Method Not Allowed\n</h1>"
                "<html>\n"
            "<body>\n"
        );
        res.SetStatus(405);
        res.SetHeader("Content-Type", "text/html");
        return;
    });

    server.AcceptConnections();

    Log::Info("Stopping server...");
    return 0;
}