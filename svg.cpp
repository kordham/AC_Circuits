#include "svg.h"
#include<memory>

using namespace coords_ns;
using namespace svg_ns;

//Constructors
junction::junction(): count{},lin_length{20},height{50} {}
junction::junction(int input_count,coords input_xy): count{input_count},lin_length{20},height{50} 
{
    xy = input_xy;
}
vertical_line::vertical_line(int input_height,coords input_xy): height{input_height}
{
    xy = input_xy;
}
horizontal_line::horizontal_line(): lin_length{20} {}
horizontal_line::horizontal_line(coords input_xy): lin_length{20}
{
    xy = input_xy;
}
box::box(): width{40},height{20},text{},font_size{10} {}
box::box(std::string input_text,coords input_xy): width{40},height{20},text{input_text},font_size{10}
{ 
    xy = input_xy;
}

junction::~junction(){};
horizontal_line::~horizontal_line(){};
vertical_line::~vertical_line(){};
box::~box(){};
svg::~svg(){};



//Functions to fetch the horizontal size of the svg components
int box::get_length()
{
    return width;
}
int horizontal_line::get_length()
{
    return lin_length;
}
int vertical_line::get_length()
{
    //vertical line has no length (thickness is neglected for simplicity)
    return 0;
}
int junction::get_length()
{
    return lin_length;
}


// Function to generate the SVG codes
std::string box::get_svg_code()
{
    //example svg code for a rectangle: 
    std::string result{"<rect x=\""};
    result+=std::to_string(xy.get_x());
    result+="\" y=\"";
    //need to bring the centre of the rectangle on the line, hence the -height/2
    result+=std::to_string(xy.get_y()-height/2);
    result+="\" width=\"";
    result+=std::to_string(width);
    result+="\" height=\"";
    result+=std::to_string(height);
    result+="\"/>";
    //example svg code for a text box: <rect width="300" height="100"/>
    result+="<text x=\"";
    result+=std::to_string(xy.get_x());
    result+="\" y=\"";
    //make sure the text is above the rectangle
    result+=std::to_string(xy.get_y()-height*3/4);
    result+="\" font-family=\"Verdana\" font-size=\"10\" fill=\"black\">";
    result+=text;
    result+="</text>";

    return result;
}
std::string horizontal_line::get_svg_code()
{
    //example svg code for a straight line: <line x1="0" y1="0" x2="200" y2="200" />
    std::string result{"<line x1=\""};
    result+=std::to_string(xy.get_x());
    result+="\" y1=\"";
    result+=std::to_string(xy.get_y());
    result+="\" x2=\"";
    result+=std::to_string(lin_length+xy.get_x());
    result+="\" y2=\"";
    result+=std::to_string(xy.get_y());
    result+="\""+this->get_svg_style()+"/>";
    return result;
}
std::string vertical_line::get_svg_code()
{
    std::string result{"<line x1=\""};
    result+=std::to_string(xy.get_x());
    result+="\" y1=\"";
    result+=std::to_string(xy.get_y());
    result+="\" x2=\"";
    result+=std::to_string(xy.get_x());
    result+="\" y2=\"";
    result+=std::to_string(xy.get_y()+height);
    result+="\" ";
    result+=this->get_svg_style();
    result+="/>";
    return result;
}
std::string junction::get_svg_code()
{
    std::string result{};
    for(int i{};i<count;i++)
    {
      std::unique_ptr<svg> line(new horizontal_line(coords{xy.get_x(),xy.get_y()+i*height}));
      result+=line->get_svg_code();
    }
    std::unique_ptr<svg> line(new vertical_line((count-1)*height,coords{xy.get_x(),xy.get_y()}));
    result+=line->get_svg_code();
    return result;
}


//Functions to get the svg style
std::string horizontal_line::get_svg_style()
{
    return "style=\"stroke:rgb(255,0,0);stroke-width:2\"";
}
std::string vertical_line::get_svg_style()
{
    return "style=\"stroke:rgb(255,0,0);stroke-width:2\"";
}

//a "junction" member function that store the beginnings of each of the outgoing wires from the junction
std::vector<coords> junction::get_beginnings()
{
    std::vector<coords> coordinates;
    for (int i{};i<count;i++){
      coords beginning{xy.get_x()+lin_length,xy.get_y()+height*i};
      coordinates.push_back(beginning);
    }
    return coordinates;
}
