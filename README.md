# restpp
restful web api framework


# example

### Parse GET parameters
```cpp 
    auto engine = restpp::Engine::make();

    // /?key=value&a=b&c=d
    engine->GET("/", [](restpp::Request &req, restpp::Response &resp) {
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

### Parse POST body
```cpp
    engine->POST("/", [](restpp::Request &req, restpp::Response &resp) {
        //parse json body
        restpp::json json;
        req.BindJSON(json);
        LOG(INFO) << json.dump();
        
        resp.JSON(200, {
            {"status", "ok"},
        });
    });
```