#include "routes.h"

const char* paths[] = {
    "/_lbundle.js",
    "/_lindex.js",
    "/_lstyles.css",
    "/favicon.ico",
    "/logo.svg",
    "/warning.svg"
};

const char* contentTypes[] = {
    "application/javascript",
    "application/javascript",
    "text/css",
    "image/x-icon",
    "image/svg+xml",
    "image/svg+xml"
};

// Number of files in the arrays
const int numPaths = sizeof(paths) / sizeof(paths[0]);
