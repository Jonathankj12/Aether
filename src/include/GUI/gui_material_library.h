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

#ifndef GUI_MATERIAL_LIBRARY_H_INCLUDED
#define GUI_MATERIAL_LIBRARY_H_INCLUDED

#include <material.h>
#include <lua_base.h>

class MaterialsLib;
class MaterialsManager;

enum class MatType
{
    EFFECTIVE,
    REAL_N,
    LIBRARY,
    SCRIPT,
    USER_LIBRARY,
    CUSTOM
};

namespace GUI
{
    class Material: public ::Material
    {
        public:
            MatType type;
            
            std::string get_description();
            std::string get_inline_lua();
            double get_lambda_validity_min();
            double get_lambda_validity_max();
            //[[deprecated]] std::string get_lua(std::string const &script_name);
            std::string get_short_description();
            //[[deprecated]] void write_lua_script();
        
        private:
            //void stream_lua(std::ostream &strm, std::string const &prefix);
        
        friend MaterialsLib;
    };
}

namespace lua_gui_material
{
    void create_metatable(lua_State *L);
    int allocate(lua_State *L);
    int lua_material_set_index(lua_State *L);
    int lua_material_set_script(lua_State *L);
    
    class Translator
    {
        public:
            
            Translator(std::filesystem::path const &relative_path);
            
            void gather(GUI::Material *material);
            std::string get_header();
            std::string operator() (GUI::Material *material) const;
            void save_to_file(GUI::Material *material);
            
        private:
            bool finalized;
            std::filesystem::path const &relative_path;
            std::vector<GUI::Material*> materials;
            
            std::string header;
            std::map<GUI::Material*,std::string> name_map;
            
            void finalize();
            void to_lua(GUI::Material *material,
                        std::ostream &strm,
                        std::string const &prefix);
    };
}



bool default_material_validator(Material *material);

class MaterialsLibDialog: public wxDialog
{
    public:
        bool selection_ok;
        GUI::Material *material;
        wxTreeCtrl *materials;
        
        bool (*accept_material)(Material*); // Validator
        
        MaterialsLibDialog(bool (*validator)(Material*)=&default_material_validator);
        
        void evt_add_to_lib(wxCommandEvent &event);
        void evt_cancel(wxCommandEvent &event);
        void evt_load_script(wxCommandEvent &event);
        void evt_ok(wxCommandEvent &event);
        void rebuild_tree();
};

class MaterialsLib
{
    private:
        static MaterialsManager *manager;
        static std::vector<GUI::Material*> data;
        
        static bool has_manager();
        static void load_material(std::filesystem::path const &fname,MatType type);
        static void write_user_lib();
        static void reorder_materials();
    public:
        static void add_material(std::filesystem::path const &fname);
        [[deprecated]] static void add_to_library(GUI::Material *data);
        static void consolidate();
        static void consolidate(GUI::Material *material);
        static void forget_manager();
        static MaterialsManager* get_manager();
        static GUI::Material* get_material_data(unsigned int n);
        static std::filesystem::path get_material_path(unsigned int n);
        static MatType get_material_type(unsigned int n);
        static std::size_t get_N_materials();
        static void initialize();
        static Material* knows_material(unsigned int &n,Material const &material,
                                        bool (*validator)(Material*)=&default_material_validator);
        static void load_script(std::filesystem::path const &path);
        [[nodiscard]] static GUI::Material* request_material(MatType type);
};

#endif // GUI_MATERIAL_LIBRARY_H_INCLUDED
