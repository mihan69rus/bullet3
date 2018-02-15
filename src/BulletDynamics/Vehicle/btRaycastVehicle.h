/*
 * Copyright (c) 2005 Erwin Coumans http://continuousphysics.com/Bullet/
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies.
 * Erwin Coumans makes no representations about the suitability 
 * of this software for any purpose.  
 * It is provided "as is" without express or implied warranty.
*/
#ifndef BT_RAYCASTVEHICLE_H
#define BT_RAYCASTVEHICLE_H

#include "BulletDynamics/Dynamics/btRigidBody.h"
#include "BulletDynamics/ConstraintSolver/btTypedConstraint.h"
#include "btVehicleRaycaster.h"
class btDynamicsWorld;
#include "LinearMath/btAlignedObjectArray.h"
#include "btWheelInfo.h"
#include "BulletDynamics/Dynamics/btActionInterface.h"

//class btVehicleTuning;

struct btRaycastVehicleData
{
    btActionInterfaceData base; // this needs to be first so that the object may be cast to a btActionInterfaceData to read the type
    char m_pad[4]; // keep things 8-byte aligned
    void *m_chassisBody;
    int m_indexRightAxis;
    int m_indexUpAxis;
    int	m_indexForwardAxis;
    char m_pad2[4]; // keep things 8-byte aligned
};

///rayCast vehicle, very special constraint that turn a rigidbody into a vehicle.
class btRaycastVehicle : public btActionInterface
{
	// though the contents of these 4 variables are reset on every call to updateVehicle,
	// keeping them as part of the class means memory will not be deallocated and reallocated
	// on every call to that function
	btAlignedObjectArray<btVector3>	m_forwardWS;
	btAlignedObjectArray<btVector3>	m_axle;
	btAlignedObjectArray<btScalar>	m_forwardImpulse;
	btAlignedObjectArray<btScalar>	m_sideImpulse;
	
		///backwards compatibility
		int	m_userConstraintType;
		int	m_userConstraintId;

public:
	class btVehicleTuning
		{
			public:

			btVehicleTuning()
				:m_suspensionStiffness(btScalar(5.88)),
				m_suspensionCompression(btScalar(0.83)),
				m_suspensionDamping(btScalar(0.88)),
				m_maxSuspensionTravelCm(btScalar(500.)),
				m_frictionSlip(btScalar(10.5)),
				m_maxSuspensionForce(btScalar(6000.))
			{
			}
			btScalar	m_suspensionStiffness;
			btScalar	m_suspensionCompression;
			btScalar	m_suspensionDamping;
			btScalar	m_maxSuspensionTravelCm;
			btScalar	m_frictionSlip;
			btScalar	m_maxSuspensionForce;

		};
private:

	btVehicleRaycaster*	m_vehicleRaycaster;
	btScalar m_currentVehicleSpeedKmHour;

	btRigidBody* m_chassisBody;

	int m_indexRightAxis;
	int m_indexUpAxis;
	int	m_indexForwardAxis;
public:

	//constructor to create a car from an existing rigidbody
	btRaycastVehicle(const btVehicleTuning& tuning,btRigidBody* chassis,	btVehicleRaycaster* raycaster );

	virtual ~btRaycastVehicle() ;


	///btActionInterface interface
	virtual void updateAction( btCollisionWorld* collisionWorld, btScalar step)
	{
        (void) collisionWorld;
		updateVehicle(step);
	}
	

	///btActionInterface interface
	void	debugDraw(btIDebugDraw* debugDrawer);
			
	const btTransform& getChassisWorldTransform() const;
	
	btScalar rayCast(btWheelInfo& wheel);

	virtual void updateVehicle(btScalar step);
	
	
	void resetSuspension();

	btScalar	getSteeringValue(int wheel) const;

	void	setSteeringValue(btScalar steering,int wheel);


	void	applyEngineForce(btScalar force, int wheel);

	const btTransform&	getWheelTransformWS( int wheelIndex ) const;

	void	updateWheelTransform( int wheelIndex, bool interpolatedTransform = true );
	
//	void	setRaycastWheelInfo( int wheelIndex , bool isInContact, const btVector3& hitPoint, const btVector3& hitNormal,btScalar depth);

	btWheelInfo&	addWheel( const btVector3& connectionPointCS0, const btVector3& wheelDirectionCS0,const btVector3& wheelAxleCS,btScalar suspensionRestLength,btScalar wheelRadius,const btVehicleTuning& tuning, bool isFrontWheel);
    void addWheel(btWheelInfo * wheelInfo);

	inline int		getNumWheels() const {
		return int (m_wheelInfo.size());
	}
	
	btAlignedObjectArray<btWheelInfo>	m_wheelInfo;


	const btWheelInfo&	getWheelInfo(int index) const;

	btWheelInfo&	getWheelInfo(int index);

	void	updateWheelTransformsWS(btWheelInfo& wheel , bool interpolatedTransform = true);

	
	void setBrake(btScalar brake,int wheelIndex);
	
	void	updateSuspension(btScalar deltaTime);

	virtual void	updateFriction(btScalar	timeStep);



	inline btRigidBody* getRigidBody()
	{
		return m_chassisBody;
	}

	const btRigidBody* getRigidBody() const
	{
		return m_chassisBody;
	}

	inline int	getRightAxis() const
	{
		return m_indexRightAxis;
	}
	inline int getUpAxis() const
	{
		return m_indexUpAxis;
	}

	inline int getForwardAxis() const
	{
		return m_indexForwardAxis;
	}

	
	///Worldspace forward vector
	btVector3 getForwardVector() const
	{
		const btTransform& chassisTrans = getChassisWorldTransform(); 

		btVector3 forwardW ( 
			  chassisTrans.getBasis()[0][m_indexForwardAxis], 
			  chassisTrans.getBasis()[1][m_indexForwardAxis], 
			  chassisTrans.getBasis()[2][m_indexForwardAxis]); 

		return forwardW;
	}

	///Velocity of vehicle (positive if velocity vector has same direction as foward vector)
	btScalar	getCurrentSpeedKmHour() const
	{
		return m_currentVehicleSpeedKmHour;
	}

	virtual void	setCoordinateSystem(int rightIndex,int upIndex,int forwardIndex)
	{
		m_indexRightAxis = rightIndex;
		m_indexUpAxis = upIndex;
		m_indexForwardAxis = forwardIndex;
	}


	///backwards compatibility
	int getUserConstraintType() const
	{
		return m_userConstraintType ;
	}

	void	setUserConstraintType(int userConstraintType)
	{
		m_userConstraintType = userConstraintType;
	};

	void	setUserConstraintId(int uid)
	{
		m_userConstraintId = uid;
	}

	int getUserConstraintId() const
	{
		return m_userConstraintId;
	}

	virtual const char * serialize(void * dataBuffer, btSerializer * serializer) const;
	void deserialize(void * dataBuffer);
	size_t calculateSerialBufferSize() const {return sizeof(btRaycastVehicleData);}

};

class btDefaultVehicleRaycaster : public btVehicleRaycaster
{
	btDynamicsWorld*	m_dynamicsWorld;
public:
	btDefaultVehicleRaycaster(btDynamicsWorld* world)
		:m_dynamicsWorld(world)
	{
	}

	virtual void* castRay(const btVector3& from,const btVector3& to, btVehicleRaycasterResult& result);

};


#endif //BT_RAYCASTVEHICLE_H

