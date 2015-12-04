#include <dai/layer.h>
#include "dai/emrun.h"
#include<vector>
namespace dai {

using namespace std;

void Layer::createEMruns()
{
	cout<<"Creating EM runs"<<endl;
	for(int i=0;i<10;i++)
	{
		EMrun objEMrun(layer_no,max_iter,i);
		layer_vector.push_back(objEMrun);
	}
}
} // end of namespace dai
