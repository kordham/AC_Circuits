/**
 * Coords class
 * this class holds the coordinate of a point (x,y)
**/ 

#ifndef COORDS_H 
#define COORDS_H

namespace coords_ns
{
    class coords
    {
    private:
        int x,y;
    public:
        static int max_x,max_y;
        coords() {}
        coords(int x0,int y0): x{x0},y{y0}
        {
            if(max_x<x0) max_x = x;
            if(max_y<y0) max_y = y;
        }
        void set(int x0,int y0);
        void set_x(int x0);
        void set_y(int y0);
        int get_x();
        int get_y();
    };
}
#endif