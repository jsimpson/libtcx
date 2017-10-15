#ifndef SUMMARY_H_
#define SUMMARY_H_

void calculate_elevation_delta(lap_t * lap, trackpoint_t * previous_trackpoint, trackpoint_t * trackpoint);
void calculate_summary_activity(activity_t * activity, lap_t * lap);
void calculate_summary_lap(activity_t * activity, lap_t * lap, trackpoint_t * trackpoint);
void calculate_summary(tcx_t * tcx);

#endif /* SUMMARY_H_ */
