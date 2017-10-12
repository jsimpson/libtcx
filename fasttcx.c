#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include "tcx.h"

lap_t * lap_current = NULL;
track_t * track_current = NULL;
trackpoint_t * trackpoint_current = NULL;

void add_lap(tcx_t * tcx, lap_t * lap);
void add_track(tcx_t * tcx, track_t * track);
void add_trackpoint(tcx_t * tcx, trackpoint_t * trackpoint);

lap_t * parse_lap(xmlDocPtr document, xmlNsPtr ns, xmlNodePtr node);
void parse_trackpoint_coordinates(trackpoint_t * trackpoint, xmlDocPtr document, xmlNsPtr ns, xmlNodePtr node);
void parse_trackpoint_heart_beat(trackpoint_t * trackpoint, xmlDocPtr document, xmlNsPtr ns, xmlNodePtr node);
void parse_trackpoint_extensions(trackpoint_t * trackpoint, xmlDocPtr document, xmlNodePtr node);
void parse_trackpoint_extensions_speed(trackpoint_t * trackpoint, xmlDocPtr document, xmlNsPtr ns, xmlNodePtr node);
trackpoint_t * parse_trackpoint(xmlDocPtr document, xmlNsPtr ns, xmlNodePtr node);

void print_lap(lap_t * lap);
void print_trackpoint(trackpoint_t * trackpoint_t);
void print_tcx(tcx_t * tcx);

void
add_lap(tcx_t * tcx, lap_t * lap)
{
    if (lap_current != NULL)
    {
        lap_current->next = lap;
    }
    else
    {
        tcx->activity->laps = lap;
    }

    lap_current = lap;
}

void
add_track(tcx_t * tcx, track_t * track)
{
    if (track_current != NULL)
    {
        track_current->next = track;
    }
    else
    {
        lap_current->tracks = track;
    }

    track_current = track;
}

void
add_trackpoint(tcx_t * tcx, trackpoint_t * trackpoint)
{
    if (trackpoint_current != NULL)
    {
        trackpoint_current->next = trackpoint;
    }
    else
    {
        track_current->trackpoints = trackpoint;
    }

    trackpoint_current = trackpoint;
}

lap_t *
parse_lap(xmlDocPtr document, xmlNsPtr ns, xmlNodePtr node)
{
    lap_t * lap = calloc(1, sizeof(lap_t));

    if (xmlHasProp(node, (xmlChar*)"StartTime"))
    {
        xmlChar * content = xmlGetProp(node, (xmlChar *)"StartTime");
        lap->start_time = strdup((const char *)content);
        xmlFree(content);
    }

    node = node->xmlChildrenNode;
    while (node != NULL) {
        if ((!xmlStrcmp(node->name, (const xmlChar *)"TotalTimeSeconds")) && (node->ns == ns))
        {
            char * end = NULL;
            xmlChar * content = xmlNodeListGetString(document, node->xmlChildrenNode, 1);
            lap->total_time = strtod((char *)content, (char **)&end);
            xmlFree(content);
        }

        if ((!xmlStrcmp(node->name, (const xmlChar *)"DistanceMeters")) && (node->ns == ns))
        {
            char * end = NULL;
            xmlChar * content = xmlNodeListGetString(document, node->xmlChildrenNode, 1);
            lap->distance = strtod((char *)content, (char **)&end);
            xmlFree(content);
        }

        if ((!xmlStrcmp(node->name, (const xmlChar *)"MaximumSpeed")) && (node->ns == ns))
        {
            char * end = NULL;
            xmlChar * content = xmlNodeListGetString(document, node->xmlChildrenNode, 1);
            lap->speed_maximum = strtod((char *)content, (char **)&end);
            xmlFree(content);
        }

        if ((!xmlStrcmp(node->name, (const xmlChar *)"Calories")) && (node->ns == ns))
        {
            xmlChar * content = xmlNodeListGetString(document, node->xmlChildrenNode, 1);
            lap->calories = atoi((char *)content);
            xmlFree(content);
        }

        if ((!xmlStrcmp(node->name, (const xmlChar *)"AverageHeartRateBpm/Value")) && (node->ns == ns))
        {
            xmlChar * content = xmlNodeListGetString(document, node->xmlChildrenNode, 1);
            lap->heart_rate_average = atoi((char *)content);
            xmlFree(content);
        }

        if ((!xmlStrcmp(node->name, (const xmlChar *)"MaximumHeartRateBpm")) && (node->ns == ns))
        {
            xmlChar * content = xmlNodeListGetString(document, node->xmlChildrenNode, 1);
            lap->heart_rate_maximum = atoi((char *)content);
            xmlFree(content);
        }

        if ((!xmlStrcmp(node->name, (const xmlChar *)"Intensity")) && (node->ns == ns))
        {
            xmlChar * content = xmlNodeListGetString(document, node->xmlChildrenNode, 1);
            lap->intensity = strdup((const char *)content);
            xmlFree(content);
        }

        node = node->next;
    }

    return lap;
}

void
parse_trackpoint_coordinates(trackpoint_t * trackpoint, xmlDocPtr document, xmlNsPtr ns, xmlNodePtr node)
{
    node = node->xmlChildrenNode;
    while (node != NULL)
    {
        if ((!xmlStrcmp(node->name, (const xmlChar *)"LatitudeDegrees")) && (node->ns == ns))
        {
            char * end;
            xmlChar * content = xmlNodeListGetString(document, node->xmlChildrenNode, 1);
            trackpoint->latitude = strtod((char *)content, (char **)&end);
            xmlFree(content);
        }

        if ((!xmlStrcmp(node->name, (const xmlChar *)"LongitudeDegrees")) && (node->ns == ns))
        {
            char * end;
            xmlChar * content = xmlNodeListGetString(document, node->xmlChildrenNode, 1);
            trackpoint->longitude = strtod((char *)content, (char **)&end);
            xmlFree(content);
        }

        node = node->next;
    }
}

void
parse_trackpoint_heart_beat(trackpoint_t * trackpoint, xmlDocPtr document, xmlNsPtr ns, xmlNodePtr node)
{
    node = node->xmlChildrenNode;
    while (node != NULL)
    {
        if ((!xmlStrcmp(node->name, (const xmlChar *)"Value")) && (node->ns == ns))
        {
            xmlChar * content = xmlNodeListGetString(document, node->xmlChildrenNode, 1);
            trackpoint->heart_rate = atoi((char *)content);
            xmlFree(content);
        }

        node = node->next;
    }
}

void
parse_trackpoint_extensions(trackpoint_t * trackpoint, xmlDocPtr document, xmlNodePtr node)
{
    xmlNsPtr ns = xmlSearchNsByHref(document, node, (const xmlChar *)"http://www.garmin.com/xmlschemas/ActivityExtension/v2");

    node = node->xmlChildrenNode;
    while (node != NULL)
    {
        if (!xmlStrcmp(node->name, (const xmlChar *)"TPX"))
        {
            parse_trackpoint_extensions_speed(trackpoint, document, ns, node);
        }

        node = node->next;
    }
}

void
parse_trackpoint_extensions_speed(trackpoint_t * trackpoint, xmlDocPtr document, xmlNsPtr ns, xmlNodePtr node)
{
    node = node->xmlChildrenNode;
    while (node != NULL)
    {
        if (!xmlStrcmp(node->name, (const xmlChar *)"Speed"))
        {
            char * end;
            xmlChar * content = xmlNodeListGetString(document, node->xmlChildrenNode, 1);
            trackpoint->speed = strtod((char *)content, (char **)&end);
            xmlFree(content);
        }

        node = node->next;
    }
}

trackpoint_t *
parse_trackpoint(xmlDocPtr document, xmlNsPtr ns, xmlNodePtr node)
{
    trackpoint_t * trackpoint = calloc(1, sizeof(trackpoint_t));

    node = node->xmlChildrenNode;
    while (node != NULL) {
        if ((!xmlStrcmp(node->name, (const xmlChar *)"Time")) && (node->ns == ns))
        {
            xmlChar * content = xmlNodeListGetString(document, node->xmlChildrenNode, 1);
            trackpoint->time = strdup((const char *)content);
            xmlFree(content);
        }

        if ((!xmlStrcmp(node->name, (const xmlChar *)"Position")) && (node->ns == ns))
        {
            parse_trackpoint_coordinates(trackpoint, document, ns, node);
        }

        if ((!xmlStrcmp(node->name, (const xmlChar *)"AltitudeMeters")) && (node->ns == ns))
        {
            char * end;
            xmlChar * content = xmlNodeListGetString(document, node->xmlChildrenNode, 1);
            trackpoint->elevation = strtod((char *)content, (char **)&end);
            xmlFree(content);
        }

        if ((!xmlStrcmp(node->name, (const xmlChar *)"DistanceMeters")) && (node->ns == ns))
        {
            char * end;
            xmlChar * content = xmlNodeListGetString(document, node->xmlChildrenNode, 1);
            trackpoint->distance = strtod((char *)content, (char **)&end);
            xmlFree(content);
        }

        if ((!xmlStrcmp(node->name, (const xmlChar *)"HeartRateBpm")) && (node->ns == ns))
        {
            parse_trackpoint_heart_beat(trackpoint, document, ns, node);
        }

        if ((!xmlStrcmp(node->name, (const xmlChar *)"Cadence")) && (node->ns == ns))
        {
            xmlChar * content = xmlNodeListGetString(document, node->xmlChildrenNode, 1);
            trackpoint->cadence = atoi((char *)content);
            xmlFree(content);
        }

        if ((!xmlStrcmp(node->name, (const xmlChar *)"Extensions")) && (node->ns == ns))
        {
            parse_trackpoint_extensions(trackpoint, document, node);
        }

        node = node->next;
    }

    return trackpoint;
}


void
print_lap(lap_t * lap)
{
    printf("lap                 :\n");
    printf("  start_time        : %s\n", lap->start_time);
    printf("  total_time        : %.2f\n", lap->total_time);
    printf("  distance          : %.2f\n", lap->distance);
    printf("  calories          : %d\n", lap->calories);
    printf("  speed_average     : %.2f\n", lap->speed_average);
    printf("  speed_maximum     : %.2f\n", lap->speed_maximum);
    printf("  heart_rate_average: %d\n", lap->heart_rate_average);
    printf("  heart_rate_maximum: %d\n", lap->heart_rate_maximum);
    printf("  intensity         : %s\n", lap->intensity);
    printf("  cadence_average   : %d\n", lap->cadence_average);
    printf("  cadence_maximum   : %d\n", lap->cadence_maximum);
}

void
print_trackpoint(trackpoint_t * trackpoint)
{
    printf("trackpoint  :\n");
    printf("  time      : %s\n", trackpoint->time);
    printf("  latitude  : %.6f\n", trackpoint->latitude);
    printf("  longitude : %.6f\n", trackpoint->longitude);
    printf("  elevation : %.2f\n", trackpoint->elevation);
    printf("  distance  : %.2f\n", trackpoint->distance);
    printf("  heart_rate: %d\n", trackpoint->heart_rate);
    printf("  cadence   : %d\n", trackpoint->cadence);
    printf("  speed     : %.2f\n", trackpoint->speed);
    printf("  power     : %d\n", trackpoint->power);
}

void
print_tcx(tcx_t * tcx)
{
    lap_t * lap = calloc(1, sizeof(lap_t));
    track_t * track = calloc(1, sizeof(track_t));
    trackpoint_t * trackpoint = calloc(1, sizeof(trackpoint_t));

    lap = tcx->activity->laps;
    while (lap != NULL)
    {
        print_lap(lap);

        track = lap->tracks;
        while (track != NULL)
        {
            trackpoint = track->trackpoints;
            while (trackpoint != NULL)
            {
                print_trackpoint(trackpoint);
                trackpoint = trackpoint->next;
            }

            track = track->next;
        }

        lap = lap->next;
    }

    free(lap);
    free(track);
    free(trackpoint);
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
    xmlXPathObjectPtr activity = xmlXPathEvalExpression((xmlChar *)"//tcx:Activity", context);
    if (activity == NULL || xmlXPathNodeSetIsEmpty(activity->nodesetval))
    {
        printf("No activities found in \"%s\"\n", tcx->filename);
        xmlFreeDoc(document);
        xmlCleanupParser();
        free(tcx);
        return 1;
    }

    tcx->activity = calloc(1, sizeof(activity_t));

    xmlXPathObjectPtr laps = xmlXPathEvalExpression((xmlChar*)"//tcx:Lap", context);
    if (laps != 0 && !xmlXPathNodeSetIsEmpty(laps->nodesetval))
    {
        xmlNodeSetPtr lap_nodes = laps->nodesetval;
        for (int i = 0; i < lap_nodes->nodeNr; i++)
        {
            if (lap_nodes->nodeTab[i]->type != XML_ELEMENT_NODE)
            {
                continue;
            }

            lap_t * lap = parse_lap(document, lap_nodes->nodeTab[i]->ns, lap_nodes->nodeTab[i]);
            add_lap(tcx, lap);

            xmlXPathObjectPtr tracks = xmlXPathEvalExpression((xmlChar*)"//tcx:Track", context);
            if (tracks != 0 && !xmlXPathNodeSetIsEmpty(tracks->nodesetval))
            {
                xmlNodeSetPtr track_nodes = tracks->nodesetval;
                for (int j = 0; j < track_nodes->nodeNr; j++)
                {
                    if (track_nodes->nodeTab[j]->type != XML_ELEMENT_NODE)
                    {
                        continue;
                    }

                    track_t * track = calloc(1, sizeof(track_t));
                    add_track(tcx, track);

                    xmlXPathObjectPtr trackpoints = xmlXPathEvalExpression((xmlChar *)"//tcx:Trackpoint", context);
                    if (trackpoints != 0 && !xmlXPathNodeSetIsEmpty(trackpoints->nodesetval))
                    {
                        xmlNodeSetPtr trackpoint_nodes = trackpoints->nodesetval;
                        for (int k = 0; k < trackpoint_nodes->nodeNr; k++)
                        {
                            if (trackpoint_nodes->nodeTab[k]->type != XML_ELEMENT_NODE)
                            {
                                continue;
                            }

                            trackpoint_t * trackpoint = parse_trackpoint(document, trackpoint_nodes->nodeTab[k]->ns, trackpoint_nodes->nodeTab[k]);
                            add_trackpoint(tcx, trackpoint);
                        }
                    }

                    if (trackpoints != NULL)
                    {
                        xmlXPathFreeObject(trackpoints);
                    }
                }

                if (tracks != NULL)
                {
                    xmlXPathFreeObject(tracks);
                }
            }
        }
    }

    if (laps != NULL)
    {
        xmlXPathFreeObject(laps);
    }

    if (activity != NULL)
    {
        xmlXPathFreeObject(activity);
    }

    xmlXPathFreeContext(context);
    xmlFreeDoc(document);
    xmlCleanupParser();

    free(tcx->activity->laps->tracks->trackpoints);
    free(tcx->activity->laps->tracks);
    free(tcx->activity->laps);
    free(tcx->activity);
    free(tcx->filename);
    free(tcx);

    return 0;
}
