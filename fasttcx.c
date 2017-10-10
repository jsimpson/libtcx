#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

typedef struct
{
    char * time;
    double latitude;
    double longitude;
    double elevation;
    double distance;
    int heart_rate;
    int cadence;
    double speed;
    double power;

    struct trackpoint * next;
} trackpoint_t;

typedef struct
{
    trackpoint_t * trackpoints;
    struct tract * next;
} track_t;

typedef struct
{
    char * start_time;
    double total_time;
    double distance;
    int calories;
    double speed_average;
    double speed_maximum;
    int heart_rate_average;
    int heart_rate_maximum;
    char * intensity;
    int cadence_average;
    int cadence_maximum;

    track_t * tracks;

    struct lap * next;
} lap_t;

typedef struct
{
    char * started_at;
    char * ended_at;
    char * total_time;
    double latitude;
    double longitude;
    int total_calories;
    double total_distance;
    double total_elevation_gain;
    double total_elevation_loss;
    double speed_average;
    double speed_maximum;
    double speed_minimum;
    double elevation_maximum;
    double elevation_minimum;
    int candence_average;
    int cadence_maximum;
    int cadence_minimum;
    int heart_rate_average;
    int heart_rate_minimum;
    int heart_rate_maximum;

    lap_t * laps;
} activity_t;

typedef struct
{
    char * filename;

    activity_t * activity;
} tcx_t;

int
main(int argc, char * argv[])
{
    tcx_t * tcx = calloc(1, sizeof(tcx_t));

    if (argc < 2)
    {
        fprintf(stderr, "Please supply a .TCX file.\n");
        return 1;
    }
    else
    {
        tcx->filename = strdup(argv[1]);
    }

    if (access(tcx->filename, F_OK) != 0)
    {
        fprintf(stderr, "Unable to locate %s.\n", tcx->filename);
        return 1;
    }

    xmlDocPtr doc = xmlReadFile(tcx->filename, NULL, 0);
    if (doc == NULL)
    {
        fprintf(stderr, "Could not parse %s.\n", tcx->filename);
        return 1;
    }

    xmlXPathContextPtr context = xmlXPathNewContext(doc);
    xmlXPathRegisterNs(context, (xmlChar*)"tcx", (xmlChar *)"http://www.garmin.com/xmlschemas/TrainingCenterDatabase/v2");

    xmlXPathObjectPtr activity = xmlXPathEvalExpression((xmlChar *)"//tcx:Activity", context);

    if (activity == NULL || xmlXPathNodeSetIsEmpty(activity->nodesetval)){
        printf("No activities found in \"%s\"\n", tcx->filename);
        return 1;
    }

    xmlNodeSetPtr lap_nodes = activity->nodesetval;

    xmlFreeDoc(doc);
    xmlCleanupParser();
    free(tcx);

    return 0;
}
