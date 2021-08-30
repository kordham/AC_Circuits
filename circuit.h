#ifndef CIRCUIT_H 
#define CIRCUIT_H
#include <iostream>
#include <string>
#include <complex>
#include <vector>
#include <memory>

namespace circuit_ns{
    class circuit_part
    {
        friend std::ostream & operator<<(std::ostream &os, std::shared_ptr<circuit_part> &s);
    protected:
        double resistance{},capacitance{},inductance{};
        std::string type;
    public:
        //the destructor is not pure virtual because this is not an abstract class
        virtual ~circuit_part();
        double get_resistance() const;
        double get_inductance() const;
        double get_capacitance() const;
        std::string get_type() const;
        virtual void get_info(std::ostream &os)=0;
    };

    class component: public circuit_part
    {
    public:
        virtual std::complex<double> get_impedance(double input_frequency) const=0;

    };

    class resistor: public component
    {
    public:
        resistor();
        resistor(double input_resistance);
        ~resistor();
        std::complex<double> get_impedance(double input_frequency) const;
        void get_info(std::ostream &os);
    };

    class capacitor: public component
    {
    public: 
        capacitor();
        capacitor(double input_capacitance);
        ~capacitor();
        std::complex<double> get_impedance(double input_frequency) const;
        void get_info(std::ostream &os);
    };

    class inductor: public component
    {
    public:
        inductor();
        inductor(double input_inductance);
        ~inductor();
        std::complex<double> get_impedance(double input_frequency) const;
        void get_info(std::ostream &os);
    };
    class wire : public circuit_part
    {
        friend std::ostream & operator<<(std::ostream &os, std::shared_ptr<wire> &w);
    protected:
        std::vector<std::shared_ptr<circuit_part>> circ_members;
        virtual void update_members()=0;
    public:
        static double frequency;
        ~wire();
        void calc_frequency();
        void add_members(std::shared_ptr<circuit_part> input_member);
        void get_info(std::ostream &os);
        std::complex<double> get_impedance();
    };

    class parallel_wire : public wire
    {
    private:
        void update_members();
    public:
        ~parallel_wire();
    };

    class series_wire : public wire
    {
    private:
        void update_members();
    public:
        ~series_wire();
    };
}
#endif