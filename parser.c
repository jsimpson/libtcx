#include <string.h>
#include <libxml/tree.h>

#include "tcx.h"
#include "parser.h"

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
