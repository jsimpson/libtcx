#ifndef PARSING_UTILS_H_
#define PARSING_UTILS_H_

#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

int xml_content_to_i(xmlDocPtr document, xmlNodePtr node);
double xml_content_to_d(xmlDocPtr document, xmlNodePtr node);

lap_t * parse_lap(xmlDocPtr document, xmlNsPtr ns, xmlNodePtr node);

void parse_trackpoint_coordinates(trackpoint_t * trackpoint, xmlDocPtr document, xmlNsPtr ns, xmlNodePtr node);
void parse_trackpoint_heart_beat(trackpoint_t * trackpoint, xmlDocPtr document, xmlNsPtr ns, xmlNodePtr node);
void parse_trackpoint_extensions(trackpoint_t * trackpoint, xmlDocPtr document, xmlNodePtr node);
void parse_trackpoint_extensions_power_and_speed(trackpoint_t * trackpoint, xmlDocPtr document, xmlNodePtr node);
trackpoint_t * parse_trackpoint(xmlDocPtr document, xmlNsPtr ns, xmlNodePtr node);

int parse_tcx_file(tcx_t * tcx);

#endif /* PARSING_UTILS_H_ */
