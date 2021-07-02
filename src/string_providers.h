#ifndef KOLIBRI_SRC_STRING_PROVIDER_H_
#define KOLIBRI_SRC_STRING_PROVIDER_H_

struct StringProviderOpenComment {
  static constexpr const char* GetString() { return "/*"; }
};

struct StringProviderCloseComment {
  static constexpr const char* GetString() { return "*/"; }
};
 
struct StringProviderBegin {
  static constexpr const char* GetString() { return "BEGIN"; }
};

struct StringProviderEnd {
  static constexpr const char* GetString() { return "END"; }
};

struct StringProviderDiv {
  static constexpr const char* GetString() { return "DIV"; }
};

struct StringProviderProgram {
  static constexpr const char* GetString() { return "PROGRAM"; }
};

struct StringProviderVar {
  static constexpr const char* GetString() { return "VAR"; }
};

struct StringProviderInteger {
  static constexpr const char* GetString() { return "INTEGER"; }
};

struct StringProviderReal {
  static constexpr const char* GetString() { return "REAL"; }
};

struct StringProviderCurlyBraceOpen {
  static constexpr const char* GetString() { return "{"; }
};

struct StringProviderCurlyBraceClose {
  static constexpr const char* GetString() { return "}"; }
};
#endif