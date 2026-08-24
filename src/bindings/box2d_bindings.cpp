#ifdef EMSCRIPTEN

#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <box2d/box2d.h>

using namespace emscripten;

// Helper wrappers for callbacks
class JSContactListener : public b2ContactListener {
public:
    emscripten::val jsObject = emscripten::val::null();

    JSContactListener() = default;
    JSContactListener(emscripten::val obj) : jsObject(obj) {}

    void SetHandler(emscripten::val obj) {
        jsObject = obj;
    }

    void BeginContact(b2Contact* contact) override {
        if (!jsObject.isNull() && !jsObject.isUndefined() && jsObject["BeginContact"].isFunction()) {
            jsObject.call<void>("BeginContact", contact);
        }
    }

    void EndContact(b2Contact* contact) override {
        if (!jsObject.isNull() && !jsObject.isUndefined() && jsObject["EndContact"].isFunction()) {
            jsObject.call<void>("EndContact", contact);
        }
    }

    void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override {
        if (!jsObject.isNull() && !jsObject.isUndefined() && jsObject["PreSolve"].isFunction()) {
            jsObject.call<void>("PreSolve", contact, oldManifold);
        }
    }

    void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override {
        if (!jsObject.isNull() && !jsObject.isUndefined() && jsObject["PostSolve"].isFunction()) {
            jsObject.call<void>("PostSolve", contact, impulse);
        }
    }
};

class JSQueryCallback : public b2QueryCallback {
public:
    emscripten::val jsCallback = emscripten::val::null();

    JSQueryCallback(emscripten::val cb) : jsCallback(cb) {}

    bool ReportFixture(b2Fixture* fixture) override {
        if (jsCallback.isFunction()) {
            return jsCallback.call<bool>("call", emscripten::val::null(), fixture);
        }
        return false;
    }
};

class JSRayCastCallback : public b2RayCastCallback {
public:
    emscripten::val jsCallback = emscripten::val::null();

    JSRayCastCallback(emscripten::val cb) : jsCallback(cb) {}

    float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction) override {
        if (jsCallback.isFunction()) {
            return jsCallback.call<float>("call", emscripten::val::null(), fixture, point, normal, fraction);
        }
        return fraction;
    }
};

class JSDraw : public b2Draw {
public:
    emscripten::val jsObj = emscripten::val::null();

    JSDraw() = default;
    JSDraw(emscripten::val obj) : jsObj(obj) {}

    void SetHandler(emscripten::val obj) { jsObj = obj; }

    void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override {
        if (!jsObj.isNull() && jsObj["DrawPolygon"].isFunction()) {
            emscripten::val arr = emscripten::val::array();
            for (int32 i = 0; i < vertexCount; ++i) {
                arr.set(i, vertices[i]);
            }
            jsObj.call<void>("DrawPolygon", arr, vertexCount, color);
        }
    }

    void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override {
        if (!jsObj.isNull() && jsObj["DrawSolidPolygon"].isFunction()) {
            emscripten::val arr = emscripten::val::array();
            for (int32 i = 0; i < vertexCount; ++i) {
                arr.set(i, vertices[i]);
            }
            jsObj.call<void>("DrawSolidPolygon", arr, vertexCount, color);
        }
    }

    void DrawCircle(const b2Vec2& center, float radius, const b2Color& color) override {
        if (!jsObj.isNull() && jsObj["DrawCircle"].isFunction()) {
            jsObj.call<void>("DrawCircle", center, radius, color);
        }
    }

    void DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color) override {
        if (!jsObj.isNull() && jsObj["DrawSolidCircle"].isFunction()) {
            jsObj.call<void>("DrawSolidCircle", center, radius, axis, color);
        }
    }

    void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) override {
        if (!jsObj.isNull() && jsObj["DrawSegment"].isFunction()) {
            jsObj.call<void>("DrawSegment", p1, p2, color);
        }
    }

    void DrawTransform(const b2Transform& xf) override {
        if (!jsObj.isNull() && jsObj["DrawTransform"].isFunction()) {
            jsObj.call<void>("DrawTransform", xf);
        }
    }

    void DrawPoint(const b2Vec2& p, float size, const b2Color& color) override {
        if (!jsObj.isNull() && jsObj["DrawPoint"].isFunction()) {
            jsObj.call<void>("DrawPoint", p, size, color);
        }
    }
};

EMSCRIPTEN_BINDINGS(box2d) {

    // ----------------------------------------------------
    // Math & Basics
    // ----------------------------------------------------
    class_<b2Vec2>("b2Vec2")
        .constructor<>()
        .constructor<float, float>()
        .property("x", &b2Vec2::x)
        .property("y", &b2Vec2::y)
        .function("SetZero", &b2Vec2::SetZero)
        .function("Set", &b2Vec2::Set)
        .function("Length", &b2Vec2::Length)
        .function("LengthSquared", &b2Vec2::LengthSquared)
        .function("Normalize", &b2Vec2::Normalize)
        .function("IsValid", &b2Vec2::IsValid)
        .function("Skew", &b2Vec2::Skew);

    class_<b2Vec3>("b2Vec3")
        .constructor<>()
        .constructor<float, float, float>()
        .property("x", &b2Vec3::x)
        .property("y", &b2Vec3::y)
        .property("z", &b2Vec3::z)
        .function("SetZero", &b2Vec3::SetZero)
        .function("Set", &b2Vec3::Set);

    class_<b2Rot>("b2Rot")
        .constructor<>()
        .constructor<float>()
        .property("s", &b2Rot::s)
        .property("c", &b2Rot::c)
        .function("Set", &b2Rot::Set)
        .function("SetIdentity", &b2Rot::SetIdentity)
        .function("GetAngle", &b2Rot::GetAngle)
        .function("GetXAxis", &b2Rot::GetXAxis)
        .function("GetYAxis", &b2Rot::GetYAxis);

    class_<b2Transform>("b2Transform")
        .constructor<>()
        .constructor<const b2Vec2&, const b2Rot&>()
        .property("p", &b2Transform::p)
        .property("q", &b2Transform::q)
        .function("SetIdentity", &b2Transform::SetIdentity)
        .function("Set", &b2Transform::Set);

    class_<b2AABB>("b2AABB")
        .constructor<>()
        .property("lowerBound", &b2AABB::lowerBound)
        .property("upperBound", &b2AABB::upperBound)
        .function("IsValid", &b2AABB::IsValid)
        .function("GetCenter", &b2AABB::GetCenter)
        .function("GetExtents", &b2AABB::GetExtents)
        .function("GetPerimeter", &b2AABB::GetPerimeter)
        .function("Combine", select_overload<void(const b2AABB&, const b2AABB&)>(&b2AABB::Combine))
        .function("Contains", &b2AABB::Contains)
        .function("RayCast", &b2AABB::RayCast);

    class_<b2Color>("b2Color")
        .constructor<>()
        .constructor<float, float, float, float>()
        .property("r", &b2Color::r)
        .property("g", &b2Color::g)
        .property("b", &b2Color::b)
        .property("a", &b2Color::a)
        .function("Set", &b2Color::Set);

    class_<b2MassData>("b2MassData")
        .constructor<>()
        .property("mass", &b2MassData::mass)
        .property("center", &b2MassData::center)
        .property("I", &b2MassData::I);

    class_<b2Filter>("b2Filter")
        .constructor<>()
        .property("categoryBits", &b2Filter::categoryBits)
        .property("maskBits", &b2Filter::maskBits)
        .property("groupIndex", &b2Filter::groupIndex);

    // ----------------------------------------------------
    // Enums
    // ----------------------------------------------------
    enum_<b2BodyType>("b2BodyType")
        .value("b2_staticBody", b2_staticBody)
        .value("b2_kinematicBody", b2_kinematicBody)
        .value("b2_dynamicBody", b2_dynamicBody);

    enum_<b2Shape::Type>("b2ShapeType")
        .value("e_circle", b2Shape::e_circle)
        .value("e_edge", b2Shape::e_edge)
        .value("e_polygon", b2Shape::e_polygon)
        .value("e_chain", b2Shape::e_chain)
        .value("e_typeCount", b2Shape::e_typeCount);

    enum_<b2JointType>("b2JointType")
        .value("e_unknownJoint", e_unknownJoint)
        .value("e_revoluteJoint", e_revoluteJoint)
        .value("e_prismaticJoint", e_prismaticJoint)
        .value("e_distanceJoint", e_distanceJoint)
        .value("e_pulleyJoint", e_pulleyJoint)
        .value("e_mouseJoint", e_mouseJoint)
        .value("e_gearJoint", e_gearJoint)
        .value("e_wheelJoint", e_wheelJoint)
        .value("e_weldJoint", e_weldJoint)
        .value("e_frictionJoint", e_frictionJoint)
        .value("e_motorJoint", e_motorJoint);

    // ----------------------------------------------------
    // Shapes
    // ----------------------------------------------------
    class_<b2Shape>("b2Shape")
        .function("GetType", &b2Shape::GetType)
        .function("GetChildCount", &b2Shape::GetChildCount)
        .function("TestPoint", &b2Shape::TestPoint)
        .property("m_radius", &b2Shape::m_radius);

    class_<b2PolygonShape, base<b2Shape>>("b2PolygonShape")
        .constructor<>()
        .property("m_centroid", &b2PolygonShape::m_centroid)
        .property("m_count", &b2PolygonShape::m_count)
        .function("SetAsBox", select_overload<void(float, float)>(&b2PolygonShape::SetAsBox))
        .function("SetAsBoxOriented", select_overload<void(float, float, const b2Vec2&, float)>(&b2PolygonShape::SetAsBox))
        .function("Validate", &b2PolygonShape::Validate);

    class_<b2CircleShape, base<b2Shape>>("b2CircleShape")
        .constructor<>()
        .property("m_p", &b2CircleShape::m_p);

    class_<b2EdgeShape, base<b2Shape>>("b2EdgeShape")
        .constructor<>()
        .property("m_vertex1", &b2EdgeShape::m_vertex1)
        .property("m_vertex2", &b2EdgeShape::m_vertex2)
        .property("m_vertex0", &b2EdgeShape::m_vertex0)
        .property("m_vertex3", &b2EdgeShape::m_vertex3)
        .property("m_oneSided", &b2EdgeShape::m_oneSided)
        .function("SetOneSided", &b2EdgeShape::SetOneSided)
        .function("SetTwoSided", &b2EdgeShape::SetTwoSided);

    class_<b2ChainShape, base<b2Shape>>("b2ChainShape")
        .constructor<>()
        .property("m_count", &b2ChainShape::m_count)
        .function("Clear", &b2ChainShape::Clear);

    // ----------------------------------------------------
    // Fixture & Body
    // ----------------------------------------------------
    class_<b2FixtureDef>("b2FixtureDef")
        .constructor<>()
        .property("shape", &b2FixtureDef::shape, allow_raw_pointers())
        .property("friction", &b2FixtureDef::friction)
        .property("restitution", &b2FixtureDef::restitution)
        .property("restitutionThreshold", &b2FixtureDef::restitutionThreshold)
        .property("density", &b2FixtureDef::density)
        .property("isSensor", &b2FixtureDef::isSensor)
        .property("filter", &b2FixtureDef::filter);

    class_<b2Fixture>("b2Fixture")
        .function("GetType", &b2Fixture::GetType)
        .function("GetShape", select_overload<b2Shape*()>(&b2Fixture::GetShape), allow_raw_pointers())
        .function("SetSensor", &b2Fixture::SetSensor)
        .function("IsSensor", &b2Fixture::IsSensor)
        .function("SetFilterData", &b2Fixture::SetFilterData)
        .function("GetFilterData", &b2Fixture::GetFilterData)
        .function("Refilter", &b2Fixture::Refilter)
        .function("GetBody", select_overload<b2Body*()>(&b2Fixture::GetBody), allow_raw_pointers())
        .function("GetNext", select_overload<b2Fixture*()>(&b2Fixture::GetNext), allow_raw_pointers())
        .function("TestPoint", &b2Fixture::TestPoint)
        .function("SetDensity", &b2Fixture::SetDensity)
        .function("GetDensity", &b2Fixture::GetDensity)
        .function("SetFriction", &b2Fixture::SetFriction)
        .function("GetFriction", &b2Fixture::GetFriction)
        .function("SetRestitution", &b2Fixture::SetRestitution)
        .function("GetRestitution", &b2Fixture::GetRestitution)
        .function("SetRestitutionThreshold", &b2Fixture::SetRestitutionThreshold)
        .function("GetRestitutionThreshold", &b2Fixture::GetRestitutionThreshold)
        .function("GetAABB", &b2Fixture::GetAABB);

    class_<b2BodyDef>("b2BodyDef")
        .constructor<>()
        .property("type", &b2BodyDef::type)
        .property("position", &b2BodyDef::position)
        .property("angle", &b2BodyDef::angle)
        .property("linearVelocity", &b2BodyDef::linearVelocity)
        .property("angularVelocity", &b2BodyDef::angularVelocity)
        .property("linearDamping", &b2BodyDef::linearDamping)
        .property("angularDamping", &b2BodyDef::angularDamping)
        .property("allowSleep", &b2BodyDef::allowSleep)
        .property("awake", &b2BodyDef::awake)
        .property("fixedRotation", &b2BodyDef::fixedRotation)
        .property("bullet", &b2BodyDef::bullet)
        .property("enabled", &b2BodyDef::enabled)
        .property("gravityScale", &b2BodyDef::gravityScale);

    class_<b2Body>("b2Body")
        .function("CreateFixture", select_overload<b2Fixture*(const b2FixtureDef*)>(&b2Body::CreateFixture), allow_raw_pointers())
        .function("CreateFixtureShape", select_overload<b2Fixture*(const b2Shape*, float)>(&b2Body::CreateFixture), allow_raw_pointers())
        .function("DestroyFixture", &b2Body::DestroyFixture, allow_raw_pointers())
        .function("SetTransform", &b2Body::SetTransform)
        .function("GetPosition", &b2Body::GetPosition)
        .function("GetAngle", &b2Body::GetAngle)
        .function("GetWorldCenter", &b2Body::GetWorldCenter)
        .function("GetLocalCenter", &b2Body::GetLocalCenter)
        .function("SetLinearVelocity", &b2Body::SetLinearVelocity)
        .function("GetLinearVelocity", &b2Body::GetLinearVelocity)
        .function("SetAngularVelocity", &b2Body::SetAngularVelocity)
        .function("GetAngularVelocity", &b2Body::GetAngularVelocity)
        .function("ApplyForce", &b2Body::ApplyForce)
        .function("ApplyForceToCenter", &b2Body::ApplyForceToCenter)
        .function("ApplyTorque", &b2Body::ApplyTorque)
        .function("ApplyLinearImpulse", &b2Body::ApplyLinearImpulse)
        .function("ApplyLinearImpulseToCenter", &b2Body::ApplyLinearImpulseToCenter)
        .function("ApplyAngularImpulse", &b2Body::ApplyAngularImpulse)
        .function("GetMass", &b2Body::GetMass)
        .function("GetInertia", &b2Body::GetInertia)
        .function("GetWorldPoint", &b2Body::GetWorldPoint)
        .function("GetWorldVector", &b2Body::GetWorldVector)
        .function("GetLocalPoint", &b2Body::GetLocalPoint)
        .function("GetLocalVector", &b2Body::GetLocalVector)
        .function("GetLinearVelocityFromWorldPoint", &b2Body::GetLinearVelocityFromWorldPoint)
        .function("GetLinearVelocityFromLocalPoint", &b2Body::GetLinearVelocityFromLocalPoint)
        .function("SetType", &b2Body::SetType)
        .function("GetType", &b2Body::GetType)
        .function("SetBullet", &b2Body::SetBullet)
        .function("IsBullet", &b2Body::IsBullet)
        .function("SetSleepingAllowed", &b2Body::SetSleepingAllowed)
        .function("IsSleepingAllowed", &b2Body::IsSleepingAllowed)
        .function("SetAwake", &b2Body::SetAwake)
        .function("IsAwake", &b2Body::IsAwake)
        .function("SetEnabled", &b2Body::SetEnabled)
        .function("IsEnabled", &b2Body::IsEnabled)
        .function("SetFixedRotation", &b2Body::SetFixedRotation)
        .function("IsFixedRotation", &b2Body::IsFixedRotation)
        .function("GetFixtureList", select_overload<b2Fixture*()>(&b2Body::GetFixtureList), allow_raw_pointers())
        .function("GetNext", select_overload<b2Body*()>(&b2Body::GetNext), allow_raw_pointers())
        .function("SetGravityScale", &b2Body::SetGravityScale)
        .function("GetGravityScale", &b2Body::GetGravityScale);

    // ----------------------------------------------------
    // Joints
    // ----------------------------------------------------
    class_<b2JointDef>("b2JointDef")
        .property("type", &b2JointDef::type)
        .property("bodyA", &b2JointDef::bodyA, allow_raw_pointers())
        .property("bodyB", &b2JointDef::bodyB, allow_raw_pointers())
        .property("collideConnected", &b2JointDef::collideConnected);

    class_<b2Joint>("b2Joint")
        .function("GetType", &b2Joint::GetType)
        .function("GetBodyA", &b2Joint::GetBodyA, allow_raw_pointers())
        .function("GetBodyB", &b2Joint::GetBodyB, allow_raw_pointers())
        .function("GetAnchorA", &b2Joint::GetAnchorA)
        .function("GetAnchorB", &b2Joint::GetAnchorB)
        .function("GetReactionForce", &b2Joint::GetReactionForce)
        .function("GetReactionTorque", &b2Joint::GetReactionTorque)
        .function("GetNext", select_overload<b2Joint*()>(&b2Joint::GetNext), allow_raw_pointers())
        .function("IsEnabled", &b2Joint::IsEnabled)
        .function("GetCollideConnected", &b2Joint::GetCollideConnected);

    class_<b2RevoluteJointDef, base<b2JointDef>>("b2RevoluteJointDef")
        .constructor<>()
        .property("localAnchorA", &b2RevoluteJointDef::localAnchorA)
        .property("localAnchorB", &b2RevoluteJointDef::localAnchorB)
        .property("referenceAngle", &b2RevoluteJointDef::referenceAngle)
        .property("enableLimit", &b2RevoluteJointDef::enableLimit)
        .property("lowerAngle", &b2RevoluteJointDef::lowerAngle)
        .property("upperAngle", &b2RevoluteJointDef::upperAngle)
        .property("enableMotor", &b2RevoluteJointDef::enableMotor)
        .property("motorSpeed", &b2RevoluteJointDef::motorSpeed)
        .property("maxMotorTorque", &b2RevoluteJointDef::maxMotorTorque)
        .function("Initialize", &b2RevoluteJointDef::Initialize, allow_raw_pointers());

    class_<b2RevoluteJoint, base<b2Joint>>("b2RevoluteJoint")
        .function("GetLocalAnchorA", &b2RevoluteJoint::GetLocalAnchorA)
        .function("GetLocalAnchorB", &b2RevoluteJoint::GetLocalAnchorB)
        .function("GetReferenceAngle", &b2RevoluteJoint::GetReferenceAngle)
        .function("GetJointAngle", &b2RevoluteJoint::GetJointAngle)
        .function("GetJointSpeed", &b2RevoluteJoint::GetJointSpeed)
        .function("IsLimitEnabled", &b2RevoluteJoint::IsLimitEnabled)
        .function("EnableLimit", &b2RevoluteJoint::EnableLimit)
        .function("GetLowerLimit", &b2RevoluteJoint::GetLowerLimit)
        .function("GetUpperLimit", &b2RevoluteJoint::GetUpperLimit)
        .function("SetLimits", &b2RevoluteJoint::SetLimits)
        .function("IsMotorEnabled", &b2RevoluteJoint::IsMotorEnabled)
        .function("EnableMotor", &b2RevoluteJoint::EnableMotor)
        .function("SetMotorSpeed", &b2RevoluteJoint::SetMotorSpeed)
        .function("GetMotorSpeed", &b2RevoluteJoint::GetMotorSpeed)
        .function("SetMaxMotorTorque", &b2RevoluteJoint::SetMaxMotorTorque)
        .function("GetMaxMotorTorque", &b2RevoluteJoint::GetMaxMotorTorque);

    class_<b2DistanceJointDef, base<b2JointDef>>("b2DistanceJointDef")
        .constructor<>()
        .property("localAnchorA", &b2DistanceJointDef::localAnchorA)
        .property("localAnchorB", &b2DistanceJointDef::localAnchorB)
        .property("length", &b2DistanceJointDef::length)
        .property("minLength", &b2DistanceJointDef::minLength)
        .property("maxLength", &b2DistanceJointDef::maxLength)
        .property("stiffness", &b2DistanceJointDef::stiffness)
        .property("damping", &b2DistanceJointDef::damping)
        .function("Initialize", &b2DistanceJointDef::Initialize, allow_raw_pointers());

    class_<b2DistanceJoint, base<b2Joint>>("b2DistanceJoint")
        .function("GetLength", &b2DistanceJoint::GetLength)
        .function("SetLength", &b2DistanceJoint::SetLength)
        .function("GetMinLength", &b2DistanceJoint::GetMinLength)
        .function("SetMinLength", &b2DistanceJoint::SetMinLength)
        .function("GetMaxLength", &b2DistanceJoint::GetMaxLength)
        .function("SetMaxLength", &b2DistanceJoint::SetMaxLength)
        .function("GetStiffness", &b2DistanceJoint::GetStiffness)
        .function("SetStiffness", &b2DistanceJoint::SetStiffness)
        .function("GetDamping", &b2DistanceJoint::GetDamping)
        .function("SetDamping", &b2DistanceJoint::SetDamping);

    class_<b2PrismaticJointDef, base<b2JointDef>>("b2PrismaticJointDef")
        .constructor<>()
        .property("localAnchorA", &b2PrismaticJointDef::localAnchorA)
        .property("localAnchorB", &b2PrismaticJointDef::localAnchorB)
        .property("localAxisA", &b2PrismaticJointDef::localAxisA)
        .property("referenceAngle", &b2PrismaticJointDef::referenceAngle)
        .property("enableLimit", &b2PrismaticJointDef::enableLimit)
        .property("lowerTranslation", &b2PrismaticJointDef::lowerTranslation)
        .property("upperTranslation", &b2PrismaticJointDef::upperTranslation)
        .property("enableMotor", &b2PrismaticJointDef::enableMotor)
        .property("maxMotorForce", &b2PrismaticJointDef::maxMotorForce)
        .property("motorSpeed", &b2PrismaticJointDef::motorSpeed)
        .function("Initialize", &b2PrismaticJointDef::Initialize, allow_raw_pointers());

    class_<b2PrismaticJoint, base<b2Joint>>("b2PrismaticJoint")
        .function("GetJointTranslation", &b2PrismaticJoint::GetJointTranslation)
        .function("GetJointSpeed", &b2PrismaticJoint::GetJointSpeed)
        .function("IsLimitEnabled", &b2PrismaticJoint::IsLimitEnabled)
        .function("EnableLimit", &b2PrismaticJoint::EnableLimit)
        .function("IsMotorEnabled", &b2PrismaticJoint::IsMotorEnabled)
        .function("EnableMotor", &b2PrismaticJoint::EnableMotor)
        .function("SetMotorSpeed", &b2PrismaticJoint::SetMotorSpeed)
        .function("GetMotorSpeed", &b2PrismaticJoint::GetMotorSpeed)
        .function("SetMaxMotorForce", &b2PrismaticJoint::SetMaxMotorForce);

    class_<b2MouseJointDef, base<b2JointDef>>("b2MouseJointDef")
        .constructor<>()
        .property("target", &b2MouseJointDef::target)
        .property("maxForce", &b2MouseJointDef::maxForce)
        .property("stiffness", &b2MouseJointDef::stiffness)
        .property("damping", &b2MouseJointDef::damping);

    class_<b2MouseJoint, base<b2Joint>>("b2MouseJoint")
        .function("SetTarget", &b2MouseJoint::SetTarget)
        .function("GetTarget", &b2MouseJoint::GetTarget)
        .function("SetMaxForce", &b2MouseJoint::SetMaxForce)
        .function("GetMaxForce", &b2MouseJoint::GetMaxForce)
        .function("SetStiffness", &b2MouseJoint::SetStiffness)
        .function("GetStiffness", &b2MouseJoint::GetStiffness)
        .function("SetDamping", &b2MouseJoint::SetDamping)
        .function("GetDamping", &b2MouseJoint::GetDamping);

    class_<b2WeldJointDef, base<b2JointDef>>("b2WeldJointDef")
        .constructor<>()
        .property("localAnchorA", &b2WeldJointDef::localAnchorA)
        .property("localAnchorB", &b2WeldJointDef::localAnchorB)
        .property("referenceAngle", &b2WeldJointDef::referenceAngle)
        .property("stiffness", &b2WeldJointDef::stiffness)
        .property("damping", &b2WeldJointDef::damping)
        .function("Initialize", &b2WeldJointDef::Initialize, allow_raw_pointers());

    class_<b2WheelJointDef, base<b2JointDef>>("b2WheelJointDef")
        .constructor<>()
        .property("localAnchorA", &b2WheelJointDef::localAnchorA)
        .property("localAnchorB", &b2WheelJointDef::localAnchorB)
        .property("localAxisA", &b2WheelJointDef::localAxisA)
        .property("enableLimit", &b2WheelJointDef::enableLimit)
        .property("lowerTranslation", &b2WheelJointDef::lowerTranslation)
        .property("upperTranslation", &b2WheelJointDef::upperTranslation)
        .property("enableMotor", &b2WheelJointDef::enableMotor)
        .property("maxMotorTorque", &b2WheelJointDef::maxMotorTorque)
        .property("motorSpeed", &b2WheelJointDef::motorSpeed)
        .property("stiffness", &b2WheelJointDef::stiffness)
        .property("damping", &b2WheelJointDef::damping)
        .function("Initialize", &b2WheelJointDef::Initialize, allow_raw_pointers());

    class_<b2MotorJointDef, base<b2JointDef>>("b2MotorJointDef")
        .constructor<>()
        .property("linearOffset", &b2MotorJointDef::linearOffset)
        .property("angularOffset", &b2MotorJointDef::angularOffset)
        .property("maxForce", &b2MotorJointDef::maxForce)
        .property("maxTorque", &b2MotorJointDef::maxTorque)
        .property("correctionFactor", &b2MotorJointDef::correctionFactor)
        .function("Initialize", &b2MotorJointDef::Initialize, allow_raw_pointers());

    class_<b2FrictionJointDef, base<b2JointDef>>("b2FrictionJointDef")
        .constructor<>()
        .property("localAnchorA", &b2FrictionJointDef::localAnchorA)
        .property("localAnchorB", &b2FrictionJointDef::localAnchorB)
        .property("maxForce", &b2FrictionJointDef::maxForce)
        .property("maxTorque", &b2FrictionJointDef::maxTorque)
        .function("Initialize", &b2FrictionJointDef::Initialize, allow_raw_pointers());

    // ----------------------------------------------------
    // Contacts & World Callbacks
    // ----------------------------------------------------
    class_<b2Contact>("b2Contact")
        .function("GetManifold", select_overload<b2Manifold*()>(&b2Contact::GetManifold), allow_raw_pointers())
        .function("IsTouching", &b2Contact::IsTouching)
        .function("SetEnabled", &b2Contact::SetEnabled)
        .function("IsEnabled", &b2Contact::IsEnabled)
        .function("GetNext", select_overload<b2Contact*()>(&b2Contact::GetNext), allow_raw_pointers())
        .function("GetFixtureA", select_overload<b2Fixture*()>(&b2Contact::GetFixtureA), allow_raw_pointers())
        .function("GetChildIndexA", &b2Contact::GetChildIndexA)
        .function("GetFixtureB", select_overload<b2Fixture*()>(&b2Contact::GetFixtureB), allow_raw_pointers())
        .function("GetChildIndexB", &b2Contact::GetChildIndexB)
        .function("SetFriction", &b2Contact::SetFriction)
        .function("GetFriction", &b2Contact::GetFriction)
        .function("ResetFriction", &b2Contact::ResetFriction)
        .function("SetRestitution", &b2Contact::SetRestitution)
        .function("GetRestitution", &b2Contact::GetRestitution)
        .function("ResetRestitution", &b2Contact::ResetRestitution);

    class_<JSContactListener>("JSContactListener")
        .constructor<>()
        .constructor<emscripten::val>()
        .function("SetHandler", &JSContactListener::SetHandler);

    class_<JSDraw>("JSDraw")
        .constructor<>()
        .constructor<emscripten::val>()
        .function("SetHandler", &JSDraw::SetHandler)
        .function("SetFlags", &JSDraw::SetFlags)
        .function("GetFlags", &JSDraw::GetFlags)
        .function("AppendFlags", &JSDraw::AppendFlags)
        .function("ClearFlags", &JSDraw::ClearFlags);

    // ----------------------------------------------------
    // World
    // ----------------------------------------------------
    class_<b2World>("b2World")
        .constructor<const b2Vec2&>()
        .function("SetGravity", &b2World::SetGravity)
        .function("GetGravity", &b2World::GetGravity)
        .function("CreateBody", &b2World::CreateBody, allow_raw_pointers())
        .function("DestroyBody", &b2World::DestroyBody, allow_raw_pointers())
        .function("CreateJoint", &b2World::CreateJoint, allow_raw_pointers())
        .function("DestroyJoint", &b2World::DestroyJoint, allow_raw_pointers())
        .function("Step", &b2World::Step)
        .function("ClearForces", &b2World::ClearForces)
        .function("GetBodyList", select_overload<b2Body*()>(&b2World::GetBodyList), allow_raw_pointers())
        .function("GetJointList", select_overload<b2Joint*()>(&b2World::GetJointList), allow_raw_pointers())
        .function("GetContactList", select_overload<b2Contact*()>(&b2World::GetContactList), allow_raw_pointers())
        .function("GetBodyCount", &b2World::GetBodyCount)
        .function("GetJointCount", &b2World::GetJointCount)
        .function("GetContactCount", &b2World::GetContactCount)
        .function("SetAllowSleeping", &b2World::SetAllowSleeping)
        .function("GetAllowSleeping", &b2World::GetAllowSleeping)
        .function("SetWarmStarting", &b2World::SetWarmStarting)
        .function("GetWarmStarting", &b2World::GetWarmStarting)
        .function("SetContinuousPhysics", &b2World::SetContinuousPhysics)
        .function("GetContinuousPhysics", &b2World::GetContinuousPhysics)
        .function("SetSubStepping", &b2World::SetSubStepping)
        .function("GetSubStepping", &b2World::GetSubStepping)
        .function("SetContactListener", select_overload<void(b2ContactListener*)>(&b2World::SetContactListener), allow_raw_pointers())
        .function("SetDebugDraw", select_overload<void(b2Draw*)>(&b2World::SetDebugDraw), allow_raw_pointers())
        .function("DebugDraw", &b2World::DebugDraw);
}

#endif
