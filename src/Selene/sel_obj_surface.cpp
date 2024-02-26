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

#include <selene.h>

extern std::ofstream plog;

extern const Vector3 unit_vec_x;
extern const Vector3 unit_vec_y;
extern const Vector3 unit_vec_z;

namespace Sel
{
    //###################
    //   Conic Section
    //###################

    void Object::set_conic_section()
    {
        type = OBJ_CONIC;
        
        conic.finalize();
        NFc = F_arr.size();
    }

//##########
//   Disk
//##########

void Object::set_disk()
{
    type=OBJ_DISK;
    
    disk.finalize();
    NFc=F_arr.size();
}

void Object::set_disk(double radius_,double in_radius_)
{
    disk.radius=radius_;
    disk.in_radius=in_radius_;
    
    set_disk();
}





//##############
//   Parabola
//##############

void Object::set_parabola()
{
    type=OBJ_PARABOLA;
    
    parabola.finalize();
    NFc = F_arr.size();
}


void Object::set_parabola(double focal_length_,double in_radius_,double length_)
{
    parabola.pr_f=focal_length_;
    parabola.pr_in_radius=in_radius_;
    parabola.pr_length=length_;
    
    set_parabola();
}


//###############
//   Rectangle
//###############

void Object::intersect_rectangle(SelRay const &ray,std::vector<RayInter> &interlist,int face_last_intersect,bool first_forward)
{
    std::array<double,1> hits;
    std::array<int,1> face_labels={0};
    
    if(!ray_inter_plane_x(ray.start,ray.dir,0,hits[0])) hits[0]=-1;
    else
    {
        Vector3 P=ray.start+hits[0]*ray.dir;
        
        if(std::abs(P.y)>box.ly/2.0 || std::abs(P.z)>box.lz/2.0)
            hits[0]=-1;
    }
    
    if(first_forward)
        push_first_forward(interlist,ray,obj_ID,hits,face_labels);
    else
        push_full_forward(interlist,ray,obj_ID,hits,face_labels);
}

Vector3 Object::normal_rectangle(RayInter const &inter)
{
    return -unit_vec_x;
}

Vector3 Object::tangent_rectangle(RayInter const &inter,Vector3 const &normal,bool up)
{
    Sel::SelFace &face=F_arr[0];
    
    int tangent_type;
    Vector3 tangent;
    
    if(up)
    {
        tangent_type=face.tangent_up;
        tangent=face.fixed_tangent_up;
    }
    else
    {
        tangent_type=face.tangent_down;
        tangent=face.fixed_tangent_down;
    }
    
         if(tangent_type==TANGENT_UNSET) return Vector3(0);
    else if(tangent_type==TANGENT_FIXED)
    {
        tangent=tangent-normal*scalar_prod(normal,tangent);
        tangent.normalize();
    }
    else
    {
        switch(tangent_type)
        {
        }
    }
    
    return tangent;
}

void Object::set_rectangle()
{
    type=OBJ_RECTANGLE;
    
    NFc=1;
    F_arr.resize(NFc);
    
    double span=std::max(box.ly,box.lz);
    
    bbox.xm=-0.05*span;
    bbox.xp=+0.05*span;
    
    bbox.ym=-1.1*box.ly/2.0;
    bbox.yp=+1.1*box.ly/2.0;
    
    bbox.zm=-1.1*box.lz/2.0;
    bbox.zp=+1.1*box.lz/2.0;
}

void Object::set_rectangle(double ly_,double lz_)
{
    box.ly=ly_;
    box.lz=lz_;
    
    set_rectangle();
}

void Object::xyz_to_uv_rectangle(double &u,double &v,int face_,double x,double y,double z)
{
    u=0.5+y/box.ly;
    v=0.5+z/box.lz;
}

void Object::default_N_uv_rectangle(int &Nu,int &Nv,int face_)
{
    double delta=std::max(box.ly,box.lz)/64.0;
    
    Nu=nearest_2np1(box.ly/delta);
    Nv=nearest_2np1(box.lz/delta);
}

//#####################
//   Spherical Patch
//#####################

void Object::intersect_spherical_patch(SelRay const &ray,std::vector<RayInter> &interlist,int face_last_intersect,bool first_forward)
{
    std::array<double,2> hits;
    std::array<int,2> face_labels={0,0};
    
    if(!ray_inter_sphere(ray.start,ray.dir,sph_r,hits[0],hits[1]))
    {
        hits[0]=-1;
        hits[1]=-1;
    }
    
    Vector3 P=ray.start+hits[0]*ray.dir;
    if(P.x<sph_cut_x) hits[0]=-1;
    
    P=ray.start+hits[1]*ray.dir;
    if(P.x<sph_cut_x) hits[1]=-1;
    
    if(first_forward)
        push_first_forward(interlist,ray,obj_ID,hits,face_labels);
    else
        push_full_forward(interlist,ray,obj_ID,hits,face_labels);
}

Vector3 Object::normal_spherical_patch(RayInter const &inter)
{
    Vector3 Fnorm=Vector3(inter.obj_x,inter.obj_y,inter.obj_z);
    
    Fnorm.normalize();
    
    return Fnorm;
}

void Object::set_spherical_patch()
{
    sph_cut_x=sph_r*(1.0-2.0*sph_cut);
    sph_cut_th=std::acos(1.0-2.0*sph_cut);
    sph_cut_rad=sph_r*std::sin(sph_cut_th);
    
    type=OBJ_SPHERE_PATCH;
    
    NFc=1;
    F_arr.resize(NFc);
    
    bbox.xm=-1.1*sph_r;
    bbox.xp=+1.1*sph_r;
    
    bbox.ym=-1.1*sph_r;
    bbox.yp=+1.1*sph_r;
    
    bbox.zm=-1.1*sph_r;
    bbox.zp=+1.1*sph_r;
    
    face_name_arr.resize(NFc);
    face_name_arr[0]="Sphere";
}

void Object::set_spherical_patch(double radius,double slice)
{
    sph_r=radius;
    sph_cut=slice;
    
    set_spherical_patch();
}

void Object::xyz_to_uv_spherical_patch(double &u,double &v,int face_,double x,double y,double z)
{
    double phi=std::atan2(z,y);
    double th=std::atan2(std::sqrt(y*y+z*z),x);
    
    u=th*std::cos(phi)/sph_cut_th;
    v=th*std::sin(phi)/sph_cut_th;
    
    u=u/2.0+0.5;
    v=v/2.0+0.5;
}

void Object::default_N_uv_spherical_patch(int &Nu,int &Nv,int face_)
{
    Nu=Nv=33;
}

}
