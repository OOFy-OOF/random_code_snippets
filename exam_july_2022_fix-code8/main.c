#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct transportStop TransportStop;

typedef enum stopType { ALEPA_BIKE, BUS, METRO, TRAM } StopType;

struct transportStop {
	double location[2];	 
	char identifier[6];	
	StopType type;		 
	char* name;			
	TransportStop* next; 
};


typedef struct map {
	TransportStop* stops;
	char* locationName;  
} Map;



TransportStop* createStop(const char* name, const char* identifier, StopType type, double location[2]) {
    TransportStop* stop = malloc(sizeof(TransportStop));  
    stop->name = malloc(strlen(name) + 1);  
    strcpy(stop->name, name);  
    strncpy(stop->identifier, identifier, 5);  
    stop->identifier[5] = '\0';  
    stop->type = type;     
    stop->location[0] = location[0];  
    stop->location[1] = location[1];  
    stop->next = NULL;  
    
    return stop;  
    
}  

 
TransportStop* createStops(char** stopNames, char** ids, StopType* types, double locations[][2], int stopCount) {  
    TransportStop* stop = createStop(stopNames[0], ids[0], types[0], locations[0]);  
        TransportStop* start = stop;  
        
    for(int i = 1; i < stopCount; i++) {  
        stop->next = createStop(stopNames[i], ids[i], types[i], locations[i]);  
        stop = stop->next;  
    }  

    return start;    
}  

 
Map* createMap(char* locationName, char** stopNames, char** stopIds, StopType* types, double locations[][2], int stopCount) {  
    Map* map = malloc(sizeof(Map));  
    map->locationName = malloc(strlen(locationName)+1);  
    strcpy(map->locationName, locationName);  

    map->stops = createStops(stopNames, stopIds, types, locations, stopCount);  
    
    return map;  
}  

const char* getTypeName(StopType type) {  
    switch (type)  
    {  
    case ALEPA_BIKE:  
        return "bike";
    case BUS:  
        return "bus";
    case METRO:  
        return "metro";  
    case TRAM:  
        return "tram";  
    default:  
        return "invalid type";  
    }  
}  





void printStopInfo(Map* map) {  
    printf("%s has the following public transportation stops:\n",  map->locationName);  
    for(TransportStop* curr = map->stops; curr; curr = curr->next) {  
        printf("Stop %s, ID %s at (%f, %f) is of type %s.\n",  
            curr->name,  
            curr->identifier,  
            curr->location[0],  
            curr->location[1],  
            getTypeName(curr->type));  
    }  
}  
 


void freeMemory(Map* map) {  

    for(TransportStop* curr = map->stops; curr != NULL;) {  
        TransportStop* del = curr;  curr = curr->next;  
        free(del->name);  
        free(del);  
    }  
    free(map->locationName);  
    free(map);  
}  


int main() {
	char* names[] = {"Aalto-yliopiston metroasema",
					 "Aalto-yliopisto (M)",
					 "Aalto-yliopisto (M), Korkeakoulunaukio",
					 "Aalto-yliopisto (M), Tietotie",
					 "Jämeräntaival",
					 "Teekkarikylä",
					 "Otaranta"};

	char* identifiers[] =
		{"OTA", "E2230", "541", "539", "547", "E2228", "null"};

	StopType types[] =
		{METRO, BUS, ALEPA_BIKE, ALEPA_BIKE, ALEPA_BIKE, BUS, TRAM};

	double locations[][2] = {{60.18448, 24.82358},
							 {60.18369, 24.82812},
							 {60.18431, 24.82666},
							 {60.18498, 24.82013},
							 {60.18812, 24.83505},
							 {60.18743, 24.83388},
							 {60.18021, 24.83468}};

	Map* map = createMap("Otaniemi", names, identifiers, types, locations, 7);

	printStopInfo(map);

	freeMemory(map);

    return 0;
}
