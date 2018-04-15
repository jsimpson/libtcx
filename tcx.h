#ifndef TCX_H_
#define TCX_H_

#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

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
    double grade;

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
    double grade_adjusted_time;
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
    activity_t * activities;
} tcx_t;

void add_activity(tcx_t * tcx, activity_t * activity);
void add_lap(lap_t * lap);
void add_track(track_t * track);
void add_trackpoint(trackpoint_t * trackpoint);

int xml_content_to_i(xmlDocPtr document, xmlNodePtr node);
double xml_content_to_d(xmlDocPtr document, xmlNodePtr node);

lap_t * parse_lap(xmlDocPtr document, xmlNsPtr ns, xmlNodePtr node);

void parse_trackpoint_coordinates(trackpoint_t * trackpoint, xmlDocPtr document, xmlNsPtr ns, xmlNodePtr node);
void parse_trackpoint_heart_beat(trackpoint_t * trackpoint, xmlDocPtr document, xmlNsPtr ns, xmlNodePtr node);
void parse_trackpoint_extensions(trackpoint_t * trackpoint, xmlDocPtr document, xmlNodePtr node);
void parse_trackpoint_extensions_power_and_speed(trackpoint_t * trackpoint, xmlDocPtr document, xmlNodePtr node);
trackpoint_t * parse_trackpoint(xmlDocPtr document, xmlNsPtr ns, xmlNodePtr node);

int parse_tcx_file(tcx_t * tcx, char * filename);

double interval_distance(trackpoint_t * previous_trackpoint, trackpoint_t * trackpoint);
double haversine_distance(coordinates_t * start, coordinates_t * end);

void calculate_grade(trackpoint_t * previous_trackpoint, trackpoint_t * trackpoint_t);
void calculate_grade_adjusted_time(lap_t * lap);
void calculate_elevation_delta(lap_t * lap, trackpoint_t * previous_trackpoint, trackpoint_t * trackpoint);
void calculate_summary_activity(activity_t * activity, lap_t * lap);
void calculate_summary_lap(activity_t * activity, lap_t * lap, trackpoint_t * trackpoint);
void calculate_summary(tcx_t * tcx);

void print_activity(activity_t * activity);
void print_lap(lap_t * lap);
void print_track(track_t * track_t);
void print_trackpoint(trackpoint_t * trackpoint_t);
void print_tcx(tcx_t * tcx);

#endif /* !TCX_H_ */
