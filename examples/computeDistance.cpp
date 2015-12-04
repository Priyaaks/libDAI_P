#include <boost/lexical_cast.hpp>
#include <dai/alldai.h>
#include <iostream>
#include <fstream>
#include <string>
#include "dai/emrun.h"
#include <dai/util.h>
#include <string>
#include <sys/stat.h>
#include <time.h>
#include <boost/tokenizer.hpp>
#include <iomanip>      // std::setprecision
#include <sstream>
#include <string>
#include <cstdlib>
#include<math.h>
using namespace std;
using namespace dai;
using namespace boost;
std::string& trim(std::string& s, const char* t = " \t\n\r\f\v")
{
	s.erase(0, s.find_first_not_of(t));
	s.erase(s.find_last_not_of(t) + 1);
	return s;
}
double euclideanDist(double v1[],double v2[],int length)
{
    double distance=0;
    for(int i=0;i<length;i++)
    {
        distance = distance+ pow((v1[i]-v2[i]),2); //Euclidean distance
        //cout<<v1[i]<<" "<<v2[i]<<" "<<distance;
    }
   // cout<<"**** "<<distance<<endl;
    distance = sqrt(distance);
    return distance;
}
double klDist(double v1[],double v2[],int length)
{
    double distance=0;
    for(int i=0;i<length;i++)
    {
        distance = distance + (v1[i]* log0( v1[i]/v2[i])); // KL distance
       // cout<<( v1[i]/v2[i])<<" "<<(v1[i]* std::log( v1[i]/v2[i]))<<endl;
       // cout<<distance<<"+";
    }
    //cout<<"="<<distance<<endl;
    return distance;
}
void calculateDistance(string child, string child1)
{
    
        const char* childfgfile = &child[0];
        const char* childfgfile1 = &child1[0];
        //cout<<"child file "<< childfgfile<<endl;
        string line1,line2;
        ifstream p1 (childfgfile);
        ifstream c1 (childfgfile1);
        getline(p1,line1);
        getline(c1,line2);
        
        int factor =0;
        int no_factors = 0;
        int* factor_names;
        int factor_pos=0;
        int* states_arr;
        int no_states = 1;
        int arr_size =1;
        int factor_states =0;
        int tot_arrays =0;
        int arr_set =0;
        int arr_set_states =0;
        int no_arr_arrset =0;
        int stat =0;
        int p_c = 0;
        double pmut=0,pm=0,distval=0;
        while(getline(p1,line1)&&getline(c1,line2))
        {
            line1 = trim(line1);
            line2 = trim(line2);
            //cout<<p_c<<" "<<pmut<<" "<<pm<<" "<<line1<<endl;
            stringstream ss(line1);
            stringstream ss1(line2);
            if(line1.empty())
            {
                p_c = 0;
                stat =0;
                //cout<<"factor variable "<<factor<<endl;
                //seed();
                //pmut = unifRand(); // generates random a value between 0 and 1 for child1
                //cout<<"Mutation probability "<<pmut<<endl;
                no_states = 1;
                arr_size =1;
                factor++;
            }
            if(pmut>pm)
            {
                //c1<<line1<<endl;
                //cout<<"No mutation"<<endl;
                //p_c++;
                //continue;
            }
            if(pmut<=pm) //do mutation handling the constraint that the states of the random variable sum to 1.
            {
                //cout<<"Mutation happening "<<endl;
                double sumo=0,sumo1=0;
                if(p_c == 0)
                {
                    //c1<<line1<<endl;
                }
                if(p_c == 1)
                {
                    ss>>no_factors;
                    //cout<<"no. of factors "<<no_factors<<endl;
                    //c1<<no_factors<<endl;
                }
                if(p_c == 2)
                {
                    factor_names = new int[no_factors];
                    std::vector<std::string> fields;
                    fields = tokenizeString( line1, true,"  ");
                    for( size_t i = 0; i < fields.size(); ++i )
                    {
                        stringstream n;
                        n << fields[i];
                        string s = n.str();
                        if( s.find_first_not_of(" ")!= std::string::npos)
                        {
                            //c1<<s<<" ";
                            n >> factor_names[i];
                            if(factor_names[i] == (factor-1))
                                factor_pos =i;
                        }
                    }
                    //c1<<endl;
                  //  cout<<"factor position "<<factor_pos<<endl;
                }
                if(p_c == 3)
                {
                    states_arr = new int[no_factors];
                    std::vector<std::string> fields1;
                    fields1 = tokenizeString( line1, true,"  ");
                    for( size_t j = 0; j < fields1.size(); ++j )
                    {
                        stringstream n1;
                        n1 << fields1[j];
                        string state = n1.str();
                        if( state.find_first_not_of(" ")!= std::string::npos)
                        {
                            //c1<<state<<" ";
                            n1 >> states_arr[j];
                            no_states = no_states * states_arr[j];
                            if(j < factor_pos)
                                arr_size = arr_size * states_arr[j];
                            if(j == factor_pos)
                            {
                                factor_states = states_arr[j];
                                arr_set_states = arr_size*states_arr[j];
                            }
                        }
                    }
                    //c1<<endl;
                    tot_arrays = no_states/arr_size;
                    arr_set = no_states/arr_set_states;
                    no_arr_arrset = factor_states;
                    //cout<<"array size "<<arr_size<<endl;
                   // cout<<no_states<<"\t"<<"no. of states "<<no_states<<"\t"<<"total arrays "<<tot_arrays<<"\t"<<"array set "<<arr_set<<"\t"<<"no. arrays "<<no_arr_arrset<<"\t"<<"factor states "<<factor_states<<endl;
                    //c1<<no_states<<endl;
                }
                if(p_c >= 5)
                {
                    int k=0;
                    while(k<arr_set)
                    {
                        std::vector< double * > Arrays, Arrays1;
                        for(int l=0; l<no_arr_arrset; l++)
                        {
                            Arrays.push_back( new double[arr_size]);
                            Arrays1.push_back( new double[arr_size]);
                        }
                        vector<double*>::iterator it2,it21;
                        it2=Arrays.begin();
                        it21=Arrays1.begin();
                        //get the current cpt values and copy them in the array
                        int l=0;
                        while(l<no_arr_arrset)
                        {
                            double* arr = *it2;
                            double* arr1 = *it21;
                           // cout<<"new arr"<<endl;
                            int m=0;
                            while(m<arr_size)
                            {
                                line1 = trim(line1);
                                line2 = trim(line2);
                                //cout<<line1 <<" "<<line2<<endl;
                                std::vector<std::string> values,values1;
//                                values = tokenizeString( line1, true," +");
//                                values1 = tokenizeString( line2, true," +");
                                char_separator<char> sep("                 ");
                                tokenizer<char_separator<char>> tokens(line1, sep);
                                for (const auto& t : tokens) {
                                    //cout<<t;
                                    if(t!=" ")
                                    {
                                        values.push_back(t);
                                        // cout<<"pushed "<<t<<endl;
                                    }
                                }
                                //cout<<endl;
                                tokenizer<char_separator<char>> tokens1(line2, sep);
                                for (const auto& t : tokens1) {
                                    //cout<<t;
                                    if(t!=" ")
                                    {
                                        values1.push_back(t);
                                        //cout<<"pushed "<<t<<endl;
                                    }
                                }
                                int ii=0;
                                //cout<<values.size()<<" "<<values1.size()<<endl;
                                for( size_t j = 0; j < values.size(); ++j )
                                {
                                    stringstream n1,n2;
                                    n1 << values[j];
                                    n2 << values1[j];
                                    string state = n1.str();
                                    string state1 = n2.str();
                                    //cout<<state<<" !! "<<state1<<" !! "<<endl;
                                    if( state.find_first_not_of(" ")!= std::string::npos && state1.find_first_not_of(" ")!= std::string::npos)
                                    {
                                        if(ii==1)
                                        {
                                            n1 >> arr[m];
                                            n2 >> arr1[m];
                                            //cout<<p_c<<" "<<line1<<" "<<"arrval = "<<values[0]<<"\tarr[m]"<<arr[m]<<" m = "<<m<<endl;
                                            m++;
                                            if(m<arr_size)
                                            {
                                             getline(p1,line1);
                                             getline(c1,line2);
                                            }
                                            ii=0;
                                        }
                                        else
                                            ii=1;
                                    } //end of if
                                } // end of for
                            } // end of while -m
                            l++;
                            if(l<no_arr_arrset)
                            {
                                getline(p1,line1);
                                getline(c1,line2);
                            }
                            ++it2; ++it21;
                        } //end of while -l
                       // cout<<"new val"<<endl;
                        vector<double*>::iterator it,it1;
                        double sumarr[arr_size];
                        for(int m=0; m<arr_size; m++)
                        {
                            it=Arrays.begin();
                            it1=Arrays1.begin();
                            double valarr[no_arr_arrset],valarr1[no_arr_arrset];
                            for(int l=0; l<no_arr_arrset; l++) // stdnormal rand values are generated and sum is found
                            {
                                double* arr = *it;
                                double* arr1 = *it1;
                                double val = arr[m];
                                double val1 =  arr1[m];
                                valarr[l]=val;
                                valarr1[l]=val1;
                                cout<<val<<" "<<val1<<endl;
                                ++it; ++it1;
                            }
                            
                            sumo+=euclideanDist(valarr,valarr1,no_arr_arrset);
                            sumo1+=klDist(valarr,valarr1,no_arr_arrset);
                           // cout<<endl;
                        }
                        k++;
                        if(k<arr_set)
                         {
                             getline(p1,line1);
                             getline(c1,line2);
                         }
                    }// end of while -k
                    cout<<"******"<<no_states<<" "<<factor<<" "<<sumo<<" "<<sumo1<<" "<<endl;
                    distval = distval+sumo;
                }// end of if p_c>=5
                
            } // end of if pmut<p_m
            p_c++;
        } // end of while loop
        c1.close();
    p1.close();
    cout<<distval<<endl;
}
// Type for storing a joint state of all variables
int main(int argc, char *argv[]) {
time_t timer1, timer2;
time(&timer1);
clock_t tStart = clock();
    string child,child1;
//    if(argc.length!=2){
//     child =  "/Applications/Utilities/libDAI-0.3.1/alarmnew_factor_graphs/alarm1l.fg";
//     child1 =  "/Applications/Utilities/libDAI-0.3.1/alarmnew_factor_graphs/alarm2l.fg";
//    }
//    else
    {
        stringstream ss2(argv[1]);
        child = ss2.str();
        stringstream ss3(argv[2]);
        child1 = ss3.str();
    }
    calculateDistance(child,child1);

time(&timer2);
double t = difftime(timer2,timer1);
(clock() - tStart)/CLOCKS_PER_SEC;
double t_c = (clock() - tStart)/CLOCKS_PER_SEC;
//cout<<"Time taken: "<<t<<"s"<<endl;
//cout<<" Processor Time taken: "<<t_c<<"s"<<endl;

    return 0;
}
