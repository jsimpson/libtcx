#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include "tcx.h"
#include "parsing_utils.h"
#include "printing.h"

void calculate_elevation_delta(lap_t * lap, trackpoint_t * previous_trackpoint, trackpoint_t * trackpoint);
void calculate_summary_activity(activity_t * activity, lap_t * lap);
void calculate_summary_lap(activity_t * activity, lap_t * lap, trackpoint_t * trackpoint);
void calculate_summary(tcx_t * tcx);

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
    activity->end_point->latitude = trackpoint->latitude;
    activity->end_point->longitude = trackpoint->longitude;
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
        activity->elevation_maximum = INT_MIN * 1.0;
        activity->elevation_minimum = INT_MAX * 1.0;
        activity->heart_rate_maximum = INT_MIN;
        activity->heart_rate_minimum = INT_MAX;
        activity->speed_maximum = INT_MIN * 1.0;
        activity->speed_minimum = INT_MAX * 1.0;

        lap = activity->laps;
        while (lap != NULL)
        {
            lap->cadence_maximum = INT_MIN;
            lap->cadence_minimum = INT_MAX;
            lap->elevation_maximum = INT_MIN * 1.0;
            lap->elevation_minimum = INT_MAX * 1.0;
            lap->heart_rate_maximum = INT_MIN;
            lap->heart_rate_minimum = INT_MAX;
            lap->speed_maximum = INT_MIN * 1.0;
            lap->speed_minimum = INT_MAX * 1.0;

            track = lap->tracks;
            while (track != NULL)
            {
                trackpoint = track->trackpoints;
                while (trackpoint != NULL)
                {
                    calculate_summary_lap(activity, lap, trackpoint);

                    if (previous_trackpoint != NULL)
                    {
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
                if (lap->speed_maximum == INT_MIN) lap->speed_maximum = 0.0;
                if (lap->speed_minimum == INT_MAX) lap->speed_minimum = 0.0;

                track = track->next;
            }

            if (lap->cadence_maximum == INT_MIN) lap->cadence_maximum = 0;
            if (lap->cadence_minimum == INT_MAX) lap->cadence_minimum = 0;
            if (lap->heart_rate_maximum == INT_MIN) lap->heart_rate_maximum = 0;
            if (lap->heart_rate_minimum == INT_MAX) lap->heart_rate_minimum = 0;
            if (lap->speed_maximum == INT_MIN) lap->speed_maximum = 0.0;
            if (lap->speed_minimum == INT_MAX) lap->speed_minimum = 0.0;

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
        if (activity->speed_maximum == INT_MIN) activity->speed_maximum = 0.0;
        if (activity->speed_minimum == INT_MAX) activity->speed_minimum = 0.0;
        if (activity->elevation_maximum == INT_MIN) activity->elevation_maximum = 0.0;
        if (activity->elevation_minimum == INT_MAX) activity-> elevation_minimum = 0.0;

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
