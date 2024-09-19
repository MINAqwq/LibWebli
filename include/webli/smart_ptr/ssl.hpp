#pragma once

#include <openssl/ssl.h>

namespace W::Deleter {
using WebBIO = struct WebBIO {
  void operator()(::BIO *x) {
    if (x == nullptr) {
      return;
    }

    BIO_free_all(x);
  }
};

using SSL_CTX = struct SSL_CTX {
  void operator()(::SSL_CTX *x) {
    if (x == nullptr) {
      return;
    }

    SSL_CTX_free(x);
  }
};
} // namespace W::Deleter