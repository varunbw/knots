#include <fstream>
#include <gtest/gtest.h>

#include "Router.hpp"
#include "Utils.hpp"

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

    req = HttpRequest(HttpMethod::GET, "/index.html", HttpVersion::HTTP_1_1, {}, {}, {}, {});

    const HandlerFunction* handlerPtr1 = router.FetchRoute(req);

    EXPECT_TRUE(handlerPtr1);
    (*handlerPtr1)(req, res);
    EXPECT_EQ(res.body, "GET for /index.html");

    req = HttpRequest(HttpMethod::POST, "/contact.html", HttpVersion::HTTP_1_1, {}, {}, {}, {});
    
    const HandlerFunction* handlerPtr2 = router.FetchRoute(req);

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

    req = HttpRequest(HttpMethod::PATCH, "/index.html", HttpVersion::HTTP_1_1, {}, {}, {}, {});
    const HandlerFunction* handlerPtr1 = router.FetchRoute(req);
    EXPECT_FALSE(handlerPtr1);

    req = HttpRequest(HttpMethod::GET, "/contact.html", HttpVersion::HTTP_1_1, {}, {},  {}, {});
    const HandlerFunction* handlerPtr2 = router.FetchRoute(req);
    EXPECT_FALSE(handlerPtr2);

    req = HttpRequest(HttpMethod::PATCH, "/about.html", HttpVersion::HTTP_1_1, {}, {},  {}, {});
    const HandlerFunction* handlerPtr3 = router.FetchRoute(req);
    EXPECT_FALSE(handlerPtr3);
}

TEST(RouterTest, CheckRouteParameterParsing) {

    std::vector<std::string> urlsToAdd = {
        "/users",
        "/users/{userId}",
        "/users/{userId}/profile",
        "/users/{userId}/settings",
        "/users/{userId}/orders",
        "/users/{userId}/orders/{orderId}",
        "/users/details",
        "/users/details/{detailId}",
        "/users/search",
        "/users/search/{query}",
        "/login",
        "/logout",
        "/auth/{provider}",
        "/auth/{provider}/callback",
        "/session/{sessionId}",
        "/products",
        "/products/{productId}",
        "/products/{productId}/reviews",
        "/products/{productId}/reviews/{reviewId}",
        "/products/category/{categoryName}",
        "/products/category/{categoryName}/page/{page}",
        "/blog",
        "/blog/{slug}",
        "/blog/{slug}/comments",
        "/blog/{slug}/comments/{commentId}",
        "/blog/archive/{year}/{month}",
        "/blog/tags/{tag}",
        "/admin",
        "/admin/users",
        "/admin/users/{userId}",
        "/admin/settings",
        "/admin/logs/{date}",
        "/files/{fileId}",
        "/files/{fileId}/download",
        "/media/images",
        "/media/images/{imageId}",
        "/cart",
        "/cart/items",
        "/cart/items/{itemId}",
        "/checkout",
        "/notifications",
        "/notifications/{notificationId}",
        "/search/{query}/page/{page}",
        "/status/{code}",
        "/settings",
        "/settings/{section}",
        "/cities/{cityName}/weather",
        "/v1/{resource}/{id}",
        "/api/{version}/users/{userId}",
        "/healthcheck",
    };
    std::vector<std::string> urlsToQuery = {
        "/users",
        "/users/100",
        "/users/100/profile",
        "/users/100/settings",
        "/users/100/orders",
        "/users/100/orders/100",
        "/users/details",
        "/users/details/100",
        "/users/search",
        "/users/search/100",
        "/login",
        "/logout",
        "/auth/100",
        "/auth/100/callback",
        "/session/100",
        "/products",
        "/products/100",
        "/products/100/reviews",
        "/products/100/reviews/100",
        "/products/category/100",
        "/products/category/100/page/100",
        "/blog",
        "/blog/100",
        "/blog/100/comments",
        "/blog/100/comments/100",
        "/blog/archive/100/100",
        "/blog/tags/100",
        "/admin",
        "/admin/users",
        "/admin/users/100",
        "/admin/settings",
        "/admin/logs/100",
        "/files/100",
        "/files/100/download",
        "/media/images",
        "/media/images/100",
        "/cart",
        "/cart/items",
        "/cart/items/100",
        "/checkout",
        "/notifications",
        "/notifications/100",
        "/search/100/page/100",
        "/status/100",
        "/settings",
        "/settings/100",
        "/cities/100/weather",
        "/v1/100/100",
        "/api/100/users/100",
        "/healthcheck",
    };
    std::vector<HttpMethod> methods = {
        HttpMethod::GET,
        HttpMethod::HEAD,
        HttpMethod::POST,
        HttpMethod::PUT,
        HttpMethod::DELETE,
        HttpMethod::CONNECT,
        HttpMethod::OPTIONS,
        HttpMethod::TRACE,
        HttpMethod::PATCH
    };
    
    Router router;

    for (const std::string& url : urlsToAdd) {
        for (const HttpMethod& method : methods) {
            router.AddRoute(
                method, url, 
                [] (const HttpRequest& req, HttpResponse& res) {
                    std::string routeParamsString{};
                    for (auto& param : req.routeParams) {
                        routeParamsString += std::format(
                            "{} : {}, ",
                            param.first, param.second
                        );
                    }

                    res.body = std::format(
                        "{}, {} | {}",
                        req.method, req.requestUrl, routeParamsString
                    );

                    res.SetHeader("Content-Type", "text/html");
                    res.SetHeader("Content-Length", std::to_string(res.body.size()));

                    return;
                }
            );
        }  
    }

    for (const std::string& url : urlsToQuery) {
        for (const HttpMethod& method : methods) {
            HttpRequest req;
            req.requestUrl = url;
            req.method = method;

            const HandlerFunction* handler = router.FetchRoute(req);

            EXPECT_TRUE(handler) << MakeErrorMessage(std::format(
                "Handler not found for Route: {}, {}",
                method, url
            ));

            for (auto& param : req.routeParams) {
                EXPECT_EQ(param.second, "100") << MakeErrorMessage(std::format(
                    "Incorrect param for Route: {}, {}",
                    method, url
                ));
            }
        }
    }
}