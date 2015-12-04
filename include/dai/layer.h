/*
 * Layer.h
 *
 *  Created on: Apr 8, 2011
 *      Author: priya
 */
#ifndef __defined_libdai_layer_h
#define __defined_libdai_layer_h


#include <dai/emrun.h>
#include<vector>
#include <dai/factor.h>
namespace dai {

class Layer {
private:
	int layer_no;
	int max_iter;
//std::vector<EMRun>  layer_vector;
	std::vector<EMRun*> _layervectors;
public:
			Layer():layer_no(0) , max_iter(0), _layervectors(){}
			Layer( int layernum, int iter ){
	    	std::cout<<"layer created"<<layernum<<std::endl;
	    	layer_no=layernum;
	    	max_iter=iter;
	    }
 	    void layerVector();
 	    void createEMruns();
 	   std::vector<EMRun> getLayerVector();
 	   int getLayerno();
 	   int getMaxiter();
 	    ~Layer(){}
};

} // end of namespace dai


#endif
