#include <fstream>
#include <gtest/gtest.h>

#include "Router.hpp"

TEST(RouterTest, RouteEquality) {

    const Route route = Route(HttpMethod::GET, "/index.html");
    const Route equalRoute = Route(HttpMethod::GET, "/index.html");
    const Route differentUrl = Route(HttpMethod::GET, "/about.html");
    const Route differentMethod = Route(HttpMethod::POST, "/index.html");
    const Route bothDifferent = Route(HttpMethod::POST, "/about.html");
    
    EXPECT_EQ(route, equalRoute);
    EXPECT_NE(route, differentUrl);
    EXPECT_NE(route, differentMethod);
    EXPECT_NE(route, bothDifferent);
}

TEST(RouterTest, FetchValidRoutes) {

    Router router;
    HttpRequest req;
    HttpResponse res;

    router.AddRoute(
        HttpMethod::GET,
        "/index.html",
        [] (const HttpRequest& req, HttpResponse& res) {
            res.body = std::string("GET for /index.html");
            return;
        }
    );
    router.AddRoute(
        HttpMethod::POST,
        "/contact.html",
        [] (const HttpRequest& req, HttpResponse& res) {
            res.body = std::string("POST for /contact.html");
            return;
        }
    );

    const HandlerFunction* handlerPtr1 = router.FetchRoute(
        HttpMethod::GET,
        "/index.html"
    );

    EXPECT_TRUE(handlerPtr1);
    (*handlerPtr1)(req, res);
    EXPECT_EQ(res.body, "GET for /index.html");
    
    const HandlerFunction* handlerPtr2 = router.FetchRoute(
        HttpMethod::POST,
        "/contact.html"
    );

    EXPECT_TRUE(handlerPtr2);
    (*handlerPtr2)(req, res);
    EXPECT_EQ(res.body, "POST for /contact.html");
}

TEST(RouterTest, FetchInvalidRoutes) {

    Router router;
    HttpRequest req;
    HttpResponse res;

    router.AddRoute(
        HttpMethod::GET,
        "/index.html",
        [] (const HttpRequest& req, HttpResponse& res) {
            res.body = std::string("GET for /index.html");
            return;
        }
    );
    router.AddRoute(
        HttpMethod::POST,
        "/contact.html",
        [] (const HttpRequest& req, HttpResponse& res) {
            res.body = std::string("POST for /contact.html");
            return;
        }
    );

    const HandlerFunction* handlerPtr1 = router.FetchRoute(
        HttpMethod::PATCH,
        "/index.html"
    );
    EXPECT_FALSE(handlerPtr1);

    const HandlerFunction* handlerPtr2 = router.FetchRoute(
        HttpMethod::GET,
        "/about.html"
    );
    EXPECT_FALSE(handlerPtr2);

    const HandlerFunction* handlerPtr3 = router.FetchRoute(
        HttpMethod::PATCH,
        "/about.html"
    );
    EXPECT_FALSE(handlerPtr3);
}