/** 
 * AC Circuit Builder
 * 
 * Mohammad Kordzanganeh - 10137275
 * 
 * The School of Physics and Astronomy
 * The University of Manchester 
 * 
 * 
 * This program can draw a circuit from user input, 
 * save it as a string and draw it graphically for the user.
 * */

#include<cmath>
#include<stdlib.h>
#include<sstream>
#include<fstream>

#include "svg.h"
#include "coords.h"
#include "circuit.h"

using namespace svg_ns;
using namespace coords_ns;
using namespace circuit_ns;


//Check if the input is in the correct format and pass it back
template<class return_type>
return_type input_checker(std::string prompt)
{
    return_type result;
    std::cout<<prompt;
    std::cin>>result;
    if(std::cin.fail())
    {
        throw("Failed to read an input.");
    } else return result;
}

// Check for string stream
void load_checker(std::istringstream &is)
{
    if(is.fail())
    {
        throw("The file has been corrupted.");
    }
}

//Function to load a saved circuit from a string
void load_circuit(std::shared_ptr<wire> circuit,std::istringstream &is)
{   
    char choice_component{};
    while(choice_component!='F'){
        (is)>>choice_component;
        double value{};
        if(choice_component=='R'){
            (is)>>value;
            load_checker(is);
            circuit->add_members(std::make_shared<resistor>(value));                
        } else if(choice_component=='C'){
            is>>value;
            load_checker(is);
            circuit->add_members(std::make_shared<capacitor>(value));
        } else if(choice_component=='I'){
            is>>value;
            load_checker(is);
            circuit->add_members(std::make_shared<inductor>(value));
        } else if(choice_component=='J'){
                int junctions{};
                is>>junctions;
                load_checker(is);
                std::shared_ptr<wire> result = std::make_shared<parallel_wire>();
                for(int i{};i<junctions;i++){
                    std::shared_ptr<wire> w = std::make_shared<series_wire>();
                    load_circuit(w,is);
                    result->add_members(w);
                }
            circuit->add_members(result);
        }
        else if(choice_component=='F') {}
        else throw("Loaded file is curropted, please check the number of F's or illegal characters such as \""+choice_component);
    }
}

//Function that takes user input, creates the circuit as well as a circuit string
void input_save(std::shared_ptr<wire> circuit,std::shared_ptr<std::string> circ)
{   
    int choice_component{};
    while(choice_component!=5){
        choice_component = input_checker<int>("Which component would you like to add?\n1)resistor\n2)capacitor\n3)inductor\n4)junction\n5)finish\n");
        double value{};
        if(choice_component==1){
            value = input_checker<double>("Please enter the resistance in Ohms:\n");
            circuit->add_members(std::make_shared<resistor>(value));
            *circ+="R";
            *circ+=std::to_string(value);
        } else if(choice_component==2){
            value = input_checker<double>("Please enter the capacitance in nano Farads:\n");
            circuit->add_members(std::make_shared<capacitor>(value));
            *circ+="C";
            *circ+=std::to_string(value);
        } else if(choice_component==3){
            value = input_checker<double>("Please enter the Inductance in milli Henries:\n");
            circuit->add_members(std::make_shared<inductor>(value));
            *circ+="I";
            *circ+=std::to_string(value);
        } else if(choice_component==4){
                int junctions{};
                junctions = input_checker<int>("How many parallel wires would you like to include?\n");
                *circ+="J";
                *circ+=std::to_string(junctions);
                std::shared_ptr<wire> result = std::make_shared<parallel_wire>();
                for(int i{};i<junctions;i++){
                    std::shared_ptr<wire> w = std::make_shared<series_wire>();
                    input_save(w,circ);
                    result->add_members(w);
                }
            circuit->add_members(result);
        }
        else if(choice_component==5){
            *circ+="F";
        } else throw("Incorrect input, please try again.");
    }
}


//Draw a single component ----------------------------------------------
void draw_component(std::shared_ptr<std::string> svg_code,coords* xy,std::string text)
{
  //each component will have a piece of wire before and after to make sure the junctions do not cross into the side of the components.
  std::unique_ptr<svg> line_before (new horizontal_line(*xy));
  xy->set_x(xy->get_x()+line_before->get_length());
  *svg_code+=line_before->get_svg_code();
  std::unique_ptr<svg> part (new box(text,*xy));
  xy->set_x(xy->get_x()+part->get_length());
  *svg_code+=part->get_svg_code();
  std::unique_ptr<svg> line_after (new horizontal_line(*xy));
  xy->set_x(xy->get_x()+line_after->get_length());
  *svg_code+=line_after->get_svg_code();
}
//Draw junction ----------------------------------------------------------
void draw_junction(std::shared_ptr<std::string> svg_code,coords* xy,int num_junctions,std::shared_ptr<std::vector<coords>> beginnings)
{
  junction junc{num_junctions,*xy};
  xy->set_x(xy->get_x()+junc.get_length());
  *svg_code+=junc.get_svg_code();
  std::vector<coords> junc_beginnings = junc.get_beginnings();
  for(auto it=junc_beginnings.begin();it<junc_beginnings.end();++it)
  {
    beginnings->push_back(*it);
  }
}

//Draw the circuit recursively
void draw_circuit(std::shared_ptr<std::string> svg_code,std::istringstream &is,std::shared_ptr<std::vector<coords>> beginnings)
{
  char choice_component{};
  coords* xy = new coords();
  *xy = beginnings->back();
  beginnings->pop_back();
  while(choice_component!='F'){
    (is)>>choice_component;
    double value{};
    if(choice_component=='R'){
      (is)>>value;
      draw_component(svg_code,xy,"R"+std::to_string(value));
    } else if(choice_component=='C'){
      is>>value;
      draw_component(svg_code,xy,"C"+std::to_string(value));
    } else if(choice_component=='I'){
      is>>value;
      draw_component(svg_code,xy,"I"+std::to_string(value));
    } else if(choice_component=='J'){
      int num_junctions{};
      is>>num_junctions;
      draw_junction(svg_code,xy,num_junctions,beginnings);
      for(int i{};i<num_junctions;i++){
        //draw the new part of the circuit, and do it recursively for n junctions
        draw_circuit(svg_code,is,beginnings);
      }
      //set the x and y coordinates to the right of the parallel wires now that they are over
      xy->set_x(coords::max_x);
      //draw a vertical line to signal the end of the parallel wires
      std::unique_ptr<svg> line(new vertical_line(50*(num_junctions-1),*xy));
      *svg_code+=line->get_svg_code();
    }
  }
} 
//Saving the circuit string 
void save(std::string circuit,std::string file_name)
{
    std::ofstream out_file;
    out_file.open (file_name+".txt");
    out_file << circuit;
    out_file.close();
}

//Function wrapper for drawing a circuit string
void draw(std::string circuit)
{
    std::istringstream in_ss(circuit);
    coords starting_coordinate{0,50};
    std::shared_ptr<std::vector<coords>> beginnings = std::make_shared<std::vector<coords>>();
    beginnings->push_back(starting_coordinate);
    std::shared_ptr<std::string> svg_code = std::make_shared<std::string>();
    draw_circuit(svg_code,in_ss,beginnings);
    std::ofstream out_file;
    out_file.open ("drawn_circuit.html");
    if(out_file.is_open()){
        out_file<<"<!DOCTYPE html><html><body><svg width=\"";
        //account for the edges of the figure: adding 50 will make sure all components are safely inside
        out_file<<std::to_string(coords::max_x+50);
        out_file<<"\" height=\"";
        out_file<<std::to_string(coords::max_y+50);
        out_file<<"\">";
        out_file << *svg_code;
        out_file<<"Sorry, your browser does not support inline SVG.</svg></body></html>";
        out_file.close();
        std::cout<<"Drawn!"<<std::endl;
    } else throw("Unable to open file");

}

//Wrap function for loading the circuit
void load(std::string file_name)
{
    std::shared_ptr<wire> new_circuit =std::make_shared<series_wire>();
    std::ifstream in_file;
    in_file.open(file_name+".txt");
    if(in_file.is_open()){
        std::string stringified_circuit;
        while(std::getline(in_file,stringified_circuit)){
            
            std::istringstream in_ss(stringified_circuit);
            load_circuit(new_circuit,in_ss);
            std::cout<<"Loaded!"<<std::endl;
        }
        in_file.close();
        char choice ='n';
        choice = input_checker<char> ("Would you like to see the information for this circuit?(y/n)\n");
        if(choice=='y' || choice=='Y'){
            std::complex<double> impedance = new_circuit->get_impedance();
            std::cout<<"Circuit:"<<std::endl<<"Total Resistance: "<<new_circuit->get_resistance()<<" Ohms\t"
                <<"Total Capacitance: "<<new_circuit->get_capacitance()<<" nano Farads\t"<<"Total Inductance: "
                <<new_circuit->get_inductance()<<" milli Henries"<<std::endl<<"Impedance:\t"<<"Resistance = "<<std::real(impedance)
                <<"\t Reactance = "<<std::imag(impedance)<<"\t --- \t Amplitude = "<<std::abs(impedance)<<"\t Phase = "
                <<std::arg(impedance)<<std::endl<<"List of Components:"<<std::endl<<new_circuit<<std::endl;
        } else std::cout<<"You chose not to see the information of this circuit"<<std::endl;
        choice = input_checker<char> ("Would you like to draw the circuit?(y/n)\n");
        if(choice=='y' || choice=='Y')
        {
            draw(stringified_circuit);
        } else std::cout<<"You chose not to draw this circuit"<<std::endl;
    } else throw("Unable to open file");
}

//Function that takes the User's input and generates the circuit
void user_generated_circuit()
{
    std::shared_ptr<std::string> circ=std::make_shared<std::string>();
    std::shared_ptr<wire> new_circuit =std::make_shared<series_wire>();
    input_save(new_circuit,circ);
    char choice ='n';
    std::cout<<"Would you like to see the information for this circuit?(y/n)"<<std::endl;
    std::cin>>choice;
    if(choice=='y' || choice=='Y'){
        std::complex<double> impedance = new_circuit->get_impedance();
        std::cout<<"Circuit:"<<std::endl<<"Total Resistance: "<<new_circuit->get_resistance()<<" Ohms\t"
            <<"Total Capacitance: "<<new_circuit->get_capacitance()<<" nano Farads\t"<<"Total Inductance: "
            <<new_circuit->get_inductance()<< " milli Henries"<<std::endl<<"Impedance:\t"<<impedance<<
            std::endl<<std::endl<<"List of Components:"<<std::endl<<new_circuit<<std::endl;
    }
    std::cout<<"Would you like to save the circuit?(y/n)"<<std::endl;
    std::cin>>choice;
    if(choice=='y' || choice=='Y')
    {
        save(*circ,"circuit");
    } else std::cout<<"You chose not to draw this circuit"<<std::endl;
    std::cout<<"Would you like to draw the circuit?(y/n)"<<std::endl;
    std::cin>>choice;
    if(choice=='y')
    {
        draw(*circ);
    }
}

void start()
{
    std::cout<<"Please choose from the below options on how you would like to proceed:"<<std::endl;
    std::cout<<"1)Create new circuit\n2)Load saved circuit"<<std::endl;
    int choice{};
    std::cin>>choice;
    while((choice!=1 && choice!=2) || std::cin.fail()){
        std::cin.clear();
        std::cin.ignore(1000, '\n');
        std::cout<<"Invalid input, please enter a choice between:"<<std::endl;
        std::cout<<"1)Create new circuit\n2)Load saved circuit"<<std::endl;
        std::cin>>choice;
    }

    if(choice==1){
        user_generated_circuit();
    } else if(choice==2){
        load("circuit");
    }
    std::cout<<"Thank you so much for using the A.C circuit builder"<<std::endl;
}


int main()
{
    std::cout.precision(3);
    std::cout<<"------------=Welcome to the A.C circuit builder=---------------"<<std::endl;
    try{
        start();
    }catch(const char* msg){
      std::cerr<<msg<<std::endl;
      std::cout<<"\nPress any key to exit the programme..."<<std::endl;
      char any_key;
      std::cin>>any_key;
    }
    return 0;
}