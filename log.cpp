#include "log.h"
#include "opts.h"

log_t clog(unsigned u) {
    return log_t::get(u);
}

log_t log_t::get(unsigned u) {
    return log_t{u < settings.verbose};
}

