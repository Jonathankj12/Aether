/*Copyright 2008-2023 - Lo�c Le Cunff

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include <filehdl.h>
#include <lua_material.h>
#include <material.h>
#include <phys_tools.h>

extern std::ofstream plog;
extern const double Pi;
extern const Imdouble Im;

//#####################
//   Material script
//#####################

Material * get_mat_pointer(lua_State *L,std::string bind_name)
{
    lua_getglobal(L,bind_name.c_str());
    return reinterpret_cast<Material*>(lua_touserdata(L,-1));
}

int lmat_add_crit_point(lua_State *L)
{
    Material *mat=get_mat_pointer(L,"binded_material");
    
    double A=lua_tonumber(L,1);
    double O=lua_tonumber(L,2);
    double P=lua_tonumber(L,3);
    double G=lua_tonumber(L,4);
    
    CritpointModel critpoint;
    critpoint.set(A,O,P,G);
    
    mat->critpoint.push_back(critpoint);
    
    return 0;
}

int lmat_add_debye(lua_State *L)
{
    Material *mat=get_mat_pointer(L,"binded_material");
    
    double ds=lua_tonumber(L,1);
    double t0=lua_tonumber(L,2);
    
    DebyeModel debye;
    debye.set(ds,t0);
    
    mat->debye.push_back(debye);
    
    return 0;
}

int lmat_add_drude(lua_State *L)
{
    Material *mat=get_mat_pointer(L,"binded_material");
    
    double wd=lua_tonumber(L,1);
    double g=lua_tonumber(L,2);
    
    DrudeModel drude;
    drude.set(wd,g);
    
    mat->drude.push_back(drude);
    
    return 0;
}

int lmat_add_lorentz(lua_State *L)
{
    Material *mat=get_mat_pointer(L,"binded_material");
    
    double A=lua_tonumber(L,1);
    double O=lua_tonumber(L,2);
    double G=lua_tonumber(L,3);
    
    LorentzModel lorentz;
    lorentz.set(A,O,G);
    
    mat->lorentz.push_back(lorentz);
    
    return 0;
}

int lmat_description(lua_State *L)
{
    Material *mat=get_mat_pointer(L,"binded_material");
    
    mat->description=lua_tostring(L,1);
    
    return 0;
}

int lmat_epsilon_infty(lua_State *L)
{
    Material *mat=get_mat_pointer(L,"binded_material");
    
    mat->eps_inf=lua_tonumber(L,1);
    
    return 0;
}

int lmat_index_infty(lua_State *L)
{
    Material *mat=get_mat_pointer(L,"binded_material");
    
    double n=lua_tonumber(L,1);
    mat->eps_inf=n*n;
    
    return 0;
}

int lmat_add_cauchy(lua_State *L)
{
    int N=lua_gettop(L);
    
    Material *mat=get_mat_pointer(L,"binded_material");
    
    std::vector<double> coeffs(N);
    
    for(int i=0;i<N;i++)
        coeffs[i]=lua_tonumber(L,i+1);
    
    mat->cauchy_coeffs.push_back(coeffs);
    
    return 0;
}

template<int type>
int lmat_add_data_table(lua_State *L)
{
    Material *mat=get_mat_pointer(L,"binded_material");
    
    std::vector<double> lambda,data_r,data_i;
    
    lua_tools::extract_vector(lambda,L,1); // Wavelength at this point
    lua_tools::extract_vector(data_r,L,2); // Input real part
    lua_tools::extract_vector(data_i,L,3); // Input imag part
    
    bool type_index=false;
    
    if constexpr(type==0) type_index=true;
    
    mat->add_spline_data(lambda,data_r,data_i,type_index);
    
    return 0;
}

int lmat_set_sellmeier(lua_State *L)
{
    Material *mat=get_mat_pointer(L,"binded_material");
    
    mat->sellmeier_B.push_back(lua_tonumber(L,1));
    mat->sellmeier_C.push_back(lua_tonumber(L,2));
    
    return 0;
}

int lmat_set_validity_range(lua_State *L)
{
    Material *mat=get_mat_pointer(L,"binded_material");
    
    mat->lambda_valid_min=lua_tonumber(L,1);
    mat->lambda_valid_max=lua_tonumber(L,2);
    
    return 0;
}

void Material::load_lua_script(std::filesystem::path const &script_path_)
{
    reset();
    
    if(!std::filesystem::is_regular_file(script_path_))
    {
        std::cerr<<"Error: Couldn't find the material "<<script_path_<<" ...\nAborting...\n";
        std::exit(EXIT_FAILURE);
        return;
    }
    
    script_path=script_path_;
    
    lua_State *L=luaL_newstate();
    luaL_openlibs(L);
    
    lua_pushlightuserdata(L,reinterpret_cast<void*>(this));
    lua_setglobal(L,"binded_material");
    
    std::filesystem::path caller_path=script_path.parent_path();
    lua_pushlightuserdata(L,reinterpret_cast<void*>(&caller_path));
    lua_setglobal(L,"lua_caller_path");
    
    lua_register(L,"add_cauchy",lmat_add_cauchy);
    lua_register(L,"add_crit_point",lmat_add_crit_point);
    lua_register(L,"add_data_epsilon",lmat_add_data_table<1>);
    lua_register(L,"add_data_index",lmat_add_data_table<0>);
    lua_register(L,"add_debye",lmat_add_debye);
    lua_register(L,"add_drude",lmat_add_drude);
    lua_register(L,"add_lorentz",lmat_add_lorentz);
    lua_register(L,"add_sellmeier",lmat_set_sellmeier);
    lua_register(L,"description",lmat_description);
    lua_register(L,"epsilon_infinity",lmat_epsilon_infty);
    lua_register(L,"epsilon_infty",lmat_epsilon_infty);
    lua_register(L,"index_infinity",lmat_index_infty);
    lua_register(L,"index_infty",lmat_index_infty);
    lua_register(L,"validity_range",lmat_set_validity_range);
    
    luaL_loadfile(L,script_path.generic_string().c_str());
    lua_pcall(L,0,0,0);
    
    lua_close(L);
}

int spec_mat_ID=0;

//int gen_absorbing_material(lua_State *L)
//{
//    std::stringstream fname;
//    
//    fname<<"buf/absorbing_mat_"<<spec_mat_ID<<".lua";
//    
//    std::ofstream file(fname.str(),std::ios::out|std::ios::trunc);
//    
//    double lambda=lua_tonumber(L,1);
//    double n=lua_tonumber(L,2);
//    double alpha_coeff=lua_tonumber(L,3);
//    double lambda_width=lua_tonumber(L,4);
//    
//    double w=2.0*Pi*c_light/lambda;
//    
//    Imdouble n=real_part+imag_part*Im;
//    Imdouble n2=n*n;
//    
//    double eps_inf=n2.real();
//    double O=w;
//    double G=w;
//    double A=n2.imag();
//    
//    file<<"set_dielectric()"<<std::endl<<std::endl;
//    file<<"epsilon_infty("<<eps_inf<<")"<<std::endl<<std::endl;
//    file<<"add_lorentz("<<A<<","<<O<<","<<G<<")"<<std::endl;
//    
//    file.close();
//    lua_pushstring(L,fname.str().c_str());
//    
//    spec_mat_ID++;
//    
//    return 1;
//}

int gen_const_material(lua_State *L)
{
    std::filesystem::path fname=PathManager::to_temporary_path("const_mat_"+std::to_string(spec_mat_ID)+".lua");
    
    std::ofstream file(fname,std::ios::out|std::ios::trunc);
    
    file<<"index_infty("<<lua_tonumber(L,1)<<")"<<std::endl;
    
    file.close();
    lua_pushstring(L,fname.generic_string().c_str());
    
    spec_mat_ID++;
    
    return 1;
}

int gen_complex_material(lua_State *L)
{
    std::filesystem::path fname=PathManager::to_temporary_path("complex_mat_"+std::to_string(spec_mat_ID)+".lua");
    
    std::ofstream file(fname,std::ios::out|std::ios::trunc);
    
    double lambda=lua_tonumber(L,1);
    double real_part=lua_tonumber(L,2);
    double imag_part=lua_tonumber(L,3);
    
    double w=2.0*Pi*c_light/lambda;
    
    Imdouble n=real_part+imag_part*Im;
    Imdouble n2=n*n;
    
    double eps_inf=n2.real();
    double O=w;
    double G=w;
    double A=n2.imag();
    
    file<<"set_dielectric()"<<std::endl<<std::endl;
    file<<"epsilon_infty("<<eps_inf<<")"<<std::endl<<std::endl;
    file<<"add_lorentz("<<A<<","<<O<<","<<G<<")"<<std::endl;
    
    file.close();
    lua_pushstring(L,fname.generic_string().c_str());
    
    spec_mat_ID++;
    
    return 1;
}

//########################
//   Material Metatable
//########################

namespace lua_material
{
    template<int type>
    int add_data_table(lua_State *L)
    {
        Material *mat=lua_get_metapointer<Material>(L,1);
        
        std::vector<double> lambda,data_r,data_i;
        
        lua_tools::extract_vector(lambda,L,2); // Wavelength at this point
        lua_tools::extract_vector(data_r,L,3); // Input real part
        lua_tools::extract_vector(data_i,L,4); // Input imag part
        
        bool type_index=false;
        
        if constexpr(type==0) type_index=true;
        
        mat->add_spline_data(lambda,data_r,data_i,type_index);
        
        return 0;
    }

    void create_metatable(lua_State *L)
    {
        lua_register(L,"Material",allocate);
        
        create_obj_metatable(L,"metatable_material");
        
        metatable_add_func(L,"name",set_name);
        metatable_add_func(L,"refractive_index",set_index);
        metatable_add_func(L,"load_script",set_script);
        
        metatable_add_func(L,"add_cauchy",add_cauchy);
        metatable_add_func(L,"add_crit_point",add_crit_point);
        metatable_add_func(L,"add_data_epsilon",add_data_table<1>);
        metatable_add_func(L,"add_data_index",add_data_table<0>);
        metatable_add_func(L,"add_debye",add_debye);
        metatable_add_func(L,"add_drude",add_drude);
        metatable_add_func(L,"add_lorentz",add_lorentz);
        metatable_add_func(L,"add_sellmeier",add_sellmeier);
        metatable_add_func(L,"description",description);
        metatable_add_func(L,"epsilon_infinity",epsilon_infinity);
        metatable_add_func(L,"validity_range",validity_range);
    }

    int add_cauchy(lua_State *L)
    {
        int N=lua_gettop(L)-1;
        
        Material *mat=lua_get_metapointer<Material>(L,1);
        
        std::vector<double> coeffs(N);
        
        for(int i=0;i<N;i++)
            coeffs[i]=lua_tonumber(L,i+2);
        
        mat->cauchy_coeffs.push_back(coeffs);
        
        return 0;
    }

    int add_crit_point(lua_State *L)
    {
        Material *mat=lua_get_metapointer<Material>(L,1);
        
        double A=lua_tonumber(L,2);
        double O=lua_tonumber(L,3);
        double P=lua_tonumber(L,4);
        double G=lua_tonumber(L,5);
        
        CritpointModel critpoint;
        critpoint.set(A,O,P,G);
        
        mat->critpoint.push_back(critpoint);
        
        return 0;
    }

    int add_debye(lua_State *L)
    {
        Material *mat=lua_get_metapointer<Material>(L,1);
        
        double ds=lua_tonumber(L,2);
        double t0=lua_tonumber(L,3);
        
        DebyeModel debye;
        debye.set(ds,t0);
        
        mat->debye.push_back(debye);
        
        return 0;
    }

    int add_drude(lua_State *L)
    {
        Material *mat=lua_get_metapointer<Material>(L,1);
        
        double wd=lua_tonumber(L,2);
        double g=lua_tonumber(L,3);
        
        DrudeModel drude;
        drude.set(wd,g);
        
        mat->drude.push_back(drude);
        
        return 0;
    }

    int add_lorentz(lua_State *L)
    {
        Material *mat=lua_get_metapointer<Material>(L,1);
        
        double A=lua_tonumber(L,2);
        double O=lua_tonumber(L,3);
        double G=lua_tonumber(L,4);
        
        LorentzModel lorentz;
        lorentz.set(A,O,G);
        
        mat->lorentz.push_back(lorentz);
        
        return 0;
    }

    int add_sellmeier(lua_State *L)
    {
        Material *mat=lua_get_metapointer<Material>(L,1);
        
        mat->sellmeier_B.push_back(lua_tonumber(L,2));
        mat->sellmeier_C.push_back(lua_tonumber(L,3));
        
        return 0;
    }
    
    int allocate(lua_State *L)
    {
        Material *p_material=lua_allocate_metapointer<Material>(L,"metatable_material");
        
        if(lua_gettop(L)>0)
        {
                 if(lua_isnumber(L,1)) p_material->set_const_n(lua_tonumber(L,1));
            else if(lua_isstring(L,1)) p_material->load_lua_script(lua_tostring(L,1));
        }
        
        return 1;
    }
    
    int description(lua_State *L)
    {
        Material *mat=lua_get_metapointer<Material>(L,1);
        
        mat->description=lua_tostring(L,2);
        
        return 0;
    }

    int epsilon_infinity(lua_State *L)
    {
        Material *mat=lua_get_metapointer<Material>(L,1);
        
        mat->eps_inf=lua_tonumber(L,2);
        
        return 0;
    }

    int set_index(lua_State *L)
    {
        Material *mat=lua_get_metapointer<Material>(L,1);
        mat->set_const_n(lua_tonumber(L,2));
        
        return 0;
    }

    int set_name(lua_State *L)
    {
        Material *mat=lua_get_metapointer<Material>(L,1);
        mat->name=lua_tostring(L,2);
        
        return 0;
    }

    int set_script(lua_State *L)
    {
        Material *mat=lua_get_metapointer<Material>(L,1);
        mat->load_lua_script(lua_tostring(L,2));
        
        return 0;
    }

    int validity_range(lua_State *L)
    {
        Material *mat=lua_get_metapointer<Material>(L,1);
        
        mat->lambda_valid_min=lua_tonumber(L,2);
        mat->lambda_valid_max=lua_tonumber(L,3);
        
        return 0;
    }
}
