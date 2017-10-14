#include <stdio.h>
#include <stdlib.h>

#include "tcx.h"

void
print_activity(activity_t * activity)
{
    printf("activity\n");
    printf("  started_at           : %s\n", activity->started_at);
    printf("  ended_at             : %s\n", activity->ended_at);
    printf("  total_time           : %s\n", activity->total_time);
    printf("  latitude             : %.6f\n", activity->latitude);
    printf("  longitude            : %.6f\n", activity->longitude);
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
    printf("  num_tracks        : %d\n", lap->num_tracks);
    printf("  start_time        : %s\n", lap->start_time);
    printf("  total_time        : %.2f\n", lap->total_time);
    printf("  distance          : %.2f\n", lap->distance);
    printf("  calories          : %d\n", lap->calories);
    printf("  speed_average     : %.2f\n", lap->speed_average);
    printf("  speed_maximum     : %.2f\n", lap->speed_maximum);
    printf("  heart_rate_average: %d\n", lap->heart_rate_average);
    printf("  heart_rate_maximum: %d\n", lap->heart_rate_maximum);
    printf("  heart_rate_minimum: %d\n", lap->heart_rate_minimum);
    printf("  intensity         : %s\n", lap->intensity);
    printf("  cadence_average   : %d\n", lap->cadence_average);
    printf("  cadence_maximum   : %d\n", lap->cadence_maximum);
    printf("  cadence_minimum   : %d\n", lap->cadence_minimum);
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
