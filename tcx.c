#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include "tcx.h"

static activity_t * current_activity = NULL;
static lap_t * current_lap = NULL;
static track_t * current_track = NULL;
static trackpoint_t * current_trackpoint = NULL;

void
add_activity(tcx_t * tcx, activity_t * activity)
{
    if (current_activity != NULL)
    {
        current_activity->next = activity;
    }
    else
    {
        tcx->activities = activity;
    }

    current_activity = activity;
    current_lap = NULL;
    current_track = NULL;
    current_trackpoint = NULL;
}

void
add_lap(lap_t * lap)
{
    if (current_lap != NULL)
    {
        current_lap->next = lap;
    }
    else
    {
        current_activity->laps = lap;
    }

    current_lap = lap;
    current_track = NULL;
    current_trackpoint = NULL;
}

void
add_track(track_t * track)
{
    current_lap->num_tracks++;

    if (current_track != NULL)
    {
        current_track->next = track;
    }
    else
    {
        current_lap->tracks = track;
    }

    current_track = track;
    current_trackpoint = NULL;
}

void
add_trackpoint(trackpoint_t * trackpoint)
{
    current_activity->num_trackpoints++;
    current_lap->num_trackpoints++;
    current_track->num_trackpoints++;

    if (current_trackpoint != NULL)
    {
        current_trackpoint->next = trackpoint;
    }
    else
    {
        current_track->trackpoints = trackpoint;
    }

    current_trackpoint = trackpoint;
}

int
xml_content_to_i(xmlDocPtr document, xmlNodePtr node)
{
    xmlChar * content = xmlNodeListGetString(document, node->xmlChildrenNode, 1);
    int ret = atoi((char *)content);
    xmlFree(content);
    return ret;
}

double
xml_content_to_d(xmlDocPtr document, xmlNodePtr node)
{
    char * end = NULL;

    xmlChar * content = xmlNodeListGetString(document, node->xmlChildrenNode, 1);
    double ret = strtod((char *)content, (char **)&end);
    xmlFree(content);
    return ret;
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
            lap->total_time = xml_content_to_d(document, node);
        }

        if ((!xmlStrcmp(node->name, (const xmlChar *)"DistanceMeters")) && (node->ns == ns))
        {
            lap->distance = xml_content_to_d(document, node);
        }

        if ((!xmlStrcmp(node->name, (const xmlChar *)"Calories")) && (node->ns == ns))
        {
            lap->calories = xml_content_to_i(document, node);
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
            trackpoint->latitude = xml_content_to_d(document, node);
        }

        if ((!xmlStrcmp(node->name, (const xmlChar *)"LongitudeDegrees")) && (node->ns == ns))
        {
            trackpoint->longitude = xml_content_to_d(document, node);
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
            trackpoint->heart_rate = xml_content_to_i(document, node);
        }

        node = node->next;
    }
}

void
parse_trackpoint_extensions(trackpoint_t * trackpoint, xmlDocPtr document, xmlNodePtr node)
{
    node = node->xmlChildrenNode;
    while (node != NULL)
    {
        if (!xmlStrcmp(node->name, (const xmlChar *)"TPX"))
        {
            parse_trackpoint_extensions_power_and_speed(trackpoint, document, node);
        }

        node = node->next;
    }
}

void
parse_trackpoint_extensions_power_and_speed(trackpoint_t * trackpoint, xmlDocPtr document, xmlNodePtr node)
{
    node = node->xmlChildrenNode;
    while (node != NULL)
    {
        if (!xmlStrcmp(node->name, (const xmlChar *)"Speed"))
        {
            trackpoint->speed = xml_content_to_d(document, node);
        }

        if (!xmlStrcmp(node->name, (const xmlChar *)"Watts"))
        {
            trackpoint->power = xml_content_to_i(document, node);
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
            trackpoint->elevation = xml_content_to_d(document, node);
        }

        if ((!xmlStrcmp(node->name, (const xmlChar *)"DistanceMeters")) && (node->ns == ns))
        {
            trackpoint->distance = xml_content_to_d(document, node);
        }

        if ((!xmlStrcmp(node->name, (const xmlChar *)"HeartRateBpm")) && (node->ns == ns))
        {
            parse_trackpoint_heart_beat(trackpoint, document, ns, node);
        }

        if ((!xmlStrcmp(node->name, (const xmlChar *)"Cadence")) && (node->ns == ns))
        {
            trackpoint->cadence = xml_content_to_i(document, node);
        }

        if ((!xmlStrcmp(node->name, (const xmlChar *)"Extensions")) && (node->ns == ns))
        {
            parse_trackpoint_extensions(trackpoint, document, node);
        }

        node = node->next;
    }

    return trackpoint;
}

int
parse_tcx_file(tcx_t * tcx, char * filename)
{
    xmlInitParser();

    xmlDocPtr document = xmlReadFile(filename, NULL, 0);
    if (document == NULL)
    {
        fprintf(stderr, "Could not parse %s.\n", filename);
        return 1;
    }

    xmlXPathContextPtr context = xmlXPathNewContext(document);
    xmlXPathRegisterNs(context, (xmlChar *)"tcx", (xmlChar *)"http://www.garmin.com/xmlschemas/TrainingCenterDatabase/v2");

    xmlXPathObjectPtr activities = xmlXPathEvalExpression((xmlChar *)"//tcx:Activity", context);
    if (activities == NULL || activities == 0 || xmlXPathNodeSetIsEmpty(activities->nodesetval))
    {
        fprintf(stderr, "No activities found in \"%s\"\n", filename);
        xmlXPathFreeContext(context);
        xmlFreeDoc(document);
        xmlCleanupParser();
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

    return 0;
}

double
interval_distance(trackpoint_t * previous_trackpoint, trackpoint_t * trackpoint)
{
    double distance = 0.00;

    if ((fabs(trackpoint->distance) > 10e-7) && ((fabs(previous_trackpoint->distance) > 10e-7)))
    {
        distance = trackpoint->distance - previous_trackpoint->distance;
    }
    else
    {
        coordinates_t * start = calloc(1, sizeof(coordinates_t));
        coordinates_t * end = calloc(1, sizeof(coordinates_t));

        start->latitude = previous_trackpoint->latitude;
        start->longitude = previous_trackpoint->longitude;
        end->latitude = trackpoint->latitude;
        end->longitude = trackpoint->longitude;

        distance = haversine_distance(start, end);

        free(start);
        free(end);
    }

    return distance;
}

#define RAD_PER_DEGREE M_PI / 180.0
#define RADIUS 6371000 // Earth's mean radius in meters

double
haversine_distance(coordinates_t * start, coordinates_t * end)
{
    double delta_latitude = end->latitude - start->latitude;
    double delta_longitude = end->longitude - start->longitude;
    double delta_latitude_rad = delta_latitude * RAD_PER_DEGREE;
    double delta_longitude_rad = delta_longitude * RAD_PER_DEGREE;
    double start_latitude_rad = start->latitude * RAD_PER_DEGREE;
    double end_latitude_rad = end->latitude * RAD_PER_DEGREE;

    double a = pow(sin(delta_latitude_rad / 2), 2) + cos(start_latitude_rad) * cos(end_latitude_rad) * pow(sin(delta_longitude_rad / 2), 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));

    return RADIUS * c;
}

void
calculate_grade(trackpoint_t * previous_trackpoint, trackpoint_t * trackpoint)
{
    double elevation = trackpoint->elevation - previous_trackpoint->elevation;
    double distance = interval_distance(previous_trackpoint, trackpoint);
    double radians = atan(elevation / distance);
    trackpoint->grade = 180 * radians / M_PI;
}

/*
 * Based on research by C.T.M. Davies:
 * https://medium.com/strava-engineering/improving-grade-adjusted-pace-b9a2a332a5dc
 * https://www.runnersworld.com/races/downhill-all-the-way
 */
#define COEFF_INCLINE 0.033 * 2
#define COEFF_DECLINE 0.01815 * 2
#define METERS_TO_FEET 3.28084

void
calculate_grade_adjusted_time(lap_t * lap)
{
    double seconds = 0.00;
    seconds += ((lap->total_elevation_gain * METERS_TO_FEET) / 100.00) * COEFF_INCLINE * lap->total_time;
    seconds -= ((lap->total_elevation_loss * METERS_TO_FEET) / 100.00) * COEFF_DECLINE * lap->total_time;
    lap->grade_adjusted_time = lap->total_time + floor(seconds);
}

void
calculate_elevation_delta(lap_t * lap, trackpoint_t * previous_trackpoint, trackpoint_t * trackpoint)
{
    double delta = trackpoint->elevation - previous_trackpoint->elevation;
    lap->total_elevation_gain += delta > 0.0 ? delta : 0.0;
    lap->total_elevation_loss -= delta < 0.0 ? delta : 0.0;
}

void
calculate_summary_activity(activity_t * activity, lap_t * lap)
{
    if (activity->started_at == NULL)
    {
        activity->started_at = lap->start_time;
    }

    if (lap->cadence_maximum > activity->cadence_maximum)
    {
        activity->cadence_maximum = lap->cadence_maximum;
    }

    if (lap->cadence_minimum < activity->cadence_minimum)
    {
        activity->cadence_minimum = lap->cadence_minimum;
    }

    if (lap->elevation_maximum > activity->elevation_maximum)
    {
        activity->elevation_maximum = lap->elevation_maximum;
    }

    if (lap->elevation_minimum < activity->elevation_minimum)
    {
        activity->elevation_minimum = lap->elevation_minimum;
    }

    if (lap->heart_rate_maximum > activity->heart_rate_maximum)
    {
        activity->heart_rate_maximum = lap->heart_rate_maximum;
    }

    if (lap->heart_rate_minimum < activity->heart_rate_minimum)
    {
        activity->heart_rate_minimum = lap->heart_rate_minimum;
    }

    if (lap->speed_maximum > activity->speed_maximum)
    {
        activity->speed_maximum = lap->speed_maximum;
    }

    if (lap->speed_minimum < activity->speed_minimum)
    {
        activity->speed_minimum = lap->speed_minimum;
    }

    activity->total_calories += lap->calories;
    activity->total_distance += lap->distance;
    activity->total_elevation_gain += lap->total_elevation_gain;
    activity->total_elevation_loss += lap->total_elevation_loss;
    activity->total_time += lap->total_time;
}

void
calculate_summary_lap(activity_t * activity, lap_t * lap, trackpoint_t * trackpoint)
{
    if (activity->start_point == NULL)
    {
        activity->start_point = calloc(1, sizeof(coordinates_t));
        activity->start_point->latitude = trackpoint->latitude;
        activity->start_point->longitude = trackpoint->longitude;
    }

    if (activity->end_point == NULL)
    {
        activity->end_point = calloc(1, sizeof(coordinates_t));
    }
    else
    {
        if (trackpoint->latitude != 0.0 && trackpoint->longitude != 0.0)
        {
            activity->end_point->latitude = trackpoint->latitude;
            activity->end_point->longitude = trackpoint->longitude;
        }
    }

    if (trackpoint->cadence > lap->cadence_maximum)
    {
        lap->cadence_maximum = trackpoint->cadence;
    }

    if (trackpoint->cadence < lap->cadence_minimum)
    {
        lap->cadence_minimum = trackpoint->cadence;
    }

    if (trackpoint->elevation > lap->elevation_maximum)
    {
        lap->elevation_maximum = trackpoint->elevation;
    }

    if (trackpoint->elevation < lap->elevation_minimum)
    {
        lap->elevation_minimum = trackpoint->elevation;
    }

    if (trackpoint->heart_rate > lap->heart_rate_maximum)
    {
        lap->heart_rate_maximum = trackpoint->heart_rate;
    }

    if (trackpoint->heart_rate != 0 && trackpoint->heart_rate < lap->heart_rate_minimum)
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

    activity->cadence_average += trackpoint->cadence;
    activity->ended_at = trackpoint->time;
    activity->heart_rate_average += trackpoint->heart_rate;
    activity->speed_average += trackpoint->speed;
}

void
calculate_summary(tcx_t * tcx)
{
    activity_t * activity = NULL;
    lap_t * lap = NULL;
    track_t * track = NULL;
    trackpoint_t * trackpoint = NULL;
    trackpoint_t * previous_trackpoint = NULL;

    activity = tcx->activities;
    while (activity != NULL)
    {
        activity->cadence_maximum = INT_MIN;
        activity->cadence_minimum = INT_MAX;
        activity->elevation_maximum = DBL_MIN;
        activity->elevation_minimum = DBL_MAX;
        activity->heart_rate_maximum = INT_MIN;
        activity->heart_rate_minimum = INT_MAX;
        activity->speed_maximum = DBL_MIN;
        activity->speed_minimum = DBL_MAX;

        lap = activity->laps;
        while (lap != NULL)
        {
            lap->cadence_maximum = INT_MIN;
            lap->cadence_minimum = INT_MAX;
            lap->elevation_maximum = DBL_MIN;
            lap->elevation_minimum = DBL_MAX;
            lap->heart_rate_maximum = INT_MIN;
            lap->heart_rate_minimum = INT_MAX;
            lap->speed_maximum = DBL_MIN;
            lap->speed_minimum = DBL_MAX;

            track = lap->tracks;
            while (track != NULL)
            {
                trackpoint = track->trackpoints;
                while (trackpoint != NULL)
                {
                    calculate_summary_lap(activity, lap, trackpoint);

                    if (previous_trackpoint != NULL)
                    {
                        calculate_grade(previous_trackpoint, trackpoint);
                        calculate_elevation_delta(lap, previous_trackpoint, trackpoint);
                    }

                    previous_trackpoint = trackpoint;
                    trackpoint = trackpoint->next;
                }

                lap->cadence_average /= lap->num_trackpoints;
                lap->heart_rate_average /= lap->num_trackpoints;
                lap->speed_average /= lap->num_trackpoints;

                if (lap->cadence_maximum == INT_MIN) lap->cadence_maximum = 0;
                if (lap->cadence_minimum == INT_MAX) lap->cadence_minimum = 0;
                if (lap->heart_rate_maximum == INT_MIN) lap->heart_rate_maximum = 0;
                if (lap->heart_rate_minimum == INT_MAX) lap->heart_rate_minimum = 0;
                if (lap->speed_maximum == DBL_MIN) lap->speed_maximum = 0.0;
                if (lap->speed_minimum == DBL_MAX) lap->speed_minimum = 0.0;

                track = track->next;
            }

            calculate_grade_adjusted_time(lap);

            if (lap->cadence_maximum == INT_MIN) lap->cadence_maximum = 0;
            if (lap->cadence_minimum == INT_MAX) lap->cadence_minimum = 0;
            if (lap->heart_rate_maximum == INT_MIN) lap->heart_rate_maximum = 0;
            if (lap->heart_rate_minimum == INT_MAX) lap->heart_rate_minimum = 0;
            if (lap->speed_maximum == DBL_MIN) lap->speed_maximum = 0.0;
            if (lap->speed_minimum == DBL_MAX) lap->speed_minimum = 0.0;

            calculate_summary_activity(activity, lap);

            lap = lap->next;
        }

        activity->cadence_average /= activity->num_trackpoints;
        activity->heart_rate_average /= activity->num_trackpoints;
        activity->speed_average /= activity->num_trackpoints;

        if (activity->cadence_maximum == INT_MIN) activity->cadence_maximum = 0;
        if (activity->cadence_minimum == INT_MAX) activity->cadence_minimum = 0;
        if (activity->heart_rate_maximum == INT_MIN) activity->heart_rate_maximum = 0;
        if (activity->heart_rate_minimum == INT_MAX) activity->heart_rate_minimum = 0;
        if (activity->speed_maximum == DBL_MIN) activity->speed_maximum = 0.0;
        if (activity->speed_minimum == DBL_MAX) activity->speed_minimum = 0.0;
        if (activity->elevation_maximum == DBL_MIN) activity->elevation_maximum = 0.0;
        if (activity->elevation_minimum == DBL_MAX) activity-> elevation_minimum = 0.0;

        activity = activity->next;
    }

    free(trackpoint);
    free(track);
    free(lap);
    free(activity);
}

void
print_activity(activity_t * activity)
{
    printf("activity\n");
    printf("  started_at           : %s\n", activity->started_at);
    printf("  ended_at             : %s\n", activity->ended_at);
    printf("  total_time           : %.2f\n", activity->total_time);
    printf("  starting latitude    : %.6f\n", activity->start_point->latitude);
    printf("  starting longitude   : %.6f\n", activity->start_point->longitude);
    printf("  ending latitude      : %.6f\n", activity->end_point->latitude);
    printf("  ending longitude     : %.6f\n", activity->end_point->longitude);
    printf("  total_calories       : %d\n", activity->total_calories);
    printf("  total_distance       : %.2f\n", activity->total_distance);
    printf("  total_elevation_gain : %.2f\n", activity->total_elevation_gain);
    printf("  total_elevation_loss : %.2f\n", activity->total_elevation_loss);
    printf("  speed_average        : %.2f\n", activity->speed_average);
    printf("  speed_maximum        : %.2f\n", activity->speed_maximum);
    printf("  speed_minimum        : %.2f\n", activity->speed_minimum);
    printf("  elevation_maximum    : %.2f\n", activity->elevation_maximum);
    printf("  elevation_minimum    : %.2f\n", activity->elevation_minimum);
    printf("  cadence_average      : %d\n", activity->cadence_average);
    printf("  cadence_maximum      : %d\n", activity->cadence_maximum);
    printf("  cadence_minimum      : %d\n", activity->cadence_minimum);
    printf("  heart_rate_average   : %d\n", activity->heart_rate_average);
    printf("  heart_rate_minimum   : %d\n", activity->heart_rate_minimum);
    printf("  heart_rate_maximum   : %d\n", activity->heart_rate_maximum);
}

void
print_lap(lap_t * lap)
{
    printf("lap\n");
    printf("  num_tracks         : %d\n", lap->num_tracks);
    printf("  start_time         : %s\n", lap->start_time);
    printf("  total_time         : %.2f\n", lap->total_time);
    printf("  distance           : %.2f\n", lap->distance);
    printf("  calories           : %d\n", lap->calories);
    printf("  speed_average      : %.2f\n", lap->speed_average);
    printf("  speed_maximum      : %.2f\n", lap->speed_maximum);
    printf("  heart_rate_average : %d\n", lap->heart_rate_average);
    printf("  heart_rate_maximum : %d\n", lap->heart_rate_maximum);
    printf("  heart_rate_minimum : %d\n", lap->heart_rate_minimum);
    printf("  intensity          : %s\n", lap->intensity);
    printf("  cadence_average    : %d\n", lap->cadence_average);
    printf("  cadence_maximum    : %d\n", lap->cadence_maximum);
    printf("  cadence_minimum    : %d\n", lap->cadence_minimum);
    printf("  elevation          : %.2f\n", lap->total_elevation_gain - lap->total_elevation_loss);
    printf("  grade_adjusted_time: %.2f\n", lap->grade_adjusted_time);
}

void
print_track(track_t * track)
{
    printf("track\n");
    printf("  num_trackpoints: %d\n", track->num_trackpoints);
}

void
print_trackpoint(trackpoint_t * trackpoint)
{
    printf("trackpoint\n");
    printf("  time      : %s\n", trackpoint->time);
    printf("  latitude  : %.6f\n", trackpoint->latitude);
    printf("  longitude : %.6f\n", trackpoint->longitude);
    printf("  elevation : %.2f\n", trackpoint->elevation);
    printf("  distance  : %.2f\n", trackpoint->distance);
    printf("  heart_rate: %d\n", trackpoint->heart_rate);
    printf("  cadence   : %d\n", trackpoint->cadence);
    printf("  speed     : %.2f\n", trackpoint->speed);
    printf("  power     : %d\n", trackpoint->power);
    printf("  grade     : %.2f\n", trackpoint->grade);
}

void
print_tcx(tcx_t * tcx)
{
    activity_t * activity = NULL;
    lap_t * lap = NULL;
    track_t * track = NULL;
    trackpoint_t * trackpoint = NULL;

    activity = tcx->activities;
    while (activity != NULL)
    {
        print_activity(activity);

        lap = activity->laps;
        while (lap != NULL)
        {
            print_lap(lap);

            track = lap->tracks;
            while (track != NULL)
            {
                print_track(track);

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

        activity = activity->next;
    }

    free(trackpoint);
    free(track);
    free(lap);
    free(activity);
}
