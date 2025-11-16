# CLAUDE.md - AI Assistant Guide for college-bbs-backend

This document provides comprehensive guidance for AI assistants working on the college-bbs-backend project.

## Table of Contents

- [Project Overview](#project-overview)
- [Technology Stack](#technology-stack)
- [Repository Structure](#repository-structure)
- [Development Environment Setup](#development-environment-setup)
- [Build System](#build-system)
- [Database Configuration](#database-configuration)
- [Development Workflow](#development-workflow)
- [Code Conventions](#code-conventions)
- [Common Tasks](#common-tasks)
- [Important Gotchas](#important-gotchas)
- [Testing](#testing)
- [Deployment Considerations](#deployment-considerations)

## Project Overview

**college-bbs-backend** is a high-performance web backend for a college Bulletin Board System (BBS) built using the Drogon C++ framework.

**Current State:** Early development phase - basic scaffolding is complete, but core features are not yet implemented.

**Key Features (Planned):**
- RESTful API for BBS functionality
- User authentication and authorization
- Thread/post management
- Database-backed data persistence
- High-performance async I/O

**License:** MIT (Copyright 2025 王肇麒)

## Technology Stack

### Core Framework
- **Drogon** - Modern C++ web framework (async/coroutine-based)
  - Built on Trantor (async networking library)
  - Supports ORM, plugins, and MVC architecture
  - Documentation: https://drogon.docsforge.com/

### Language
- **C++17** (minimum)
- **C++20** (preferred for coroutine support)
- Modern C++ features: `std::any`, `std::string_view`, optional coroutines

### Database
- **PostgreSQL** (primary RDBMS)
  - Host: 127.0.0.1
  - Port: 5432
  - Connection pooling enabled

### Build System
- **CMake** (minimum version 3.5)
- Standard build directory: `college-bbs/build/`

### Additional Dependencies
- **JsonCpp** - JSON parsing (via Drogon)
- **libpq** - PostgreSQL client library
- **OpenSSL** - For HTTPS support (optional)

## Repository Structure

```
college-bbs-backend/
├── .git/                          # Git repository
├── .gitignore                     # C/C++, CMake, IDE ignores
├── LICENSE                        # MIT License
├── README.md                      # Basic project info
├── CLAUDE.md                      # This file - AI assistant guide
└── college-bbs/                   # Main project directory
    ├── build/                     # Build artifacts (gitignored)
    │   ├── college-bbs           # Main executable
    │   └── test/
    │       └── college-bbs_test  # Test executable
    ├── models/                    # Database ORM models
    │   └── model.json            # Model generation config
    ├── controllers/               # HTTP request handlers (to be created)
    ├── filters/                   # Request/response filters (to be created)
    ├── plugins/                   # Custom plugins (to be created)
    ├── views/                     # CSP templates (to be created)
    ├── test/                      # Test suite
    │   ├── CMakeLists.txt        # Test build config
    │   └── test_main.cc          # Test entry point
    ├── CMakeLists.txt            # Main build configuration
    ├── config.json               # Active server configuration
    ├── config.json.bak           # Configuration backup
    ├── config.yaml               # Alternative config (not used)
    ├── main.cc                   # Application entry point
    └── .gitignore                # Project-specific ignores
```

## Development Environment Setup

### Prerequisites

1. **C++ Compiler** with C++17+ support
   - GCC 8+ or Clang 7+

2. **CMake** 3.5 or higher
   ```bash
   cmake --version
   ```

3. **Drogon Framework**
   - Installation: https://github.com/drogonframework/drogon/wiki/ENG-02-Installation

4. **PostgreSQL** 9.5+ (including development headers)
   ```bash
   sudo apt-get install postgresql postgresql-contrib libpq-dev  # Ubuntu/Debian
   ```

### Initial Setup

1. Clone the repository:
   ```bash
   git clone <repository-url>
   cd college-bbs-backend
   ```

2. Configure PostgreSQL database:
   - Create database and user
   - Update `college-bbs/models/model.json` with credentials
   - Update `college-bbs/config.json` if needed

3. Build the project:
   ```bash
   cd college-bbs
   mkdir -p build
   cd build
   cmake ..
   make
   ```

## Build System

### CMake Configuration

**Main CMakeLists.txt** (`college-bbs/CMakeLists.txt`):
- Project name: `college-bbs`
- Auto-detects C++ standard (14/17/20)
- Requires `Drogon::Drogon` package
- Monitors source directories:
  - `controllers/` → `CTL_SRC`
  - `filters/` → `FILTER_SRC`
  - `plugins/` → `PLUGIN_SRC`
  - `models/` → `MODEL_SRC`
- Compiles views using `drogon_create_views`

### Build Commands

```bash
# Standard build
cd college-bbs/build
cmake ..
make

# Rebuild from scratch
rm -rf build
mkdir build
cd build
cmake ..
make

# Build with debug symbols
cmake -DCMAKE_BUILD_TYPE=Debug ..
make

# Build tests
make college-bbs_test
```

### Running the Application

```bash
# From build directory
./college-bbs

# Server will listen on 0.0.0.0:8080 by default
```

**Important:** The current `main.cc` uses a hardcoded config path:
```cpp
drogon::app().loadConfigFile("/home/mikey/proj/college-bbs-backend/college-bbs/config.json");
```

**AI Assistant Note:** When modifying deployment or making the app portable, change this to a relative path or command-line argument.

## Database Configuration

### Model Configuration (`models/model.json`)

**Key Settings:**
- `rdbms`: "postgresql" (or "mysql", "sqlite3")
- `host`: "127.0.0.1"
- `port`: 5432
- `dbname`: Database name (currently empty - needs configuration)
- `user`: Database user (currently empty - needs configuration)
- `password`: Database password (currently empty - needs configuration)
- `schema`: "public" (PostgreSQL schema)
- `tables`: Array of tables to model (empty = all tables)

### ORM Model Generation

**Generate models from existing database:**
```bash
cd college-bbs
drogon_ctl create model models
```

This will:
1. Connect to the database specified in `models/model.json`
2. Generate C++ ORM classes for each table
3. Place generated files in `models/` directory

**Model Features:**
- **Relationships**: Configured in `relationships` section (currently disabled)
  - Supports: has_one, has_many, many_to_many
- **Data Conversion**: Transform data before/after DB operations (currently disabled)
- **RESTful Controllers**: Auto-generate CRUD endpoints (currently disabled)

### Database Schema Best Practices

When creating database tables for BBS functionality:

1. **Users Table**: id, username, email, password_hash, created_at, updated_at
2. **Threads/Topics Table**: id, title, author_id, created_at, updated_at
3. **Posts Table**: id, thread_id, author_id, content, created_at, updated_at
4. **Use proper indexes** on foreign keys and frequently queried columns
5. **Timestamps**: Use `TIMESTAMP WITH TIME ZONE` in PostgreSQL

## Development Workflow

### Creating New Features

1. **Database First** (if data model changes):
   ```bash
   # Create migration/schema changes in PostgreSQL
   # Then regenerate models
   cd college-bbs
   drogon_ctl create model models
   ```

2. **Create Controller**:
   ```bash
   cd college-bbs
   drogon_ctl create controller <ControllerName>
   # Example: drogon_ctl create controller api::v1::UserController
   ```

   Generated files go to `controllers/` directory.

3. **Create Filter** (for auth, logging, etc.):
   ```bash
   drogon_ctl create filter <FilterName>
   # Example: drogon_ctl create filter AuthFilter
   ```

4. **Create Plugin**:
   ```bash
   drogon_ctl create plugin <PluginName>
   ```

5. **Rebuild**:
   ```bash
   cd build
   make
   ```

### Git Workflow

**Branches:**
- `main`: Stable release branch
- `claude/claude-md-mi1qqhk8jtem82qf-013F9q9juPm4nFxfGpFXjRR9`: Current AI development branch

**Commit Messages:**
- Project uses Chinese commit messages
- Format: `[类型] 描述`
- Example: `[初始化] 创建Drogon项目`
- Common types: [初始化], [新增], [修复], [优化], [文档]

**AI Assistant Guidelines:**
- Use descriptive English or Chinese commit messages
- Always commit to the designated Claude branch
- Push with: `git push -u origin <branch-name>`
- Retry failed pushes up to 4 times with exponential backoff (2s, 4s, 8s, 16s)

## Code Conventions

### File Organization

**Controllers** (`controllers/`):
- One controller per resource/feature
- Naming: `<Resource>Controller.h` and `<Resource>Controller.cc`
- Use namespaces: `namespace api::v1`
- HTTP method handlers: `getUser`, `createPost`, etc.

**Models** (`models/`):
- Auto-generated by Drogon ORM
- Don't manually edit generated files
- Extend models by creating wrapper classes if needed

**Filters** (`filters/`):
- Authentication filters
- CORS filters
- Request validation filters
- Logging filters

### C++ Coding Style

**General Guidelines:**
- Use modern C++ features (smart pointers, auto, range-for)
- Prefer `std::string_view` for read-only string parameters
- Use `const` wherever possible
- RAII for resource management

**Async Programming:**
- Drogon uses callbacks and coroutines (C++20)
- Example callback:
  ```cpp
  void handleRequest(const HttpRequestPtr& req,
                     std::function<void(const HttpResponsePtr&)>&& callback)
  ```
- Example coroutine (C++20):
  ```cpp
  Task<HttpResponsePtr> handleRequest(HttpRequestPtr req)
  ```

**Error Handling:**
- Use `try-catch` for exceptional cases
- Return appropriate HTTP status codes
- Use Drogon's built-in error responses

### JSON Handling

```cpp
// Reading JSON from request
Json::Value json = *req->getJsonObject();
std::string username = json["username"].asString();

// Creating JSON response
Json::Value response;
response["status"] = "success";
response["data"] = userData;
auto resp = HttpResponse::newHttpJsonResponse(response);
```

## Common Tasks

### Task 1: Add a New REST API Endpoint

**Example: Create User Registration Endpoint**

1. **Create Controller:**
   ```bash
   cd college-bbs
   drogon_ctl create controller -h api::v1::UserController
   ```

2. **Edit Controller** (`controllers/api_v1_UserController.cc`):
   ```cpp
   void UserController::registerUser(const HttpRequestPtr& req,
                                     std::function<void(const HttpResponsePtr&)>&& callback)
   {
       auto json = req->getJsonObject();
       // Validation, business logic, database operations

       Json::Value response;
       response["status"] = "success";
       callback(HttpResponse::newHttpJsonResponse(response));
   }
   ```

3. **Register Route** (in header file):
   ```cpp
   METHOD_LIST_BEGIN
   ADD_METHOD_TO(UserController::registerUser, "/api/v1/users/register", Post);
   METHOD_LIST_END
   ```

4. **Rebuild and Test:**
   ```bash
   cd build && make && ./college-bbs
   ```

### Task 2: Add Authentication Filter

1. **Create Filter:**
   ```bash
   drogon_ctl create filter AuthFilter
   ```

2. **Implement Filter Logic** (`filters/AuthFilter.cc`)

3. **Apply to Controllers:**
   ```cpp
   METHOD_LIST_BEGIN
   ADD_METHOD_TO(UserController::getProfile, "/api/v1/users/profile", Get, "AuthFilter");
   METHOD_LIST_END
   ```

### Task 3: Update Database Schema

1. **Modify PostgreSQL schema** directly

2. **Regenerate models:**
   ```bash
   cd college-bbs
   drogon_ctl create model models
   ```

3. **Review generated files** in `models/`

4. **Update controllers** to use new model fields

### Task 4: Add Configuration Option

1. **Edit `config.json`:**
   ```json
   {
     "app": {
       "custom_settings": {
         "max_post_length": 10000
       }
     }
   }
   ```

2. **Access in code:**
   ```cpp
   auto &config = app().getCustomConfig();
   int maxLength = config["max_post_length"].asInt();
   ```

## Important Gotchas

### 1. Hardcoded Config Path in main.cc

**Issue:** `main.cc:6` contains an absolute path:
```cpp
drogon::app().loadConfigFile("/home/mikey/proj/college-bbs-backend/college-bbs/config.json");
```

**AI Assistant Action Required:**
- When deploying or improving portability, change to:
  ```cpp
  drogon::app().loadConfigFile("./config.json");
  // Or use command-line argument
  ```

### 2. Empty Database Configuration

**Issue:** `models/model.json` has empty `dbname`, `user`, and `password` fields.

**AI Assistant Action Required:**
- Always ask user for database credentials before attempting to generate models
- Never commit credentials to version control
- Consider using environment variables for sensitive data

### 3. Missing Source Directories

**Issue:** CMakeLists.txt references `controllers/`, `filters/`, `plugins/` directories that don't exist yet.

**AI Assistant Action:**
- Create directories as needed:
  ```bash
  mkdir -p college-bbs/{controllers,filters,plugins,views}
  ```

### 4. Thread Configuration

**Current Setting:** `number_of_threads: 1` in `config.json`

**AI Assistant Note:**
- Single-threaded mode is for development only
- For production, set to CPU core count or higher
- Typical value: `std::thread::hardware_concurrency()`

### 5. Debug Log Level

**Current Setting:** `log_level: DEBUG` in `config.json`

**AI Assistant Note:**
- Debug logging has performance overhead
- Change to `INFO` or `WARN` for production
- Available levels: TRACE, DEBUG, INFO, WARN, ERROR, FATAL

### 6. No HTTPS Enabled

**Current Setting:** `https: false` in listeners

**AI Assistant Note:**
- HTTP only suitable for development
- For production, enable HTTPS with SSL certificates
- See `config.yaml` for HTTPS configuration examples

## Testing

### Test Framework

**Test Entry Point:** `college-bbs/test/test_main.cc`

**Drogon Test Macros:**
```cpp
DROGON_TEST(TestName)
{
    // Test code
    CHECK(condition);
}
```

### Running Tests

```bash
cd college-bbs/build
make college-bbs_test
./test/college-bbs_test
```

### Writing Tests

**Async Test Example:**
```cpp
DROGON_TEST(UserRegistration)
{
    auto client = HttpClient::newHttpClient("http://127.0.0.1:8080");
    auto req = HttpRequest::newHttpRequest();
    req->setMethod(Post);
    req->setPath("/api/v1/users/register");

    Json::Value body;
    body["username"] = "testuser";
    req->setBody(body.toStyledString());

    client->sendRequest(req, [TEST_CTX](ReqResult result, const HttpResponsePtr& resp) {
        REQUIRE(result == ReqResult::Ok);
        CHECK(resp->getStatusCode() == k200OK);
    });
}
```

### Test Best Practices

1. **Isolation**: Each test should be independent
2. **Cleanup**: Reset database state between tests
3. **Coverage**: Test both success and error paths
4. **Async**: Use TEST_CTX for async operations
5. **Descriptive**: Use clear test names

## Deployment Considerations

### Configuration for Production

**Changes needed in `config.json`:**

1. **Threads**: Increase to match CPU cores
   ```json
   "number_of_threads": 16
   ```

2. **Log Level**: Reduce verbosity
   ```json
   "log_level": "WARN"
   ```

3. **Enable Session** (if needed for authentication):
   ```json
   "session": {
       "timeout": 1200,
       "cookie_key": "JSESSIONID"
   }
   ```

4. **Enable HTTPS**:
   ```json
   {
       "address": "0.0.0.0",
       "port": 443,
       "https": true,
       "cert": "/path/to/cert.pem",
       "key": "/path/to/key.pem"
   }
   ```

5. **Database Connection Pool**: Configure in config.json
   ```json
   "db_clients": [{
       "rdbms": "postgresql",
       "host": "127.0.0.1",
       "port": 5432,
       "dbname": "college_bbs",
       "user": "app_user",
       "password": "${DB_PASSWORD}",
       "connection_number": 10,
       "fast": false
   }]
   ```

### Environment Variables

**Recommended approach for secrets:**

```bash
# Export variables
export DB_PASSWORD="secure_password"
export SESSION_SECRET="random_secret_key"

# Reference in config or code
```

### Monitoring

**Built-in Plugins:**

1. **Prometheus Metrics** (see `config.yaml`):
   - Endpoint: `/metrics`
   - Plugin: `drogon::plugin::PromExporter`

2. **Access Logger**:
   - Plugin: `drogon::plugin::AccessLogger`
   - Logs all HTTP requests

### Docker Deployment (Future)

**Recommended Dockerfile structure:**
```dockerfile
FROM drogonframework/drogon:latest
WORKDIR /app
COPY college-bbs/ .
RUN mkdir build && cd build && cmake .. && make
CMD ["./build/college-bbs"]
```

## Resources

### Drogon Documentation
- Official Docs: https://drogon.docsforge.com/
- GitHub: https://github.com/drogonframework/drogon
- Wiki: https://github.com/drogonframework/drogon/wiki

### C++ References
- Modern C++: https://en.cppreference.com/
- C++20 Coroutines: https://en.cppreference.com/w/cpp/language/coroutines

### PostgreSQL
- Official Docs: https://www.postgresql.org/docs/
- Best Practices: https://wiki.postgresql.org/wiki/Don%27t_Do_This

## AI Assistant Quick Reference

### Common Questions & Answers

**Q: How do I add a new API endpoint?**
A: Create a controller with `drogon_ctl create controller`, add method, register route with `ADD_METHOD_TO`, rebuild.

**Q: Database credentials are missing, what should I do?**
A: Ask the user for PostgreSQL connection details before attempting model generation.

**Q: Should I create a new file or edit existing?**
A: Always prefer editing existing files unless specifically asked to create new ones.

**Q: What commit message format should I use?**
A: Follow the pattern `[类型] 描述` in Chinese, or use clear English descriptions.

**Q: How do I handle async operations?**
A: Use Drogon's callback pattern or C++20 coroutines (Task<T>) if available.

**Q: Where should business logic go?**
A: Controllers for HTTP handling, separate service classes for complex business logic.

### Pre-flight Checklist for AI Assistants

Before making changes:
- [ ] Read relevant source files
- [ ] Check if database configuration is complete
- [ ] Verify build directory exists
- [ ] Understand current project state (early development)
- [ ] Check for hardcoded paths that need fixing
- [ ] Consider thread safety for async operations

After making changes:
- [ ] Rebuild project (`cd build && make`)
- [ ] Run tests if applicable
- [ ] Update this CLAUDE.md if workflow/structure changes
- [ ] Commit with descriptive message
- [ ] Push to correct branch

---

**Last Updated:** 2025-11-16
**Project Phase:** Initial Development
**Drogon Version:** Latest (installed from system)
**C++ Standard:** C++17 minimum, C++20 recommended
