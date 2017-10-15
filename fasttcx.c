#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include "tcx.h"
#include "parser.h"
#include "printing.h"
#include "summary.h"

int
main(int argc, char const * argv[])
{
    tcx_t * tcx = calloc(1, sizeof(tcx_t));

    if (argc < 2)
    {
        fprintf(stderr, "Please supply a .TCX file.\n");
        free(tcx);
        return 1;
    }
    else
    {
        tcx->filename = strdup(argv[1]);
    }

    if (access(tcx->filename, F_OK) != 0)
    {
        fprintf(stderr, "Unable to locate %s.\n", tcx->filename);
        free(tcx->filename);
        free(tcx);
        return 1;
    }

    xmlInitParser();

    xmlDocPtr document = xmlReadFile(tcx->filename, NULL, 0);
    if (document == NULL)
    {
        fprintf(stderr, "Could not parse %s.\n", tcx->filename);
        free(tcx->filename);
        free(tcx);
        return 1;
    }

    xmlXPathContextPtr context = xmlXPathNewContext(document);
    xmlXPathRegisterNs(context, (xmlChar *)"tcx", (xmlChar *)"http://www.garmin.com/xmlschemas/TrainingCenterDatabase/v2");

    xmlXPathObjectPtr activities = xmlXPathEvalExpression((xmlChar *)"//tcx:Activity", context);
    if (activities == NULL || activities == 0 || xmlXPathNodeSetIsEmpty(activities->nodesetval))
    {
        printf("No activities found in \"%s\"\n", tcx->filename);
        xmlXPathFreeContext(context);
        xmlFreeDoc(document);
        xmlCleanupParser();
        free(tcx->filename);
        free(tcx);
        return 1;
    }
    else
    {
        for (int i = 0; i < activities->nodesetval->nodeNr; i++)
        {
            activity_t * activity = calloc(1, sizeof(activity_t));
            add_activity(tcx, activity);

            xmlNodePtr laps = activities->nodesetval->nodeTab[i]->xmlChildrenNode;
            while (laps != NULL)
            {
                if (!xmlStrcmp(laps->name, (const xmlChar *)"Lap"))
                {
                    lap_t * lap = parse_lap(document, laps->ns, laps);
                    add_lap(lap);

                    xmlNodePtr tracks = laps->xmlChildrenNode;
                    while (tracks != NULL)
                    {
                        if (!xmlStrcmp(tracks->name, (const xmlChar *)"Track"))
                        {
                            track_t * track = calloc(1, sizeof(track_t));
                            add_track(track);

                            xmlNodePtr trackpoints = tracks->xmlChildrenNode;
                            while (trackpoints != NULL)
                            {
                                if (!xmlStrcmp(trackpoints->name, (const xmlChar *)"Trackpoint"))
                                {
                                    trackpoint_t * trackpoint = parse_trackpoint(document, trackpoints->ns, trackpoints);
                                    add_trackpoint(trackpoint);
                                }

                                trackpoints = trackpoints->next;
                            }

                            if (trackpoints != NULL)
                            {
                                xmlFree(trackpoints);
                            }
                        }

                        tracks = tracks->next;
                    }

                    if (tracks != NULL)
                    {
                        xmlFree(tracks);
                    }
                }

                laps = laps->next;
            }

            if (laps != NULL)
            {
                xmlFree(laps);
            }
        }
    }

    if (activities != NULL)
    {
        xmlXPathFreeObject(activities);
    }

    xmlXPathFreeContext(context);
    xmlFreeDoc(document);
    xmlCleanupParser();

    calculate_summary(tcx);

    free(tcx->filename);
    free(tcx);
}
