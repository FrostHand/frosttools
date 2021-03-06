#pragma once

template<class Real> class Vector3D;
template<class Real,bool row_order> class Matrix3;

#define TINY 0.0000000005

class Quaternion
{
public:
    float x,y,z,w;

public:

    inline Quaternion()
        : x(0.0f), y(0.0f), z(0.0f), w(1.0f)
    {};
    inline Quaternion(float x_, float y_, float z_, float w_)
        : x(x_), y(y_), z(z_), w(w_)
    {};
    //---------------------------------------------------------------
    inline void set(float x_, float y_, float z_, float w_) {
        x = x_; y = y_; z = z_; w = w_;
    };
    inline void set(const Quaternion& q) {
        x = q.x; y = q.y; z = q.z; w = q.w;
    };

    //-- operations --------------------------------------------
    inline void ident(void) {
        x = 0.0f; y = 0.0f; z = 0.0f; w = 1.0f;
    };

    inline void conjugate(void) {
        x = -x; y = -y; z = -z;
    };

    inline Quaternion conjugated(void) const{
        return Quaternion( -x, -y, -z, w );
    }

    inline void scale(float s) {
        x *= s; y *= s; z *= s; w *= s;
    };

    inline float norm(void) const {
        return x*x + y*y + z*z + w*w;
    };

    inline float magnitude(void) const {
        return (float)sqrt( norm() );
    };

    inline Quaternion inverted(void) const{
        float in = 1.0f / norm();
        return Quaternion( -x*in, -y*in, -z*in, w*in );
    }

    inline void invert(void) {
        float in = 1.0f / norm();
        set( -x*in, -y*in, -z*in, w*in );
    }

    /**
      stabilize quaternion length within 1 - 1/4
      this operation is a lot faster than normalization
      and preserve length goes to 0 or infinity
    */
    inline void stabilize_length() {
       float cs = (float)(fabs(x) + fabs(y) + fabs(z) + fabs(w));
       if( cs > 0.0f )
           set( x/=cs,  y/=cs,  z/=cs,  w/=cs );
       else
           ident();
    }
    /**
        scale quaternion that its norm goes to 1
        the appearing of 0 magnitude or near is a error,
        so we can be sure that can divide by magnitude
    */
    inline void normalize(void) {
        float m = magnitude();
        if( m < 0.0000005 )
        {
            stabilize_length();
            m = magnitude();
        }
        scale( 1.0f/m );
    }

/*
    do we need it ?
    bool isequal( const xxquaternion& q, float delta){
        return ( fabs(q.x - x) < delta ) &&
               ( fabs(q.y - y) < delta ) &&
               ( fabs(q.z - z) < delta ) &&
               ( fabs(q.w - w) < delta );
    }
*/
    //----operators------------------------------------------------
    inline Quaternion operator-()const {
        return Quaternion(-x, -y, -z, -w);
    }

    inline Quaternion operator+(const Quaternion& q)const {
        return Quaternion(x + q.x, y + q.y, z + q.z, w + q.w);
    }

    inline Quaternion operator-(const Quaternion& q)const {
        return Quaternion(x - q.x, y - q.y, z - q.z, w - q.w);
    }

    inline Quaternion operator*(const float& s)const {
        return Quaternion(x*s, y*s, z*s, w*s);
    }

    /**
      q( cross(v,v') + wv' + w'v, ww' - dot(v,v') )
    */
    inline Quaternion operator*(const Quaternion& q)const {
        return Quaternion(w*q.x + x*q.w + y*q.z - z*q.y,
                            w*q.y + y*q.w + z*q.x - x*q.z,
                            w*q.z + z*q.w + x*q.y - y*q.x,
                            w*q.w - x*q.x - y*q.y - z*q.z);
        // 16 multiplications    12 addidtions    0 variables
    }

    /**
      alternative way of quaternion multiplication,
      can speedup multiplication for some systems (PDA for example)
      http://mathforum.org/library/drmath/view/51464.html
      http://www.lboro.ac.uk/departments/ma/gallery/quat/src/quat.ps
      in provided code by url's many bugs, have to be rewriten.
    */
    inline Quaternion mul_alt( const Quaternion& q) const {
        float t0 = (x-y)*(q.y-q.x);
        float t1 = (w+z)*(q.w+q.z);
        float t2 = (w-z)*(q.y+q.x);
        float t3 = (x+y)*(q.w-q.z);
        float t4 = (x-z)*(q.z-q.y);
        float t5 = (x+z)*(q.z+q.y);
        float t6 = (w+y)*(q.w-q.x);
        float t7 = (w-y)*(q.w+q.x);

        float t8 = t5 + t6 + t7;
        float t9 = (t4 + t8)*0.5f;
        return Quaternion ( t3+t9-t6,
                              t2+t9-t7,
                              t1+t9-t8,
                              t0+t9-t5 );
        // 9 multiplications    27  addidtions    8 variables
        // but of couse we can clean 4 variables
/*
        float r = w, i = z, j = y, k =x;
        float br = q.w,  bi = q.z, bj = q.y, bk =q.x;
        float t0 = (k-j)*(bj-bk);
        float t1 = (r+i)*(br+bi);
        float t2 = (r-i)*(bj+bk);
        float t3 = (k+j)*(br-bi);
        float t4 = (k-i)*(bi-bj);
        float t5 = (k+i)*(bi+bj);
        float t6 = (r+j)*(br-bk);
        float t7 = (r-j)*(br+bk);
        float t8 = t5 + t6 + t7;
        float t9 = (t4 + t8)*0.5;
        float rr = t0+t9-t5;
        float ri = t1+t9-t8;
        float rj = t2+t9-t7;
        float rk = t3+t9-t6;
        return xxquaternion ( rk, rj, ri, rr );
*/
    }

    inline const Quaternion& operator+=(const Quaternion& q) {
        x += q.x; y += q.y; z += q.z; w += q.w;
        return *this;
    };

    inline const Quaternion& operator-=(const Quaternion& q) {
        x -= q.x; y -= q.y; z -= q.z; w -= q.w;
        return *this;
    };

    inline const Quaternion& operator*=(const Quaternion& q) {
          set((*this)*q);  // have no optimization here
    };

    //---- usefull methods -------------------------------
    /**
      return rotation angle from -PI to PI
    */
    inline float get_rotation()const{
        if( w < 0 )
            return 2.0f*(float)atan2(-sqrt( x*x + y*y + z*z ), -w );
        else
            return 2.0f*(float)atan2( sqrt( x*x + y*y + z*z ),  w );
    }

    /**
      rotate vector by quaternion
    */

    inline Vector3D<float> rotate(const Vector3D<float>& v)const{
        Quaternion q(v[0] * w + v[2] * y - v[1] * z,
                     v[1] * w + v[0] * z - v[2] * x,
                     v[2] * w + v[1] * x - v[0] * y,
                     v[0] * x + v[1] * y + v[2] * z);

        return Vector3D<float>(w * q.x + x * q.w + y * q.z - z * q.y,
                       w * q.y + y * q.w + z * q.x - x * q.z,
                       w * q.z + z * q.w + x * q.y - y * q.x)*( 1.0f/norm() );

        // 29  multiplications, 20 addidtions, 4 variables
        // 5
       /*
        // refrence implementation
        xxquaternion r = (*this)*xxquaternion(v.x, v.y, v.z, 0)*this->inverted();
        return vector3( r.x, r.y, r.z );
       */

       /*
        // alternative implementation
        float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;
        x2 = q.x + q.x; y2 = q.y + q.y; z2 = q.z + q.z;

        xx = q.x * x2;   xy = q.x * y2;   xz = q.x * z2;
        yy = q.y * y2;   yz = q.y * z2;   zz = q.z * z2;
        wx = q.w * x2;   wy = q.w * y2;   wz = q.w * z2;

        return vector3( v.x  - v.x * (yy + zz) + v.y * (xy - wz) + v.z * (xz + wy),
                        v.y  + v.x * (xy + wz) - v.y * (xx + zz) + v.z * (yz - wx),
                        v.z  + v.x * (xz - wy) + v.y * (yz + wx) - v.z * (xx + yy) )*( 1.0f/norm() );
        // 18 multiplications, 21 addidtions, 12 variables
       */
    };
   //-----------------------------------
    /**
      create a unit quaternion from axis angle representation
    */
    inline void unit_from_axis_angle(const Vector3D<float>& axis, const float& angle){
        Vector3D<float> v(axis);
        v.normalise();
        float half_angle = angle*0.5f;
        float sin_a = (float)sin(half_angle);
        set(v[0]*sin_a, v[1]*sin_a, v[2]*sin_a, (float)cos(half_angle));
    };
    //-----------------------------------
    /**
      convert a quaternion to axis angle representation,
      preserve the axis direction and angle from -PI to +PI
    */
    inline void to_axis_angle(Vector3D<float>& axis, float& angle)const {
        float vl = (float)sqrt( x*x + y*y + z*z );
        if( vl > TINY )
        {
            float ivl = 1.0f/vl;
            axis=Vector3D<float>( x*ivl, y*ivl, z*ivl );
            if( w < 0 )
                angle = 2.0f*(float)atan2(-vl, -w); //-PI,0
            else
                angle = 2.0f*(float)atan2( vl,  w); //0,PI
        }else{
            axis = Vector3D<float>(0,0,0);
            angle = 0;
        }
    };
    //-----------------------------------
    /**
       create a unit quaternion by rotation matrix
       martix must contain only rotation (not scale or shear)

       For numerical stability we find first the greatest component of quaternion
       and than search others from this one
    */
    inline void unit_from_matrix( const Matrix3<float,true>& mtx )
    {
        typedef float mtx_elm[3][3];
        //const mtx_elm& m = mtx.c;
        float tr = mtx(0,0) + mtx(1,1) + mtx(2,2); // trace of martix
        if (tr > 0.0f){                         // if trace positive than "w" is biggest component
            set( mtx(1,2) - mtx(2,1), mtx(2,0) - mtx(0,2), mtx(0,1) - mtx(1,0), tr+1.0f );
            scale( 0.5f/(float)sqrt( w ) );     // "w" contain the "norm * 4"

        }else                                   // Some of vector components is bigger
        if( (mtx(0,0) > mtx(1,1) ) && ( mtx(0,0) > mtx(2,2)) ) {
            set( 1.0f + mtx(0,0) - mtx(1,1) - mtx(2,2), mtx(1,0) + mtx(0,1),
                 mtx(2,0) + mtx(0,2), mtx(1,2) - mtx(2,1) );
            scale( 0.5f/(float)sqrt( x ) );

        }else
        if ( mtx(1,1) > mtx(2,2) ){
            set( mtx(1,0) + mtx(0,1), 1.0f + mtx(1,1) - mtx(0,0) - mtx(2,2),
                 mtx(2,1) + mtx(1,2), mtx(2,0) - mtx(0,2) );
            scale( 0.5f/(float)sqrt( y ) );

        }else{
            set( mtx(2,0) + mtx(0,2), mtx(2,1) + mtx(1,2),
                 1.0f + mtx(2,2) - mtx(0,0) - mtx(1,1), mtx(0,1) - mtx(1,0) );
            scale( 0.5f/(float)sqrt( z ) );

        }
    }
    //----------------------------------------------------------------------------
    /**
       create a nonunit quaternion from rotation matrix
       martix must contain only rotation (not scale or shear)
       the result quaternion length is numerical stable
    */
    inline void from_matrix( const Matrix3<float,true> & mtx )
    {
        typedef float mtx_elm[3][3];
       // const mtx_elm& m = mtx.c;
		/*
        float tr = mtx(0,0) + mtx(1,1) + mtx(2,2); // trace of martix
        if (tr > 0.0f){                         // if trace positive than "w" is biggest component
            set( mtx(1,2)) - m[2][1], m[2][0] - m[0][2], m[0][1] - m[1][0], tr + 1.0f );
        }else                                   // Some of vector components is bigger
        if( (m[0][0] > m[1][1] ) && ( m[0][0] > m[2][2]) ) {
            set( 1.0f + m[0][0] - m[1][1] - m[2][2], m[1][0] + m[0][1],
                 m[2][0] + m[0][2], m[1][2] - m[2][1] );
        }else
        if ( m[1][1] > m[2][2] ){
            set( m[1][0] + m[0][1], 1.0f + m[1][1] - m[0][0] - m[2][2],
                 m[2][1] + m[1][2], m[2][0] - m[0][2] );
        }else{
            set( m[2][0] + m[0][2], m[2][1] + m[1][2],
                 1.0f + m[2][2] - m[0][0] - m[1][1], m[0][1] - m[1][0] );
        }*/
    }
    //----------------------------------------------------------------------------
    /**
      set the rotation to matrix
    */
    inline void to_matrix( Matrix3<float,true>& m  )const  {
        float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;
        float s  = 2.0f/norm();
        x2 = x * s;    y2 = y * s;    z2 = z * s;
        xx = x * x2;   xy = x * y2;   xz = x * z2;
        yy = y * y2;   yz = y * z2;   zz = z * z2;
        wx = w * x2;   wy = w * y2;   wz = w * z2;
/*
        m.c[0][0] = 1.0f - (yy + zz);
        m.c[1][0] = xy - wz;
        m.c[2][0] = xz + wy;

        m.c[0][1] = xy + wz;
        m.c[1][1] = 1.0f - (xx + zz);
        m.c[2][1] = yz - wx;

        m.c[0][2] = xz - wy;
        m.c[1][2] = yz + wx;
        m.c[2][2] = 1.0f - (xx + yy);
*/
		m(0,0) = 1.0f - (yy + zz);
        m(1,0) = xy - wz;
        m(2,0) = xz + wy;

        m(0,1) = xy + wz;
        m(1,1) = 1.0f - (xx + zz);
        m(2,1) = yz - wx;

        m(0,2) = xz - wy;
        m(1,2) = yz + wx;
        m(2,2) = 1.0f - (xx + yy);
        //1 division    16 multiplications    15 addidtions    12 variables
    }
    //-------------------------------------------------
    /**
      set the rotation from unit quat to matrix
    */
    inline void unit_to_matrix( Matrix3<float,true>& m  )const  {
        float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;
        x2 = x + x;    y2 = y + y;    z2 = z + z;
        xx = x * x2;   xy = x * y2;   xz = x * z2;
        yy = y * y2;   yz = y * z2;   zz = z * z2;
        wx = w * x2;   wy = w * y2;   wz = w * z2;

        m(0,0) = 1.0f - (yy + zz);
        m(1,0) = xy - wz;
        m(2,0) = xz + wy;

        m(0,1) = xy + wz;
        m(1,1) = 1.0f - (xx + zz);
        m(2,1) = yz - wx;

        m(0,2) = xz - wy;
        m(1,2) = yz + wx;
        m(2,2) = 1.0f - (xx + yy);
    }
    //-----------------------------------------------------------
    /**
     shortest arc quaternion rotate one vector to another by shortest path.
     create rotation from -> to, for any length vectors.
    */
    inline void shortest_arc(const Vector3D<float>& from, const Vector3D<float>& to )
    {

        Vector3D<float> c( from ^ to );
        set( c[0], c[1], c[2], from & to );
        normalize();    // if "from" or "to" not unit, normalize quat
        w += 1.0f;      // reducing angle to halfangle
        if( w <= TINY ) // angle close to PI
        {
            if( ( from[2]*from[2] ) > ( from[0]*from[0] ) )
                set( 0, from[2], - from[1], w ); //from*vector3(1,0,0)
            else
                set( from[1], - from[0], 0, w ); //from*vector3(0,0,1)
        }
        normalize();
    }

};
//--- global operators ----------------------------------------------
static inline float inner_product(const Quaternion& q0,const Quaternion& q1) {
    return q0.x*q1.x + q0.y*q1.y + q0.z*q1.z + q0.w*q1.w;
}

/**
  return some q' that have property
  q'*q' = q

  it is experimental !!!
  have to be carefully checked !!
*/
static inline Quaternion sqrt(const Quaternion& q ){
    float m = q.magnitude();
    float w = q.w + m;
    return Quaternion(q.x, q.y, q.z, w )*(float)sqrt(0.5f/w);
}
//-------------------------------------------------------------------
/**
  perform linear interpolation (approximate rotation interpolation) of two unit quats,
  result quaternion nonunit, its length lay between. sqrt(2)/2  and 1.0
*/
inline Quaternion lerp_unit(const Quaternion& q1, const Quaternion& q2, float t ){
    float inner = inner_product( q1, q2 );
    if(inner < 0 ){
        return q1 - (q2+q1)*t;
    }else{
        return q1 + (q2-q1)*t;
    }
}
//-------------------------------------------------------------------
/**
  class perform linear interpolation (approximate rotation interpolation),
  result quaternion nonunit, its length lay between. sqrt(2)/2  and 1.0
*/
class q_lerper
{
    Quaternion  q1, q2;
public:

    inline void setup_from_unit(const Quaternion& q1_, const Quaternion& q2_){
        q1 =  q1_;
        q2 =  q2_;
        float inner = inner_product( q1, q2 );
        if(inner < 0 ) q2 = -q2;
        q2 -= q1;

    }

    inline void setup(const Quaternion& q1_, const Quaternion& q2_){
        q1 =  q1_;     q1.normalize();
        q2 =  q2_;     q2.normalize();
        setup_from_unit( q1, q2);
    }

    inline void interpolate( float t, Quaternion& result_q){
          result_q = q1 + q2*t;
    }
};
//-------------------------------------------------------------------
/**
 Perform Spherical Linear Interpolation of the quaternions,
 return unit length quaternion
*/
class q_slerper
{
    Quaternion q1, q2;
    float omega;

public:

    inline void setup_from_unit(const Quaternion& q1_, const Quaternion& q2_){
        q1 = q1_ ;
        q2 = q2_ ;
        float cos_omega = inner_product(q1, q2);
        if(cos_omega < 0 )
        {
            cos_omega = -cos_omega;
            q2 = -q2;
        }
        if(cos_omega > 0.9999f ) cos_omega = 0.9999f;
        omega = (float)acos( cos_omega );
        float inv_sin_omega = (float)(1.0/sin( omega ));
        q1.scale(inv_sin_omega);
        q2.scale(inv_sin_omega);
    }

    inline void setup(const Quaternion& q1_, const Quaternion& q2_){
        q1 = q1_;
        q1.normalize();
        q2 = q2_;
        q2.normalize();
        setup_from_unit(q1, q2);
    }

    inline void interpolate( float t, Quaternion& result_q ){
        result_q = q1*(float)sin( (1.0 - t)*omega ) + q2*(float)sin( t*omega );
    }
};
//-------------------------------------------------------------------------------
//**************************** Quaternion Utilites ******************************
//-------------------------------------------------------------------------------
/**
 help function for arcball
 orthogonal projection on sphere of radius 1,
 standing in (x = 0, y = 0)
*/
inline Vector3D<float> ortho_project_on_sphere( float x , float y )
{
    Vector3D<float> p(x, y, 0);
    float ls = p & p;//.len_squared();
    if ( ls >= 1.0f )
        p.normalise();
    else
	    p[2] = (float)sqrt(1.0f - ls);
    return p;
}
//-------------------------------------------------------------------------------
/*
  calculate rotation of arcball user input,
  used to perform object rotation by mouse.
  "from" and "to" the mouse on screen coordinates (with - x,y on screen)
  in range of -1 to +1
  the arcball radius is 1.0 and it stand in a center (x = 0, y = 0)
*/
inline Quaternion arcball(const Vector3D<float>& from, const Vector3D<float>& to)
{
    Vector3D<float> p_f = ortho_project_on_sphere(from[0], from[1] );
    Vector3D<float> p_t = ortho_project_on_sphere(to[0], to[1] );
    Vector3D<float> c   = p_f ^ p_t;
    float   d   = p_f & p_t;
    return  Quaternion(c[0], c[1], c[2], d);
}
//--------------------------------------------------
/**
 rotate as a half arcball, more realistic manner (like 3dmax).
*/
inline Quaternion trackball( const Vector3D<float>& from, const Vector3D<float>& to )
{
    Quaternion rot;
    rot.shortest_arc( ortho_project_on_sphere(from[0], from[1] ),
                      ortho_project_on_sphere(to[0], to[1] ) );
    return rot;
}
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
/**
   Decompose the rotation on to 2 parts.
   1. Twist - rotation around the "direction" vector
   2. Swing - rotation around axis that is perpendicular to "direction" vector
   The rotation can be composed back by
   rotation = swing * twist

   have singularity in case of swing_rotation close to 180 degrees rotation.
   input quaternion can be nonunit length, in that case output can be nonunit
   if input quaternion unit, output is unit both
*/
inline void swing_twist_decomposition( const Quaternion& rotation,
                                       const Vector3D<float>&      direction,
                                       Quaternion&       swing,
                                       Quaternion&       twist)
{
    Vector3D<float> ra( rotation.x, rotation.y, rotation.z ); // rotation axis
    Vector3D<float> p = vecProject( ra, direction ); // return projection v1 on to v2  (parallel component)
    twist.set( p[0], p[1], p[2], rotation.w );
    twist.normalize();
    swing = rotation * twist.conjugated();
}
//-------------------------------------------------------------------------------

