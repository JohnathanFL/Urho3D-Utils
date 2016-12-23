#include "CharacterController.hpp"

using namespace Urho3D;


CharacterController::CharacterController(Context * context) : LogicComponent(context) {
	SetUpdateEventMask(USE_FIXEDUPDATE);
}

CharacterController::~CharacterController() {
	
}

void CharacterController::SetupNode(float height) {

	height_ = height;

	cameraNode_ = node_->CreateChild("CameraNode");
	camera_ = cameraNode_->CreateComponent<Camera>();
	cameraNode_->SetPosition(Vector3(0.0f, height_/2.0f, 0.0f));
	//          / C \  <-- Camera height standing: 1.5f
	//         |    |
	//         |  c | <-- Camera height crouching: 0.5f
	//          \  /
	// Character controller ^

	rigidBody_ = node_->CreateComponent<RigidBody>();
	rigidBody_->SetMass(1.0f);
	rigidBody_->SetAngularFactor(Vector3::ZERO);
	
	// If you've come looking for this again, you probably need to enable this.
	//rigidBody_->SetCcdRadius(1.0f);
	//rigidBody_->SetCcdMotionThreshold(1.0f);

	collider_ = node_->CreateComponent<CollisionShape>();
	collider_->SetBox(Vector3(height_/4.0f, height_, height_ / 4.0f));
	//collider_->SetCapsule(height_ / 4.0f, height_);
}

void CharacterController::Start() {
	SubscribeToEvent(E_KEYUP, URHO3D_HANDLER(CharacterController, HandleKeyUp));
	SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(CharacterController, HandleKeyDown));
	SubscribeToEvent(GetNode(), E_NODECOLLISION, URHO3D_HANDLER(CharacterController, HandleNodeCollision));
	SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(CharacterController, HandleCamera));
	

	
}
/* TODO: Implement smooth crouching.
float CharacterController::GetCrouchHeight(float x) {
	return (Cos(x/720.0f)+1)/2;
}*/

void CharacterController::FixedUpdate(float timeStep) {
	Input* input = GetSubsystem<Input>();

	if (!onGround_)
		inAirTimer_ += timeStep;
	else
		inAirTimer_ = 0.0f;

	bool softGrounded = inAirTimer_ < 0.1f;

	Vector3 moveDir = Vector3::ZERO;
	const Vector3& velocity = rigidBody_->GetLinearVelocity();

	Vector3 planeVelocity(velocity.x_, 0.0f, velocity.z_);
	

	// MOVEMENT

	if (input->GetKeyDown(KEY_W))
		moveDir += Vector3::FORWARD;
	if (input->GetKeyDown(KEY_S))
		moveDir += Vector3::BACK;
	if (input->GetKeyDown(KEY_A))
		moveDir += Vector3::LEFT;
	if (input->GetKeyDown(KEY_D))
		moveDir += Vector3::RIGHT;

	if (moveDir.LengthSquared() > 0.0f)
		moveDir.Normalize();

	rigidBody_->ApplyImpulse(Quaternion(camYaw, Vector3::UP) * moveDir * (softGrounded ? 0.8f : 0.02f) * (sprinting_ ? 2.0f : 1.0f) * (crouching_ ? 0.5f : 1.0f));
	//                      ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
	//  Future me, you have no idea how many tears were shed over this
	//  GODDAMNED QUATERNION.

	if (softGrounded) {
		Vector3 brakeForce = -planeVelocity * 0.2f;
		rigidBody_->ApplyImpulse(brakeForce);
		if (input->GetKeyDown(KEY_SPACE)) {
			URHO3D_LOGINFO("Space pressed!");
			if (okToJump_) {
				rigidBody_->ApplyImpulse(Vector3::UP * 7.0f);
				okToJump_ = false;
			}
		} else
			okToJump_ = true;
	}

	if (onGround_) {
		/// TODO: Play on ground animation when you have a model
	} else {
		/// TODO: Play off ground animation
	}

	onGround_ = false;
}

void CharacterController::HandleCamera(StringHash eventType, VariantMap & eventData) {
	using namespace Update;

	Input* input = GetSubsystem<Input>();

	IntVector2 mouseDelta = input->GetMouseMove();
	camYaw += mouseSensitivity * mouseDelta.x_;
	camPitch += mouseSensitivity *mouseDelta.y_;
	camPitch = Clamp(camPitch, -90.0f, 90.0f);

	cameraNode_->SetRotation(Quaternion(camPitch, camYaw, 0.0f));

	int wheelMove = input->GetMouseMoveWheel();
	cameraNode_->SetPosition(cameraNode_->GetPosition() + Vector3(0.0f, 0.0f, wheelMove));


}

void CharacterController::HandleNodeCollision(StringHash eventType, VariantMap & eventData) {

	// Check collision contacts and see if character is standing on ground (look for a contact that has near vertical normal)
	using namespace NodeCollision;

	MemoryBuffer contacts(eventData[P_CONTACTS].GetBuffer());

	while (!contacts.IsEof()) {
		Vector3 contactPosition = contacts.ReadVector3();
		Vector3 contactNormal = contacts.ReadVector3();
		/*float contactDistance = */contacts.ReadFloat();
		/*float contactImpulse = */contacts.ReadFloat();

		// If contact is below node center and pointing up, assume it's a ground contact
		if (contactPosition.y_ < (node_->GetPosition().y_ + 1.0f)) {
			float level = contactNormal.y_;
			if (level > 0.75)
				onGround_ = true;
		}
	}
}

void CharacterController::HandleKeyDown(StringHash eventType, VariantMap & eventData) {
	using namespace KeyDown;
	int key = eventData[P_KEY].GetInt();

	/// \todo Implement smooth crouching.

	switch (key) {
	case KEY_SHIFT:
		sprinting_ = true;
		break;
	case KEY_CTRL:
		crouching_ = true;
		sprinting_ = false;
		cameraNode_->SetPosition(Vector3(0.0f, (height_/2) - crouchOffSet_, 0.0f));
		collider_->SetBox(Vector3(height_ / 4.0f, height_, height_-crouchOffSet_ / 4.0f));
		break;
	}
}

void CharacterController::HandleKeyUp(StringHash eventType, VariantMap & eventData) {
	using namespace KeyDown;
	int key = eventData[P_KEY].GetInt();

	switch (key) {
	case KEY_SHIFT:
		sprinting_ = false;
		break;
	case KEY_CTRL:
		crouching_ = false;
		cameraNode_->SetPosition(Vector3(0.0f, height_/2.0f, 0.0f));
		collider_->SetBox(Vector3(height_ / 4.0f, height_, height_ / 4.0f));
		break;
	}

}


