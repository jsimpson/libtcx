#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include "tcx.h"

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
