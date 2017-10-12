typedef struct
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
    trackpoint_t * trackpoints;
    struct track * next;
} track_t;

typedef struct lap
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

