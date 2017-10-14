#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include "tcx.h"
#include "parsing_utils.h"
#include "printing.h"

void calculate_summary(tcx_t * tcx);

void
calculate_summary(tcx_t * tcx)
{
    activity_t * activity = NULL;
    lap_t * lap = NULL;
    track_t * track = NULL;
    trackpoint_t * trackpoint = NULL;

    activity = tcx->activities;
    while (activity != NULL)
    {
        lap = activity->laps;
        while (lap != NULL)
        {
            track = lap->tracks;
            while (track != NULL)
            {
                trackpoint = track->trackpoints;
                while (trackpoint != NULL)
                {
                    if (trackpoint->cadence > lap->cadence_maximum)
                    {
                        lap->cadence_maximum = trackpoint->cadence;
                    }

                    if (trackpoint->cadence < lap->cadence_minimum)
                    {
                        lap->cadence_minimum = trackpoint->cadence;
                    }

                    if (trackpoint->heart_rate > lap->heart_rate_maximum)
                    {
                        lap->heart_rate_maximum = trackpoint->heart_rate;
                    }

                    if (trackpoint->heart_rate < lap->heart_rate_minimum)
                    {
                        lap->heart_rate_minimum = trackpoint->heart_rate;
                    }

                    if (trackpoint->speed > lap->speed_maximum)
                    {
                        lap->speed_maximum = trackpoint->speed;
                    }

                    if (trackpoint->speed < lap->speed_minimum)
                    {
                        lap->speed_minimum = trackpoint->speed;
                    }

                    lap->cadence_average += trackpoint->cadence;
                    lap->heart_rate_average += trackpoint->heart_rate;
                    lap->speed_average += trackpoint->speed;

                    trackpoint = trackpoint->next;
                }

                lap->cadence_average /= lap->num_trackpoints;
                lap->heart_rate_average /= lap->num_trackpoints;
                lap->speed_average /= lap->num_trackpoints;

                track = track->next;
            }

            lap = lap->next;
        }

        activity = activity->next;
    }

    free(trackpoint);
    free(track);
    free(lap);
    free(activity);
}

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
        free(tcx);
        return 1;
    }

    xmlInitParser();

    xmlDocPtr document = xmlReadFile(tcx->filename, NULL, 0);
    if (document == NULL)
    {
        fprintf(stderr, "Could not parse %s.\n", tcx->filename);
        free(tcx);
        return 1;
    }

    xmlXPathContextPtr context = xmlXPathNewContext(document);
    xmlXPathRegisterNs(context, (xmlChar *)"tcx", (xmlChar *)"http://www.garmin.com/xmlschemas/TrainingCenterDatabase/v2");

    xmlXPathObjectPtr activities = xmlXPathEvalExpression((xmlChar *)"//tcx:Activity", context);
    if (activities == NULL || xmlXPathNodeSetIsEmpty(activities->nodesetval))
    {
        printf("No activities found in \"%s\"\n", tcx->filename);
        xmlFreeDoc(document);
        xmlCleanupParser();
        free(tcx);
        return 1;
    }

    if (activities != 0 && !xmlXPathNodeSetIsEmpty(activities->nodesetval))
    {
        xmlNodeSetPtr activity_nodeset = activities->nodesetval;
        for (int i = 0; i < activity_nodeset->nodeNr; i++)
        {
            if (activity_nodeset->nodeTab[i]->type != XML_ELEMENT_NODE)
            {
                continue;
            }

            activity_t * activity = calloc(1, sizeof(activity_t));
            add_activity(tcx, activity);

            xmlXPathObjectPtr laps = xmlXPathEvalExpression((xmlChar *)"//tcx:Lap", context);
            if (laps != 0 && !xmlXPathNodeSetIsEmpty(laps->nodesetval))
            {
                xmlNodeSetPtr lap_nodeset = laps->nodesetval;
                for (int j = 0; j < lap_nodeset->nodeNr; j++)
                {
                    if (lap_nodeset->nodeTab[j]->type != XML_ELEMENT_NODE)
                    {
                        continue;
                    }

                    lap_t * lap = parse_lap(document, lap_nodeset->nodeTab[j]->ns, lap_nodeset->nodeTab[j]);
                    add_lap(lap);

                    xmlNodePtr tracks = lap_nodeset->nodeTab[j]->xmlChildrenNode;
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
            }

            if (laps != NULL)
            {
                xmlXPathFreeObject(laps);
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

    print_tcx(tcx);

    free(tcx->filename);
    free(tcx);
}
