# restpp
restful web api framework


# example
```cpp 
auto engine = restpp::Engine::make();
engine->GET("/", [](const restpp::Request &req, restpp::Response &resp) {
    resp.body = "hello world";
    resp.status = 200;
    //...
});
```