#include "coords.h"

using namespace coords_ns;

void coords::set(int x0,int y0)
{
    x = x0;
    y = y0;
    if(max_x<x0) max_x = x0;
    if(max_y<y0) max_y = y0;
}
void coords::set_x(int x0)
{
    x = x0;
    if(max_x<x0) max_x = x0;
}
void coords::set_y(int y0)
{
    y = y0;
    if(max_y<y0) max_y = y0;
}
int coords::get_x() 
{
    return x;
}
int coords::get_y()
{
    return y;
}

namespace coords_ns
{  
    int coords::max_x =0; int coords::max_y = 0;
}