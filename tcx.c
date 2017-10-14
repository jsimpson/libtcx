#include <stdio.h>

#include "tcx.h"

activity_t * current_activity = NULL;
lap_t * current_lap = NULL;
track_t * current_track = NULL;
trackpoint_t * current_trackpoint = NULL;

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
