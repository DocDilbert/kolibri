#ifndef INTERTEST_SRC_STRING_PROVIDER_H_
#define INTERTEST_SRC_STRING_PROVIDER_H_

struct StringProviderOpenComment {
  constexpr const char* GetString() { return "/*"; }
};

struct StringProviderCloseComment {
  constexpr const char* GetString() { return "*/"; }
};
 
struct StringProviderBegin {
  constexpr const char* GetString() { return "BEGIN"; }
};

struct StringProviderEnd {
  constexpr const char* GetString() { return "END"; }
};

struct StringProviderDiv {
  constexpr const char* GetString() { return "DIV"; }
};

struct StringProviderProgram {
  constexpr const char* GetString() { return "PROGRAM"; }
};

struct StringProviderVar {
  constexpr const char* GetString() { return "VAR"; }
};

struct StringProviderInteger {
  constexpr const char* GetString() { return "INTEGER"; }
};

struct StringProviderReal {
  constexpr const char* GetString() { return "REAL"; }
};
#endif