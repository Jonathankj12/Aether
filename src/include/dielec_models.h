/*Copyright 2008-2022 - Lo�c Le Cunff

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef DIELEC_MODELS_H_INCLUDED
#define DIELEC_MODELS_H_INCLUDED

#include <bitmap3.h>
#include <mathUT.h>
#include <phys_constants.h>
#include <iostream>
#include <fstream>
#include <vector>

class DebyeModel
{
    private:
        double ds,t0;
        
    public:
        DebyeModel();
        DebyeModel(DebyeModel const &D);
    
        Imdouble eval(double) const;
        std::string get_matlab(int ID) const;
        void get_time_exp(Imdouble &a,Imdouble &b);
        void set(double ds,double t0);
        void show();
        
        void operator = (DebyeModel const &D);
};

class DrudeModel
{
    private:
        double wd,wd2,g;
        
    public:
        DrudeModel();
        DrudeModel(DrudeModel const &D);
        
        Imdouble eval(double) const;
        std::string get_matlab(int ID) const;
        void get_time_exp(Imdouble &a,Imdouble &b);
        double get_sigma();
        void set(double wd,double g);
        void show();
        
        void operator = (DrudeModel const &D);
};

class LorentzModel
{
    private:
        double A,O,O2,G;
        
    public:
        LorentzModel();
        LorentzModel(LorentzModel const &D);
        
        Imdouble eval(double) const;
        std::string get_matlab(int ID) const;
        void get_time_exp(Imdouble &a,Imdouble &b);
        void set(double A,double O,double G);
        void show();
        
        void operator = (LorentzModel const &D);
};

class CritpointModel
{
    private:
        double A,O,P,G;
        
    public:
        CritpointModel();
        CritpointModel(CritpointModel const &D);
        
        Imdouble eval(double) const;
        std::string get_matlab(int ID) const;
        void get_time_exp(Imdouble &a,Imdouble &b);
        void set(double A,double O,double P,double G);
        void show();
        
        void operator = (CritpointModel const &D);
};

class Dielec_model
{
    public:
        Dielec_model();
        Dielec_model(Dielec_model const& D);
        ~Dielec_model();
        
        double ei;
        
        std::vector<DebyeModel> debye_arr;
        std::vector<DrudeModel> drude_arr;
        std::vector<LorentzModel> lorentz_arr;
        std::vector<CritpointModel> cp_arr;
        
        void set_const(double ei);
        void add_debye(double ds,double t0);
        void add_drude(double wd,double g);
        void add_lorentz(double A,double O,double G);
        void add_critpoint(double A,double O,double P,double G);
        
        void clear();
        Imdouble eval(double) const; 
        double get_const();
        void get_time_exp(Grid1<Imdouble> &va,Grid1<Imdouble> &vb);
        std::string get_matlab() const;
        int get_N_models();
        double get_sigma();
        void show();
        void operator = (Dielec_model const &D);
};

#endif // DIELEC_MODELS_H_INCLUDED
