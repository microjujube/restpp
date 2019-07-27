# restpp
Restful Web API Framework

![restpp](https://raw.githubusercontent.com/microjujube/restpp/master/resources/restpp.png)

# example

### Parse GET parameters
```cpp 
#include <restpp/Logger.hpp>
#include <restpp/Engine.hpp>
#include <string>

int main(int argc, char *argv[]) {
    auto engine = restpp::Engine::make();

    engine->StaticFile("/", "./index.html");

    engine->GET("/hello", [](restpp::Context::sptr &ctx) {
        ctx->ParseGetParam();
        auto dict = ctx->GetParams();
        for (auto &key : dict.keys()) {
            LOG(INFO) << key << " = " << dict[key];
        }

        ctx->JSON(200, {
                {"status", "ok"},
        });
    });

    engine->Run("0.0.0.0:8080");
    return 0;
}
```

### Serve POST request and bind json
```cpp
 engine->POST("/hello", [](restpp::Context::sptr &ctx) {
        restpp::json json;
        ctx->ShouldBindJSON(json);
        LOG(INFO) << json.dump();
        ctx->JSON(200, {
                {"status", "ok"},
        });
    });
```
### Serve A static File
```cpp 
#include <restpp/Logger.hpp>
#include <restpp/Engine.hpp>
#include <string>

int main(int argc, char *argv[]) {
    auto engine = restpp::Engine::make();

    engine->StaticFile("/", "index.html");

    engine->Run("0.0.0.0:8080");
    return 0;
}
```

### Serve a directory
```cpp
#include <restpp/Logger.hpp>
#include <restpp/Engine.hpp>
#include <string>
int main(int argc, char *argv[]) {
    auto engine = restpp::Engine::make();
    engine->Static("/static", "static");
    engine->Run("0.0.0.0:8080");
    return 0;
}
```
