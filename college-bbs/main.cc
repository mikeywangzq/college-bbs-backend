#include <drogon/drogon.h>

int main(int argc, char *argv[]) {
    // Load config file - use relative path for portability
    std::string config_file = "./config.json";

    // Allow custom config file via command line argument
    if (argc > 1) {
        config_file = argv[1];
    }

    drogon::app().loadConfigFile(config_file);

    // Run HTTP framework, the method will block in the internal event loop
    drogon::app().run();

    return 0;
}
