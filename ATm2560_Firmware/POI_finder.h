/*
 * POI_finder.h
 *
 *  Created on: 19.08.2011
 *      Author: kolja Windeler
 */

#ifndef POIFINDER_H_
#define POIFINDER_H_

class speedo_poi_finder{

#define POI_FINDER_PRECOMMA_LONGITUDE 0
#define POI_FINDER_POSTCOMMA_LONGITUDE 1
#define POI_FINDER_UNUSED_POSTCOMMA_LONGITUDE 2
#define POI_FINDER_PRECOMMA_LATITUDE 3
#define POI_FINDER_POSTCOMMA_LATITUDE 4
#define POI_FINDER_UNUSED_POSTCOMMA_LATITUDE 5
#define POI_FINDER_WAIT_LINE_BREAK 6

public:
	speedo_poi_finder();
	int calc(int file_id);
private:
	void write_navigation_switch_warning(int file_id, int point_id);
	int write_navi_file(int8_t* pointer_of_text_buffer,uint8_t* text_buffer,uint32_t* loaded_latitude,uint32_t* loaded_longitude,bool* append_to_write_file,int8_t* points_to_copy, int8_t* state, int8_t* n, SdFile* read_file,fpos_t* best_dist_seek);
};
//extern speedo_poi_finder* pPOI_Finder;

#endif /* POIFINDER_H_ */
