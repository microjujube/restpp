# restpp
restful web api framework


# example
```cpp 
auto engine = restpp::Engine::make();

engine->GET("/hello", [](const restpp::Request &req, restpp::Response &resp) {
    LOG(INFO) << req.method;
    LOG(INFO) << req.target;
    LOG(INFO) << req.params;
    LOG(INFO) << req.body;

    resp.JSON(200, {
            {"status", "ok"},
        });
});

engine->Run("0.0.0.0:8080");
```