#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

int parse_element_to_i(char * element, xmlXPathContextPtr context);
double parse_element_to_f(char * element, xmlXPathContextPtr context);
char * parse_element_to_s(char * element, xmlXPathContextPtr context);

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
    struct track * next;
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
parse_element_to_i(char * element, xmlXPathContextPtr context)
{
    int value = 0;

    xmlXPathObjectPtr expr = xmlXPathEvalExpression((xmlChar *)element, context);
    if (expr != NULL)
    {
        xmlChar * content = expr->nodesetval->nodeTab[0]->content;
        value = atoi((char *)content);
        xmlXPathFreeObject(expr);
    }

    return value;
}

double
parse_element_to_f(char * element, xmlXPathContextPtr context)
{
    double value = 0.0;

    xmlXPathObjectPtr expr = xmlXPathEvalExpression((xmlChar *)element, context);
    if (expr != NULL)
    {
        char * end = NULL;
        xmlChar * content = expr->nodesetval->nodeTab[0]->content;
        value = strtod((char *)content, (char **)&end);
        xmlXPathFreeObject(expr);
    }

    return value;
}

char *
parse_element_to_s(char * element, xmlXPathContextPtr context)
{
    char * value = 0;

    xmlXPathObjectPtr expr = xmlXPathEvalExpression((xmlChar *)element, context);
    if (expr != NULL)
    {
        xmlChar * content = expr->nodesetval->nodeTab[0]->content;
        value = (char *)content;
        xmlXPathFreeObject(expr);
    }

    return value;
}

int
main(int argc, char * argv[])
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
    xmlNodeSetPtr lap_nodes = activity->nodesetval;
    tcx->activity->laps = calloc(lap_nodes->nodeNr, sizeof(lap_t));

    for (int i = 0; i < lap_nodes->nodeNr; i++)
    {
        if (lap_nodes->nodeTab[i]->type != XML_ELEMENT_NODE)
        {
            continue;
        }

        tcx->activity->laps[i].total_time = parse_element_to_f("//tcx:TotalTimeSeconds/text()", context);
        tcx->activity->laps[i].distance = parse_element_to_f("//tcx:DistanceMeters/text()", context);
        tcx->activity->laps[i].calories = parse_element_to_i("//tcx:Calories/text()", context);
        tcx->activity->laps[i].heart_rate_average = parse_element_to_i("//tcx:AverageHeartRateBpm/tcx:Value/text()", context);
        tcx->activity->laps[i].heart_rate_maximum = parse_element_to_i("//tcx:MaximumHeartRateBpm/tcx:Value/text()", context);
        tcx->activity->laps[i].intensity = parse_element_to_s("//tcx:Intensity/text()", context);
    }

    xmlXPathFreeObject(activity);
    xmlXPathFreeContext(context);
    xmlFreeDoc(document);
    xmlCleanupParser();

    free(tcx->activity->laps);
    free(tcx->activity);
    free(tcx->filename);
    free(tcx);

    return 0;
}
