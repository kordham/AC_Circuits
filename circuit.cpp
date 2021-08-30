#include "circuit.h"

using namespace circuit_ns;

//Constructors
resistor::resistor() 
{
    type = "resistor";
}
resistor::resistor(double input_resistance)
{
    resistance = input_resistance;
    type="resistor";
}
capacitor::capacitor()
{
    type = "capacitor";
}
capacitor::capacitor(double input_capacitance) 
{
    capacitance = input_capacitance;
    type = "capacitor";
}
inductor::inductor()
{
    type = "inductor";
}
inductor::inductor(double input_inductance)
{
    inductance = input_inductance;
    type = "inductor";
}

//Destructors
series_wire::~series_wire() {}
parallel_wire::~parallel_wire() {}
resistor::~resistor() {}
inductor::~inductor() {}
capacitor::~capacitor() {}
wire::~wire() {}
circuit_part::~circuit_part() {}


//Functions to get values and types
double circuit_part::get_resistance() const
{
    return resistance;
}
double circuit_part::get_inductance() const
{
    return inductance;
}
double circuit_part::get_capacitance() const
{
    return capacitance;
}
std::string circuit_part::get_type() const
{
    return type;
}

//Functions to get impedance of a component
std::complex<double> resistor::get_impedance(double input_frequency) const
{
    return std::complex<double> (resistance,0);
}
std::complex<double> capacitor::get_impedance(double input_frequency) const
{
    if(capacitance>0 && input_frequency>0){
        return std::complex<double>(0,-1.e9/(input_frequency*capacitance));
    } else return std::complex<double>(0,0);
    
}
std::complex<double> inductor::get_impedance(double input_frequency) const
{
    if(inductance>0 && input_frequency>0){
        return std::complex<double>(0,input_frequency*inductance*1.e-3);
    } else return std::complex<double>(0,0);   
}


//Functions to pass component info to the output stream
void resistor::get_info(std::ostream &os)
{
    os<<"Resistor of resistance: "<<resistance<<" Ohms\t Impedance : "<<this->get_impedance(wire::frequency)<<std::endl;
}
void capacitor::get_info(std::ostream &os)
{
    os<<"Capacitor of capacitance: "<<capacitance<<" nano Farads\t Impedance: "<<this->get_impedance(wire::frequency)<<std::endl;
}
void inductor::get_info(std::ostream &os)
{
    os<<"Inductor of inductance: "<<inductance<<" milli Henries\t  Impedance: "<<this->get_impedance(wire::frequency)<<std::endl;
}

//add either a component or a wire to this wire
void wire::add_members(std::shared_ptr<circuit_part> input_member)
{
    //append to the vector of base class pointers
    circ_members.push_back(input_member);
    //re-calculate the values
    update_members();
}


//pass info of this wire to the output stream
void wire::get_info(std::ostream &os)
{
    //print recursively to get the component details
    for(std::vector<std::shared_ptr<circuit_part>>::iterator it = circ_members.begin();it<circ_members.end();++it){
        os<<(*it);
    }
}

//updating functions 
void parallel_wire::update_members()
{
    //adding components in parallel.  Need to add the inverses of resistance and inductace, but simply adding the capcitances
    double inv_resistance{},inv_inductance{},temp_capacitance{};
    for(std::vector<std::shared_ptr<circuit_part>>::iterator it = circ_members.begin();it<circ_members.end();++it){
        if ((*it)->get_resistance()!=0) inv_resistance+=1./(*it)->get_resistance();
        temp_capacitance+=(*it)->get_capacitance();
        if ((*it)->get_inductance()!=0) inv_inductance+=1./(*it)->get_inductance();
    }
    if(inv_resistance==0){
            resistance=0;
        } else resistance=1./inv_resistance;
        if(inv_inductance==0){
            inductance=0;
        } else inductance=1./inv_inductance;

    capacitance = temp_capacitance;
}  
void series_wire::update_members()
{
    //the oppose of the parallel wires happen here
    double inv_capacitance{},temp_resistance{},temp_inductance{};
    for(std::vector<std::shared_ptr<circuit_part>>::iterator it = circ_members.begin();it<circ_members.end();++it){
        if ((*it)->get_capacitance()!=0) inv_capacitance +=1./(*it)->get_capacitance();
        temp_resistance +=(*it)->get_resistance();
        temp_inductance +=(*it)->get_inductance();
    }
    if(inv_capacitance==0){
        capacitance=0;
    }  else  capacitance=1./inv_capacitance;
    resistance = temp_resistance;
    inductance = temp_inductance;
    
}
//calculating frequency and impedance 
void wire::calc_frequency()
{
    if(inductance!=0 && capacitance!=0)  frequency = 1.e6/sqrt(inductance*capacitance);//1.e6 is correcting for the units
    else frequency =0;
}
std::complex<double> wire::get_impedance()
{
    this->calc_frequency();
    if(frequency!=0) return std::complex<double>(resistance,frequency*inductance*(1.e-3)-1./(frequency*capacitance*1.e-9)); //the numbers are correcting the units
    else return std::complex<double>(resistance,0);
}
//Overloads
namespace circuit_ns
{
    double wire::frequency=0;
    std::ostream & operator<<(std::ostream &os, std::shared_ptr<circuit_part> &s)
    {
        s->get_info(os);
        return os;
    }
    std::ostream & operator<<(std::ostream &os, std::shared_ptr<wire> &w)
    {
        w->get_info(os);
        return os;
    }
}