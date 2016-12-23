#pragma once
#include <Urho3D/Urho3DAll.h>

using namespace Urho3D;

class CharacterController : public LogicComponent {
	URHO3D_OBJECT(CharacterController, LogicComponent);
public:
	CharacterController(Context* context);
	~CharacterController();
	
	static void RegisterObject(Context* context) {
		context->RegisterFactory<CharacterController>();
	}

	virtual void Start();
	virtual void FixedUpdate(float timeStep);

	void HandleCamera(StringHash eventType, VariantMap & eventData);
	void HandleNodeCollision(StringHash eventType, VariantMap& eventData);
	void HandleKeyDown(StringHash eventType, VariantMap& eventData);
	void HandleKeyUp(StringHash eventType, VariantMap& eventData);

	/// Gives the parent node a Collider/Rigid body/Camera
	void SetupNode(float height);

	float GetCrouchHeight(float x);

	bool inCrouchAnim_ = false;
	bool crouching_ = false;
	float timeSinceCrouch_;
	float crouchOffSet_ = 1.0f;
	float height_;

	bool sprinting_ = false;
	bool onGround_;
	bool okToJump_;
	float inAirTimer_;

	float jumpForce_;
	float moveForce_;
	float forwardsMoveForce_;



	float mouseSensitivity = 0.05f; /// TODO: Load this from settings.
	float camPitch = 0.0f;
	float camYaw = 0.0f;

	Controls controls_;

	RigidBody* rigidBody_;
	CollisionShape* collider_;
	CollisionShape* footCollider_;

	Node* cameraNode_;
	Camera* camera_;

	VariantMap controlSettings; // TODO: Implement control settings.

private:


protected:



};