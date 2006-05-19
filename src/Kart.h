//  $Id: Kart.h,v 1.6 2005/09/30 16:45:04 joh Exp $
//
//  SuperTuxKart - a fun racing game with go-kart
//  Copyright (C) 2004 Steve Baker <sjbaker1@airmail.net>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#ifndef HEADER_KART_H
#define HEADER_KART_H

#include "Moveable.h"
#include "KartControl.h"
#include "ParticleSystem.h"
#include "KartProperties.h"
#include "Attachment.h"
#include "Collectable.h"


class SkidMark;
class Kart;
class Herring;

class KartParticleSystem : public ParticleSystem
{
public:
  Kart* kart;

  KartParticleSystem ( Kart* kart, int num, float _create_rate,
      int _turn_to_face, float sz, float bsphere_size);

  virtual void update ( float t ) ;
  virtual void particle_create( int index, Particle* p );
  virtual void particle_update( float deltaTime, int index, Particle *p );
  virtual void particle_delete( int index, Particle* p );
};

class Kart : public Moveable {
protected:
  Attachment   attachment;
  Collectable  collectable;

  int          grid_position;
  int          racePosition;
  bool         powersliding;
  KartControl  controls;           // The position of the karts controlls 
  unsigned int trackHint;          // index in driveline                  
  float        ZipperTimeLeft;
  sgVec2       last_track_coords;
  sgVec2       curr_track_coords;
  float        prevAccel;          // acceleration at previous time step
  bool         skidding;           // true if the kart is currently skidding

private:
  int                 num_herring_gobbled;
  ssgSimpleState*     smokepuff;
  // don't delete the following 2 vars (they're kids in the hirarchy)
  KartParticleSystem* smoke_system;
  ssgTransform*       exhaust_pipe;

  float               wheel_position;
  ssgTransform*       wheel_front_l;
  ssgTransform*       wheel_front_r;
  ssgTransform*       wheel_rear_l;
  ssgTransform*       wheel_rear_r;

  SkidMark*           skidmark_left;
  SkidMark*           skidmark_right;
  
  int                 raceLap;             // number of finished(!) laps
  int                 finishingPosition;    // saves the end rank
 protected:
  int                 rescue;

  /** Search the given branch of objects that match the wheel names
      and if so assign them to wheel_* variables */
  void  load_wheels          (ssgBranch* obj);
    
public:
  const KartProperties *kartProperties;

  Kart(const KartProperties* kartProperties_, int position_ ) ;
  virtual ~Kart();

  void load_data();

  virtual void placeModel ();
  const KartProperties* getKartProperties() const 
                                        { return kartProperties; }
  void           setKartProperties   (const KartProperties *kp) 
                                        { kartProperties=kp;}
  void           attach              (attachmentType attachment_, float time)
                                        { attachment.set(attachment_, time);}
  void           gotZipper           (float angle, float time)
                                        { wheelie_angle=angle; 
					  ZipperTimeLeft=time;              }
  void           setCollectable      (collectableType t, int n) 
                                        { collectable.set(t, n);            }
  void           setPosition         (int p)    {racePosition = p;          }
  float          getDistanceDownTrack() { return curr_track_coords[1];      }
  attachmentType getAttachment       () { return  attachment.getType();     }
  void           setAttachmentType   (attachmentType t)
                                        { attachment.set(t);                }
  Collectable   *getCollectable      () { return &collectable;              }
  int            getNumCollectables  () { return  collectable.getNum();     }
  int            getNumHerring       () { return  num_herring_gobbled;      }
  int            getLap              () { return  raceLap;                  }
  int            getFinishingPosition() { return  finishingPosition;        }
  int            getPosition         () { return  racePosition ;            }
  float          getSteerAngle() const  { return  velocity.hpr[0];          }
  void           handleRescue        ();
  void           beginPowerslide     ();
  void           endPowerslide       ();
  void           processSkidMarks    ();
  void           getClosestKart      (float *cdist, int *closest);
  void           handleMagnet        (float cdist, int closest);
  void           doZipperProcessing  (float dt);
  void           updatePhysics       (float dt);
  float          NormalizedLateralForce(float alpha, float corner) const;

  // Functions to access the current kart properties (which might get changed,
  // e.g. mass increase or air_friction increase depending on attachment etc.)
  // -------------------------------------------------------------------------
  const sgVec3*  getColour        () const {return &kartProperties->color;     }
  float          getMass          () const {return kartProperties->mass
                                                 + attachment.WeightAdjust();  }
  float          getAirFriction   () const {return kartProperties->air_friction
                                                 + attachment.AirFrictAdjust();}
  float          getRollResistance() const {return kartProperties->roll_resistance;}
  float          getMaxPower      () const {return kartProperties->engine_power;}
  float          getBrakeFactor   () const {return kartProperties->brake_factor;}
  float          getWheelBase     () const {return kartProperties->wheel_base; }
  float          getHeightCOG     () const {return kartProperties->heightCOG;  }
  float          getTireGrip      () const {return kartProperties->tire_grip;  }
  float          getMaxSteerAngle () const {return kartProperties->max_steer_angle;}
  float          getCornerStiffF  () const {return kartProperties->corn_f;     }
  float          getCornerStiffR  () const {return kartProperties->corn_r;     }
  float          getInertia       () const {return kartProperties->inertia;    }
  float _lateralForce(float cornering, float sideslip);
  virtual void   collectedHerring    (Herring* herring);
  virtual void   reset               ();
  virtual void   handleZipper        ();
  virtual void   forceCrash          ();
  virtual void   doObjectInteractions();
  virtual void   doLapCounting       ();
  virtual void   doCollisionAnalysis (float dt, float hot );
  virtual void   update              (float dt );
  virtual void   OutsideTrack        (int isReset) {rescue=true;}
  
} ;

class TrafficDriver : public Kart {
public:
  TrafficDriver (const KartProperties* kartProperties_, sgVec3 _pos )
    : Kart (kartProperties_, 0 )
  {
    sgCopyVec3 ( reset_pos.xyz, _pos ) ;
    reset () ;
  }
  virtual void doObjectInteractions () ;
  virtual void doLapCounting        () ;
  virtual void doZipperProcessing   () ;
  virtual void update (float delta) ;
  
} ;

#ifdef JH
class NetworkKartDriver : public Kart {
public:
  NetworkKartDriver ( int _pos, ssgTransform *m ) : Kart ( _pos, m )
  {
  }

  virtual void update () ;
} ;


#endif

void print_model(ssgEntity* entity, int indent, int maxLevel);

#endif

/* EOF */
  
