# restpp
restful web api framework


# example
```cpp 
    auto engine = restpp::Engine::make();

    engine->GET("/", [](restpp::Request &req, restpp::Response &resp) {
        LOG(INFO) << req.getMethod();
        LOG(INFO) << req.getTarget();
        LOG(INFO) << req.getParams();
        LOG(INFO) << req.getBody();

        //parse get parameters
        req.ParseGetParam();
        for (auto const &key:req.GetParams().keys()) {
            LOG(INFO) << key << "=" << req.GetParams()[key];
        }

        resp.JSON(200, {
                {"status", "ok"},
        });
    });

    engine->Run("0.0.0.0:8080");
```