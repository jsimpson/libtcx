#ifndef TCX_H_
#define TCX_H_

typedef struct coordinates
{
    double latitude;
    double longitude;
} coordinates_t;

typedef struct trackpoint
{
    char * time;
    double latitude;
    double longitude;
    double elevation;
    double distance;
    int heart_rate;
    int cadence;
    double speed;
    int power;

    struct trackpoint * next;
} trackpoint_t;

typedef struct track
{
    int num_trackpoints;
    trackpoint_t * trackpoints;
    struct track * next;
} track_t;

typedef struct lap
{
    int num_tracks;
    int num_trackpoints;
    char * start_time;
    double total_time;
    double distance;
    int calories;
    double speed_average;
    double speed_maximum;
    double speed_minimum;
    int heart_rate_average;
    int heart_rate_maximum;
    int heart_rate_minimum;
    char * intensity;
    int cadence_average;
    int cadence_maximum;
    int cadence_minimum;
    double elevation_maximum;
    double elevation_minimum;
    double total_elevation_gain;
    double total_elevation_loss;
    track_t * tracks;
    struct lap * next;
} lap_t;

typedef struct activity
{
    int num_trackpoints;
    int num_laps;
    char * started_at;
    char * ended_at;
    double total_time;
    struct coordinates * start_point;
    struct coordinates * end_point;
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
    int cadence_average;
    int cadence_maximum;
    int cadence_minimum;
    int heart_rate_average;
    int heart_rate_minimum;
    int heart_rate_maximum;
    lap_t * laps;
    struct activity * next;
} activity_t;

typedef struct
{
    char * filename;
    activity_t * activities;
} tcx_t;

void add_activity(tcx_t * tcx, activity_t * activity);
void add_lap(lap_t * lap);
void add_track(track_t * track);
void add_trackpoint(trackpoint_t * trackpoint);

#endif /* TCX_H_ */
