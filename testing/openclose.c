#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <qdmi.h>

int QDMI_load_libraries(QInfo sesioninfo);
int QDMI_internal_startup(QInfo info);
int QDMI_session_init(QInfo info, QDMI_Session *session);
int QDMI_session_finalize(QDMI_Session session);
int QDMI_internal_shutdown(void);

#define CHECK_ERR(a,b) { if (a!=QDMI_SUCCESS) { printf("Error: %i at %s\n",a,b); return 1; }}

int main(int argc, char** argv)
{
    QInfo info;
    QDMI_Session session = NULL;
    int err;

    err = QInfo_create(&info);

    printf("\nQDMI_load_libraries: %d\n", QDMI_load_libraries(info));
    CHECK_ERR(err, "During Create");

    printf("\nQDMI_internal_startup: %d\n", QDMI_internal_startup(info));
    CHECK_ERR(err, "During Startup");

    printf("\nQDMI_internal_init: %d\n", QDMI_session_init(info, &session));
    CHECK_ERR(err, "During Startup");

    printf("\nQDMI_internal_finalize: %d\n", QDMI_session_finalize(session));

    printf("\nQDMI_internal_shutdown: %d\n", QDMI_internal_shutdown());
    CHECK_ERR(err, "During Shutdown");

    err = QInfo_free(info);
    CHECK_ERR(err, "During Free");

    return 0;
}

