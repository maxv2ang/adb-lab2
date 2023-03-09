#include <cstdio>

#include "bmgr.hpp"

#define MAXPAGES 50000

using ustcadb::BMgr;

int main() {
    printf("Init Buffer Manager...\n");
    BMgr *bmgr = new BMgr("../data.dbf");

    // create data for `data.dbf`
    int numPages = bmgr->GetNumPages();
    for (; numPages < MAXPAGES; ++numPages) {
        bmgr->FixNewPage(numPages);
    }

    printf("Open trace file...\n");
    FILE *fp;
    if (!(fp = fopen("../data-5w-50w-zipf.txt", "r"))) {
        fprintf(stderr, "ERROR: Unable to open the trace file.\n");
        exit(1);
    }

    printf("Start tracing...\n");
    int type, page_id;
    fseek(fp, 0, SEEK_SET);
    while ((fscanf(fp, "%d,%d", &type, &page_id)) == 2) {
        bmgr->FixPage(page_id, type);
    }

    printf("-----------------------------------\n");
    bmgr->PrintInfo();

    delete bmgr;
    fclose(fp);

    return 0;
}