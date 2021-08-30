#ifndef SVG_H 
#define SVG_H
#include "coords.h"
#include<string>
#include<vector>
#include<memory>

using namespace coords_ns;

namespace svg_ns {
    class svg
    {
    protected:
    coords xy;
    public:
    virtual ~svg();
    virtual std::string get_svg_code()=0;
    virtual int get_length()=0;
    };

    class box : public svg
    {
    private:
    std::string text;
    int width,height;
    int font_size;
    public:
    box();
    box(std::string input_text,coords input_xy);
    ~box();
    int get_length();
    std::string get_svg_code();
    };


    class horizontal_line : public svg
    {
    private:
    int lin_length;
    public:
    horizontal_line();
    horizontal_line(coords input_xy);
    ~horizontal_line();
    int get_length();
    std::string get_svg_style();
    std::string get_svg_code();
    };


    class vertical_line : public svg
    {
    private:
    int height;
    public:
    vertical_line(int input_height,coords input_xy);
    ~vertical_line();
    std::string get_svg_style();
    int get_length();
    std::string get_svg_code();
    };
    class junction : public svg
    {
    private:
    int count;
    int lin_length;
    int height;
    public:
    junction();
    junction(int input_count,coords input_xy);
    ~junction();
    int get_length();
    std::string get_vline_style();
    std::string get_svg_code();
    std::vector<coords> get_beginnings();
    };
}
#endif