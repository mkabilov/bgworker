#include "postgres.h"

extern void BackgroundWorkerMain(Datum args);
void elog_log(char *string);
int postmaster_is_dead(int rc);
int get_got_sigterm();